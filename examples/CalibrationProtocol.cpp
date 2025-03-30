#include <Arduino.h>

// Now include project headers
#include "../src/hardware/HardwareManager.h"
#include "../src/hardware/LEDInterface.h"
#include "../src/hardware/MPU9250Interface.h"
#include "../src/core/SystemTypes.h"
#include "../src/core/Config.h"

// Get hardware manager instance
HardwareManager* hardware = HardwareManager::getInstance();

// Calibration states
enum CalibrationState {
  STATE_WARMUP,
  STATE_POSITION_OFFER,
  STATE_POSITION_CALM,
  STATE_POSITION_OATH,
  STATE_POSITION_DIG,
  STATE_POSITION_SHIELD,
  STATE_POSITION_NULL,
  STATE_COMPLETE
};

// Timing constants (in milliseconds)
const unsigned long WARMUP_DURATION = 30000;  // 30 seconds warmup
const unsigned long POSITION_DURATION = 30000; // 30 seconds per position
const unsigned long TRANSITION_DURATION = 5000; // 5 seconds transition
const unsigned long WARMUP_WARNING_DURATION = 5000; // 5 seconds warning
const unsigned long STATUS_UPDATE_INTERVAL = 2000; // Status update every 2 seconds

// Current calibration state
CalibrationState currentState = STATE_WARMUP;
unsigned long stateStartTime = 0;
unsigned long lastUpdateTime = 0;
unsigned long lastStatusTime = 0;
uint8_t sampleCount = 0;
int samplesCollected = 0;

// LED animation variables
uint8_t pulseValue = 0;
bool pulseIncreasing = true;
uint8_t flashCounter = 0;

// Data collection variables
const int MAX_SAMPLES = 300; // 10 samples per second for 30 seconds
int sampleIndex = 0;
SensorData positionSamples[MAX_SAMPLES];

// Function prototypes
void updateLEDs();
void collectSensorData();
void transitionToNextState();
void printCSVHeader();
void outputSampleCSV(const SensorData& sample, uint8_t position);
const char* getStateName(CalibrationState state);
const char* getPositionDescription(CalibrationState state);
uint8_t getPositionForState(CalibrationState state);
void printStatusUpdate();

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);  // Allow time for serial connection to establish
  
  Serial.println("\n==================================");
  Serial.println("PrismaTech Gauntlet 3.0 - Calibration Protocol");
  Serial.println("==================================");
  
  // Initialize hardware
  Serial.println("\nInitializing hardware...");
  if (hardware->init()) {
    Serial.println("Hardware initialized successfully");
  } else {
    Serial.println("Failed to initialize hardware");
    while (true) {
      delay(1000);
    }
  }
  
  // Set initial state and time
  stateStartTime = millis();
  lastUpdateTime = millis();
  lastStatusTime = millis();
  
  // Print instructions
  Serial.println("\n==================================");
  Serial.println("Calibration Protocol Instructions:");
  Serial.println("==================================");
  Serial.println("1. The gauntlet will start with a 30-second warmup period");
  Serial.println("2. After warmup, it will glow the color for the first position");
  Serial.println("3. Hold each position for 30 seconds when prompted");
  Serial.println("4. A flashing white light indicates transition between positions");
  Serial.println("5. Data will be streamed to serial output during collection");
  Serial.println("==================================\n");
  
  // Print the CSV header for data analysis
  printCSVHeader();
  
  Serial.println("\n==================================");
  Serial.println("Starting 30-second warmup period...");
  Serial.println("Get ready to place the gauntlet on your hand");
  Serial.println("==================================\n");
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - stateStartTime;
  
  // Update hardware (read sensors)
  hardware->update();
  
  // Non-blocking LED updates (50ms interval = ~20Hz)
  if (currentMillis - lastUpdateTime >= 50) {
    lastUpdateTime = currentMillis;
    updateLEDs();
    
    // If in a position state, collect sensor data
    if (currentState > STATE_WARMUP && currentState < STATE_COMPLETE) {
      // Collect data at approximately 10Hz (every 5th update)
      if (sampleCount++ % 5 == 0) {
        collectSensorData();
        samplesCollected++;
      }
    }
  }
  
  // Periodic status updates (non-blocking)
  if (currentMillis - lastStatusTime >= STATUS_UPDATE_INTERVAL) {
    lastStatusTime = currentMillis;
    printStatusUpdate();
  }
  
  // Check for state transitions
  switch (currentState) {
    case STATE_WARMUP:
      if (elapsedTime >= WARMUP_DURATION) {
        transitionToNextState();
      }
      break;
      
    case STATE_POSITION_OFFER:
    case STATE_POSITION_CALM:
    case STATE_POSITION_OATH:
    case STATE_POSITION_DIG:
    case STATE_POSITION_SHIELD:
    case STATE_POSITION_NULL:
      if (elapsedTime >= POSITION_DURATION) {
        transitionToNextState();
      }
      break;
      
    case STATE_COMPLETE:
      // Just keep the white pulse going
      break;
  }
}

void updateLEDs() {
  // Different animations based on state
  switch (currentState) {
    case STATE_WARMUP: {
      unsigned long elapsedTime = millis() - stateStartTime;
      
      if (elapsedTime < WARMUP_DURATION - WARMUP_WARNING_DURATION) {
        // Soft white pulsing during warmup
        uint8_t newPulseValue;
        if (pulseIncreasing) {
          newPulseValue = min(100, pulseValue + 2);
          if (newPulseValue >= 100) pulseIncreasing = false;
        } else {
          newPulseValue = max(20, pulseValue - 2);
          if (newPulseValue <= 20) pulseIncreasing = true;
        }
        
        // Only update if the value actually changed
        if (newPulseValue != pulseValue) {
          pulseValue = newPulseValue;
          hardware->setBrightness(pulseValue);
        }
        Color white = {255, 255, 255};
        hardware->setAllLEDs(white);
      } else {
        // Rapidly blinking brighter white light for final 5 seconds
        flashCounter = (flashCounter + 1) % 10;
        if (flashCounter < 5) {
          Color white = {255, 255, 255};
          hardware->setBrightness(180);
          hardware->setAllLEDs(white);
        } else {
          Color off = {0, 0, 0};
          hardware->setAllLEDs(off);
        }
      }
      break;
    }
    
    case STATE_POSITION_OFFER:
    case STATE_POSITION_CALM:
    case STATE_POSITION_OATH:
    case STATE_POSITION_DIG:
    case STATE_POSITION_SHIELD:
    case STATE_POSITION_NULL: {
      // Create the right color for this position
      uint8_t position = getPositionForState(currentState);
      
      // Get position color directly from Config
      Color posColor;
      switch(position) {
        case POS_OFFER:  posColor = {Config::Colors::OFFER_COLOR[0], Config::Colors::OFFER_COLOR[1], Config::Colors::OFFER_COLOR[2]}; break;
        case POS_CALM:   posColor = {Config::Colors::CALM_COLOR[0], Config::Colors::CALM_COLOR[1], Config::Colors::CALM_COLOR[2]}; break;
        case POS_OATH:   posColor = {Config::Colors::OATH_COLOR[0], Config::Colors::OATH_COLOR[1], Config::Colors::OATH_COLOR[2]}; break;
        case POS_DIG:    posColor = {Config::Colors::DIG_COLOR[0], Config::Colors::DIG_COLOR[1], Config::Colors::DIG_COLOR[2]}; break;
        case POS_SHIELD: posColor = {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}; break;
        case POS_NULL:   posColor = {Config::Colors::NULL_COLOR[0], Config::Colors::NULL_COLOR[1], Config::Colors::NULL_COLOR[2]}; break;
        default:         posColor = {255, 255, 255}; break;
      }
      
      // Set the color through the hardware manager
      hardware->setBrightness(150);
      hardware->setAllLEDs(posColor);
      break;
    }
    
    case STATE_COMPLETE: {
      // Completion pulse white
      uint8_t newPulseValue;
      if (pulseIncreasing) {
        newPulseValue = min(180, pulseValue + 5);
        if (newPulseValue >= 180) pulseIncreasing = false;
      } else {
        newPulseValue = max(30, pulseValue - 5);
        if (newPulseValue <= 30) pulseIncreasing = true;
      }
      
      // Only update if the value actually changed
      if (newPulseValue != pulseValue) {
        pulseValue = newPulseValue;
        hardware->setBrightness(pulseValue);
      }
      Color white = {255, 255, 255};
      hardware->setAllLEDs(white);
      break;
    }
  }
  
  // Show the LEDs
  hardware->updateLEDs();
}

void collectSensorData() {
  SensorData data = hardware->getSensorData();
  
  // Record in our array if space available
  if (sampleIndex < MAX_SAMPLES) {
    positionSamples[sampleIndex++] = data;
  }
  
  // Output to serial in CSV format, but don't output every sample to keep console clean
  // Only output every 10th sample (approximately once per second)
  if (samplesCollected % 10 == 0) {
    uint8_t position = getPositionForState(currentState);
    outputSampleCSV(data, position);
  }
}

void transitionToNextState() {
  // Reset sample collection
  sampleIndex = 0;
  samplesCollected = 0;
  
  // Move to next state
  switch (currentState) {
    case STATE_WARMUP:
      currentState = STATE_POSITION_OFFER;
      break;
    case STATE_POSITION_OFFER:
      currentState = STATE_POSITION_CALM;
      break;
    case STATE_POSITION_CALM:
      currentState = STATE_POSITION_OATH;
      break;
    case STATE_POSITION_OATH:
      currentState = STATE_POSITION_DIG;
      break;
    case STATE_POSITION_DIG:
      currentState = STATE_POSITION_SHIELD;
      break;
    case STATE_POSITION_SHIELD:
      currentState = STATE_POSITION_NULL;
      break;
    case STATE_POSITION_NULL:
      currentState = STATE_COMPLETE;
      break;
    default:
      break;
  }
  
  // Reset state timing
  stateStartTime = millis();
  
  // Print state change information
  const char* stateName = getStateName(currentState);
  const char* positionDesc = getPositionDescription(currentState);
  
  Serial.print("\n==================================\n");
  Serial.print("Transitioning to: ");
  Serial.println(stateName);
  
  if (currentState != STATE_COMPLETE) {
    Serial.println(positionDesc);
    Serial.println("Hold this position for 30 seconds");
  } else {
    Serial.println("Calibration complete! Data collection finished.");
    Serial.println("You can now analyze the collected data.");
    Serial.println("\n==================================");
    Serial.println("Next steps:");
    Serial.println("1. Make sure you've saved the calibration data");
    Serial.println("2. Run 'python utils/analyze_calibration.py logs/calibration_data_TIMESTAMP.csv'");
    Serial.println("3. Copy the thresholds from logs/suggested_thresholds.txt to your code");
    Serial.println("4. Remember to note the sensor placement and orientation!");
    Serial.println("==================================\n");
  }
  Serial.print("==================================\n\n");
  
  // If not complete, show flashing white for transition
  if (currentState != STATE_COMPLETE) {
    // Flash white for transition
    hardware->setBrightness(150);
    Color white = {255, 255, 255};
    hardware->setAllLEDs(white);
    hardware->updateLEDs();
    
    // Short delay to make the transition noticeable
    delay(100);
  }
}

void printStatusUpdate() {
  unsigned long elapsedTime = millis() - stateStartTime;
  unsigned long remainingTime = 0;
  
  switch (currentState) {
    case STATE_WARMUP:
      remainingTime = (WARMUP_DURATION - elapsedTime) / 1000;
      if (remainingTime <= WARMUP_WARNING_DURATION / 1000 && remainingTime > 0) {
        Serial.print("WARNING: ");
        Serial.print(remainingTime);
        Serial.println(" seconds until first position. Get ready!");
      } else if (remainingTime > 0 && remainingTime % 5 == 0) { // only print every 5 seconds
        Serial.print("Warmup: ");
        Serial.print(remainingTime);
        Serial.println(" seconds remaining");
      }
      break;
      
    case STATE_POSITION_OFFER:
    case STATE_POSITION_CALM:
    case STATE_POSITION_OATH:
    case STATE_POSITION_DIG:
    case STATE_POSITION_SHIELD:
    case STATE_POSITION_NULL:
      remainingTime = (POSITION_DURATION - elapsedTime) / 1000;
      if (remainingTime > 0 && remainingTime % 5 == 0) { // only print every 5 seconds
        Serial.print("Current position: ");
        Serial.print(getStateName(currentState));
        Serial.print(" - ");
        Serial.print(remainingTime);
        Serial.println(" seconds remaining");
      }
      break;
      
    case STATE_COMPLETE:
      // Nothing to update in the complete state
      break;
  }
}

void printCSVHeader() {
  // Get timestamp for this calibration run
  char timestamp[24];
  time_t now = time(nullptr);
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
  
  Serial.println("# PrismaTech Gauntlet 3.0 Calibration Data");
  Serial.println("# =====================================");
  Serial.println(String("# Date: ") + timestamp);
  Serial.println("# IMPORTANT: Record these details for this calibration run:");
  Serial.println("# Sensor placement (back-of-hand/forearm/etc): ");  
  Serial.println("# Sensor orientation (face-down/face-up/etc): ");
  Serial.println("# Additional notes: ");
  Serial.println("# =====================================");
  Serial.println("timestamp,position,accelX,accelY,accelZ,gyroX,gyroY,gyroZ");
}

void outputSampleCSV(const SensorData& sample, uint8_t position) {
  Serial.print(sample.timestamp);
  Serial.print(",");
  Serial.print(position);
  Serial.print(",");
  Serial.print(sample.accelX);
  Serial.print(",");
  Serial.print(sample.accelY);
  Serial.print(",");
  Serial.print(sample.accelZ);
  Serial.print(",");
  Serial.print(sample.gyroX);
  Serial.print(",");
  Serial.print(sample.gyroY);
  Serial.print(",");
  Serial.println(sample.gyroZ);
}

const char* getStateName(CalibrationState state) {
  switch (state) {
    case STATE_WARMUP: return "Warmup";
    case STATE_POSITION_OFFER: return "Position: Offer (Purple)";
    case STATE_POSITION_CALM: return "Position: Calm (Yellow)";
    case STATE_POSITION_OATH: return "Position: Oath (Red)";
    case STATE_POSITION_DIG: return "Position: Dig (Green)";
    case STATE_POSITION_SHIELD: return "Position: Shield (Blue)";
    case STATE_POSITION_NULL: return "Position: Null (Orange)";
    case STATE_COMPLETE: return "Calibration Complete";
    default: return "Unknown State";
  }
}

const char* getPositionDescription(CalibrationState state) {
  switch (state) {
    case STATE_POSITION_OFFER: 
      return "Z-axis dominant positive\nHold your hand forward with palm up";
    case STATE_POSITION_CALM:
      return "Z-axis dominant negative\nHold your hand forward with palm down";
    case STATE_POSITION_OATH:
      return "Y-axis dominant negative\nHold your hand pointing upwards";
    case STATE_POSITION_DIG:
      return "Y-axis dominant positive\nHold your hand pointing downwards";
    case STATE_POSITION_SHIELD:
      return "X-axis dominant negative\nHold your hand to the side with palm out";
    case STATE_POSITION_NULL:
      return "X-axis dominant positive\nHold your hand to the side with palm in";
    default:
      return "";
  }
}

uint8_t getPositionForState(CalibrationState state) {
  switch (state) {
    case STATE_POSITION_OFFER: return POS_OFFER;
    case STATE_POSITION_CALM: return POS_CALM;
    case STATE_POSITION_OATH: return POS_OATH;
    case STATE_POSITION_DIG: return POS_DIG;
    case STATE_POSITION_SHIELD: return POS_SHIELD;
    case STATE_POSITION_NULL: return POS_NULLPOS;
    default: return POS_UNKNOWN;
  }
} 