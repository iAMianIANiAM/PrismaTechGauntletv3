#ifndef QUICKCAST_SPELLS_MODE_H
#define QUICKCAST_SPELLS_MODE_H

#include "../core/SystemTypes.h"
#include "../hardware/HardwareManager.h"
// #include "../animation/AnimationController.h" // Removed include for non-existent controller
#include <stdint.h>

/**
 * @brief Handles the execution and rendering of QuickCast spell effects.
 * 
 * This mode is entered when a QuickCast gesture is detected in IdleMode.
 * It plays the corresponding spell animation for its duration and then 
 * transitions back to IdleMode.
 */
class QuickCastSpellsMode {
public:
    QuickCastSpellsMode();

    /**
     * @brief Initializes the mode with necessary dependencies.
     * @param hardware Pointer to the HardwareManager instance.
     // Removed AnimationController dependency from init signature
     * @return True if initialization is successful, false otherwise.
     */
    bool init(HardwareManager* hardware);

    /**
     * @brief Called when entering this mode.
     * @param spellType The specific QuickCast spell to execute.
     */
    void enter(SpellType spellType);

    /**
     * @brief Main update loop for the mode.
     * Manages spell timing, updates animations, and checks for exit conditions.
     * @return The requested mode transition (usually TO_IDLE when spell finishes, or NONE).
     */
    ModeTransition update();

    /**
     * @brief Called when exiting this mode.
     * Performs cleanup, stops animations, and resets state.
     */
    void exit();
    
    /**
     * @brief Explicitly renders LEDs for this mode.
     * This method exists for API consistency with other modes.
     * LED rendering is normally handled within the update() method.
     */
    void renderLEDs();

private:
    HardwareManager* hardwareManager_;
    // AnimationController* animationController_; // Removed member

    SpellType activeSpell_;
    SpellState spellState_;
    uint32_t spellStartTime_;
    uint32_t spellDuration_;
    uint32_t lastUpdateTime_;

    // Private Spell Rendering Methods
    void renderRainbowBurst(uint32_t currentTime, uint32_t elapsedTime);
    void renderLightningBlast(uint32_t currentTime, uint32_t elapsedTime);
    void renderLumina(uint32_t currentTime, uint32_t elapsedTime);

    // Private helper methods for specific spell logic can be added here
    // e.g., void updateRainbowBurst();
    // e.g., bool checkForLuminaExitGesture();
};

#endif // QUICKCAST_SPELLS_MODE_H 