#ifndef GAUNTLET_CONTROLLER_H
#define GAUNTLET_CONTROLLER_H

#include <Arduino.h>
#include "SystemTypes.h"
#include "../hardware/HardwareManager.h"
#include "../detection/UltraBasicPositionDetector.h"
#include "../modes/IdleMode.h"

enum class SystemMode {
    IDLE,
    // INVOCATION, // To be removed in Phase 4
    // RESOLUTION, // To be removed in Phase 4
    FREECAST,
    QUICKCAST_SPELL // Add new mode for QuickCast execution
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
    
    // System state
    SystemMode currentMode;
    
    // Helper methods
    void showTransitionAnimation(CRGB color);
    
    // Loop timing helpers
    unsigned long lastUpdateTime;
    unsigned long updateInterval;
    bool hasElapsed(unsigned long startTime, unsigned long duration) const;
    void maintainLoopTiming();
    
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
};

#endif // GAUNTLET_CONTROLLER_H 