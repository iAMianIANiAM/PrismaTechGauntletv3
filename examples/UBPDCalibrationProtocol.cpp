#include <Arduino.h>

// Include project headers
#include "../src/hardware/HardwareManager.h"
#include "../src/hardware/LEDInterface.h"
#include "../src/hardware/MPU9250Interface.h"
#include "../src/detection/UltraBasicPositionDetector.h"
#include "../src/core/SystemTypes.h"
#include "../src/core/Config.h"

// Calibration state machine
enum CalibrationState {
  STATE_STANDBY,        // Waiting for user to begin calibration
  STATE_INSTRUCTIONS,   // Display instructions to user
  STATE_WARMUP,         // Allow IMU to stabilize
  STATE_POSITION_OFFER, // Calibrating Offer position
  STATE_POSITION_CALM,  // Calibrating Calm position 
  STATE_POSITION_OATH,  // Calibrating Oath position
  STATE_POSITION_DIG,   // Calibrating Dig position
  STATE_POSITION_SHIELD,// Calibrating Shield position
  STATE_POSITION_NULL,  // Calibrating Null position
  STATE_ANALYSIS,       // Calculate threshold values
  STATE_RESULTS,        // Display and output results
  STATE_DETECTION       // Test the calibrated values
};

// Timing constants (in milliseconds)
const unsigned long WARMUP_DURATION = 15000;        // 15 seconds warmup
const unsigned long POSITION_DURATION = 15000;      // 15 seconds per position
const unsigned long TRANSITION_DURATION = 5000;     // 5 seconds transition
const unsigned long INSTRUCTION_DURATION = 5000;    // 5 seconds for instructions
const unsigned long LED_UPDATE_INTERVAL = 50;       // Update LEDs every 50ms (20Hz)
const unsigned long SENSOR_SAMPLE_INTERVAL = 100;   // Sample sensor every 100ms (10Hz)
const unsigned long STATUS_UPDATE_INTERVAL = 2000;  // Status update every 2 seconds

// Get hardware manager instance
HardwareManager* hardware = HardwareManager::getInstance();
UltraBasicPositionDetector* detector = new UltraBasicPositionDetector();

// Current calibration state
CalibrationState currentState = STATE_STANDBY;
unsigned long stateStartTime = 0;
unsigned long lastLEDUpdateTime = 0;
unsigned long lastSensorSampleTime = 0;
unsigned long lastStatusUpdateTime = 0;

// LED animation variables
uint8_t pulseValue = 0;
bool pulseIncreasing = true;
uint8_t flashCounter = 0;

// Data collection variables
struct PositionSampleData {
  float accumX = 0.0f;
  float accumY = 0.0f;
  float accumZ = 0.0f;
  float varX = 0.0f;
  float varY = 0.0f;
  float varZ = 0.0f;
  int validSamples = 0;
};

PositionSampleData positionData[6]; // One for each position

// Function prototypes
void updateLEDs();
void collectSensorData();
void transitionToNextState();
const char* getStateName(CalibrationState state);
const char* getPositionName(uint8_t position);
uint8_t getPositionForState(CalibrationState state);
void printStatusUpdate();
void printInstructions();
void calculateThresholds();
void processSerialCommands();
Color getPositionColor(uint8_t position);
void enterDetectionMode();

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);  // Allow time for serial connection to establish
  
  Serial.println("\n==================================");
  Serial.println("PrismaTech Gauntlet 3.0 - UBPD Calibration Protocol");
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
  
  // Initialize position detector
  Serial.println("Initializing UBPD...");
  if (detector->init(hardware)) {
    Serial.println("UBPD initialized successfully");
  } else {
    Serial.println("Failed to initialize UBPD");
    while (true) {
      delay(1000);
    }
  }
  
  // Set initial state and time
  stateStartTime = millis();
  lastLEDUpdateTime = millis();
  lastSensorSampleTime = millis();
  lastStatusUpdateTime = millis();
  
  // Print welcome message
  Serial.println("\n==================================");
  Serial.println("UBPD Calibration Protocol Ready");
  Serial.println("==================================");
  Serial.println("Enter command to begin:");
  Serial.println("  c - Start Calibration Routine");
  Serial.println("  d - Enter Detection Mode");
  Serial.println("==================================\n");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Update hardware (read sensors)
  hardware->update();
  
  // Process any serial commands
  processSerialCommands();
  
  // Non-blocking LED updates
  if (currentMillis - lastLEDUpdateTime >= LED_UPDATE_INTERVAL) {
    lastLEDUpdateTime = currentMillis;
    updateLEDs();
  }
  
  // Non-blocking sensor sampling (during calibration)
  if (currentState >= STATE_POSITION_OFFER && 
      currentState <= STATE_POSITION_NULL && 
      currentMillis - lastSensorSampleTime >= SENSOR_SAMPLE_INTERVAL) {
    lastSensorSampleTime = currentMillis;
    collectSensorData();
  }
  
  // Periodic status updates
  if (currentMillis - lastStatusUpdateTime >= STATUS_UPDATE_INTERVAL) {
    lastStatusUpdateTime = currentMillis;
    printStatusUpdate();
  }
  
  // Check for state transitions based on timing
  unsigned long elapsedTime = currentMillis - stateStartTime;
  
  switch (currentState) {
    case STATE_STANDBY:
      // Stay in standby until user command
      break;
      
    case STATE_INSTRUCTIONS:
      if (elapsedTime >= INSTRUCTION_DURATION) {
        transitionToNextState();
      }
      break;
      
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
      
    case STATE_ANALYSIS:
      // Calculate thresholds and move to results
      calculateThresholds();
      transitionToNextState();
      break;
      
    case STATE_RESULTS:
      // Stay in results until user command
      break;
      
    case STATE_DETECTION:
      // Stay in detection mode until user command
      // Update position detection
      detector->update();
      break;
  }
}

void updateLEDs() {
  // Different animations based on state
  switch (currentState) {
    case STATE_STANDBY: {
      // Gentle white pulsing
      if (pulseIncreasing) {
        pulseValue = min(80, pulseValue + 2);
        if (pulseValue >= 80) pulseIncreasing = false;
      } else {
        pulseValue = max(20, pulseValue - 2);
        if (pulseValue <= 20) pulseIncreasing = true;
      }
      
      hardware->setBrightness(pulseValue);
      Color white = {255, 255, 255};
      hardware->setAllLEDs(white);
      break;
    }
    
    case STATE_INSTRUCTIONS: {
      // Sequence through position colors
      unsigned long elapsed = millis() - stateStartTime;
      uint8_t colorIndex = (elapsed / 500) % 6; // Change color every 500ms
      
      Color posColor = getPositionColor(colorIndex);
      hardware->setBrightness(150);
      hardware->setAllLEDs(posColor);
      break;
    }
    
    case STATE_WARMUP: {
      unsigned long elapsed = millis() - stateStartTime;
      
      if (elapsed < WARMUP_DURATION - 5000) {
        // Accelerating white pulse
        uint8_t cycleTime = map(elapsed, 0, WARMUP_DURATION - 5000, 50, 10);
        uint8_t cyclePos = (elapsed / cycleTime) % 100;
        
        uint8_t brightness = cyclePos < 50 ? map(cyclePos, 0, 50, 20, 100) : map(cyclePos, 50, 100, 100, 20);
        hardware->setBrightness(brightness);
        Color white = {255, 255, 255};
        hardware->setAllLEDs(white);
      } else {
        // Final countdown - flashing white
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
      // Solid color for the current position
      uint8_t position = getPositionForState(currentState);
      Color posColor = getPositionColor(position);
      
      hardware->setBrightness(150);
      hardware->setAllLEDs(posColor);
      break;
    }
    
    case STATE_ANALYSIS: {
      // Rotating white dot
      static uint8_t dotPosition = 0;
      dotPosition = (dotPosition + 1) % 12;
      
      hardware->setBrightness(150);
      for (uint8_t i = 0; i < 12; i++) {
        if (i == dotPosition) {
          Color white = {255, 255, 255};
          hardware->setLED(i, white);
        } else {
          Color off = {0, 0, 0};
          hardware->setLED(i, off);
        }
      }
      break;
    }
    
    case STATE_RESULTS: {
      // Green flashing on success
      flashCounter = (flashCounter + 1) % 20;
      if (flashCounter < 10) {
        Color green = {0, 255, 0};
        hardware->setBrightness(150);
        hardware->setAllLEDs(green);
      } else {
        Color off = {0, 0, 0};
        hardware->setAllLEDs(off);
      }
      break;
    }
    
    case STATE_DETECTION: {
      // Get current position and confidence from detector
      PositionReading position = detector->getCurrentPosition();
      
      // Update LEDs based on position
      Color posColor = getPositionColor(position.position);
      hardware->setBrightness(map(position.confidence, 0, 100, 50, 200));
      hardware->setAllLEDs(posColor);
      break;
    }
  }
  
  // Show the LEDs
  hardware->updateLEDs();
}

void collectSensorData() {
  // Get the current position we're calibrating
  uint8_t currentPosition = getPositionForState(currentState);
  PositionSampleData& data = positionData[currentPosition];
  
  // Get raw sensor data
  SensorData rawData = hardware->getSensorData();
  
  // Create a temporary variable for processed data
  ProcessedData processedData;
  
  // Explicitly process the raw data instead of using stored values
  detector->processRawData(rawData, processedData);
  
  // Skip obviously invalid readings
  if (abs(rawData.accelX) > 30000 || abs(rawData.accelY) > 30000 || abs(rawData.accelZ) > 30000) {
    Serial.print("Skipping invalid sample - raw values too extreme: ");
    Serial.print(rawData.accelX); Serial.print(", ");
    Serial.print(rawData.accelY); Serial.print(", ");
    Serial.println(rawData.accelZ);
    return;
  }
  
  // Accumulate values for averaging
  data.accumX += processedData.accelX;
  data.accumY += processedData.accelY;
  data.accumZ += processedData.accelZ;
  data.validSamples++;
  
  // Show raw and scaled values periodically
  if (data.validSamples % 5 == 0) {
    float scalingFactor = detector->getScalingFactor();
    
    Serial.print("Sample ");
    Serial.print(data.validSamples);
    Serial.print(" | Raw: X=");
    Serial.print(rawData.accelX);
    Serial.print(" Y=");
    Serial.print(rawData.accelY);
    Serial.print(" Z=");
    Serial.print(rawData.accelZ);
    
    Serial.print(" | Scaled (");
    Serial.print(scalingFactor, 8);
    Serial.print("): X=");
    Serial.print(processedData.accelX, 2);
    Serial.print(" Y=");
    Serial.print(processedData.accelY, 2);
    Serial.print(" Z=");
    Serial.print(processedData.accelZ, 2);
    Serial.println(" m/s²");
  }
}

void transitionToNextState() {
  // Determine the next state based on current state
  CalibrationState nextState;
  
  switch (currentState) {
    case STATE_STANDBY:
      nextState = STATE_INSTRUCTIONS;
      break;
      
    case STATE_INSTRUCTIONS:
      nextState = STATE_WARMUP;
      break;
      
    case STATE_WARMUP:
      nextState = STATE_POSITION_OFFER;
      break;
      
    case STATE_POSITION_OFFER:
      nextState = STATE_POSITION_CALM;
      break;
      
    case STATE_POSITION_CALM:
      nextState = STATE_POSITION_OATH;
      break;
      
    case STATE_POSITION_OATH:
      nextState = STATE_POSITION_DIG;
      break;
      
    case STATE_POSITION_DIG:
      nextState = STATE_POSITION_SHIELD;
      break;
      
    case STATE_POSITION_SHIELD:
      nextState = STATE_POSITION_NULL;
      break;
      
    case STATE_POSITION_NULL:
      nextState = STATE_ANALYSIS;
      break;
      
    case STATE_ANALYSIS:
      nextState = STATE_RESULTS;
      break;
      
    case STATE_RESULTS:
      // Stay in results until user command
      return;
      
    case STATE_DETECTION:
      // Stay in detection until user command
      return;
      
    default:
      nextState = STATE_STANDBY;
      break;
  }
  
  // Update the state
  currentState = nextState;
  stateStartTime = millis();
  
  // Print status message for the new state
  Serial.print("\n==== Entering ");
  Serial.print(getStateName(currentState));
  Serial.println(" ====");
  
  // For position states, print instructions
  if (currentState >= STATE_POSITION_OFFER && currentState <= STATE_POSITION_NULL) {
    uint8_t position = getPositionForState(currentState);
    Serial.print("\nHold your hand in the ");
    Serial.print(getPositionName(position));
    Serial.print(" position (");
    
    // Print position description based on TrueFunctionGuide
    switch (position) {
      case POS_OFFER:
        Serial.println("Hand forward, Palm Up - Z-axis dominant positive)");
        break;
      case POS_CALM:
        Serial.println("Hand forward, Palm Down - Z-axis dominant negative)");
        break;
      case POS_OATH:
        Serial.println("Hand pointing upwards - Y-axis dominant negative)");
        break;
      case POS_DIG:
        Serial.println("Hand pointing downwards - Y-axis dominant positive)");
        break;
      case POS_SHIELD:
        Serial.println("Hand to the side, Palm Out - X-axis dominant negative)");
        break;
      case POS_NULLPOS:
        Serial.println("Hand to the side, Palm In - X-axis dominant positive)");
        break;
    }
    
    Serial.print("Collecting data for 15 seconds. Hold position steady.\n");
  }
}

const char* getStateName(CalibrationState state) {
  switch (state) {
    case STATE_STANDBY: return "Standby Mode";
    case STATE_INSTRUCTIONS: return "Instructions Mode";
    case STATE_WARMUP: return "Warmup Mode";
    case STATE_POSITION_OFFER: return "Offer Position Calibration";
    case STATE_POSITION_CALM: return "Calm Position Calibration";
    case STATE_POSITION_OATH: return "Oath Position Calibration";
    case STATE_POSITION_DIG: return "Dig Position Calibration";
    case STATE_POSITION_SHIELD: return "Shield Position Calibration";
    case STATE_POSITION_NULL: return "Null Position Calibration";
    case STATE_ANALYSIS: return "Analysis Mode";
    case STATE_RESULTS: return "Results Mode";
    case STATE_DETECTION: return "Detection Mode";
    default: return "Unknown State";
  }
}

const char* getPositionName(uint8_t position) {
  switch (position) {
    case POS_OFFER: return "OFFER (Palm Up)";      // Purple (Z-axis positive)
    case POS_CALM: return "CALM (Palm Down)";      // Yellow (Z-axis negative) 
    case POS_OATH: return "OATH (Hand Up)";        // Red (Y-axis negative)
    case POS_DIG: return "DIG (Hand Down)";        // Green (Y-axis positive)
    case POS_SHIELD: return "SHIELD (Palm Out)";   // Blue (X-axis negative)
    case POS_NULLPOS: return "NULL (Palm In)";     // Orange (X-axis positive)
    default: return "UNKNOWN";
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

void printStatusUpdate() {
  switch (currentState) {
    case STATE_STANDBY:
      Serial.println("Ready for calibration. Enter 'c' to begin.");
      break;
      
    case STATE_INSTRUCTIONS:
      // No periodic updates during instructions
      break;
      
    case STATE_WARMUP:
      // Countdown to first position
      {
        unsigned long remaining = WARMUP_DURATION - (millis() - stateStartTime);
        Serial.print("Warming up IMU... ");
        Serial.print(remaining / 1000);
        Serial.println(" seconds remaining");
      }
      break;
      
    case STATE_POSITION_OFFER:
    case STATE_POSITION_CALM:
    case STATE_POSITION_OATH:
    case STATE_POSITION_DIG:
    case STATE_POSITION_SHIELD:
    case STATE_POSITION_NULL:
      // Show progress for position calibration
      {
        uint8_t position = getPositionForState(currentState);
        unsigned long elapsed = millis() - stateStartTime;
        unsigned long remaining = POSITION_DURATION - elapsed;
        
        Serial.print("Calibrating ");
        Serial.print(getPositionName(position));
        Serial.print(" position... ");
        Serial.print(remaining / 1000);
        Serial.print(" seconds remaining (");
        Serial.print(positionData[position].validSamples);
        Serial.println(" samples collected)");
      }
      break;
      
    case STATE_ANALYSIS:
      Serial.println("Analyzing calibration data...");
      break;
      
    case STATE_RESULTS:
      // No periodic updates during results
      break;
      
    case STATE_DETECTION: {
      // Get raw sensor data
      SensorData rawData = hardware->getSensorData();
      
      // Process the raw data directly
      ProcessedData processed;
      detector->processRawData(rawData, processed);
      
      // Get current position and confidence
      PositionReading position = detector->getCurrentPosition();
      
      // Get color name based on the position
      const char* colorName = "";
      switch (position.position) {
        case POS_OFFER: colorName = "Purple"; break;   // OFFER_COLOR
        case POS_CALM: colorName = "Yellow"; break;    // CALM_COLOR
        case POS_OATH: colorName = "Red"; break;       // OATH_COLOR  
        case POS_DIG: colorName = "Green"; break;      // DIG_COLOR
        case POS_SHIELD: colorName = "Blue"; break;    // SHIELD_COLOR
        case POS_NULLPOS: colorName = "Orange"; break; // NULL_COLOR
        default: colorName = "White"; break;           // UNKNOWN_COLOR
      }
      
      // Format a nice display with columns - include color in display
      Serial.print(getPositionName(position.position));
      Serial.print(" (");
      Serial.print(colorName);
      Serial.print(")");
      Serial.print(" | ");
      
      // Print confidence
      Serial.print(position.confidence, 1);
      Serial.print("%      | ");
      
      // Print raw values
      Serial.print("X:");
      Serial.print(rawData.accelX);
      Serial.print(" Y:");
      Serial.print(rawData.accelY);
      Serial.print(" Z:");
      Serial.print(rawData.accelZ);
      Serial.print(" | ");
      
      // Print processed values
      Serial.print("X:");
      Serial.print(processed.accelX, 2);
      Serial.print(" Y:");
      Serial.print(processed.accelY, 2);
      Serial.print(" Z:");
      Serial.print(processed.accelZ, 2);
      Serial.println(" m/s²");
      
      break;
    }
  }
}

void printInstructions() {
  Serial.println("\n==================================");
  Serial.println("Calibration Routine Instructions:");
  Serial.println("==================================");
  Serial.println("1. You will be guided through calibrating each hand position");
  Serial.println("2. For each position, hold your hand steady for 15 seconds");
  Serial.println("3. The LED will show the color of the position being calibrated");
  Serial.println("4. Follow the serial monitor instructions for each position");
  Serial.println("5. After all positions are calibrated, thresholds will be calculated");
  Serial.println("6. You can then test the calibration in Detection Mode");
  Serial.println("==================================\n");
  
  Serial.println("Positions to calibrate:");
  Serial.println("1. OFFER (Purple): Hand forward, Palm Up");
  Serial.println("2. CALM (Yellow): Hand forward, Palm Down");
  Serial.println("3. OATH (Red): Hand pointing upwards");
  Serial.println("4. DIG (Green): Hand pointing downwards");
  Serial.println("5. SHIELD (Blue): Hand to the side, Palm Out");
  Serial.println("6. NULL (Orange): Hand to the side, Palm In");
  Serial.println("\nPreparing to begin calibration...");
}

void calculateThresholds() {
  Serial.println("\n==================================");
  Serial.println("Calculating Thresholds");
  Serial.println("==================================");
  
  // Calculate thresholds for each position
  for (uint8_t position = 0; position < 6; position++) {
    // Skip UNKNOWN position
    if (position == POS_UNKNOWN) {
      continue;
    }
    
    // Only process positions with valid samples
    if (positionData[position].validSamples > 0) {
      // Calculate means
      float meanX = positionData[position].accumX / positionData[position].validSamples;
      float meanY = positionData[position].accumY / positionData[position].validSamples;
      float meanZ = positionData[position].accumZ / positionData[position].validSamples;
      
      // Calculate standard deviations
      float stdDevX = sqrt(positionData[position].varX / positionData[position].validSamples);
      float stdDevY = sqrt(positionData[position].varY / positionData[position].validSamples);
      float stdDevZ = sqrt(positionData[position].varZ / positionData[position].validSamples);
      
      Serial.print("Position: ");
      Serial.println(getPositionName(position));
      Serial.print("  Mean (m/s²): X=");
      Serial.print(meanX);
      Serial.print(", Y=");
      Serial.print(meanY);
      Serial.print(", Z=");
      Serial.println(meanZ);
      
      Serial.print("  StdDev (m/s²): X=");
      Serial.print(stdDevX);
      Serial.print(", Y=");
      Serial.print(stdDevY);
      Serial.print(", Z=");
      Serial.println(stdDevZ);
      
      // Determine dominant axis based on highest absolute mean value
      uint8_t dominantAxis = 0; // X-axis
      float dominantValue = abs(meanX);
      
      if (abs(meanY) > dominantValue) {
        dominantAxis = 1; // Y-axis
        dominantValue = abs(meanY);
      }
      
      if (abs(meanZ) > dominantValue) {
        dominantAxis = 2; // Z-axis
        dominantValue = abs(meanZ);
      }
      
      // Set threshold based on dominant axis mean with safety margin
      float threshold = dominantValue * 0.85; // 85% of measured value
      
      // Preserve the sign of the threshold
      if ((dominantAxis == 0 && meanX < 0) ||
          (dominantAxis == 1 && meanY < 0) ||
          (dominantAxis == 2 && meanZ < 0)) {
        threshold = -threshold;
      }
      
      // Store the threshold and dominant axis in the detector
      detector->setThreshold(position, threshold);
      detector->setDominantAxis(position, dominantAxis);
      
      Serial.print("  Dominant Axis: ");
      Serial.print(dominantAxis);
      Serial.print(" (");
      switch (dominantAxis) {
        case 0: Serial.print("X"); break;
        case 1: Serial.print("Y"); break;
        case 2: Serial.print("Z"); break;
      }
      Serial.println(")");
      
      Serial.print("  Calculated Threshold: ");
      Serial.print(threshold);
      Serial.println(" m/s²");
    }
  }
  
  Serial.println("\n==================================");
  Serial.println("Calibration Results");
  Serial.println("==================================");
  
  // Print human-readable format
  detector->printCalibrationData();
  
  // Print Config.h format
  detector->printConfigFormat();
  
  Serial.println("\n==================================");
  Serial.println("Calibration Complete!");
  Serial.println("Enter 'd' to test detection or 'c' to recalibrate");
  Serial.println("==================================\n");
}

void processSerialCommands() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    // Process command based on current state
    switch (cmd) {
      case 'c':
      case 'C':
        // Start calibration
        if (currentState == STATE_STANDBY || currentState == STATE_RESULTS || currentState == STATE_DETECTION) {
          // Reset data collection
          for (uint8_t i = 0; i < 6; i++) {
            positionData[i].accumX = 0.0f;
            positionData[i].accumY = 0.0f;
            positionData[i].accumZ = 0.0f;
            positionData[i].varX = 0.0f;
            positionData[i].varY = 0.0f;
            positionData[i].varZ = 0.0f;
            positionData[i].validSamples = 0;
          }
          
          // Start instructions
          currentState = STATE_INSTRUCTIONS;
          stateStartTime = millis();
          printInstructions();
        }
        break;
        
      case 'd':
      case 'D':
        // Enter detection mode
        if (currentState == STATE_STANDBY || currentState == STATE_RESULTS || currentState == STATE_DETECTION) {
          enterDetectionMode();
        }
        break;
        
      case 's':
      case 'S':
        // Return to standby from any state
        currentState = STATE_STANDBY;
        stateStartTime = millis();
        Serial.println("\n==================================");
        Serial.println("Returned to Standby Mode");
        Serial.println("Enter 'c' for Calibration or 'd' for Detection");
        Serial.println("==================================");
        break;
    }
    
    // Consume any remaining characters
    while (Serial.available()) {
      Serial.read();
    }
  }
}

Color getPositionColor(uint8_t position) {
  Color result = {255, 255, 255}; // Default to white
  
  switch (position) {
    case POS_OFFER:
      result = {Config::Colors::OFFER_COLOR[0], Config::Colors::OFFER_COLOR[1], Config::Colors::OFFER_COLOR[2]};
      break;
    case POS_CALM:
      result = {Config::Colors::CALM_COLOR[0], Config::Colors::CALM_COLOR[1], Config::Colors::CALM_COLOR[2]};
      break;
    case POS_OATH:
      result = {Config::Colors::OATH_COLOR[0], Config::Colors::OATH_COLOR[1], Config::Colors::OATH_COLOR[2]};
      break;
    case POS_DIG:
      result = {Config::Colors::DIG_COLOR[0], Config::Colors::DIG_COLOR[1], Config::Colors::DIG_COLOR[2]};
      break;
    case POS_SHIELD:
      result = {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]};
      break;
    case POS_NULLPOS:
      result = {Config::Colors::NULL_COLOR[0], Config::Colors::NULL_COLOR[1], Config::Colors::NULL_COLOR[2]};
      break;
  }
  
  return result;
}

void enterDetectionMode() {
  Serial.println("\n==================================");
  Serial.println("Entering Detection Mode");
  Serial.println("==================================");
  Serial.println("Using ECHO reference scaling factor");
  Serial.println("Try holding each position to test detection");
  Serial.println("Press 'q' to return to standby");
  
  // First, validate the scaling factor to ensure gravity reading is accurate
  Serial.println("\nValidating scaling factor against gravity...");
  Serial.println("Place device flat on table and keep still");
  
  // Give user time to place device flat
  delay(3000);
  
  // Collect samples and verify expected ~9.81 m/s² on Z-axis when flat
  float sumZ = 0.0f;
  int numSamples = 20;
  
  for (int i = 0; i < numSamples; i++) {
    // Get fresh sensor data
    hardware->update();
    SensorData rawData = hardware->getSensorData();
    
    // Process with current scaling factor
    ProcessedData processed;
    detector->processRawData(rawData, processed);
    
    // Accumulate Z readings (should be ~9.81 m/s² if scaling is correct)
    sumZ += processed.accelZ;
    
    // Show progress
    Serial.print(".");
    delay(100);
  }
  
  // Calculate average Z reading
  float avgZ = sumZ / numSamples;
  
  // Calculate error percentage against expected gravity value
  float expectedGravity = 9.81f;
  float errorPct = abs((avgZ - expectedGravity) / expectedGravity) * 100.0f;
  
  Serial.println();
  Serial.print("Average Z reading: ");
  Serial.print(avgZ, 2);
  Serial.print(" m/s² (Expected: ");
  Serial.print(expectedGravity, 2);
  Serial.println(" m/s²)");
  
  Serial.print("Error percentage: ");
  Serial.print(errorPct, 1);
  Serial.println("%");
  
  if (errorPct > 10.0f) {
    Serial.println("WARNING: Gravity reading error exceeds 10%. Scaling factor may need adjustment.");
    Serial.print("Current scaling factor: ");
    Serial.println(detector->getScalingFactor(), 10);
  } else {
    Serial.println("Scaling factor validation passed!");
  }
  
  // Transition to detection state
  currentState = STATE_DETECTION;
  stateStartTime = millis();
  
  // Change LED color to white
  hardware->setBrightness(100);
  Color white = {255, 255, 255};
  hardware->setAllLEDs(white);
  
  // Display column headers for detection output
  Serial.println("\nPosition    | Confidence | Raw Accel | Processed (m/s²)");
  Serial.println("------------|------------|-----------|-------------------");
} 