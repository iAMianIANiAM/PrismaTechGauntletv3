#include "QuickCastSpellsMode.h"
#include "../core/Config.h"
#include "../core/SystemTypes.h"
#include "../hardware/HardwareManager.h"
// #include "../animation/AnimationController.h" // Not used
#include "../utils/DebugTools.h" // Added for DEBUG prints
#include <FastLED.h> // Needed for CRGB utilities if used

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
    
    // Clear LEDs on exit to ensure clean state for IdleMode
    hardwareManager_->setAllLEDs({Config::Colors::BLACK[0], Config::Colors::BLACK[1], Config::Colors::BLACK[2]});
    hardwareManager_->updateLEDs();
}

// --- Private Spell Rendering Methods ---

void QuickCastSpellsMode::renderRainbowBurst(uint32_t currentTime, uint32_t elapsedTime) {
    // TODO: Implement non-blocking, accelerating outward rainbow animation
    // Example placeholder: simple rainbow cycle
    uint8_t hue = (elapsedTime / 20) % 256; // Cycle hue over time
    // hardwareManager_->fillRainbow(hue, 10); // This function doesn't exist in HardwareManager
    // Placeholder: Set all LEDs to a changing color based on hue
    CHSV hsvColor = CHSV(hue, 255, 255); // Using FastLED's HSV type
    CRGB rgbColor; // Using FastLED's RGB type
    hsv2rgb_rainbow(hsvColor, rgbColor);
    hardwareManager_->setAllLEDs({rgbColor.r, rgbColor.g, rgbColor.b});
    hardwareManager_->updateLEDs(); 
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