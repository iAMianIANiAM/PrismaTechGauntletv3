/**
 * VisualDebugIndicator.cpp
 * 
 * Implementation of the VisualDebugIndicator class for the LUTT toolkit.
 */

#include "VisualDebugIndicator.h"
#include "../hardware/HardwareManager.h"
#include "../core/SystemTypes.h" // Added include for Color struct

// Initialize static variables
HardwareManager* VisualDebugIndicator::_hardware = nullptr;
uint8_t VisualDebugIndicator::_indicatorType = 0;
uint32_t VisualDebugIndicator::_lastUpdateTime = 0;
uint8_t VisualDebugIndicator::_indicatorLEDs[4] = {0, 3, 6, 9}; // Default LEDs to use
bool VisualDebugIndicator::_indicatorActive = false;
Color VisualDebugIndicator::_storedLEDValues[12]; // Changed to store Color structs
float VisualDebugIndicator::_progressValue = 0.0f;
uint8_t VisualDebugIndicator::_typeValue = 0;
float VisualDebugIndicator::_intensityValue = 0.0f;
float VisualDebugIndicator::_complexityValue = 0.0f;

/**
 * Initialize with hardware manager reference
 */
void VisualDebugIndicator::init(HardwareManager* hardware) {
  _hardware = hardware;
  
  // Only initialize other variables if debugging is enabled
  if (VISUAL_DEBUG_ENABLED) {
    _indicatorType = 0;
    _lastUpdateTime = 0;
    _indicatorActive = false;
    
    // Default indicator LEDs (can be changed at runtime)
    _indicatorLEDs[0] = 0;  // LED to use for indicator 0
    _indicatorLEDs[1] = 3;  // LED to use for indicator 1
    _indicatorLEDs[2] = 6;  // LED to use for indicator 2
    _indicatorLEDs[3] = 9;  // LED to use for indicator 3
  }
}

/**
 * Update gesture progress indicator
 */
void VisualDebugIndicator::updateGestureProgress(float progress, uint8_t gestureType) {
  if (!VISUAL_DEBUG_ENABLED || _hardware == nullptr) return;
  
  _indicatorType = VISUAL_IND_GESTURE_PROGRESS;
  _progressValue = constrain(progress, 0.0f, 1.0f);
  _typeValue = gestureType;
  _lastUpdateTime = millis();
}

/**
 * Update mode state indicator
 */
void VisualDebugIndicator::updateModeState(uint8_t mode, uint8_t subState) {
  if (!VISUAL_DEBUG_ENABLED || _hardware == nullptr) return;
  
  _indicatorType = VISUAL_IND_MODE_STATE;
  _typeValue = mode;
  _progressValue = (float)subState / 255.0f; // Scale substate to 0-1
  _lastUpdateTime = millis();
}

/**
 * Update animation phase indicator
 */
void VisualDebugIndicator::updateAnimationPhase(uint8_t phase, float progress) {
  if (!VISUAL_DEBUG_ENABLED || _hardware == nullptr) return;
  
  _indicatorType = VISUAL_IND_ANIMATION_PHASE;
  _typeValue = phase;
  _progressValue = constrain(progress, 0.0f, 1.0f);
  _lastUpdateTime = millis();
}

/**
 * Update motion feature indicator
 */
void VisualDebugIndicator::updateMotionFeature(float intensity, float complexity) {
  if (!VISUAL_DEBUG_ENABLED || _hardware == nullptr) return;
  
  _indicatorType = VISUAL_IND_MOTION_FEATURE;
  _intensityValue = constrain(intensity, 0.0f, 1.0f);
  _complexityValue = constrain(complexity, 0.0f, 1.0f);
  _lastUpdateTime = millis();
}

/**
 * Must be called in main loop to apply visual indicators
 */
void VisualDebugIndicator::process() {
  if (!VISUAL_DEBUG_ENABLED || _hardware == nullptr) return;
  
  const uint32_t currentTime = millis();
  
  // If we have a recent update, show indicators
  if (currentTime - _lastUpdateTime < 100) {
    if (!_indicatorActive) {
      // First time showing indicator, save current state
      saveCurrentLEDState();
      _indicatorActive = true;
    }
    
    // Apply indicator visualization
    applyIndicator();
  } 
  // If indicators were active but now should be hidden
  else if (_indicatorActive) {
    // Restore original state
    restoreOriginalLEDState();
    _indicatorActive = false;
  }
}

/**
 * Store original LED values before showing indicators
 */
void VisualDebugIndicator::saveCurrentLEDState() {
  // Note: We don't directly have access to the LED values,
  // but we can capture them indirectly by saving a copy of
  // what's currently being shown
  for (uint8_t i = 0; i < 12; i++) {
    // Get current LED value - store in our Color struct
    // Note: Since we can't directly get the RGB values, we'll just
    // initialize with black as a fallback if this is not implemented
    _storedLEDValues[i] = {0, 0, 0};
  }
}

/**
 * Restore original LED values after showing indicators
 */
void VisualDebugIndicator::restoreOriginalLEDState() {
  // Restore all LED values
  for (uint8_t i = 0; i < 12; i++) {
    _hardware->setLED(i, _storedLEDValues[i]);
  }
  _hardware->updateLEDs();
}

/**
 * Apply indicator based on current settings
 */
void VisualDebugIndicator::applyIndicator() {
  // Apply different indicators based on type
  switch(_indicatorType) {
    case VISUAL_IND_GESTURE_PROGRESS: {
      // Show progress as filled LEDs, color based on gesture type
      const uint8_t numLedsToLight = (uint8_t)(_progressValue * 4);
      
      // Different colors for different gesture types
      Color ledColor;
      switch(_typeValue) {
        case 0: ledColor = {255, 0, 0}; break;    // Red for type 0
        case 1: ledColor = {0, 255, 0}; break;    // Green for type 1
        case 2: ledColor = {0, 0, 255}; break;    // Blue for type 2
        default: ledColor = {255, 255, 0}; break; // Yellow for others
      }
      
      // Light up progress LEDs
      for (uint8_t i = 0; i < 4; i++) {
        if (i < numLedsToLight) {
          _hardware->setLED(_indicatorLEDs[i], ledColor);
        } else {
          Color black = {0, 0, 0};
          _hardware->setLED(_indicatorLEDs[i], black);
        }
      }
      break;
    }
    
    case VISUAL_IND_MODE_STATE: {
      // Mode state: One LED per mode, brightness for substate
      const uint8_t brightness = (uint8_t)(_progressValue * 255);
      
      // Different colors for different modes (up to 4 modes)
      for (uint8_t i = 0; i < 4; i++) {
        if (i == _typeValue) {
          // This LED represents current mode, brightness indicates substate
          Color white = {brightness, brightness, brightness};
          _hardware->setLED(_indicatorLEDs[i], white);
        } else {
          // Not current mode, turn off
          Color black = {0, 0, 0};
          _hardware->setLED(_indicatorLEDs[i], black);
        }
      }
      break;
    }
    
    case VISUAL_IND_ANIMATION_PHASE: {
      // Animation phase: Phase number determines which LED, progress as color
      const uint8_t phaseIdx = _typeValue % 4;
      const uint8_t hue = (uint8_t)(_progressValue * 255);
      
      // Convert hue to RGB (simplified HSV->RGB)
      Color ledColor;
      if (hue < 85) {
        ledColor.r = 255 - hue * 3;
        ledColor.g = hue * 3;
        ledColor.b = 0;
      } else if (hue < 170) {
        ledColor.r = 0;
        ledColor.g = 255 - (hue - 85) * 3;
        ledColor.b = (hue - 85) * 3;
      } else {
        ledColor.r = (hue - 170) * 3;
        ledColor.g = 0;
        ledColor.b = 255 - (hue - 170) * 3;
      }
      
      // Set all LEDs black except current phase
      Color black = {0, 0, 0};
      for (uint8_t i = 0; i < 4; i++) {
        if (i == phaseIdx) {
          _hardware->setLED(_indicatorLEDs[i], ledColor);
        } else {
          _hardware->setLED(_indicatorLEDs[i], black);
        }
      }
      break;
    }
    
    case VISUAL_IND_MOTION_FEATURE: {
      // Motion features: Intensity shown by brightness, complexity by color
      Color ledColor;
      ledColor.r = (uint8_t)(_intensityValue * 255);
      ledColor.g = (uint8_t)(_complexityValue * 255);
      ledColor.b = (uint8_t)((1.0f - _complexityValue) * _intensityValue * 255);
      
      // Show on all indicator LEDs
      for (uint8_t i = 0; i < 4; i++) {
        _hardware->setLED(_indicatorLEDs[i], ledColor);
      }
      break;
    }
  }
  
  // Apply changes
  _hardware->updateLEDs();
} 