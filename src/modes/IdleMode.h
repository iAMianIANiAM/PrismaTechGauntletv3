#ifndef IDLE_MODE_H
#define IDLE_MODE_H

#include <Arduino.h>
#include <FastLED.h>
#include "../hardware/HardwareManager.h"
#include "../detection/PositionDetector.h"
#include "../core/SystemTypes.h"

enum class ModeTransition {
    NONE,
    TO_INVOCATION,
    TO_FREECAST
};

class IdleMode {
private:
    // Dependencies
    HardwareManager* hardwareManager;
    PositionDetector* positionDetector;
    
    // State tracking
    PositionReading currentPosition;
    PositionReading previousPosition;
    unsigned long positionChangedTime;
    unsigned long nullPositionStartTime;
    bool inNullCountdown;
    
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
    void updateColorTransition();
    
public:
    IdleMode();
    bool init(HardwareManager* hardware, PositionDetector* detector);
    void initialize();
    void update();
    ModeTransition checkForTransition();
    void renderLEDs();
    void setInterpolationEnabled(bool enabled);
};

#endif // IDLE_MODE_H 