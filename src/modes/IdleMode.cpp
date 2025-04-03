#include "IdleMode.h"
#include "../core/Config.h"
#include "../core/SystemTypes.h"
#include "../diagnostics/VisualDebugIndicator.h"
// GestureTransitionTracker is included via IdleMode.h

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
      shieldPositionStartTime(0),
      inShieldCountdown(false),
      calmOfferTracker_(POS_CALM, POS_OFFER, Config::QUICKCAST_WINDOW_MS, SpellTransition::TO_RAINBOW),
      digOathTracker_(POS_DIG, POS_OATH, Config::QUICKCAST_WINDOW_MS, SpellTransition::TO_LIGHTNING),
      nullShieldTracker_(POS_NULLPOS, POS_SHIELD, Config::QUICKCAST_WINDOW_MS, SpellTransition::TO_LUMINA),
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
    shieldPositionStartTime = 0;
    inShieldCountdown = false;
    
    // Reset QuickCast trackers
    resetAllSpellTrackers();

    // Set initial colors
    currentColor = CRGB::Black;
    targetColor = getPositionColor(POS_UNKNOWN);
    previousColor = CRGB::Black;
    colorTransitionStartTime = millis();
    
    // Explicitly set brightness for Idle mode
    hardwareManager->setBrightness(IDLE_BRIGHTNESS);
    
    // Set LEDs to initial state
    renderLEDs();
}

void IdleMode::update() {
    // Update the detector for position detection
    PositionReading newPosition = positionDetector->update();
    uint32_t currentTime = millis(); // Get current time once

    // If position has changed
    if (newPosition.position != currentPosition.position) {
        // Update position tracking
        previousPosition = currentPosition;
        currentPosition = newPosition;
        positionChangedTime = currentTime;
        
        // Start shield position timing if we've entered the shield position
        if (newPosition.position == POS_SHIELD && previousPosition.position != POS_SHIELD) {
            shieldPositionStartTime = currentTime;
            inShieldCountdown = false;
        }
        
        // Set new target color based on position
        previousColor = currentColor;
        targetColor = getPositionColor(newPosition.position);
        colorTransitionStartTime = currentTime;
    }
    
    // Update color transition
    updateColorTransition();
    
    // Update QuickCast gesture trackers
    calmOfferTracker_.update(static_cast<HandPosition>(currentPosition.position), currentTime);
    digOathTracker_.update(static_cast<HandPosition>(currentPosition.position), currentTime);
    nullShieldTracker_.update(static_cast<HandPosition>(currentPosition.position), currentTime);

    // Check for shield position countdown trigger (after 3 seconds) - Keep for Freecast transition
    if (currentPosition.position == POS_SHIELD && !inShieldCountdown) {
        unsigned long shieldDuration = currentTime - shieldPositionStartTime;
        if (shieldDuration >= Config::LONGSHIELD_WARNING_MS) { // 3 second threshold
            inShieldCountdown = true;
            
            // Update visual debug indicator to show shield countdown
            float progress = (float)shieldDuration / (float)Config::LONGSHIELD_TIME_MS;
            VisualDebugIndicator::updateGestureProgress(progress, 2);
        }
    }
    
    // Display the current position
    renderLEDs();
}

ModeTransition IdleMode::checkForTransition() {
    // Check for long SHIELD gesture (triggers Freecast mode)
    if (detectLongShieldGesture()) {
        return ModeTransition::TO_FREECAST;
    }
    
    // Note: Invocation mode transition (originally CalmOffer) is now handled 
    //       by QuickCast spells, potentially leading to a different mode.
    //       Keeping TO_INVOCATION in ModeTransition enum for now, but not triggered here.
    
    return ModeTransition::NONE;
}

SpellTransition IdleMode::checkForSpellTransition() {
    SpellTransition detectedTransition = SpellTransition::NONE;

    if (calmOfferTracker_.isTransitionComplete()) {
        detectedTransition = calmOfferTracker_.getTransitionType();
        resetAllSpellTrackers(); // Reset all trackers once one completes
    } else if (digOathTracker_.isTransitionComplete()) {
        detectedTransition = digOathTracker_.getTransitionType();
        resetAllSpellTrackers();
    } else if (nullShieldTracker_.isTransitionComplete()) {
        detectedTransition = nullShieldTracker_.getTransitionType();
        resetAllSpellTrackers();
    }

    return detectedTransition;
}

void IdleMode::resetAllSpellTrackers() {
    calmOfferTracker_.reset();
    digOathTracker_.reset();
    nullShieldTracker_.reset();
}

void IdleMode::renderLEDs() {
    // First, clear all LEDs
    hardwareManager->setAllLEDs({0, 0, 0});
    
    // For SHIELD position with countdown, show special animation (Keep for Freecast transition)
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
            return CRGB(Config::Colors::OFFER_COLOR[0], Config::Colors::OFFER_COLOR[1], Config::Colors::OFFER_COLOR[2]);
        case POS_CALM:
            return CRGB(Config::Colors::CALM_COLOR[0], Config::Colors::CALM_COLOR[1], Config::Colors::CALM_COLOR[2]);
        case POS_OATH:
            return CRGB(Config::Colors::OATH_COLOR[0], Config::Colors::OATH_COLOR[1], Config::Colors::OATH_COLOR[2]);
        case POS_DIG:
            return CRGB(Config::Colors::DIG_COLOR[0], Config::Colors::DIG_COLOR[1], Config::Colors::DIG_COLOR[2]);
        case POS_SHIELD:
            return CRGB(Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]);
        case POS_NULLPOS:
            return CRGB(Config::Colors::NULL_COLOR[0], Config::Colors::NULL_COLOR[1], Config::Colors::NULL_COLOR[2]);
        case POS_UNKNOWN:
        default:
            return CRGB(Config::Colors::UNKNOWN_COLOR[0], Config::Colors::UNKNOWN_COLOR[1], Config::Colors::UNKNOWN_COLOR[2]);
    }
}

bool IdleMode::detectLongShieldGesture() {
    // Check if we are in the SHIELD position
    if (currentPosition.position == POS_SHIELD) {
        unsigned long shieldDuration = millis() - shieldPositionStartTime;
        
        // Check if the duration exceeds the required time (e.g., 5 seconds)
        if (shieldDuration >= Config::LONGSHIELD_TIME_MS) {
            // Also check if the countdown flag was set (meaning we passed the warning time)
            if (inShieldCountdown) {
                // Reset state before transition
                inShieldCountdown = false; 
                shieldPositionStartTime = 0; 
                return true;
            }
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
    
    Serial.println(F("====================="));
}
#endif 