#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>

/**
 * @brief Defines the system operational modes
 */
enum SystemMode {
  MODE_IDLE,         // Default monitoring state
  MODE_INVOCATION,   // Three-slot position recording
  MODE_RESOLUTION,   // Spell effect determination and display
  MODE_FREECAST      // Motion-to-pattern translation
};

/**
 * @brief Defines the recognized hand positions
 */
enum HandPosition {
  POS_OFFER,    // Hand forward, Palm Up (Purple)
  POS_CALM,     // Hand forward, Palm Down (Yellow)
  POS_OATH,     // Hand pointing upwards (Red)
  POS_DIG,      // Hand pointing downwards (Green)
  POS_SHIELD,   // Hand to the side, Palm Out (Blue)
  POS_NULL,     // Hand to the side, Palm In (Orange)
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