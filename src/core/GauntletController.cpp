#include "GauntletController.h"
#include "Config.h"
#include <Arduino.h>

GauntletController::GauntletController() 
    : hardwareManager(nullptr), 
      positionDetector(nullptr),
      idleMode(nullptr),
      currentMode(SystemMode::IDLE),
      lastUpdateTime(0),
      updateInterval(20) // 50Hz update rate
{
}

GauntletController::~GauntletController() {
    // Free allocated resources
    if (idleMode) {
        delete idleMode;
    }
    
    if (positionDetector) {
        delete positionDetector;
    }
    
    if (hardwareManager) {
        delete hardwareManager;
    }
}

void GauntletController::initialize() {
    // Initialize hardware
    hardwareManager = new HardwareManager();
    if (!hardwareManager->init()) {
        Serial.println(F("Hardware initialization failed!"));
        return;
    }
    
    // Initialize position detector
#ifdef USE_ULTRA_BASIC_POSITION_DETECTOR
    positionDetector = new UltraBasicPositionDetector();
    if (!static_cast<UltraBasicPositionDetector*>(positionDetector)->init(hardwareManager)) {
        Serial.println(F("Position detector initialization failed!"));
        return;
    }
#else
    positionDetector = new PositionDetector();
    if (!positionDetector->init(hardwareManager)) {
        Serial.println(F("Position detector initialization failed!"));
        return;
    }
#endif
    
    // Initialize Idle Mode
    idleMode = new IdleMode();
    if (!idleMode->init(hardwareManager, positionDetector)) {
        Serial.println(F("Idle mode initialization failed!"));
        return;
    }
    idleMode->initialize();
    
    // Set starting system mode
    currentMode = SystemMode::IDLE;
    lastUpdateTime = millis();
    
    Serial.println(F("GauntletController initialized successfully"));
}

void GauntletController::update() {
    // Get current time
    unsigned long currentTime = millis();
    
    // Update hardware
    hardwareManager->update();
    
    // Execute mode-specific logic
    switch (currentMode) {
        case SystemMode::IDLE:
            // Update idle mode
            idleMode->update();
            
            // Check for transition to another mode
            ModeTransition transition = idleMode->checkForTransition();
            if (transition == ModeTransition::TO_INVOCATION) {
                // Transition to Invocation Mode
                currentMode = SystemMode::INVOCATION;
                Serial.println(F("Transitioning to Invocation Mode"));
                // TODO: Initialize invocation mode
            }
            else if (transition == ModeTransition::TO_FREECAST) {
                // Transition to Freecast Mode
                currentMode = SystemMode::FREECAST;
                Serial.println(F("Transitioning to Freecast Mode"));
                // TODO: Initialize freecast mode
            }
            break;
            
        case SystemMode::INVOCATION:
            // TODO: Implement Invocation Mode
            Serial.println(F("Invocation Mode not implemented yet!"));
            // Temporary fallback to Idle Mode
            currentMode = SystemMode::IDLE;
            break;
            
        case SystemMode::RESOLUTION:
            // TODO: Implement Resolution Mode
            Serial.println(F("Resolution Mode not implemented yet!"));
            // Temporary fallback to Idle Mode
            currentMode = SystemMode::IDLE;
            break;
            
        case SystemMode::FREECAST:
            // TODO: Implement Freecast Mode
            Serial.println(F("Freecast Mode not implemented yet!"));
            // Temporary fallback to Idle Mode
            currentMode = SystemMode::IDLE;
            break;
    }
    
    // Maintain consistent loop timing
    maintainLoopTiming();
}

bool GauntletController::hasElapsed(unsigned long startTime, unsigned long duration) const {
    unsigned long currentTime = millis();
    return (currentTime - startTime) >= duration;
}

void GauntletController::maintainLoopTiming() {
    // Calculate time spent in this loop iteration
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - lastUpdateTime;
    
    // If we've used less than our target update interval, delay the remainder
    if (elapsedTime < updateInterval) {
        delay(updateInterval - elapsedTime);
    }
    
    // Update the last update time
    lastUpdateTime = millis();
}

void GauntletController::setInterpolationEnabled(bool enabled) {
    if (idleMode) {
        idleMode->setInterpolationEnabled(enabled);
    }
}

SystemMode GauntletController::getCurrentMode() const {
    return currentMode;
} 