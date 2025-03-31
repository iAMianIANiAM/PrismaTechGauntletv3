#ifndef IDLE_MODE_H
#define IDLE_MODE_H

#include <Arduino.h>
#include <FastLED.h>
#include "../hardware/HardwareManager.h"
#include "../detection/UltraBasicPositionDetector.h"
#include "../core/SystemTypes.h"

class IdleMode {
private:
    // Dependencies
    HardwareManager* hardwareManager;
    UltraBasicPositionDetector* positionDetector;
    
    // State tracking
    PositionReading currentPosition;
    PositionReading previousPosition;
    unsigned long positionChangedTime;
    unsigned long nullPositionStartTime;
    bool inNullCountdown;
    
    // Shield position tracking for LongShield gesture
    unsigned long shieldPositionStartTime;
    bool inShieldCountdown;
    
    // CalmOffer tracking variables
    unsigned long calmExitTime;
    bool inCalmOfferWindow;
    
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
    bool detectCalmOfferGesture();
    bool detectLongNullGesture();
    bool detectLongShieldGesture();
    void updateColorTransition();
    
public:
    IdleMode();
    bool init(HardwareManager* hardware, UltraBasicPositionDetector* detector);
    void initialize();
    void update();
    ModeTransition checkForTransition();
    void renderLEDs();
    void setInterpolationEnabled(bool enabled);
};

#endif // IDLE_MODE_H 