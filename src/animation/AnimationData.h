#ifndef ANIMATION_DATA_H
#define ANIMATION_DATA_H

#include <Arduino.h>
#include "../core/SystemTypes.h"

/**
 * @brief Single frame of LED animation data
 */
struct AnimationFrame {
  uint8_t ledValues[12][3];  // RGB values for each LED
  uint8_t duration;          // Frame duration in 10ms units
};

// Pre-defined colors for animations
namespace AnimColors {
  // Basic colors
  const PROGMEM uint8_t BLACK[3] = {0, 0, 0};
  const PROGMEM uint8_t WHITE[3] = {255, 255, 255};
  const PROGMEM uint8_t RED[3] = {255, 0, 0};          // Same as NULL position
  const PROGMEM uint8_t GREEN[3] = {0, 255, 0};        // Same as DIG position
  const PROGMEM uint8_t BLUE[3] = {0, 0, 255};         // Same as CALM position
  const PROGMEM uint8_t YELLOW[3] = {255, 255, 0};     // Same as OATH position
  const PROGMEM uint8_t CYAN[3] = {0, 255, 255};
  const PROGMEM uint8_t MAGENTA[3] = {255, 0, 255};
  const PROGMEM uint8_t PURPLE[3] = {128, 0, 255};     // Same as OFFER position
  const PROGMEM uint8_t ORANGE[3] = {255, 165, 0};
  const PROGMEM uint8_t PINK[3] = {255, 105, 180};     // Same as SHIELD position
  
  // Position colors (matching Config.h)
  const PROGMEM uint8_t OFFER[3] = {128, 0, 255};    // Purple (more blue)
  const PROGMEM uint8_t CALM[3] = {0, 0, 255};       // Blue (was Yellow)
  const PROGMEM uint8_t OATH[3] = {255, 255, 0};     // Yellow (was Red)
  const PROGMEM uint8_t DIG[3] = {0, 255, 0};        // Green (unchanged)
  const PROGMEM uint8_t SHIELD[3] = {255, 105, 180}; // Pink (was Blue)
  const PROGMEM uint8_t NULL_POS[3] = {255, 0, 0};   // Red (was Orange)
  
  // Spell effect colors
  const PROGMEM uint8_t INFERNO_BASE[3] = {255, 60, 0};  // Deep orange-red
  const PROGMEM uint8_t INFERNO_ACCENT[3] = {255, 220, 0}; // Yellow-orange
  const PROGMEM uint8_t BLIZZARD_BASE[3] = {0, 150, 255};  // Deep blue
  const PROGMEM uint8_t BLIZZARD_ACCENT[3] = {200, 240, 255}; // Light blue
  const PROGMEM uint8_t GROWTH_BASE[3] = {0, 180, 0};     // Medium green
  const PROGMEM uint8_t GROWTH_ACCENT[3] = {180, 255, 0}; // Yellow-green
  const PROGMEM uint8_t DETECT_BASE[3] = {128, 0, 128};   // Purple
  const PROGMEM uint8_t SCAN_BEAM[3] = {255, 255, 255};   // White beam
}

// Transition animations stored in program memory

// Transition to Invocation (purple flash)
const PROGMEM AnimationFrame invocationTransition[] = {
  // Frame 1: All LEDs purple (50% brightness)
  {
    {{64, 0, 64}, {64, 0, 64}, {64, 0, 64}, {64, 0, 64},
     {64, 0, 64}, {64, 0, 64}, {64, 0, 64}, {64, 0, 64},
     {64, 0, 64}, {64, 0, 64}, {64, 0, 64}, {64, 0, 64}}, 
    5  // 50ms duration
  },
  // Frame 2: All LEDs purple (100% brightness)
  {
    {{128, 0, 128}, {128, 0, 128}, {128, 0, 128}, {128, 0, 128},
     {128, 0, 128}, {128, 0, 128}, {128, 0, 128}, {128, 0, 128},
     {128, 0, 128}, {128, 0, 128}, {128, 0, 128}, {128, 0, 128}}, 
    10 // 100ms duration
  },
  // Frame 3: All LEDs off
  {
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
     {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
     {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, 
    5  // 50ms duration
  }
};

// Transition to Freecast (shield blue flash)
const PROGMEM AnimationFrame freecastTransition[] = {
  // Frame 1: All LEDs shield blue (50% brightness)
  {
    {{Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}, 
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}, 
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}, 
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2},
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}, 
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}, 
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}, 
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2},
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}, 
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}, 
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}, 
     {Config::Colors::SHIELD_COLOR[0]/2, Config::Colors::SHIELD_COLOR[1]/2, Config::Colors::SHIELD_COLOR[2]/2}}, 
    5  // 50ms duration
  },
  // Frame 2: All LEDs shield blue (100% brightness)
  {
    {{Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}, 
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}, 
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}, 
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]},
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}, 
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}, 
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}, 
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]},
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}, 
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}, 
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}, 
     {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]}}, 
    10 // 100ms duration
  },
  // Frame 3: All LEDs off
  {
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
     {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
     {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, 
    5  // 50ms duration
  }
};

// Checkpoint flash animation (camera flash effect)
const PROGMEM AnimationFrame checkpointFlash[] = {
  // Frame 1: All LEDs white (25% brightness)
  {
    {{64, 64, 64}, {64, 64, 64}, {64, 64, 64}, {64, 64, 64},
     {64, 64, 64}, {64, 64, 64}, {64, 64, 64}, {64, 64, 64},
     {64, 64, 64}, {64, 64, 64}, {64, 64, 64}, {64, 64, 64}}, 
    2  // 20ms duration
  },
  // Frame 2: All LEDs white (100% brightness)
  {
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
     {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
     {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}}, 
    10 // 100ms duration
  },
  // Frame 3: All LEDs white (25% brightness)
  {
    {{64, 64, 64}, {64, 64, 64}, {64, 64, 64}, {64, 64, 64},
     {64, 64, 64}, {64, 64, 64}, {64, 64, 64}, {64, 64, 64},
     {64, 64, 64}, {64, 64, 64}, {64, 64, 64}, {64, 64, 64}}, 
    5  // 50ms duration
  },
  // Frame 4: All LEDs off
  {
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
     {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
     {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, 
    3  // 30ms duration
  }
};

// Helper function to read colors from PROGMEM
inline void readColorFromProgmem(const uint8_t* colorPtr, Color& color) {
  color.r = pgm_read_byte(&colorPtr[0]);
  color.g = pgm_read_byte(&colorPtr[1]);
  color.b = pgm_read_byte(&colorPtr[2]);
}

#endif // ANIMATION_DATA_H 