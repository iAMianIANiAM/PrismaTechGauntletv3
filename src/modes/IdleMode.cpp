#include "IdleMode.h"
#include "../core/Config.h"
#include "../core/SystemTypes.h"

// Define the static constants
const uint8_t IdleMode::IDLE_LEDS[4] = {0, 3, 6, 9};
const uint8_t IdleMode::IDLE_BRIGHTNESS = 204; // 80% of 255
const uint16_t IdleMode::COLOR_TRANSITION_MS = 300;

IdleMode::IdleMode() 
    : hardwareManager(nullptr),
      positionDetector(nullptr),
      currentPosition({POS_UNKNOWN, 0, 0}),
      previousPosition({POS_UNKNOWN, 0, 0}),
      positionChangedTime(0),
      nullPositionStartTime(0),
      inNullCountdown(false),
      shieldPositionStartTime(0),
      inShieldCountdown(false),
      calmExitTime(0),
      inCalmOfferWindow(false),
      currentColor(CRGB::Black),
      targetColor(CRGB::Black),
      previousColor(CRGB::Black),
      colorTransitionStartTime(0),
      interpolationEnabled(true)
{
}

bool IdleMode::init(HardwareManager* hardware, UltraBasicPositionDetector* detector) {
    // Store dependencies
    hardwareManager = hardware;
    positionDetector = detector;
    
    if (!hardwareManager || !positionDetector) {
        return false;
    }
    
    return true;
}

void IdleMode::initialize() {
    // Initialize with default values
    currentPosition = {POS_UNKNOWN, 0, 0};
    previousPosition = {POS_UNKNOWN, 0, 0};
    positionChangedTime = millis();
    nullPositionStartTime = 0;
    inNullCountdown = false;
    shieldPositionStartTime = 0;
    inShieldCountdown = false;
    
    // Initialize CalmOffer tracking
    calmExitTime = 0;
    inCalmOfferWindow = false;
    
    // Set initial colors
    currentColor = CRGB::Black;
    targetColor = getPositionColor(POS_UNKNOWN);
    previousColor = CRGB::Black;
    colorTransitionStartTime = millis();
    
    // Set LEDs to initial state
    renderLEDs();
}

void IdleMode::update() {
    // Update the detector for position detection
    PositionReading newPosition = positionDetector->update();
    
    // If position has changed
    if (newPosition.position != currentPosition.position) {
        // Update position tracking
        previousPosition = currentPosition;
        currentPosition = newPosition;
        positionChangedTime = millis();
        
        // CalmOffer tracking - if leaving CALM position
        if (previousPosition.position == POS_CALM && currentPosition.position != POS_CALM) {
            calmExitTime = millis();
            inCalmOfferWindow = true;
        }
        // If returning to CALM, cancel the CalmOffer window
        else if (currentPosition.position == POS_CALM) {
            inCalmOfferWindow = false;
        }
        
        // Start null position timing if we've entered the null position
        // Disabled as part of LongNull to LongShield transition
        // if (newPosition.position == POS_NULLPOS && previousPosition.position != POS_NULLPOS) {
        //     nullPositionStartTime = millis();
        //     inNullCountdown = false;
        // }
        
        // Start shield position timing if we've entered the shield position
        if (newPosition.position == POS_SHIELD && previousPosition.position != POS_SHIELD) {
            shieldPositionStartTime = millis();
            inShieldCountdown = false;
        }
        
        // Set new target color based on position
        previousColor = currentColor;
        targetColor = getPositionColor(newPosition.position);
        colorTransitionStartTime = millis();
    }
    
    // Update color transition
    updateColorTransition();
    
    // Check for null position countdown trigger (after 3 seconds)
    // Disabled as part of LongNull to LongShield transition
    // if (currentPosition.position == POS_NULLPOS && !inNullCountdown) {
    //     unsigned long nullDuration = millis() - nullPositionStartTime;
    //     if (nullDuration >= 3000) { // 3 second threshold
    //         inNullCountdown = true;
    //     }
    // }
    
    // Check for shield position countdown trigger (after 3 seconds)
    if (currentPosition.position == POS_SHIELD && !inShieldCountdown) {
        unsigned long shieldDuration = millis() - shieldPositionStartTime;
        if (shieldDuration >= Config::LONGSHIELD_WARNING_MS) { // 3 second threshold
            inShieldCountdown = true;
        }
    }
    
    // Check if CalmOffer window has expired
    if (inCalmOfferWindow && (millis() - calmExitTime >= 1000)) {
        inCalmOfferWindow = false;
    }
    
    // Display the current position
    renderLEDs();
}

ModeTransition IdleMode::checkForTransition() {
    // Check for Calm->Offer gesture (triggers Invocation mode)
    if (detectCalmOfferGesture()) {
        return ModeTransition::TO_INVOCATION;
    }
    
    // Check for long SHIELD gesture (triggers Freecast mode)
    if (detectLongShieldGesture()) {
        return ModeTransition::TO_FREECAST;
    }
    
    return ModeTransition::NONE;
}

void IdleMode::renderLEDs() {
    // First, clear all LEDs
    hardwareManager->setAllLEDs({0, 0, 0});
    
    // For NULL position with countdown, show special animation
    // Disabled as part of LongNull to LongShield transition
    // if (currentPosition.position == POS_NULLPOS && inNullCountdown) {
    //     // Calculate how long we've been in NULL position
    //     unsigned long nullDuration = millis() - nullPositionStartTime;
    //     
    //     // If we're in the 3-5 second window, show flashing countdown
    //     if (nullDuration >= 3000 && nullDuration < 5000) {
    //         // Flash at 2Hz (250ms on, 250ms off)
    //         if ((millis() / 250) % 2 == 0) {
    //             // On phase - show orange
    //             Color orange = {255, 165, 0};
    //             for (int i = 0; i < 4; i++) {
    //                 hardwareManager->setLED(IDLE_LEDS[i], orange);
    //             }
    //         }
    //         // Off phase is handled by the initial clear
    //     }
    // } 
    // For SHIELD position with countdown, show special animation
    if (currentPosition.position == POS_SHIELD && inShieldCountdown) {
        // Calculate how long we've been in SHIELD position
        unsigned long shieldDuration = millis() - shieldPositionStartTime;
        
        // If we're in the 3-5 second window, show flashing countdown
        if (shieldDuration >= Config::LONGSHIELD_WARNING_MS && shieldDuration < Config::LONGSHIELD_TIME_MS) {
            // Flash at 2Hz (250ms on, 250ms off)
            if ((millis() / 250) % 2 == 0) {
                // Use Shield blue color
                Color blue = {Config::Colors::SHIELD_COLOR[0], 
                              Config::Colors::SHIELD_COLOR[1], 
                              Config::Colors::SHIELD_COLOR[2]};
                for (int i = 0; i < 4; i++) {
                    hardwareManager->setLED(IDLE_LEDS[i], blue);
                }
            }
            // Off phase is handled by the initial clear
        }
    }
    else {
        // Normal position display - convert CRGB to Color struct
        Color displayColor = {
            currentColor.r,
            currentColor.g,
            currentColor.b
        };
        
        // Set the four indicator LEDs
        for (int i = 0; i < 4; i++) {
            hardwareManager->setLED(IDLE_LEDS[i], displayColor);
        }
    }
    
    // Update the LED display
    hardwareManager->updateLEDs();
}

CRGB IdleMode::getPositionColor(uint8_t position) {
    switch (position) {
        case POS_OFFER:
            return CRGB(128, 0, 255);  // Purple (more blue)
        case POS_CALM:
            return CRGB(0, 0, 255);    // Blue (was Yellow)
        case POS_OATH:
            return CRGB(255, 255, 0);  // Yellow (was Red)
        case POS_DIG:
            return CRGB(0, 255, 0);    // Green (unchanged)
        case POS_SHIELD:
            return CRGB(255, 105, 180); // Pink (was Blue)
        case POS_NULLPOS:
            return CRGB(255, 0, 0);    // Red (was Orange)
        case POS_UNKNOWN:
        default:
            return CRGB(255, 255, 255); // White
    }
}

bool IdleMode::detectCalmOfferGesture() {
    // Check if we're in a CalmOffer window and in the OFFER position
    if (inCalmOfferWindow && currentPosition.position == POS_OFFER) {
        // Check if we're within the 1000ms window
        if (millis() - calmExitTime < 1000) {
            // Reset the window to prevent repeated triggers
            inCalmOfferWindow = false;
            
            #ifdef DEBUG_MODE
            Serial.print(F("CalmOffer detected! Window duration: "));
            Serial.print(millis() - calmExitTime);
            Serial.println(F("ms"));
            #endif
            
            return true;
        }
    }
    
    return false;
}

bool IdleMode::detectLongShieldGesture() {
    // Check if we're in SHIELD position and have been for the required time
    if (inShieldCountdown && currentPosition.position == POS_SHIELD) {
        unsigned long currentTime = millis();
        unsigned long shieldDuration = currentTime - shieldPositionStartTime;
        
        // Check if we've held SHIELD long enough
        if (shieldDuration >= Config::LONGSHIELD_TIME_MS) {
            inShieldCountdown = false; // Reset to prevent repeated triggers
            return true;
        }
    }
    
    return false;
}

void IdleMode::updateColorTransition() {
    // If interpolation is disabled, just snap to target color
    if (!interpolationEnabled) {
        currentColor = targetColor;
        return;
    }
    
    // Calculate how far through the transition we are
    unsigned long currentTime = millis();
    unsigned long transitionTime = currentTime - colorTransitionStartTime;
    
    // If past transition time, just use target color
    if (transitionTime >= COLOR_TRANSITION_MS) {
        currentColor = targetColor;
        return;
    }
    
    // Otherwise interpolate between previous and target color
    float progress = (float)transitionTime / COLOR_TRANSITION_MS;
    
    // Linear interpolation of RGB values
    currentColor.r = previousColor.r + (targetColor.r - previousColor.r) * progress;
    currentColor.g = previousColor.g + (targetColor.g - previousColor.g) * progress;
    currentColor.b = previousColor.b + (targetColor.b - previousColor.b) * progress;
}

void IdleMode::setInterpolationEnabled(bool enabled) {
    interpolationEnabled = enabled;
    
    // If disabling, snap to target color
    if (!enabled) {
        currentColor = targetColor;
    }
}

bool IdleMode::detectLongNullGesture() {
    // Disabled as of 202503312330 as part of LongNull to LongShield transition
    // See chronicle_v6.md for details
    return false;
}

#ifdef DEBUG_MODE
void IdleMode::printStatus() const {
    Serial.println(F("=== Idle Mode Status ==="));
    Serial.print(F("Current Position: "));
    
    switch (currentPosition.position) {
        case POS_OFFER:    Serial.println(F("OFFER (Purple)")); break;
        case POS_CALM:     Serial.println(F("CALM (Blue)")); break;
        case POS_OATH:     Serial.println(F("OATH (Yellow)")); break;
        case POS_DIG:      Serial.println(F("DIG (Green)")); break;
        case POS_SHIELD:   Serial.println(F("SHIELD (Pink)")); break;
        case POS_NULLPOS:  Serial.println(F("NULL (Red)")); break;
        case POS_UNKNOWN:  Serial.println(F("UNKNOWN (White)")); break;
        default:           Serial.println(F("DEFAULT (White)")); break;
    }
    
    Serial.print(F("In Null Countdown: "));
    Serial.println(inNullCountdown ? F("YES") : F("NO"));
    
    Serial.print(F("In CalmOffer Window: "));
    Serial.println(inCalmOfferWindow ? F("YES") : F("NO"));
    
    if (inCalmOfferWindow) {
        unsigned long windowDuration = millis() - calmExitTime;
        Serial.print(F("CalmOffer Window Duration: "));
        Serial.print(windowDuration);
        Serial.println(F("ms"));
    }
    
    if (currentPosition.position == POS_NULLPOS) {
        unsigned long nullDuration = millis() - nullPositionStartTime;
        Serial.print(F("Null Position Duration: "));
        Serial.print(nullDuration);
        Serial.println(F("ms"));
    }
    
    Serial.println(F("====================="));
}
#endif 