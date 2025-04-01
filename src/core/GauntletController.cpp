#include "GauntletController.h"
#include "Config.h"
#include <Arduino.h>
#include "../utils/DebugTools.h"

GauntletController::GauntletController() 
    : hardwareManager(nullptr), 
      positionDetector(nullptr),
      idleMode(nullptr),
      quickCastMode(nullptr),
      freecastMode(nullptr),
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
    if (quickCastMode) {
        delete quickCastMode;
    }
    if (freecastMode) {
        delete freecastMode;
    }
    
    if (positionDetector) {
        delete positionDetector;
    }
    
    if (hardwareManager) {
        delete hardwareManager;
    }
}

void GauntletController::initialize() {
    DEBUG_PRINTLN("GauntletController::initialize() called");
    
    // Get HardwareManager instance
    hardwareManager = HardwareManager::getInstance();
    if (!hardwareManager || !hardwareManager->init()) {
        DEBUG_PRINTLN("ERROR: HardwareManager initialization failed!");
        while(1) delay(1000);
    }
    
    // Initialize position detector
    positionDetector = new UltraBasicPositionDetector();
    if (!positionDetector->init(hardwareManager)) {
        Serial.println(F("Position detector initialization failed!"));
        while(1) delay(1000);
    }
    
    // Initialize Idle Mode
    idleMode = new IdleMode();
    if (!idleMode->init(hardwareManager, positionDetector)) {
        DEBUG_PRINTLN("ERROR: IdleMode initialization failed!");
        while(1) delay(1000);
    }
    idleMode->initialize();
    
    // Initialize QuickCastSpells Mode
    quickCastMode = new QuickCastSpellsMode();
    if (!quickCastMode->init(hardwareManager)) {
        Serial.println(F("QuickCastSpells mode initialization failed!"));
        while(1) delay(1000);
    }
    
    // Initialize Freecast Mode
    freecastMode = new FreeCastMode();
    if (!freecastMode->init(hardwareManager, positionDetector)) {
        DEBUG_PRINTLN("ERROR: FreeCastMode initialization failed!");
        while(1) delay(1000);
    }
    
    // Set starting system mode
    currentMode = SystemMode::IDLE;
    lastUpdateTime = millis();
    
    Serial.println(F("GauntletController initialized successfully"));
}

void GauntletController::update() {
    // Update hardware first
    hardwareManager->update();
    
    // Update current mode
    ModeTransition modeTransition = ModeTransition::NONE;
    SpellTransition spellTransition = SpellTransition::NONE;

    switch (currentMode) {
        case SystemMode::IDLE:
            idleMode->update();
            modeTransition = idleMode->checkForTransition();
            spellTransition = idleMode->checkForSpellTransition();
            
            if (spellTransition != SpellTransition::NONE) {
                DEBUG_PRINTF("Spell transition detected: %d\n", static_cast<int>(spellTransition));
                // Pass the detected spell type to QuickCast mode
                SpellType typeToCast = SpellType::NONE;
                if (spellTransition == SpellTransition::TO_RAINBOW) typeToCast = SpellType::RAINBOW;
                else if (spellTransition == SpellTransition::TO_LIGHTNING) typeToCast = SpellType::LIGHTNING;
                else if (spellTransition == SpellTransition::TO_LUMINA) typeToCast = SpellType::LUMINA;
                
                if (typeToCast != SpellType::NONE) {
                    quickCastMode->enter(typeToCast);
                    currentMode = SystemMode::QUICKCAST_SPELL;
                    modeTransition = ModeTransition::NONE; // Prevent immediate mode change after spell start
                    DEBUG_PRINTLN("Transitioning to QuickCast Mode");
                }
            }
            break;
            
        case SystemMode::QUICKCAST_SPELL:
            modeTransition = quickCastMode->update();
            if (modeTransition == ModeTransition::TO_IDLE) {
                currentMode = SystemMode::IDLE;
                idleMode->initialize();
                Serial.println(F("Transitioning back to Idle Mode from QuickCast"));
            }
            break;

        case SystemMode::FREECAST:
            modeTransition = freecastMode->update();
            break;
            
        default:
             Serial.print(F("ERROR: Unknown SystemMode: ")); Serial.println((int)currentMode);
             currentMode = SystemMode::IDLE;
             idleMode->initialize();
             break;
    }
    
    // Handle mode transitions
    if (modeTransition != ModeTransition::NONE) {
        handleModeTransition(modeTransition);
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

void GauntletController::handleModeTransition(ModeTransition modeTransition) {
    switch (modeTransition) {
        case ModeTransition::TO_FREECAST:
            currentMode = SystemMode::FREECAST;
            break;
        case ModeTransition::TO_IDLE:
            DEBUG_PRINTLN("Transitioning to Idle Mode");
            // Re-initialize IdleMode state before entering
            idleMode->initialize(); 
            currentMode = SystemMode::IDLE;
            break;
        // Handle other transitions (e.g., TO_INVOCATION) if added later
    }
} 