#include <Arduino.h>
#include "hardware/HardwareManager.h"
#include "detection/UltraBasicPositionDetector.h"
#include "core/Config.h"
#include "core/SystemTypes.h"

// Serial communication
#define SERIAL_BAUD_RATE 115200

// Update interval
#define UPDATE_INTERVAL 100  // ms

// Position names for debugging
const char* positionNames[] = {
  "NULL (Orange)",   // POS_NULL
  "SHIELD (Blue)",   // POS_SHIELD
  "OFFER (Purple)",  // POS_OFFER
  "OATH (Red)",      // POS_OATH
  "DIG (Green)",     // POS_DIG
  "CALM (Yellow)",   // POS_CALM
  "UNKNOWN (White)"  // POS_UNKNOWN
};

// Increase Arduino loop task stack size
#if CONFIG_ARDUINO_LOOP_STACK_SIZE < 16384
#undef CONFIG_ARDUINO_LOOP_STACK_SIZE
#define CONFIG_ARDUINO_LOOP_STACK_SIZE 16384
#endif

// Global objects
HardwareManager* hardware = nullptr;
UltraBasicPositionDetector positionDetector;
unsigned long lastUpdateTime = 0;

// Pre-allocated variables for loop to reduce stack usage
PositionReading positionReading;
ProcessedData processedData;
Color positionColor;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  
  Serial.println("\n\n=== PrismaTech Gauntlet 3.0 ===");
  Serial.println("Ultra Basic Position Detection System");
  
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
  
  // Add delay after hardware initialization
  delay(100);
  
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
  
  Serial.println("\nGauntlet ready. Detecting hand positions.");
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
    positionReading = positionDetector.update();
    
    // Get processed data for display
    processedData = positionDetector.getProcessedData();
    
    // Display sensor data
    Serial.print("Accel: X=");
    Serial.print(processedData.accelX, 2);
    Serial.print(" Y=");
    Serial.print(processedData.accelY, 2);
    Serial.print(" Z=");
    Serial.print(processedData.accelZ, 2);
    
    // Display position
    Serial.print(" | Position: ");
    if (positionReading.position < 7) {
      Serial.print(positionNames[positionReading.position]);
      Serial.print(" (");
      Serial.print(positionReading.confidence);
      Serial.println("%)");
    } else {
      Serial.println("INVALID");
    }
    
    // Update LEDs based on detected position - use predeclared variable
    switch (positionReading.position) {
      case POS_NULL:
        positionColor = {Config::Colors::NULL_COLOR[0], Config::Colors::NULL_COLOR[1], Config::Colors::NULL_COLOR[2]};
        break;
      case POS_SHIELD:
        positionColor = {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]};
        break;
      case POS_OFFER:
        positionColor = {Config::Colors::OFFER_COLOR[0], Config::Colors::OFFER_COLOR[1], Config::Colors::OFFER_COLOR[2]};
        break;
      case POS_OATH:
        positionColor = {Config::Colors::OATH_COLOR[0], Config::Colors::OATH_COLOR[1], Config::Colors::OATH_COLOR[2]};
        break;
      case POS_DIG:
        positionColor = {Config::Colors::DIG_COLOR[0], Config::Colors::DIG_COLOR[1], Config::Colors::DIG_COLOR[2]};
        break;
      case POS_CALM:
        positionColor = {Config::Colors::CALM_COLOR[0], Config::Colors::CALM_COLOR[1], Config::Colors::CALM_COLOR[2]};
        break;
      default:
        positionColor = {Config::Colors::UNKNOWN_COLOR[0], Config::Colors::UNKNOWN_COLOR[1], Config::Colors::UNKNOWN_COLOR[2]};
        break;
    }
    
    hardware->setAllLEDs(positionColor);
    hardware->updateLEDs();
  }
  
  // Add a small delay to prevent watchdog issues
  delay(10);
}