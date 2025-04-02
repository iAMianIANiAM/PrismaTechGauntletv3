#include "QuickCastSpellsMode.h"
#include "../core/Config.h"
#include "../core/SystemTypes.h"
#include "../hardware/HardwareManager.h"
// #include "../animation/AnimationController.h" // Not used
#include "../utils/DebugTools.h" // Added for DEBUG prints
#include <FastLED.h> // Needed for CRGB utilities if used
#include <Arduino.h> // For math functions

QuickCastSpellsMode::QuickCastSpellsMode()
    : hardwareManager_(nullptr),
      // animationController_(nullptr), // Removed
      activeSpell_(SpellType::NONE),
      spellState_(SpellState::INACTIVE),
      spellStartTime_(0),
      spellDuration_(0),
      lastUpdateTime_(0) // Added for animation timing
    {}

bool QuickCastSpellsMode::init(HardwareManager* hardware) {
    hardwareManager_ = hardware;
    // animationController_ = animation; // Removed
    if (!hardwareManager_) {
        return false;
    }
    return true;
}

void QuickCastSpellsMode::enter(SpellType spellType) {
    activeSpell_ = spellType;
    spellState_ = SpellState::RUNNING; // Start immediately
    spellStartTime_ = millis();
    lastUpdateTime_ = spellStartTime_; // Initialize update timer

    hardwareManager_->setAllLEDs({Config::Colors::BLACK[0], Config::Colors::BLACK[1], Config::Colors::BLACK[2]});
    hardwareManager_->updateLEDs();

    switch (activeSpell_) {
        case SpellType::RAINBOW:
            spellDuration_ = Config::Spells::RAINBOW_DURATION_MS;
            // Initial setup for Rainbow Burst if needed
            DEBUG_PRINTLN("QuickCast: Entering Rainbow Burst");
            break;
        case SpellType::LIGHTNING:
            spellDuration_ = Config::Spells::LIGHTNING_DURATION_MS;
            // Initial setup for Lightning Blast if needed (e.g., initial flash)
            hardwareManager_->setAllLEDs({Config::Colors::UNKNOWN_COLOR[0], Config::Colors::UNKNOWN_COLOR[1], Config::Colors::UNKNOWN_COLOR[2]}); // Full brightness is implicit
            hardwareManager_->updateLEDs();
            // Delay slightly for flash visibility before starting crackle
            lastUpdateTime_ = millis() + 50; // Add slight offset for next update
            DEBUG_PRINTLN("QuickCast: Entering Lightning Blast");
            break;
        case SpellType::LUMINA:
            spellDuration_ = Config::Spells::LUMINA_DURATION_MS;
            // Initial setup for Lumina: Set 6 LEDs to white at 80%
            hardwareManager_->setAllLEDs({Config::Colors::BLACK[0], Config::Colors::BLACK[1], Config::Colors::BLACK[2]});
            // Set brightness first
            hardwareManager_->setBrightness(Config::Spells::LUMINA_BRIGHTNESS); 
            for (int i = 0; i < 6; ++i) { // Set first 6 LEDs
                 hardwareManager_->setLED(i, {Config::Colors::UNKNOWN_COLOR[0], Config::Colors::UNKNOWN_COLOR[1], Config::Colors::UNKNOWN_COLOR[2]});
            }
             hardwareManager_->updateLEDs();
            DEBUG_PRINTLN("QuickCast: Entering Lumina");
            break;
        default:
            spellState_ = SpellState::INACTIVE;
            activeSpell_ = SpellType::NONE;
            spellDuration_ = 0;
            DEBUG_PRINTLN("QuickCast: Entering with UNKNOWN spell type!");
            break;
    }
}

ModeTransition QuickCastSpellsMode::update() {
    if (spellState_ != SpellState::RUNNING) {
        return ModeTransition::NONE; 
    }

    uint32_t currentTime = millis();
    uint32_t elapsedTime = currentTime - spellStartTime_;
    // uint32_t deltaTime = currentTime - lastUpdateTime_; // Time since last update

    // Render the active spell animation
    switch (activeSpell_) {
        case SpellType::RAINBOW:
            renderRainbowBurst(currentTime, elapsedTime);
            break;
        case SpellType::LIGHTNING:
            renderLightningBlast(currentTime, elapsedTime);
            break;
        case SpellType::LUMINA:
            renderLumina(currentTime, elapsedTime);
            break;
        default:
            break; // Should not happen
    }

    // Check if spell duration has expired
    if (elapsedTime >= spellDuration_) {
        DEBUG_PRINTF("QuickCast spell '%d' duration complete. Transitioning to IDLE.\n", static_cast<int>(activeSpell_));
        exit(); // Perform cleanup before transition
        return ModeTransition::TO_IDLE;
    }
    
    // No special exit for Lumina needed per guide (only timeout)

    lastUpdateTime_ = currentTime; // Store time for next delta calculation
    return ModeTransition::NONE; // Stay in this mode
}

void QuickCastSpellsMode::exit() {
    DEBUG_PRINTLN("Exiting QuickCastSpellsMode.");
        
    spellState_ = SpellState::INACTIVE;
    activeSpell_ = SpellType::NONE;
    spellStartTime_ = 0;
    spellDuration_ = 0;
    
    // Reset brightness to default before clearing LEDs
    hardwareManager_->setBrightness(Config::DEFAULT_BRIGHTNESS);
    
    // Clear LEDs on exit to ensure clean state for IdleMode
    hardwareManager_->setAllLEDs({Config::Colors::BLACK[0], Config::Colors::BLACK[1], Config::Colors::BLACK[2]});
    hardwareManager_->updateLEDs();
}

/**
 * @brief Explicitly renders LEDs for this mode.
 * This method exists for API consistency with other modes.
 * While update() already handles the spell-specific rendering, this ensures
 * that the controller can call renderLEDs() on all mode types consistently.
 */
void QuickCastSpellsMode::renderLEDs() {
    // LEDs are already updated within the spell render methods
    // This method exists for API consistency with other modes
    hardwareManager_->updateLEDs();
    
    DEBUG_PRINTLN("QuickCastSpellsMode::renderLEDs called");
}

// --- Private Spell Rendering Methods ---

void QuickCastSpellsMode::renderRainbowBurst(uint32_t currentTime, uint32_t elapsedTime) {
    // Calculate elapsed time and phase
    uint8_t phase = (elapsedTime < 2000) ? 0 : 
                    (elapsedTime < 4000) ? 1 :
                    (elapsedTime < 6000) ? 2 :
                    (elapsedTime < 8000) ? 3 : 4;
    
    // Determine if spell is complete
    if (elapsedTime >= Config::Spells::RAINBOW_DURATION_MS) {
        DEBUG_PRINTLN("Rainbow Burst completed duration");
        return;
    }
    
    // Render appropriate phase animation
    switch (phase) {
        case 0: renderRainbowPhase1(elapsedTime); break;  // 0-2s: Slow pulse & swirl
        case 1: renderRainbowPhase2(elapsedTime); break;  // 2-4s: Medium pulse & swirl
        case 2: renderRainbowPhase3(elapsedTime); break;  // 4-6s: Fast pulse & swirl
        case 3: renderRainbowPhase4(elapsedTime - 6000); break;  // 6-8s: Burst & colored pops
    }
}

void QuickCastSpellsMode::renderRainbowPhase1(unsigned long elapsed) {
    // 0-2s: Slow pulse (1Hz) & slow swirl (1 rotation/s)
    float pulseProgress = (elapsed % 1000) / 1000.0f;
    float brightness = 0.6f + 0.4f * sin(pulseProgress * 2 * PI);
    
    float rotationProgress = (elapsed % 1000) / 1000.0f;
    renderRainbowSwirl(rotationProgress, brightness);
    
    DEBUG_PRINTLN("Rainbow Phase 1");
}

void QuickCastSpellsMode::renderRainbowPhase2(unsigned long elapsed) {
    // 2-4s: Medium pulse (2Hz) & medium swirl (2 rotations/s)
    float pulseProgress = (elapsed % 500) / 500.0f;
    float brightness = 0.6f + 0.4f * sin(pulseProgress * 2 * PI);
    
    float rotationProgress = (elapsed % 500) / 500.0f;
    renderRainbowSwirl(rotationProgress, brightness);
    
    DEBUG_PRINTLN("Rainbow Phase 2");
}

void QuickCastSpellsMode::renderRainbowPhase3(unsigned long elapsed) {
    // 4-6s: Fast pulse (4Hz) & fast swirl (4 rotations/s)
    float pulseProgress = (elapsed % 250) / 250.0f;
    float brightness = 0.6f + 0.4f * sin(pulseProgress * 2 * PI);
    
    float rotationProgress = (elapsed % 250) / 250.0f;
    renderRainbowSwirl(rotationProgress, brightness);
    
    DEBUG_PRINTLN("Rainbow Phase 3");
}

void QuickCastSpellsMode::renderRainbowPhase4(unsigned long elapsed) {
    // 6-8s: White burst fading + colored pops
    if (elapsed < 50) {
        // Initial white burst at 60% brightness
        DEBUG_PRINTLN("Rainbow WHITE BURST!");
        for (int i = 0; i < Config::NUM_LEDS; i++) {
            Color white = {153, 153, 153}; // 60% white
            hardwareManager_->setLED(i, white);
        }
        return;
    }
    
    // Fade out 6 LEDs (even indices: 0,2,4,6,8,10)
    float fadeProgress = min(1.0f, elapsed / 2000.0f);
    uint8_t whiteBrightness = 153 * (1.0f - fadeProgress);
    
    // Set fading white LEDs (even indices)
    for (int i = 0; i < Config::NUM_LEDS; i += 2) {
        Color fadeWhite = {whiteBrightness, whiteBrightness, whiteBrightness};
        hardwareManager_->setLED(i, fadeWhite);
    }
    
    // Colored pop sequence - predetermined timings for "random" appearance
    // Each LED gets a ~250ms "pop" of color then turns off
    renderColorPop(1, 300, elapsed, 255, 0, 0);      // LED 1: Red at 300ms
    renderColorPop(7, 600, elapsed, 255, 255, 0);    // LED 7: Yellow at 600ms
    renderColorPop(3, 900, elapsed, 0, 255, 0);      // LED 3: Green at 900ms
    renderColorPop(9, 1200, elapsed, 0, 0, 255);     // LED 9: Blue at 1200ms
    renderColorPop(5, 1500, elapsed, 128, 0, 255);   // LED 5: Purple at 1500ms
    renderColorPop(11, 1800, elapsed, 255, 105, 180); // LED 11: Pink at 1800ms
    
    DEBUG_PRINTLN("Rainbow Phase 4");
}

void QuickCastSpellsMode::renderRainbowSwirl(float progress, float brightness) {
    // Render rainbow pattern with given rotation progress and brightness
    for (int i = 0; i < Config::NUM_LEDS; i++) {
        float hue = fmod(progress + (i / (float)Config::NUM_LEDS), 1.0f);
        uint8_t r, g, b;
        hsvToRgb(hue, 1.0f, brightness, r, g, b);
        Color rainbow = {r, g, b};
        hardwareManager_->setLED(i, rainbow);
    }
    hardwareManager_->updateLEDs();
}

void QuickCastSpellsMode::renderColorPop(uint8_t led, 
                                       unsigned long popTime, 
                                       unsigned long elapsed,
                                       uint8_t r, uint8_t g, uint8_t b) {
    // If within the pop window, show color, otherwise off
    if (elapsed >= popTime && elapsed < popTime + 250) {
        Color popColor = {r, g, b};
        hardwareManager_->setLED(led, popColor);
    } else if (elapsed >= popTime + 250) {
        Color black = {0, 0, 0};
        hardwareManager_->setLED(led, black);
    }
}

void QuickCastSpellsMode::hsvToRgb(float h, float s, float v, 
                                 uint8_t &r, uint8_t &g, uint8_t &b) {
    // Standard HSV to RGB conversion
    int i = int(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    
    float r_f, g_f, b_f;
    
    switch (i % 6) {
        case 0: r_f = v; g_f = t; b_f = p; break;
        case 1: r_f = q; g_f = v; b_f = p; break;
        case 2: r_f = p; g_f = v; b_f = t; break;
        case 3: r_f = p; g_f = q; b_f = v; break;
        case 4: r_f = t; g_f = p; b_f = v; break;
        case 5: r_f = v; g_f = p; b_f = q; break;
    }
    
    r = r_f * 255;
    g = g_f * 255;
    b = b_f * 255;
}

void QuickCastSpellsMode::renderLightningBlast(uint32_t currentTime, uint32_t elapsedTime) {
    // Skip initial flash period
    if (elapsedTime < 50) { 
        return; // Still showing initial flash
    }

    // TODO: Implement non-blocking lightning crackle effect
    // Example placeholder: Random flashes on some LEDs
    if ((currentTime / 100) % 2 == 0) { // Flash roughly 5 times/sec
        hardwareManager_->setAllLEDs({Config::Colors::BLACK[0], Config::Colors::BLACK[1], Config::Colors::BLACK[2]});
        // Set brightness to full for flash
        hardwareManager_->setBrightness(255);
        for(int i=0; i<Config::NUM_LEDS; ++i) {
            if(random(0, 5) == 0) { // 1 in 5 chance for a pixel to light up
                // Choose a lightning color: red, blue, purple, white
                uint8_t colorType = random(0, 4);
                Color c = {Config::Colors::BLACK[0], Config::Colors::BLACK[1], Config::Colors::BLACK[2]};
                if (colorType == 0) c = {Config::Colors::RED[0], Config::Colors::RED[1], Config::Colors::RED[2]};
                else if (colorType == 1) c = {Config::Colors::BLUE[0], Config::Colors::BLUE[1], Config::Colors::BLUE[2]};
                else if (colorType == 2) c = {Config::Colors::PURPLE[0], Config::Colors::PURPLE[1], Config::Colors::PURPLE[2]};
                else c = {Config::Colors::UNKNOWN_COLOR[0], Config::Colors::UNKNOWN_COLOR[1], Config::Colors::UNKNOWN_COLOR[2]};
                hardwareManager_->setLED(i, c); // Brightness is set globally
            }
        }
        hardwareManager_->updateLEDs();
    } else if ((currentTime / 100) % 2 != 0 && (lastUpdateTime_ / 100) % 2 == 0) {
         // Turn off LEDs after flash interval if they were just on
         hardwareManager_->setAllLEDs({Config::Colors::BLACK[0], Config::Colors::BLACK[1], Config::Colors::BLACK[2]});
         hardwareManager_->updateLEDs();
    }
}

void QuickCastSpellsMode::renderLumina(uint32_t currentTime, uint32_t elapsedTime) {
    // Calculate fade progress (0.0 to 1.0) based on elapsed time
    float fadeProgress = (float)elapsedTime / (float)spellDuration_;
    if (fadeProgress > 1.0f) fadeProgress = 1.0f;

    // Fade amount increases as time progresses (fade faster near the end? TBD)
    // Simple linear fade for now:
    uint8_t currentBrightness = Config::Spells::LUMINA_BRIGHTNESS * (1.0f - fadeProgress);

    // Re-set the 6 LEDs with the new brightness
    hardwareManager_->setAllLEDs({Config::Colors::BLACK[0], Config::Colors::BLACK[1], Config::Colors::BLACK[2]});
    hardwareManager_->setBrightness(currentBrightness);
    for (int i = 0; i < 6; ++i) { 
        hardwareManager_->setLED(i, {Config::Colors::UNKNOWN_COLOR[0], Config::Colors::UNKNOWN_COLOR[1], Config::Colors::UNKNOWN_COLOR[2]});
    }
    hardwareManager_->updateLEDs();
} 