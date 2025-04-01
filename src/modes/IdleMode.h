#ifndef IDLE_MODE_H
#define IDLE_MODE_H

#include <Arduino.h>
#include <FastLED.h>
#include "../hardware/HardwareManager.h"
#include "../detection/UltraBasicPositionDetector.h"
#include "../core/SystemTypes.h"
#include "../detection/GestureTransitionTracker.h"

class IdleMode {
private:
    // Dependencies
    HardwareManager* hardwareManager;
    UltraBasicPositionDetector* positionDetector;
    
    // State tracking
    PositionReading currentPosition;
    PositionReading previousPosition;
    unsigned long positionChangedTime;
    
    // Shield position tracking for LongShield gesture (for Freecast transition)
    unsigned long shieldPositionStartTime;
    bool inShieldCountdown;
    
    // QuickCast Gesture Trackers
    GestureTransitionTracker calmOfferTracker_;
    GestureTransitionTracker digOathTracker_;
    GestureTransitionTracker nullShieldTracker_;
    
    // Color transition state
    CRGB currentColor;
    CRGB targetColor;
    CRGB previousColor;
    unsigned long colorTransitionStartTime;
    bool interpolationEnabled;
    
    // Configuration
    static const uint8_t IDLE_LEDS[4];
    static const uint8_t IDLE_BRIGHTNESS;
    static const uint16_t COLOR_TRANSITION_MS;
    
    // Internal methods
    CRGB getPositionColor(uint8_t position);
    bool detectLongShieldGesture();
    void updateColorTransition();
    void resetAllSpellTrackers();
    
public:
    IdleMode();
    bool init(HardwareManager* hardware, UltraBasicPositionDetector* detector);
    void initialize();
    void update();
    ModeTransition checkForTransition();
    void renderLEDs();
    void setInterpolationEnabled(bool enabled);
    SpellTransition checkForSpellTransition();
};

#endif // IDLE_MODE_H 