#include <Arduino.h>
#include "hardware/HardwareManager.h"
#include "detection/UltraBasicPositionDetector.h"
#include "core/Config.h"
#include "core/SystemTypes.h"
#include "modes/IdleMode.h"
#include "modes/FreeCastMode.h"

// Serial communication
#define SERIAL_BAUD_RATE 115200

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

// Increase Arduino loop task stack size
#if CONFIG_ARDUINO_LOOP_STACK_SIZE < 16384
#undef CONFIG_ARDUINO_LOOP_STACK_SIZE
#define CONFIG_ARDUINO_LOOP_STACK_SIZE 16384
#endif

// Global objects
HardwareManager* hardware = nullptr;
UltraBasicPositionDetector positionDetector;
IdleMode idleMode;
FreeCastMode freeCastMode;
unsigned long lastUpdateTime = 0;
unsigned long invocationModeStartTime = 0; // Track Invocation Mode start time

// Use existing SystemMode enum from SystemTypes.h
SystemMode currentMode = MODE_IDLE;
bool idleModeInitialized = false;
bool freeCastModeInitialized = false;

// Pre-allocated variables for loop to reduce stack usage
PositionReading positionReading;
ProcessedData processedData;
Color positionColor;

// Mode transition handling
void handleModeTransition(ModeTransition transition) {
  // Create color variables outside the switch to avoid jump issues
  Color purple = {128, 0, 128};
  Color shieldBlue = {Config::Colors::SHIELD_COLOR[0], 
                      Config::Colors::SHIELD_COLOR[1], 
                      Config::Colors::SHIELD_COLOR[2]};
  
  switch (transition) {
    case ModeTransition::TO_INVOCATION:
      currentMode = MODE_INVOCATION;
      invocationModeStartTime = millis(); // Reset animation start time
      
      // Visual feedback for Invocation Mode transition - purple flash
      for (int i = 0; i < 3; i++) {
        hardware->setAllLEDs(purple);
        hardware->updateLEDs();
        delay(100);
        hardware->setAllLEDs({0, 0, 0});
        hardware->updateLEDs();
        delay(50);
      }
      
      Serial.println("CalmOffer gesture detected! Transitioning to Invocation Mode");
      break;
      
    case ModeTransition::TO_FREECAST:
      currentMode = MODE_FREECAST;
      
      // Visual feedback for Freecast Mode transition - shield blue flash
      for (int i = 0; i < 3; i++) {
        hardware->setAllLEDs(shieldBlue);
        hardware->updateLEDs();
        delay(100);
        hardware->setAllLEDs({0, 0, 0});
        hardware->updateLEDs();
        delay(50);
      }
      
      // Initialize FreeCast mode
      if (freeCastModeInitialized) {
        freeCastMode.initialize();
      }
      
      Serial.println("LongShield gesture detected! Transitioning to Freecast Mode");
      break;
      
    case ModeTransition::TO_IDLE:
      currentMode = MODE_IDLE;
      
      // Visual feedback for returning to Idle Mode - shield blue flash
      for (int i = 0; i < 3; i++) {
        hardware->setAllLEDs(shieldBlue);
        hardware->updateLEDs();
        delay(100);
        hardware->setAllLEDs({0, 0, 0});
        hardware->updateLEDs();
        delay(50);
      }
      
      // Reinitialize Idle mode
      if (idleModeInitialized) {
        idleMode.initialize();
      }
      
      Serial.println("Exiting FreeCast Mode! Returning to Idle Mode");
      break;
      
    default:
      break;
  }
}

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
  
  // Initialize IdleMode after position detector
  if (pdInitOk) {
    idleModeInitialized = idleMode.init(hardware, &positionDetector);
    Serial.print("Idle Mode initialization: ");
    Serial.println(idleModeInitialized ? "SUCCESS" : "FAILED");
    
    if (idleModeInitialized) {
      idleMode.initialize();
    } else {
      Serial.println("WARNING: IdleMode initialization failed. Proceeding with basic functionality.");
    }
    
    // Initialize FreeCastMode
    freeCastModeInitialized = freeCastMode.init(hardware, &positionDetector);
    Serial.print("FreeCast Mode initialization: ");
    Serial.println(freeCastModeInitialized ? "SUCCESS" : "FAILED");
    
    if (!freeCastModeInitialized) {
      Serial.println("WARNING: FreeCastMode initialization failed. This functionality will not be available.");
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
    
    // Mode-specific handling
    if (currentMode == MODE_IDLE) {
      if (idleModeInitialized) {
        // Update idle mode and check for gestures
        idleMode.update();
        ModeTransition transition = idleMode.checkForTransition();
        
        if (transition != ModeTransition::NONE) {
          handleModeTransition(transition);
        } else {
          // Only render LEDs with idle mode if no transition occurred
          idleMode.renderLEDs();
        }
        
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
      } else {
        // Fallback to original LED handling if IdleMode not available
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
          case POS_NULLPOS:
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
    } else if (currentMode == MODE_INVOCATION) {
      // Placeholder for Invocation Mode with rainbow animation
      // Will be fully implemented in future phase
      unsigned long animationDuration = 6000; // Rainbow animation for 6 seconds
      
      // Generate a dazzling rainbow pattern
      unsigned long animationElapsed = currentTime - invocationModeStartTime;
      
      if (animationElapsed < animationDuration) {
        // Create a dazzling rainbow pattern that rotates around the ring
        for (int i = 0; i < Config::NUM_LEDS; i++) {
          // Calculate hue based on LED position and time
          // This creates a rotating rainbow effect
          uint8_t hue = (i * 21) + (animationElapsed / 10) % 255; // Rotate every ~2.5 seconds
          
          // Calculate brightness - add pulsing effect
          uint8_t brightness = 255;
          if (i % 3 == 0) {
            // Add a sparkle effect on every third LED
            brightness = 128 + 127 * sin((animationElapsed + i * 50) / 100.0);
          }
          
          // Convert HSV to RGB for the LED
          CRGB color;
          color.setHSV(hue, 255, brightness);
          
          // Set the LED color
          hardware->setLED(i, {color.r, color.g, color.b});
        }
        hardware->updateLEDs();
        
        if (animationElapsed % 1000 < 20) { // Print status once per second
          Serial.print("Rainbow animation: ");
          Serial.print((animationDuration - animationElapsed) / 1000);
          Serial.println(" seconds remaining");
        }
      } else {
        // Return to Idle Mode after animation completes
        currentMode = MODE_IDLE;
        Serial.println("Rainbow animation complete. Returning to Idle Mode");
      }
    } else if (currentMode == MODE_FREECAST) {
      // Use the FreeCastMode implementation
      if (freeCastModeInitialized) {
        // Update FreeCast mode and check for exit gesture
        freeCastMode.update();
        ModeTransition transition = freeCastMode.checkForTransition();
        
        if (transition == ModeTransition::TO_IDLE) {
          // Handle transition back to Idle mode
          handleModeTransition(ModeTransition::TO_IDLE);
        } else {
          // Render LEDs with FreeCast mode
          freeCastMode.renderLEDs();
        }
        
        // Display minimal debug info
        Serial.println("In FreeCast Mode - capturing and displaying motion patterns");
      } else {
        // Fallback if FreeCast mode isn't initialized
        Serial.println("FreeCast Mode not available. Returning to Idle Mode.");
        currentMode = MODE_IDLE;
      }
    }
  }
  
  // Add a small delay to prevent watchdog issues
  delay(10);
}