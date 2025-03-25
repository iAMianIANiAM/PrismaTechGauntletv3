#include "LEDInterface.h"
#include "../utils/DebugTools.h"
#include <Adafruit_NeoPixel.h>

// NeoPixel instance for the LED ring
Adafruit_NeoPixel pixels(Config::NUM_LEDS, Config::LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

bool LEDInterface::init() {
  // Configure the LED power control pin if defined
  if (Config::LED_POWER_PIN != 0) {
    pinMode(Config::LED_POWER_PIN, OUTPUT);
    digitalWrite(Config::LED_POWER_PIN, HIGH); // Enable LED power
  }
  
  // Initialize the NeoPixel library
  pixels.begin();
  pixels.setBrightness(brightness);
  pixels.clear();
  pixels.show();
  
  // Reset LED buffer
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    ledBuffer[i] = {0, 0, 0};
  }
  
  return true;
}

void LEDInterface::setLED(uint8_t index, const Color& color) {
  if (index < Config::NUM_LEDS) {
    ledBuffer[index] = color;
  }
}

void LEDInterface::setAllLEDs(const Color& color) {
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    ledBuffer[i] = color;
  }
}

void LEDInterface::setLEDRange(uint8_t startIndex, uint8_t count, const Color& color) {
  for (int i = 0; i < count; i++) {
    if (startIndex + i < Config::NUM_LEDS) {
      ledBuffer[startIndex + i] = color;
    }
  }
}

void LEDInterface::setBrightness(uint8_t newBrightness) {
  brightness = newBrightness;
  pixels.setBrightness(brightness);
}

void LEDInterface::show() {
  // Apply brightness and set pixels
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    Color adjustedColor = applyBrightness(ledBuffer[i]);
    pixels.setPixelColor(i, adjustedColor.r, adjustedColor.g, adjustedColor.b);
  }
  
  // Update the LED ring
  pixels.show();
}

void LEDInterface::clear() {
  // Clear buffer
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    ledBuffer[i] = {0, 0, 0};
  }
  
  // Clear LEDs
  pixels.clear();
  pixels.show();
}

Color LEDInterface::getColorForPosition(uint8_t position) const {
  return mapPositionToColor(position);
}

Color LEDInterface::mapPositionToColor(uint8_t position) const {
  Color result = {0, 0, 0};
  
  switch (position) {
    case POS_OFFER:
      result.r = Config::Colors::OFFER_COLOR[0];
      result.g = Config::Colors::OFFER_COLOR[1];
      result.b = Config::Colors::OFFER_COLOR[2];
      break;
    case POS_CALM:
      result.r = Config::Colors::CALM_COLOR[0];
      result.g = Config::Colors::CALM_COLOR[1];
      result.b = Config::Colors::CALM_COLOR[2];
      break;
    case POS_OATH:
      result.r = Config::Colors::OATH_COLOR[0];
      result.g = Config::Colors::OATH_COLOR[1];
      result.b = Config::Colors::OATH_COLOR[2];
      break;
    case POS_DIG:
      result.r = Config::Colors::DIG_COLOR[0];
      result.g = Config::Colors::DIG_COLOR[1];
      result.b = Config::Colors::DIG_COLOR[2];
      break;
    case POS_SHIELD:
      result.r = Config::Colors::SHIELD_COLOR[0];
      result.g = Config::Colors::SHIELD_COLOR[1];
      result.b = Config::Colors::SHIELD_COLOR[2];
      break;
    case POS_NULL:
      result.r = Config::Colors::NULL_COLOR[0];
      result.g = Config::Colors::NULL_COLOR[1];
      result.b = Config::Colors::NULL_COLOR[2];
      break;
    case POS_UNKNOWN:
    default:
      result.r = Config::Colors::UNKNOWN_COLOR[0];
      result.g = Config::Colors::UNKNOWN_COLOR[1];
      result.b = Config::Colors::UNKNOWN_COLOR[2];
      break;
  }
  
  return result;
}

Color LEDInterface::applyBrightness(const Color& color) const {
  Color result;
  // Apply brightness scaling
  result.r = (color.r * brightness) / 255;
  result.g = (color.g * brightness) / 255;
  result.b = (color.b * brightness) / 255;
  return result;
} 