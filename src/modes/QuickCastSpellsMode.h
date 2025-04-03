#ifndef QUICKCAST_SPELLS_MODE_H
#define QUICKCAST_SPELLS_MODE_H

#include "../core/SystemTypes.h"
#include "../hardware/HardwareManager.h"
// Removed: #include "../detection/UltraBasicPositionDetector.h"
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
    ~QuickCastSpellsMode();

    /**
     * @brief Initializes the mode with necessary dependencies.
     * @param hardware Pointer to the HardwareManager instance.
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
    
    /**
     * @brief Immediately stops the active spell and cleans up
     * 
     * Used by ShakeCancel to abort spell animations early
     */
    void stopActiveSpell();

private:
    HardwareManager* hardwareManager_;

    SpellType activeSpell_;
    SpellState spellState_;
    uint32_t spellStartTime_;
    uint32_t spellDuration_;
    uint32_t lastUpdateTime_;

    // Removed: All position detection and gesture recognition related variables
    
    // Private Spell Rendering Methods
    void renderRainbowBurst(uint32_t currentTime, uint32_t elapsedTime);
    void renderLightningBlast(uint32_t currentTime, uint32_t elapsedTime);
    void renderLumina(uint32_t currentTime, uint32_t elapsedTime);

    // Rainbow Burst helper methods
    void renderRainbowPhase1(unsigned long elapsed);
    void renderRainbowPhase2(unsigned long elapsed);
    void renderRainbowPhase3(unsigned long elapsed);
    void renderRainbowPhase4(unsigned long elapsed);
    void renderRainbowSwirl(float progress, float brightness);
    void renderColorPop(uint8_t led, unsigned long popTime, unsigned long elapsed, uint8_t r, uint8_t g, uint8_t b);
    void hsvToRgb(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b);

    // Removed: Lumina cancellation method
    
    // Spell cleanup helper
    void cleanupSpell();
};

#endif // QUICKCAST_SPELLS_MODE_H 