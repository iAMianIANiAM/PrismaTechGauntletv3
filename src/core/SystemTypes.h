#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>

/**
 * @brief Defines the system operational modes
 * Operational modes as defined in the TrueFunctionGuide:
 * - Idle Mode: Default state monitoring hand positions
 * - Invocation Mode: Structured sequence recording positions into three slots
 * - Resolution Mode: Spell effect determination based on recorded positions
 * - Freecast Mode: Creative motion-to-pattern translation
 */
enum SystemMode {
  MODE_IDLE,         // Default monitoring state
  MODE_INVOCATION,   // Three-slot position recording
  MODE_RESOLUTION,   // Spell effect determination and display
  MODE_FREECAST      // Motion-to-pattern translation
};

/**
 * @brief Defines the possible mode transitions
 * Used by IdleMode and FreeCastMode to signal transition requests
 */
enum class ModeTransition {
  NONE,              // No transition requested
  TO_INVOCATION,     // Transition to Invocation Mode (from CalmOffer gesture)
  TO_FREECAST,       // Transition to Freecast Mode (from LongNull gesture)
  TO_IDLE            // Return to Idle Mode (from exit gesture)
};

/**
 * @brief Defines the recognized hand positions
 * Hand positions as defined in the TrueFunctionGuide:
 * - Offer: Hand forward, Palm Up (Purple - RGB: 128, 0, 128)
 * - Calm: Hand forward, Palm Down (Yellow - RGB: 255, 255, 0)
 * - Oath: Hand pointing upwards (Red - RGB: 255, 0, 0)
 * - Dig: Hand pointing downwards (Green - RGB: 0, 255, 0)
 * - Shield: Hand to the side, Palm Out (Blue - RGB: 0, 0, 255)
 * - Null: Hand to the side, Palm In (Orange - RGB: 255, 165, 0)
 */
enum HandPosition {
  POS_OFFER,    // Hand forward, Palm Up (Purple)
  POS_CALM,     // Hand forward, Palm Down (Blue)
  POS_OATH,     // Hand pointing upwards (Yellow)
  POS_DIG,      // Hand pointing downwards (Green)
  POS_SHIELD,   // Hand to the side, Palm Out (Pink)
  POS_NULLPOS,  // Hand to the side, Palm In (Red)
  POS_UNKNOWN   // Default/Unclear (White)
};

/**
 * @brief RGB color structure
 */
struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

/**
 * @brief Memory-efficient position detection result
 */
struct PositionReading {
  uint8_t position;   // HandPosition enum value
  uint8_t confidence; // 0-100 scale
  uint32_t timestamp; // Millisecond timestamp
};

/**
 * @brief Raw IMU sensor data
 */
struct SensorData {
  int16_t accelX, accelY, accelZ;
  int16_t gyroX, gyroY, gyroZ;
  uint32_t timestamp;
};

/**
 * @brief Invocation slots for spell determination
 */
struct InvocationSlots {
  uint8_t positions[3]; // HandPosition values for each slot
  bool filled[3];       // Whether each slot has been filled
  uint8_t currentSlot;  // Currently active slot (0-2)
};

#endif // SYSTEM_TYPES_H 