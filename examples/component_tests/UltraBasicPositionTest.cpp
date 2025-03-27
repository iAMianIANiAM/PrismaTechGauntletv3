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
  "OFFER (Purple)",  // POS_OFFER
  "CALM (Yellow)",   // POS_CALM
  "OATH (Red)",      // POS_OATH
  "DIG (Green)",     // POS_DIG
  "SHIELD (Blue)",   // POS_SHIELD
  "NULL (Orange)",   // POS_NULL
  "UNKNOWN (White)"  // POS_UNKNOWN
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
    positionDetector.calibratePosition(pos, 50);
    
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
  
  Serial.println("\nCalibration complete! All positions calibrated.");
  Serial.println("The device will now operate using your calibrated values.");
  Serial.println("Move between positions to see detection results.");
  
  // Final success indication
  for (int i = 0; i < 5; i++) {
    hardware->setAllLEDs({0, 255, 0});
    hardware->updateLEDs();
    delay(200);
    hardware->setAllLEDs({0, 0, 0});
    hardware->updateLEDs();
    delay(200);
  }
  
  delay(1000);
}

void displaySensorData(const ProcessedData& data, uint8_t position) {
  Serial.print("Accel: X=");
  Serial.print(data.accelX, 2);
  Serial.print(" Y=");
  Serial.print(data.accelY, 2);
  Serial.print(" Z=");
  Serial.print(data.accelZ, 2);
  Serial.print(" | Position: ");
  
  if (position <= POS_UNKNOWN) {
    Serial.println(positionNames[position]);
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