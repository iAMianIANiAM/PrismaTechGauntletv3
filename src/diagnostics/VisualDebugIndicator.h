/**
 * VisualDebugIndicator.h
 * 
 * A lightweight utility that uses existing LEDs to visualize system state
 * without affecting normal operation.
 * 
 * Part of the Lightweight Universal Troubleshooting Toolkit (LUTT)
 */

#ifndef VISUAL_DEBUG_INDICATOR_H
#define VISUAL_DEBUG_INDICATOR_H

#include <Arduino.h>

// Forward declaration to avoid circular dependencies
class HardwareManager;
struct Color;

// Control whether visual indicators are enabled
#ifndef VISUAL_DEBUG_ENABLED
#define VISUAL_DEBUG_ENABLED 0
#endif

// Indicator types
#define VISUAL_IND_GESTURE_PROGRESS    0
#define VISUAL_IND_MODE_STATE          1
#define VISUAL_IND_ANIMATION_PHASE     2
#define VISUAL_IND_MOTION_FEATURE      3

class VisualDebugIndicator {
public:
  /**
   * Initialize with hardware manager reference
   * @param hardware Pointer to the hardware manager instance
   */
  static void init(HardwareManager* hardware);
  
  /**
   * Update gesture progress indicator
   * @param progress Progress value from 0.0 to 1.0
   * @param gestureType Type of gesture being tracked
   */
  static void updateGestureProgress(float progress, uint8_t gestureType);
  
  /**
   * Update mode state indicator
   * @param mode Current mode ID
   * @param subState Current substate within mode
   */
  static void updateModeState(uint8_t mode, uint8_t subState);
  
  /**
   * Update animation phase indicator
   * @param phase Current animation phase
   * @param progress Progress within the phase (0.0 to 1.0)
   */
  static void updateAnimationPhase(uint8_t phase, float progress);
  
  /**
   * Update motion feature indicator
   * @param intensity Motion intensity (0.0 to 1.0)
   * @param complexity Motion complexity/feature density (0.0 to 1.0)
   */
  static void updateMotionFeature(float intensity, float complexity);
  
  /**
   * Must be called in main loop to apply visual indicators
   * without interfering with normal LED operation
   */
  static void process();

private:
  // Store original LED values before showing indicators
  static void saveCurrentLEDState();
  
  // Restore original LED values after showing indicators
  static void restoreOriginalLEDState();
  
  // Apply indicator based on current settings
  static void applyIndicator();
  
  // Hardware manager reference for LED access
  static HardwareManager* _hardware;
  
  // Current indicator type being displayed
  static uint8_t _indicatorType;
  
  // Last update time for indicator timing
  static uint32_t _lastUpdateTime;
  
  // Which LEDs to use for indicators (customizable)
  static uint8_t _indicatorLEDs[4];
  
  // Is indicator currently active?
  static bool _indicatorActive;
  
  // Original RGB values for all LEDs (to restore later)
  static Color _storedLEDValues[12];
  
  // Parameters for current indicator
  static float _progressValue;
  static uint8_t _typeValue;
  static float _intensityValue;
  static float _complexityValue;
};

#endif // VISUAL_DEBUG_INDICATOR_H 