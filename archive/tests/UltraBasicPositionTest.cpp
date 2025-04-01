/**
 * @file UltraBasicPositionTest.cpp
 * @brief Test application for the Ultra Basic Position Detection system
 */

#include <Arduino.h>
#include "../../src/hardware/HardwareManager.h"
#include "../../src/detection/UltraBasicPositionDetector.h"
#include "../../src/core/Config.h"

// Serial communication
#define SERIAL_BAUD_RATE 115200

// Pin definitions
#define BUTTON_PIN 0  // Define button pin (GPIO0 as default)

// Update interval
#define UPDATE_INTERVAL 100  // ms

// Position names for debugging
const char* positionNames[] = {
  "Offer (Purple)",  // POS_OFFER
  "Calm (Blue)",     // POS_CALM
  "Oath (Yellow)",   // POS_OATH
  "Dig (Green)",     // POS_DIG
  "Shield (Pink)",   // POS_SHIELD
  "Null (Red)",      // POS_NULLPOS
  "Unknown (White)"  // POS_UNKNOWN
};

// Global objects
HardwareManager* hardware = nullptr;
UltraBasicPositionDetector positionDetector;
unsigned long lastUpdateTime = 0;
bool calibrationMode = false;

// Function prototypes
void handleCalibration();
void displaySensorData(const ProcessedData& data, uint8_t position);
Color getPositionColor(uint8_t position);
void outputCalibrationValues();

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  
  Serial.println("\n\n=== Ultra Basic Position Detection Test ===");
  Serial.println("Testing position detection with physical unit thresholds");
  
  // Initialize hardware
  hardware = HardwareManager::getInstance();
  bool hwInitOk = hardware->init();
  Serial.print("Hardware initialization: ");
  Serial.println(hwInitOk ? "SUCCESS" : "FAILED");
  
  if (!hwInitOk) {
    Serial.println("Hardware initialization failed. Cannot proceed.");
    while (1) {
      delay(1000);  // Wait forever
    }
  }
  
  // Initialize position detector
  bool pdInitOk = positionDetector.init(hardware);
  Serial.print("Position detector initialization: ");
  Serial.println(pdInitOk ? "SUCCESS" : "FAILED");
  
  if (!pdInitOk) {
    Serial.println("Position detector initialization failed. Cannot proceed.");
    while (1) {
      delay(1000);  // Wait forever
    }
  }
  
  // Set default brightness
  hardware->setBrightness(Config::DEFAULT_BRIGHTNESS);
  
  // Check if we should enter calibration mode
  Serial.println("\nSend 'c' to enter calibration mode...");
  Serial.println("Send any other character to proceed to normal operation.");
  Serial.println("Waiting 10 seconds for command...");
  
  // Wait for serial command
  unsigned long startTime = millis();
  bool receivedCommand = false;
  
  while (millis() - startTime < 10000) {
    // Flash all LEDs blue/green to indicate waiting
    hardware->setAllLEDs({0, 100, 100});
    hardware->updateLEDs();
    delay(250);
    hardware->setAllLEDs({0, 0, 0});
    hardware->updateLEDs();
    delay(250);
    
    // Check for serial input
    if (Serial.available()) {
      char command = Serial.read();
      
      // Clear any remaining characters in buffer
      while (Serial.available()) {
        Serial.read();
      }
      
      if (command == 'c' || command == 'C') {
        calibrationMode = true;
        Serial.println("\n=== Entering Calibration Mode ===");
        handleCalibration();
        calibrationMode = false;
        
        // Output calibration values in Config.h format
        outputCalibrationValues();
      }
      
      receivedCommand = true;
      break;
    }
  }
  
  if (!receivedCommand) {
    Serial.println("No command received. Proceeding to normal operation.");
  }
  
  // Ready indicator
  for (int i = 0; i < 3; i++) {
    hardware->setAllLEDs({0, 255, 0});
    hardware->updateLEDs();
    delay(200);
    hardware->setAllLEDs({0, 0, 0});
    hardware->updateLEDs();
    delay(200);
  }
  
  Serial.println("\nPosition detection ready. Move the device to test positions.");
  Serial.println("Processed data and detected positions will be displayed here.");
  Serial.println("------------------------------------------------------");
  
  lastUpdateTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Update hardware
  hardware->update();
  
  // Check for serial commands
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 't' || cmd == 'T') {
      // Redisplay threshold values
      Serial.println("\n=== CURRENT THRESHOLD VALUES ===");
      outputCalibrationValues();
    }
  }
  
  // Process at regular intervals
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = currentTime;
    
    // Update position detector
    PositionReading positionReading = positionDetector.update();
    
    // Get processed data for display
    ProcessedData processedData = positionDetector.getProcessedData();
    
    // Display sensor data and position
    displaySensorData(processedData, positionReading.position);
    
    // Update LEDs based on detected position
    Color positionColor = getPositionColor(positionReading.position);
    hardware->setAllLEDs(positionColor);
    hardware->updateLEDs();
  }
}

void handleCalibration() {
  Serial.println("Starting position calibration process...");
  Serial.println("Follow the LED prompts and instructions for each position.");
  Serial.println("Hold each position steady when instructed.\n");
  delay(3000);
  
  // Position names for clearer instructions
  const char* posNames[] = {
    "OFFER (Purple - Palm up, fingers extended)",
    "CALM (Yellow - Palm down, fingers extended)",
    "OATH (Red - Hand vertical, palm facing inward)",
    "DIG (Green - Hand vertical, palm facing outward)",
    "SHIELD (Blue - Arm extended forward, palm facing forward)",
    "NULL (Orange - Arm extended backward, palm facing back)"
  };
  
  // Calibrate each position with better timing
  for (uint8_t pos = 0; pos < 6; pos++) {
    // Announce next position with countdown
    Serial.print("\n=== NEXT POSITION: ");
    Serial.print(posNames[pos]);
    Serial.println(" ===");
    Serial.println("Get ready in:");
    
    // Countdown with distinct color
    Color prepColor = getPositionColor(pos);
    for (int i = 5; i > 0; i--) {
      Serial.print(i);
      Serial.println(" seconds...");
      
      // Pulse LEDs to indicate preparation
      for (int j = 0; j < 5; j++) {
        hardware->setAllLEDs({prepColor.r/2, prepColor.g/2, prepColor.b/2});
        hardware->updateLEDs();
        delay(100);
        hardware->setAllLEDs({0, 0, 0});
        hardware->updateLEDs();
        delay(100);
      }
    }
    
    // Actual calibration with clear visual feedback
    Serial.println("\nHOLD POSITION NOW!");
    Serial.println("Collecting samples...");
    
    // Solid color during sample collection
    hardware->setAllLEDs(prepColor);
    hardware->updateLEDs();
    
    // Perform actual calibration (50 samples)
    float threshold = positionDetector.calibratePosition(pos, 50);
    
    // Display the calibrated threshold
    Serial.print("Calibrated threshold for position ");
    Serial.print(posNames[pos]);
    Serial.print(": ");
    Serial.println(threshold);
    
    // Confirm completion
    Serial.println("Position calibrated successfully!");
    Serial.println("You can relax now while we prepare for the next position.");
    
    // Rest period with relaxing blue pulse
    Serial.println("Rest for a moment...");
    for (int i = 0; i < 10; i++) {
      uint8_t brightness = 50 + 50 * sin(i * 0.6);
      hardware->setAllLEDs({0, 0, brightness});
      hardware->updateLEDs();
      delay(200);
    }
    hardware->setAllLEDs({0, 0, 0});
    hardware->updateLEDs();
  }
  
  Serial.println("\n=== Calibration Complete ===");
  Serial.println("All positions have been calibrated and thresholds are set for this session.");
  Serial.println("These thresholds will be used until power is cycled.");
  Serial.println("Proceeding to normal operation with new thresholds.\n");
  
  // Confirm calibration completion with RGB
  for (int i = 0; i < 2; i++) {
    hardware->setAllLEDs({255, 0, 0});
    hardware->updateLEDs();
    delay(200);
    hardware->setAllLEDs({0, 255, 0});
    hardware->updateLEDs();
    delay(200);
    hardware->setAllLEDs({0, 0, 255});
    hardware->updateLEDs();
    delay(200);
    hardware->setAllLEDs({0, 0, 0});
    hardware->updateLEDs();
    delay(200);
  }
}

void outputCalibrationValues() {
  Serial.println("\n// ======= CALIBRATED THRESHOLD VALUES =======");
  Serial.println("// Copy these values directly into Config.h");
  Serial.print("// Generated on ");
  // We'd normally print date/time here, but not available on ESP32 without RTC
  Serial.println("device boot");
  Serial.println("");
  Serial.println("// UBPD Default position thresholds in m/s² (for UltraBasicPositionDetector)");
  Serial.println("// Update these values after calibration with values from serial output");
  Serial.println("constexpr float DEFAULT_POSITION_THRESHOLDS[6] = {");
  
  // Get thresholds from the detector
  for (int i = 0; i < 6; i++) {
    Serial.print("  ");
    Serial.print(positionDetector.getThreshold(i), 2);
    Serial.print("f,  // ");
    Serial.print(positionNames[i]);
    Serial.println(i < 5 ? "," : "");
  }
  
  Serial.println("};");
  Serial.println("// ===========================================");
  Serial.println("\nIMPORTANT: Copy these values to src/core/Config.h to make them permanent!");
  Serial.println("After updating Config.h, rebuild and upload the application.");
}

void displaySensorData(const ProcessedData& data, uint8_t position) {
  Serial.print("Accel: X=");
  Serial.print(data.accelX, 2);
  Serial.print(" Y=");
  Serial.print(data.accelY, 2);
  Serial.print(" Z=");
  Serial.print(data.accelZ, 2);
  
  Serial.print(" | Position: ");
  if (position < 7) {
    Serial.print(positionNames[position]);
    Serial.print(" (");
    if (position < 6) {
      Serial.print(positionDetector.getThreshold(position), 2);
    }
    Serial.println(")");
  } else {
    Serial.println("INVALID");
  }
}

Color getPositionColor(uint8_t position) {
  switch (position) {
    case POS_OFFER:
      return {Config::Colors::OFFER_COLOR[0], Config::Colors::OFFER_COLOR[1], Config::Colors::OFFER_COLOR[2]};
    case POS_CALM:
      return {Config::Colors::CALM_COLOR[0], Config::Colors::CALM_COLOR[1], Config::Colors::CALM_COLOR[2]};
    case POS_OATH:
      return {Config::Colors::OATH_COLOR[0], Config::Colors::OATH_COLOR[1], Config::Colors::OATH_COLOR[2]};
    case POS_DIG:
      return {Config::Colors::DIG_COLOR[0], Config::Colors::DIG_COLOR[1], Config::Colors::DIG_COLOR[2]};
    case POS_SHIELD:
      return {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]};
    case POS_NULL:
      return {Config::Colors::NULL_COLOR[0], Config::Colors::NULL_COLOR[1], Config::Colors::NULL_COLOR[2]};
    default:
      return {Config::Colors::UNKNOWN_COLOR[0], Config::Colors::UNKNOWN_COLOR[1], Config::Colors::UNKNOWN_COLOR[2]};
  }
} 