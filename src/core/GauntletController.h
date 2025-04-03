#ifndef GAUNTLET_CONTROLLER_H
#define GAUNTLET_CONTROLLER_H

#include <Arduino.h>
#include "SystemTypes.h"
#include "../hardware/HardwareManager.h"
#include "../detection/UltraBasicPositionDetector.h"
#include "../modes/IdleMode.h"
#include "../modes/QuickCastSpellsMode.h"
#include "../modes/FreeCastMode.h"

enum class SystemMode {
    IDLE,
    FREECAST,
    QUICKCAST_SPELL
};

/**
 * @brief Main controller for the PrismaTech Gauntlet system
 * 
 * Coordinates all subsystems and manages the main execution loop
 */
class GauntletController {
private:
    HardwareManager* hardwareManager;
    UltraBasicPositionDetector* positionDetector;
    
    // Mode components
    IdleMode* idleMode;
    QuickCastSpellsMode* quickCastMode;
    FreeCastMode* freecastMode;
    
    // System state
    SystemMode currentMode;
    
    // Helper methods
    void showTransitionAnimation(CRGB color);
    
    // Loop timing helpers
    unsigned long lastUpdateTime;
    unsigned long updateInterval;
    bool hasElapsed(unsigned long startTime, unsigned long duration) const;
    void maintainLoopTiming();
    
    // Private helper methods
    void handleModeTransition(ModeTransition transition);
    void handleShakeCancellation();
    void playCancelAnimation();
    
    // State tracking
    bool inModeTransition; // Flag to prevent cancellation during transitions
    
public:
    GauntletController();
    ~GauntletController();
    
    void initialize();
    void update();
    
    // Configuration methods
    void setInterpolationEnabled(bool enabled);

    /**
     * @brief Get the current system mode
     * @return Current SystemMode
     */
    SystemMode getCurrentMode() const;
    
    /**
     * @brief Get the hardware manager instance
     * @return Pointer to the HardwareManager
     */
    HardwareManager* getHardwareManager() const {
        return hardwareManager;
    }
};

#endif // GAUNTLET_CONTROLLER_H 