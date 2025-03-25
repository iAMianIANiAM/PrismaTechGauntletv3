#ifndef LED_INTERFACE_H
#define LED_INTERFACE_H

#include "../core/SystemTypes.h"
#include "../core/Config.h"

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