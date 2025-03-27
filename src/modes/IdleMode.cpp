#include "IdleMode.h"
#include "../core/Config.h"
#include "../core/SystemTypes.h"

// Initialize static constants
const uint8_t IdleMode::IDLE_LEDS[4] = {0, 3, 6, 9};
const uint8_t IdleMode::IDLE_BRIGHTNESS = 204; // 80% of 255
const uint16_t IdleMode::COLOR_TRANSITION_MS = 300;

IdleMode::IdleMode(PositionDetector* detector, LEDInterface* leds)
    : positionDetector(detector), ledInterface(leds),
      currentPosition(Position::DEFAULT), previousPosition(Position::DEFAULT),
      positionChangedTime(0), nullPositionStartTime(0), inNullCountdown(false),
      interpolationEnabled(true) {
    
    // Initialize colors
    currentColor = CRGB(255, 255, 255); // White (default)
    targetColor = currentColor;
    previousColor = currentColor;
    colorTransitionStartTime = 0;
}

void IdleMode::initialize() {
    // Reset all state
    currentPosition = positionDetector->getCurrentPosition();
    previousPosition = currentPosition;
    positionChangedTime = millis();
    nullPositionStartTime = 0;
    inNullCountdown = false;
    
    // Initialize colors
    targetColor = getPositionColor(currentPosition);
    currentColor = targetColor;
    previousColor = currentColor;
    
    // Turn off all LEDs initially
    ledInterface->clearAllPixels();
    
    // Set initial LED state
    renderLEDs();
}

void IdleMode::update() {
    // Get current position from detector
    Position newPosition = positionDetector->getCurrentPosition();
    
    // Handle position changes
    if (newPosition != currentPosition) {
        previousPosition = currentPosition;
        currentPosition = newPosition;
        positionChangedTime = millis();
        
        // Update colors for transition
        previousColor = currentColor;
        targetColor = getPositionColor(currentPosition);
        
        if (!interpolationEnabled) {
            // Immediate color change if interpolation disabled
            currentColor = targetColor;
        } else {
            // Start transition if interpolation enabled
            colorTransitionStartTime = millis();
        }
    }
    
    // Update color interpolation if enabled
    if (interpolationEnabled) {
        updateColorTransition();
    }
    
    // Render LEDs
    renderLEDs();
}

ModeTransition IdleMode::checkForTransition() {
    // Check for CalmOffer gesture (triggers Invocation Mode)
    if (detectCalmOfferGesture()) {
        return ModeTransition::TO_INVOCATION;
    }
    
    // Check for LongNull gesture (triggers Freecast Mode)
    if (detectLongNullGesture()) {
        return ModeTransition::TO_FREECAST;
    }
    
    return ModeTransition::NONE;
}

void IdleMode::setInterpolationEnabled(bool enabled) {
    interpolationEnabled = enabled;
    if (!enabled) {
        // If disabled, immediately set current color to target color
        currentColor = targetColor;
    }
}

CRGB IdleMode::getPositionColor(Position position) {
    switch (position) {
        case Position::OFFER:    return CRGB(128, 0, 128);  // Purple
        case Position::CALM:     return CRGB(255, 255, 0);  // Yellow
        case Position::OATH:     return CRGB(255, 0, 0);    // Red
        case Position::DIG:      return CRGB(0, 255, 0);    // Green
        case Position::SHIELD:   return CRGB(0, 0, 255);    // Blue
        case Position::NULL_POS: return CRGB(255, 165, 0);  // Orange
        default:                 return CRGB(255, 255, 255); // White
    }
}

bool IdleMode::detectCalmOfferGesture() {
    // Simple logic: if we detect Offer within 1000ms of leaving Calm, recognize gesture
    if (previousPosition == Position::CALM && 
        currentPosition == Position::OFFER &&
        (millis() - positionChangedTime < 1000)) {
        return true;
    }
    return false;
}

bool IdleMode::detectLongNullGesture() {
    if (currentPosition == Position::NULL_POS) {
        if (previousPosition != Position::NULL_POS) {
            nullPositionStartTime = millis();
        }
        
        unsigned long nullDuration = millis() - nullPositionStartTime;
        
        // Start countdown animation after 3 seconds
        if (nullDuration > 3000 && !inNullCountdown) {
            inNullCountdown = true;
        }
        
        // Trigger after 5 seconds
        if (nullDuration > 5000) {
            return true;
        }
    } else {
        // Reset if position changed
        inNullCountdown = false;
    }
    
    return false;
}

void IdleMode::updateColorTransition() {
    unsigned long elapsed = millis() - colorTransitionStartTime;
    
    if (elapsed >= COLOR_TRANSITION_MS) {
        // Transition complete
        currentColor = targetColor;
    } else {
        // Calculate interpolation progress (0.0 to 1.0)
        float progress = static_cast<float>(elapsed) / COLOR_TRANSITION_MS;
        
        // Linear interpolation between colors
        currentColor.r = previousColor.r + progress * (targetColor.r - previousColor.r);
        currentColor.g = previousColor.g + progress * (targetColor.g - previousColor.g);
        currentColor.b = previousColor.b + progress * (targetColor.b - previousColor.b);
    }
}

void IdleMode::renderLEDs() {
    // Handle LongNull countdown animation
    if (inNullCountdown) {
        unsigned long nullDuration = millis() - nullPositionStartTime;
        if (nullDuration > 3000 && nullDuration < 5000) {
            // Flash orange (500ms on/off)
            if ((millis() / 500) % 2 == 0) {
                // Orange flash
                for (uint8_t i = 0; i < 4; i++) {
                    ledInterface->setPixel(IDLE_LEDS[i], CRGB(255, 165, 0), IDLE_BRIGHTNESS);
                }
            } else {
                // Off phase
                for (uint8_t i = 0; i < 4; i++) {
                    ledInterface->setPixel(IDLE_LEDS[i], CRGB(0, 0, 0), 0);
                }
            }
            ledInterface->show();
            return;
        }
    }
    
    // Normal position display
    for (uint8_t i = 0; i < 4; i++) {
        ledInterface->setPixel(IDLE_LEDS[i], currentColor, IDLE_BRIGHTNESS);
    }
    ledInterface->show();
} 