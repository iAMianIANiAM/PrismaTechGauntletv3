#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include "../core/SystemTypes.h"
#include "../hardware/HardwareManager.h"

/**
 * @brief Coordinates all LED animations for the gauntlet
 */
class AnimationSystem {
public:
  /**
   * @brief Initialize the animation system
   * @param hardware Pointer to hardware manager
   * @return True if initialization successful
   */
  bool init(HardwareManager* hardware);
  
  /**
   * @brief Update animations, called from main loop
   */
  void update();
  
  // Idle mode animations
  
  /**
   * @brief Show the current hand position in Idle mode
   * @param position Current position reading
   */
  void showIdlePosition(const PositionReading& position);
  
  /**
   * @brief Show warning animation for LongNull approaching completion
   * @param progress Progress value 0.0-1.0
   */
  void showLongNullWarning(float progress);
  
  // Transition animations
  
  /**
   * @brief Show transition to Invocation mode animation
   */
  void showTransitionToInvocation();
  
  /**
   * @brief Show transition to Freecast mode animation
   */
  void showTransitionToFreecast();
  
  // Invocation mode animations
  
  /**
   * @brief Show slot indicator animation
   * @param slotIndex Slot being activated (0-2)
   */
  void showSlotIndicator(uint8_t slotIndex);
  
  /**
   * @brief Show countdown animation
   * @param progress Progress value 0.0-1.0
   */
  void showCountdownAnimation(float progress);
  
  /**
   * @brief Show checkpoint flash effect
   */
  void showCheckpointFlash();
  
  /**
   * @brief Show slot confirmation animation
   * @param slotIndex Slot being confirmed (0-2)
   * @param position Position detected for this slot
   */
  void showSlotConfirmation(uint8_t slotIndex, uint8_t position);
  
  // Resolution mode animations
  
  /**
   * @brief Show resolution preparation animation
   * @param slots Invocation slots data
   */
  void showResolutionPrepare(const InvocationSlots& slots);
  
  /**
   * @brief Show spell selection animation
   * @param slots Invocation slots data
   */
  void showResolutionSelect(const InvocationSlots& slots);
  
  /**
   * @brief Show spell core animation
   * @param slots Invocation slots data
   */
  void showResolutionCore(const InvocationSlots& slots);
  
  /**
   * @brief Determine which spell to cast based on slots
   * @param slots Invocation slots data
   * @return Spell type index (0-5)
   */
  uint8_t determineSpellType(const InvocationSlots& slots);
  
  /**
   * @brief Show spell effect animation
   * @param spellType Spell type index (0-5)
   * @param progress Progress value 0.0-1.0
   */
  void showSpellEffect(uint8_t spellType, float progress);
  
  /**
   * @brief Show complete blackout (all LEDs off)
   */
  void showBlackout();
  
  /**
   * @brief Show return to idle animation
   * @param progress Progress value 0.0-1.0
   * @param position Current position
   */
  void showIdleReturn(float progress, const PositionReading& position);
  
  // Freecast mode animations
  
  /**
   * @brief Show freecast initialization animation
   */
  void showFreecastInit();
  
  /**
   * @brief Show freecast collecting animation
   * @param progress Progress value 0.0-1.0
   */
  void showFreecastCollecting(float progress);
  
  /**
   * @brief Show freecast pattern animation
   * @param motionData Array of motion data samples
   * @param sampleCount Number of samples
   * @param progress Progress value 0.0-1.0
   */
  void showFreecastPattern(SensorData* motionData, uint8_t sampleCount, float progress);

private:
  // Hardware reference
  HardwareManager* hardwareRef = nullptr;
  
  // Animation state tracking
  uint8_t currentAnimationFrame = 0;
  unsigned long lastFrameTime = 0;
  unsigned long animationStartTime = 0;
  
  // Helper methods for various animations
  
  // Idle animations
  void showIdlePositionIndicators(uint8_t position);
  
  // Spell effects
  void showInfernoEffect(float progress);
  void showBlizzardEffect(float progress);
  void showWildGrowthEffect(float progress);
  void showDetectMagicEffect(float progress);
  void showRainbowBurstEffect(float progress);
  void showRainbowPopEffect(float progress);
  
  // Utility functions
  Color interpolateColor(const Color& start, const Color& end, float progress);
  Color colorWheel(uint8_t position);
  uint8_t countPositionInSlots(const InvocationSlots& slots, uint8_t position);
};

#endif // ANIMATION_SYSTEM_H 