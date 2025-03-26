/**
 * @file SimplePositionTest.cpp
 * @brief Basic test to verify position detection using preprocessor guards
 */

#include <Arduino.h>
#include "../../src/hardware/HardwareManager.h"
#include "../../src/detection/PositionDetector.h"
#include "../../src/core/Config.h"

// Serial communication
#define SERIAL_BAUD_RATE 115200

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
PositionDetector positionDetector;
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 100;  // ms

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  
  Serial.println("\n\n=== Simple Position Detection Test ===");
  Serial.println("Testing position detection with updated thresholds");
  
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
  Serial.println("Raw data and detected positions will be displayed here.");
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
    
    // Get sensor data
    const SensorData& sensorData = hardware->getSensorData();
    
    // Detect position
    PositionReading positionReading = positionDetector.detectPosition(sensorData);
    
    // Print raw data and detected position
    Serial.print("Accel: X=");
    Serial.print(sensorData.accelX);
    Serial.print(", Y=");
    Serial.print(sensorData.accelY);
    Serial.print(", Z=");
    Serial.print(sensorData.accelZ);
    Serial.print(" | Position: ");
    
    if (positionReading.position <= POS_UNKNOWN) {
      Serial.print(positionNames[positionReading.position]);
      
      // Set LEDs to show detected position
      Color positionColor;
      switch (positionReading.position) {
        case POS_OFFER:
          positionColor = {Config::Colors::OFFER_COLOR[0], Config::Colors::OFFER_COLOR[1], Config::Colors::OFFER_COLOR[2]};
          break;
        case POS_CALM:
          positionColor = {Config::Colors::CALM_COLOR[0], Config::Colors::CALM_COLOR[1], Config::Colors::CALM_COLOR[2]};
          break;
        case POS_OATH:
          positionColor = {Config::Colors::OATH_COLOR[0], Config::Colors::OATH_COLOR[1], Config::Colors::OATH_COLOR[2]};
          break;
        case POS_DIG:
          positionColor = {Config::Colors::DIG_COLOR[0], Config::Colors::DIG_COLOR[1], Config::Colors::DIG_COLOR[2]};
          break;
        case POS_SHIELD:
          positionColor = {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]};
          break;
        case POS_NULL:
          positionColor = {Config::Colors::NULL_COLOR[0], Config::Colors::NULL_COLOR[1], Config::Colors::NULL_COLOR[2]};
          break;
        default:
          positionColor = {Config::Colors::UNKNOWN_COLOR[0], Config::Colors::UNKNOWN_COLOR[1], Config::Colors::UNKNOWN_COLOR[2]};
          break;
      }
      
      hardware->setAllLEDs(positionColor);
      hardware->updateLEDs();
    } else {
      Serial.print("INVALID");
      hardware->setAllLEDs({255, 0, 0});  // Red for invalid
      hardware->updateLEDs();
    }
    
    Serial.println();
  }
} 