#include "GauntletController.h"
#include "Config.h"
#include <Arduino.h>

GauntletController::GauntletController() 
    : hardwareManager(nullptr), positionDetector(nullptr), idleMode(nullptr),
      currentMode(SystemMode::IDLE), lastUpdateTime(0), updateInterval(20) {
}

GauntletController::~GauntletController() {
    if (idleMode != nullptr) {
        delete idleMode;
        idleMode = nullptr;
    }
    
    if (positionDetector != nullptr) {
        delete positionDetector;
        positionDetector = nullptr;
    }
    
    if (hardwareManager != nullptr) {
        delete hardwareManager;
        hardwareManager = nullptr;
    }
}

void GauntletController::initialize() {
    // Initialize hardware
    hardwareManager = new HardwareManager();
    if (!hardwareManager->initialize()) {
        Serial.println(F("Hardware initialization failed!"));
        return;
    }
    
    // Initialize position detector
    positionDetector = new PositionDetector(hardwareManager->getMPU());
    positionDetector->initialize();
    
    // Initialize Idle Mode
    idleMode = new IdleMode(positionDetector, hardwareManager->getLEDs());
    idleMode->initialize();
    
    // Set starting system mode
    currentMode = SystemMode::IDLE;
    lastUpdateTime = millis();
    
    Serial.println(F("GauntletController initialized successfully"));
}

void GauntletController::update() {
    // Update hardware
    hardwareManager->update();
    
    // Update position detector
    positionDetector->update();
    
    // Handle current mode
    switch (currentMode) {
        case SystemMode::IDLE: {
            idleMode->update();
            ModeTransition transition = idleMode->checkForTransition();
            
            if (transition == ModeTransition::TO_INVOCATION) {
                // Handle transition to Invocation Mode
                showTransitionAnimation(CRGB(128, 0, 128)); // Purple flash
                // TODO: Initialize and switch to Invocation Mode once implemented
                Serial.println(F("Invocation Mode triggered but not yet implemented"));
            } else if (transition == ModeTransition::TO_FREECAST) {
                // Handle transition to Freecast Mode
                showTransitionAnimation(CRGB(255, 165, 0)); // Orange flash
                // TODO: Initialize and switch to Freecast Mode once implemented
                Serial.println(F("Freecast Mode triggered but not yet implemented"));
            }
            break;
        }
        
        case SystemMode::INVOCATION:
            // TODO: Implement Invocation Mode handling
            break;
            
        case SystemMode::RESOLUTION:
            // TODO: Implement Resolution Mode handling
            break;
            
        case SystemMode::FREECAST:
            // TODO: Implement Freecast Mode handling
            break;
    }
    
    // Maintain consistent loop timing
    maintainLoopTiming();
}

void GauntletController::setInterpolationEnabled(bool enabled) {
    if (idleMode != nullptr) {
        idleMode->setInterpolationEnabled(enabled);
    }
}

void GauntletController::showTransitionAnimation(CRGB color) {
    LEDInterface* leds = hardwareManager->getLEDs();
    
    // Flash all LEDs once in the given color
    for (int i = 0; i < NUM_LEDS; i++) {
        leds->setPixel(i, color, 255);
    }
    leds->show();
    delay(200);
    
    // Clear all LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
        leds->setPixel(i, CRGB(0, 0, 0), 0);
    }
    leds->show();
    delay(100);
}

bool GauntletController::hasElapsed(unsigned long startTime, unsigned long duration) const {
    return (millis() - startTime) >= duration;
}

void GauntletController::maintainLoopTiming() {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - lastUpdateTime;
    
    // If we haven't used our full time slice, delay the remainder
    if (elapsedTime < updateInterval) {
        delay(updateInterval - elapsedTime);
    }
    
    lastUpdateTime = millis();
} 