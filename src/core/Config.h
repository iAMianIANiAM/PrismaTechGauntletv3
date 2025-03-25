#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/**
 * @brief System-wide configuration parameters
 */
namespace Config {
  // Hardware pins
  constexpr uint8_t LED_DATA_PIN = 12;  // WS2812 data pin (GPIO12 confirmed in hardware analysis)
  constexpr uint8_t LED_POWER_PIN = 5;  // Power control for LEDs
  constexpr uint8_t I2C_SDA_PIN = 21;   // I2C data line
  constexpr uint8_t I2C_SCL_PIN = 22;   // I2C clock line
  
  // LED configuration
  constexpr uint8_t NUM_LEDS = 12;      // Number of LEDs in the ring
  constexpr uint8_t DEFAULT_BRIGHTNESS = 100; // Default brightness (0-255) - 39% brightness as per hardware analysis
  
  // Sensor configuration
  constexpr uint8_t POSITION_SAMPLE_RATE = 50;  // Hz
  constexpr uint8_t FREECAST_SAMPLE_RATE = 100; // Hz
  
  // Position detection
  constexpr uint16_t AXIS_THRESHOLD = 1500;     // Minimum value for dominant axis
  constexpr uint8_t MIN_CONFIDENCE = 60;        // Minimum confidence for position change
  constexpr uint16_t DEBOUNCE_TIME_MS = 100;    // Position debounce time
  
  // Timing parameters
  constexpr uint16_t CALMOFFER_WINDOW_MS = 1000;  // Time window for CalmOffer gesture
  constexpr uint16_t LONGNULL_TIME_MS = 5000;     // Time required for LongNull gesture
  constexpr uint16_t LONGNULL_WARNING_MS = 3000;  // When to start flashing during LongNull
  
  // Invocation timing
  constexpr uint16_t INDICATOR_PHASE_MS = 1000;   // Slot indicator duration
  constexpr uint16_t COUNTDOWN_PHASE_MS = 2000;   // Countdown animation duration
  constexpr uint16_t CHECKPOINT_PHASE_MS = 200;   // Position recording duration
  constexpr uint16_t CONFIRMATION_PHASE_MS = 1000; // Confirmation animation duration
  
  // Resolution timing
  constexpr uint16_t DEFINITE_SPELL_MS = 5000;    // Duration of standard spells
  constexpr uint16_t EXTENDED_SPELL_MS = 15000;   // Duration of extended spells
  constexpr uint16_t BLACKOUT_PERIOD_MS = 500;    // Transition blackout duration
  constexpr uint16_t IDLE_RETURN_MS = 2500;       // Fade-in duration for idle return
  
  // Freecast timing
  constexpr uint16_t FREECAST_COLLECTION_MS = 2000; // Motion data collection window
  constexpr uint16_t FREECAST_DISPLAY_MS = 2000;    // Pattern display duration
  
  // Power management
  constexpr uint32_t IDLE_SLEEP_DELAY_MS = 300000; // Time before entering sleep mode (5 min)
  
  // Define position colors (R, G, B)
  namespace Colors {
    constexpr uint8_t OFFER_COLOR[3] = {128, 0, 128};    // Purple
    constexpr uint8_t CALM_COLOR[3] = {255, 255, 0};     // Yellow
    constexpr uint8_t OATH_COLOR[3] = {255, 0, 0};       // Red
    constexpr uint8_t DIG_COLOR[3] = {0, 255, 0};        // Green
    constexpr uint8_t SHIELD_COLOR[3] = {0, 0, 255};     // Blue
    constexpr uint8_t NULL_COLOR[3] = {255, 165, 0};     // Orange
    constexpr uint8_t UNKNOWN_COLOR[3] = {255, 255, 255}; // White
  }
}

#endif // CONFIG_H 