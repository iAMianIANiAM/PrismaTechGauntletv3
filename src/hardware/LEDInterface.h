#ifndef LED_INTERFACE_H
#define LED_INTERFACE_H

#include "../core/SystemTypes.h"
#include "../core/Config.h"
#include <FastLED.h>

/**
 * @brief Interface to control the WS2812 RGB LED ring
 */
class LEDInterface {
public:
  /**
   * @brief Initialize the LED hardware
   * @return True if initialization was successful
   */
  bool init();
  
  /**
   * @brief Set a single LED to a specific color
   * @param index LED index (0-11)
   * @param color RGB color value
   */
  void setLED(uint8_t index, const Color& color);
  
  /**
   * @brief Set all LEDs to the same color
   * @param color RGB color value
   */
  void setAllLEDs(const Color& color);
  
  /**
   * @brief Fill all LEDs with a solid CRGB color
   * @param color CRGB color for FastLED compatibility
   */
  void fillSolid(CRGB color);
  
  /**
   * @brief Set a range of LEDs to the same color
   * @param startIndex First LED index
   * @param count Number of LEDs to set
   * @param color RGB color value
   */
  void setLEDRange(uint8_t startIndex, uint8_t count, const Color& color);
  
  /**
   * @brief Set the global brightness scaling factor
   * @param brightness Brightness level (0-255)
   */
  void setBrightness(uint8_t brightness);
  
  /**
   * @brief Send the current LED values to the physical LEDs
   */
  void show();
  
  /**
   * @brief Clear all LEDs (set to black)
   */
  void clear();
  
  /**
   * @brief Get the color for a specific hand position
   * @param position Hand position enum value
   * @return RGB color for the position
   */
  Color getColorForPosition(uint8_t position) const;
  
  /**
   * @brief Create a rainbow effect across all LEDs
   * @param initialHue Starting hue value (0-255)
   * @param deltaHue Hue change between adjacent LEDs
   */
  void rainbow(uint8_t initialHue, uint8_t deltaHue);
  
  /**
   * @brief Create a quick rainbow animation (for CalmOffer gesture)
   */
  void rainbowBurst();
  
  /**
   * @brief Gradually fade all LEDs toward black
   * @param fadeAmount Amount to decrease each color channel (1-255)
   */
  void fadeToBlack(uint8_t fadeAmount);
  
  /**
   * @brief Create a pulsing animation with the specified color
   * @param color Color to pulse
   * @param pulseCount Number of pulse cycles
   * @param pulseDuration Duration of each pulse cycle in milliseconds
   */
  void pulse(const Color& color, uint8_t pulseCount, uint16_t pulseDuration);

private:
  // LED pixel buffer
  Color ledBuffer[Config::NUM_LEDS];
  
  // Current brightness (0-255)
  uint8_t brightness = Config::DEFAULT_BRIGHTNESS;
  
  // Map a position to its color
  Color mapPositionToColor(uint8_t position) const;
  
  // Apply brightness to a color
  Color applyBrightness(const Color& color) const;
};

#endif // LED_INTERFACE_H 