/**
 * @file PositionDetectorTest.cpp
 * @brief Test application for the Position Detector implementation
 */

#include <Arduino.h>
#include "../detection/PositionDetector.h"
#include "../hardware/HardwareManager.h"
#include "../core/SystemTypes.h"
#include "../core/Config.h"

// Update interval in milliseconds
#define UPDATE_INTERVAL 50
#define SERIAL_BAUD_RATE 115200

// Position names for debug output
const char* positionNames[] = {
  "OFFER (Purple)",  // POS_OFFER
  "CALM (Yellow)",   // POS_CALM
  "OATH (Red)",      // POS_OATH
  "DIG (Green)",     // POS_DIG
  "SHIELD (Blue)",   // POS_SHIELD
  "NULL (Orange)",   // POS_NULL
  "UNKNOWN (White)"  // POS_UNKNOWN
};

// Position colors from Config namespace
const Color positionColors[] = {
  {Config::Colors::OFFER_COLOR[0], Config::Colors::OFFER_COLOR[1], Config::Colors::OFFER_COLOR[2]},       // POS_OFFER (Purple)
  {Config::Colors::CALM_COLOR[0], Config::Colors::CALM_COLOR[1], Config::Colors::CALM_COLOR[2]},          // POS_CALM (Yellow)
  {Config::Colors::OATH_COLOR[0], Config::Colors::OATH_COLOR[1], Config::Colors::OATH_COLOR[2]},          // POS_OATH (Red)
  {Config::Colors::DIG_COLOR[0], Config::Colors::DIG_COLOR[1], Config::Colors::DIG_COLOR[2]},             // POS_DIG (Green)
  {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]},    // POS_SHIELD (Blue)
  {Config::Colors::NULL_COLOR[0], Config::Colors::NULL_COLOR[1], Config::Colors::NULL_COLOR[2]},          // POS_NULL (Orange)
  {Config::Colors::UNKNOWN_COLOR[0], Config::Colors::UNKNOWN_COLOR[1], Config::Colors::UNKNOWN_COLOR[2]}  // POS_UNKNOWN (White)
};

// Global variables
HardwareManager* hardware = nullptr;
PositionDetector positionDetector;
PositionReading lastPosition = {POS_UNKNOWN, 0, 0};
unsigned long lastUpdateTime = 0;
unsigned long positionStartTime = 0;
unsigned long positionDuration = 0;

/**
 * @brief Setup function
 */
void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(500);
  
  Serial.println("\n\n=== Position Detector Test Application ===");
  Serial.println("This application tests the Position Detector implementation");
  Serial.println("with real-time LED feedback for detected positions.");
  
  // Get hardware manager instance
  hardware = HardwareManager::getInstance();
  
  // Initialize hardware
  bool hwInitResult = hardware->init();
  Serial.print("Hardware initialization: ");
  Serial.println(hwInitResult ? "SUCCESS" : "FAILED");
  
  if (!hwInitResult) {
    Serial.println("Hardware initialization failed. Cannot proceed.");
    while (1) {
      // Set all LEDs to red to indicate error
      hardware->setAllLEDs({255, 0, 0});
      hardware->updateLEDs();
      delay(500);
      hardware->setAllLEDs({0, 0, 0});
      hardware->updateLEDs();
      delay(500);
    }
  }
  
  // Initialize position detector
  bool pdInitResult = positionDetector.init(hardware);
  Serial.print("Position Detector initialization: ");
  Serial.println(pdInitResult ? "SUCCESS" : "FAILED");
  
  if (!pdInitResult) {
    Serial.println("Position Detector initialization failed. Cannot proceed.");
    while (1) {
      // Set all LEDs to red to indicate error
      hardware->setAllLEDs({255, 0, 0});
      hardware->updateLEDs();
      delay(500);
      hardware->setAllLEDs({0, 0, 0});
      hardware->updateLEDs();
      delay(500);
    }
  }
  
  // Use default brightness
  hardware->setBrightness(Config::DEFAULT_BRIGHTNESS);
  
  // Display startup animation
  for (int i = 0; i < 3; i++) {
    hardware->setAllLEDs({0, 255, 0});
    hardware->updateLEDs();
    delay(200);
    hardware->setAllLEDs({0, 0, 0});
    hardware->updateLEDs();
    delay(200);
  }
  
  // Initialize timing variables
  lastUpdateTime = millis();
  positionStartTime = millis();
  
  Serial.println("\nPosition Detector Test ready!");
  Serial.println("Move your hand to different positions to see LED feedback");
  Serial.println("and position detection results in the serial monitor.");
  Serial.println("\nDetected positions will be displayed with their associated colors.");
  Serial.println("-----------------------------------------------------");
}

/**
 * @brief Main loop
 */
void loop() {
  unsigned long currentTime = millis();
  
  // Update hardware state
  hardware->update();
  
  // Process position detection at the specified interval
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = currentTime;
    
    // Get latest sensor data
    const SensorData& sensorData = hardware->getSensorData();
    
    // Detect position
    PositionReading currentPosition = positionDetector.detectPosition(sensorData);
    
    // Check if position has changed
    if (currentPosition.position != lastPosition.position) {
      // Calculate how long the previous position was held
      positionDuration = currentTime - positionStartTime;
      
      // Output information about the position change
      Serial.print("\nPosition changed from ");
      Serial.print(positionNames[lastPosition.position]);
      Serial.print(" to ");
      Serial.println(positionNames[currentPosition.position]);
      Serial.print("Previous position held for: ");
      Serial.print(positionDuration);
      Serial.println(" ms");
      
      // Output raw sensor data for debugging
      Serial.print("Sensor data: X=");
      Serial.print(sensorData.accelX);
      Serial.print(", Y=");
      Serial.print(sensorData.accelY);
      Serial.print(", Z=");
      Serial.println(sensorData.accelZ);
      
      // Reset position timer
      positionStartTime = currentTime;
      
      // Update last position
      lastPosition = currentPosition;
    }
    
    // Always update LEDs to show current position
    if (currentPosition.position <= POS_UNKNOWN) {
      // Set all LEDs to the color corresponding to the current position
      hardware->setAllLEDs(positionColors[currentPosition.position]);
    } else {
      // Invalid position, set white as fallback
      hardware->setAllLEDs({255, 255, 255});
    }
    hardware->updateLEDs();
    
    // Output position duration every second for stable positions
    if (currentTime - positionStartTime >= 1000 && (currentTime - positionStartTime) % 1000 < UPDATE_INTERVAL) {
      positionDuration = currentTime - positionStartTime;
      Serial.print("Holding ");
      Serial.print(positionNames[currentPosition.position]);
      Serial.print(" for ");
      Serial.print(positionDuration);
      Serial.println(" ms");
    }
  }
} 