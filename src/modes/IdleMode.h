#ifndef IDLE_MODE_H
#define IDLE_MODE_H

#include <Arduino.h>
#include <FastLED.h>
#include "../detection/PositionDetector.h"
#include "../hardware/LEDInterface.h"
#include "../core/SystemTypes.h"

enum class ModeTransition {
    NONE,
    TO_INVOCATION,
    TO_FREECAST
};

class IdleMode {
private:
    // Dependencies
    PositionDetector* positionDetector;
    LEDInterface* ledInterface;
    
    // State tracking
    Position currentPosition;
    Position previousPosition;
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
    
    // Color mappings
    CRGB getPositionColor(Position position);
    
    // Gesture detection
    bool detectCalmOfferGesture();
    bool detectLongNullGesture();
    void updateNullCountdown();
    
    // Color transition
    void updateColorTransition();
    
public:
    IdleMode(PositionDetector* detector, LEDInterface* leds);
    void initialize();
    void update();
    ModeTransition checkForTransition();
    void renderLEDs();
    void setInterpolationEnabled(bool enabled);
};

#endif // IDLE_MODE_H 