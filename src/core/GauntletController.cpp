#include "GauntletController.h"
#include "Config.h"
#include <Arduino.h>
#include "../utils/DebugTools.h"

// LUTT Diagnostic includes (conditionally compiled)
#if DIAG_LOGGING_ENABLED
#include "../diagnostics/DiagnosticLogger.h"
#include "../diagnostics/StateSnapshotCapture.h"
#endif

GauntletController::GauntletController() 
    : hardwareManager(nullptr), 
      positionDetector(nullptr),
      idleMode(nullptr),
      quickCastMode(nullptr),
      freecastMode(nullptr),
      currentMode(SystemMode::IDLE),
      lastUpdateTime(0),
      updateInterval(20), // 50Hz update rate
      inModeTransition(false)
{
    #if DIAG_LOGGING_ENABLED
    DIAG_INFO(DIAG_TAG_MODE, "GauntletController constructed");
    #endif
}

GauntletController::~GauntletController() {
    #if DIAG_LOGGING_ENABLED
    DIAG_INFO(DIAG_TAG_MODE, "GauntletController destroyed");
    #endif

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
    
    #if DIAG_LOGGING_ENABLED
    DIAG_INFO(DIAG_TAG_MODE, "GauntletController initializing");
    #endif
    
    // Get HardwareManager instance
    hardwareManager = HardwareManager::getInstance();
    if (!hardwareManager || !hardwareManager->init()) {
        DEBUG_PRINTLN("ERROR: HardwareManager initialization failed!");
        
        #if DIAG_LOGGING_ENABLED
        DIAG_CRITICAL(DIAG_TAG_MODE, "HardwareManager initialization failed");
        #endif
        
        while(1) delay(1000);
    }
    
    // Initialize position detector
    positionDetector = new UltraBasicPositionDetector();
    if (!positionDetector->init(hardwareManager)) {
        Serial.println(F("Position detector initialization failed!"));
        
        #if DIAG_LOGGING_ENABLED
        DIAG_CRITICAL(DIAG_TAG_MODE, "Position detector initialization failed");
        #endif
        
        while(1) delay(1000);
    }
    
    // Initialize Idle Mode
    idleMode = new IdleMode();
    if (!idleMode->init(hardwareManager, positionDetector)) {
        DEBUG_PRINTLN("ERROR: IdleMode initialization failed!");
        
        #if DIAG_LOGGING_ENABLED
        DIAG_CRITICAL(DIAG_TAG_MODE, "IdleMode initialization failed");
        #endif
        
        while(1) delay(1000);
    }
    idleMode->initialize();
    
    // Initialize QuickCastSpells Mode
    quickCastMode = new QuickCastSpellsMode();
    if (!quickCastMode->init(hardwareManager)) {
        Serial.println(F("QuickCastSpells mode initialization failed!"));
        
        #if DIAG_LOGGING_ENABLED
        DIAG_CRITICAL(DIAG_TAG_MODE, "QuickCastSpellsMode initialization failed");
        #endif
        
        while(1) delay(1000);
    }
    
    // Initialize Freecast Mode
    freecastMode = new FreeCastMode();
    if (!freecastMode->init(hardwareManager, positionDetector)) {
        DEBUG_PRINTLN("ERROR: FreeCastMode initialization failed!");
        
        #if DIAG_LOGGING_ENABLED
        DIAG_CRITICAL(DIAG_TAG_MODE, "FreeCastMode initialization failed");
        #endif
        
        while(1) delay(1000);
    }
    
    // Set starting system mode
    currentMode = SystemMode::IDLE;
    lastUpdateTime = millis();
    
    Serial.println(F("GauntletController initialized successfully"));
    
    #if DIAG_LOGGING_ENABLED
    DIAG_INFO(DIAG_TAG_MODE, "GauntletController initialized successfully");
    #endif
}

void GauntletController::update() {
    // Update hardware first
    hardwareManager->update();
    
    // Check for shake cancellation (only in non-idle modes)
    if (currentMode != SystemMode::IDLE) {
        ShakeGestureDetector* shakeDetector = hardwareManager->getShakeDetector();
        if (shakeDetector && shakeDetector->isShakeDetected()) {
            DEBUG_PRINTLN("Shake cancellation detected");
            handleShakeCancellation();
            
            // Skip further updates this cycle
            maintainLoopTiming();
            return;
        }
    }
    
    // Update current mode
    ModeTransition modeTransition = ModeTransition::NONE;
    SpellTransition spellTransition = SpellTransition::NONE;
    
    #if DIAG_LOGGING_ENABLED
    unsigned long updateStartTime = millis();
    #endif

    // Starting mode transition - set flag
    if (modeTransition != ModeTransition::NONE || spellTransition != SpellTransition::NONE) {
        inModeTransition = true;
    }

    switch (currentMode) {
        case SystemMode::IDLE:
            idleMode->update();
            modeTransition = idleMode->checkForTransition();
            spellTransition = idleMode->checkForSpellTransition();
            
            if (spellTransition != SpellTransition::NONE) {
                DEBUG_PRINTF("Spell transition detected: %d\n", static_cast<int>(spellTransition));
                
                #if DIAG_LOGGING_ENABLED
                DIAG_INFO(DIAG_TAG_MODE, "Spell transition detected: %d", (int)spellTransition);
                #endif
                
                // Pass the detected spell type to QuickCast mode
                SpellType typeToCast = SpellType::NONE;
                if (spellTransition == SpellTransition::TO_RAINBOW) typeToCast = SpellType::RAINBOW;
                else if (spellTransition == SpellTransition::TO_LIGHTNING) typeToCast = SpellType::LIGHTNING;
                else if (spellTransition == SpellTransition::TO_LUMINA) typeToCast = SpellType::LUMINA;
                
                if (typeToCast != SpellType::NONE) {
                    #if DIAG_LOGGING_ENABLED
                    DIAG_INFO(DIAG_TAG_MODE, "Transitioning to QuickCast Mode with spell type: %d", (int)typeToCast);
                    
                    // Capture mode transition
                    StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_MODE_CHANGE, "GauntletController::update");
                    StateSnapshotCapture::addField("previousMode", "IDLE");
                    StateSnapshotCapture::addField("newMode", "QUICKCAST_SPELL");
                    StateSnapshotCapture::addField("spellType", (int)typeToCast);
                    StateSnapshotCapture::addField("spellTransition", (int)spellTransition);
                    #endif
                    
                    quickCastMode->enter(typeToCast);
                    currentMode = SystemMode::QUICKCAST_SPELL;
                    modeTransition = ModeTransition::NONE; // Prevent immediate mode change after spell start
                    DEBUG_PRINTLN("Transitioning to QuickCast Mode");
                }
            }
            break;
            
        case SystemMode::QUICKCAST_SPELL:
            modeTransition = quickCastMode->update();
            // Explicitly render LEDs for QuickCast, similar to FreeCast mode
            quickCastMode->renderLEDs();
            if (modeTransition == ModeTransition::TO_IDLE) {
                #if DIAG_LOGGING_ENABLED
                DIAG_INFO(DIAG_TAG_MODE, "QuickCast completed, transitioning back to Idle");
                
                // Capture mode transition
                StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_MODE_CHANGE, "GauntletController::update");
                StateSnapshotCapture::addField("previousMode", "QUICKCAST_SPELL");
                StateSnapshotCapture::addField("newMode", "IDLE");
                StateSnapshotCapture::addField("reason", "SPELL_COMPLETE");
                #endif
                
                currentMode = SystemMode::IDLE;
                idleMode->initialize();
                Serial.println(F("Transitioning back to Idle Mode from QuickCast"));
            }
            break;

        case SystemMode::FREECAST:
            modeTransition = freecastMode->update();
            freecastMode->renderLEDs();
            if (modeTransition == ModeTransition::TO_IDLE) {
                #if DIAG_LOGGING_ENABLED
                DIAG_INFO(DIAG_TAG_MODE, "FreeCast completed, transitioning back to Idle");
                
                // Capture mode transition
                StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_MODE_CHANGE, "GauntletController::update");
                StateSnapshotCapture::addField("previousMode", "FREECAST");
                StateSnapshotCapture::addField("newMode", "IDLE");
                StateSnapshotCapture::addField("reason", "FREECAST_EXIT");
                #endif
            }
            break;
            
        default:
             Serial.print(F("ERROR: Unknown SystemMode: ")); Serial.println((int)currentMode);
             
             #if DIAG_LOGGING_ENABLED
             DIAG_ERROR(DIAG_TAG_MODE, "Unknown SystemMode: %d", (int)currentMode);
             #endif
             
             currentMode = SystemMode::IDLE;
             idleMode->initialize();
             break;
    }
    
    // Handle mode transitions
    if (modeTransition != ModeTransition::NONE) {
        #if DIAG_LOGGING_ENABLED
        DIAG_INFO(DIAG_TAG_MODE, "Mode transition detected: %d", (int)modeTransition);
        #endif
        
        inModeTransition = true;
        handleModeTransition(modeTransition);
        inModeTransition = false;
    }
    
    #if DIAG_LOGGING_ENABLED
    unsigned long updateDuration = millis() - updateStartTime;
    // Log only if update took longer than expected
    if (updateDuration > updateInterval) {
        DIAG_WARNING(DIAG_TAG_MODE, "Slow update cycle: %lu ms (target: %lu ms)", 
                    updateDuration, updateInterval);
    }
    #endif
    
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
    #if DIAG_LOGGING_ENABLED
    DIAG_INFO(DIAG_TAG_MODE, "Handling mode transition: %d from mode: %d", 
             (int)modeTransition, (int)currentMode);
             
    // Determine new mode based on transition
    SystemMode newMode = currentMode; // Default to current
    if (modeTransition == ModeTransition::TO_FREECAST) newMode = SystemMode::FREECAST;
    else if (modeTransition == ModeTransition::TO_IDLE) newMode = SystemMode::IDLE;
    
    // Capture snapshot for mode transition
    StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_MODE_CHANGE, "GauntletController::handleModeTransition");
    StateSnapshotCapture::addField("previousMode", (int)currentMode);
    StateSnapshotCapture::addField("newMode", (int)newMode);
    StateSnapshotCapture::addField("transitionType", (int)modeTransition);
    #endif
    
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

void GauntletController::handleShakeCancellation() {
    // Skip if already transitioning between modes
    if (inModeTransition) {
        DEBUG_PRINTLN("Shake cancellation ignored - already in transition");
        return;
    }
    
    #if DIAG_LOGGING_ENABLED
    DIAG_INFO(DIAG_TAG_MODE, "Shake cancellation detected, exiting mode: %d", (int)currentMode);
    
    // Capture mode transition
    StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_MODE_CHANGE, "GauntletController::handleShakeCancellation");
    StateSnapshotCapture::addField("previousMode", (int)currentMode);
    StateSnapshotCapture::addField("newMode", (int)SystemMode::IDLE);
    StateSnapshotCapture::addField("reason", "SHAKE_CANCEL");
    #endif
    
    // Play cancellation animation
    playCancelAnimation();
    
    // Clean up based on current mode
    switch (currentMode) {
        case SystemMode::QUICKCAST_SPELL:
            quickCastMode->stopActiveSpell();
            break;
        case SystemMode::FREECAST:
            freecastMode->reset();
            break;
        default:
            // Nothing to do for other modes
            break;
    }
    
    // Transition to idle mode
    idleMode->initialize();
    currentMode = SystemMode::IDLE;
    
    DEBUG_PRINTLN("Transitioned to Idle Mode via ShakeCancel");
}

void GauntletController::playCancelAnimation() {
    LEDInterface* ledInterface = hardwareManager->getLEDInterface();
    
    // Simple white flash sequence
    for (int i = 0; i < Config::ShakeDetection::CANCEL_FLASH_COUNT; i++) {
        // Convert to CRGB for FastLED compatibility
        CRGB white = CRGB::White;
        ledInterface->fillSolid(white);
        ledInterface->show();
        delay(Config::ShakeDetection::CANCEL_FLASH_DURATION_MS);
        
        CRGB black = CRGB::Black;
        ledInterface->fillSolid(black);
        ledInterface->show();
        delay(Config::ShakeDetection::CANCEL_FLASH_DURATION_MS);
    }
} 