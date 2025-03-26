#include "LEDInterface.h"
#include "../utils/DebugTools.h"
#include <FastLED.h>

// FastLED array for the LED ring
CRGB leds[Config::NUM_LEDS];

bool LEDInterface::init() {
  // Configure the LED power control pin if defined
  if (Config::LED_POWER_PIN != 0) {
    pinMode(Config::LED_POWER_PIN, OUTPUT);
    digitalWrite(Config::LED_POWER_PIN, HIGH); // Enable LED power
  }
  
  // Initialize the FastLED library
  // Using the GRB color order as specified in hardware analysis
  FastLED.addLeds<WS2812, Config::LED_DATA_PIN, GRB>(leds, Config::NUM_LEDS);
  FastLED.setBrightness(brightness);
  FastLED.clear();
  FastLED.show();
  
  // Reset LED buffer
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    ledBuffer[i] = {0, 0, 0};
  }
  
#ifndef SUPPRESS_LED_DEBUG
  DEBUG_PRINTF("LED Interface initialized on pin %d\n", Config::LED_DATA_PIN);
  DEBUG_PRINTF("Brightness set to %d\n", brightness);
#endif
  
  return true;
}

void LEDInterface::setLED(uint8_t index, const Color& color) {
  if (index < Config::NUM_LEDS) {
    ledBuffer[index] = color;
  } else {
#ifndef SUPPRESS_LED_DEBUG
    DEBUG_PRINTF("WARNING: LED index out of bounds: %d\n", index);
#endif
  }
}

void LEDInterface::setAllLEDs(const Color& color) {
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    ledBuffer[i] = color;
  }
}

void LEDInterface::setLEDRange(uint8_t startIndex, uint8_t count, const Color& color) {
  uint8_t endIndex = startIndex + count - 1;
  if (endIndex >= Config::NUM_LEDS) {
#ifndef SUPPRESS_LED_DEBUG
    DEBUG_PRINTF("WARNING: LED range partially out of bounds: %d to %d\n", 
                 startIndex, endIndex);
#endif
    endIndex = Config::NUM_LEDS - 1;
  }
  
  for (int i = startIndex; i <= endIndex; i++) {
    ledBuffer[i] = color;
  }
}

void LEDInterface::setBrightness(uint8_t newBrightness) {
  brightness = newBrightness;
  FastLED.setBrightness(brightness);
  
  // Only print debug message if not suppressed for calibration
#ifndef SUPPRESS_LED_DEBUG
  DEBUG_PRINTF("LED brightness set to %d\n", brightness);
#endif
}

void LEDInterface::show() {
  // Apply colors to the FastLED array
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    Color color = ledBuffer[i];
    leds[i] = CRGB(color.r, color.g, color.b);
  }
  
  // Update the LED ring
  FastLED.show();
}

void LEDInterface::clear() {
  // Clear buffer
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    ledBuffer[i] = {0, 0, 0};
  }
  
  // Clear LEDs
  FastLED.clear();
  FastLED.show();
  
#ifndef SUPPRESS_LED_DEBUG
  DEBUG_PRINTLN("LEDs cleared");
#endif
}

Color LEDInterface::getColorForPosition(uint8_t position) const {
  return mapPositionToColor(position);
}

// Additional animation methods
void LEDInterface::fadeToBlack(uint8_t fadeAmount) {
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    // Fade each channel
    if (ledBuffer[i].r > fadeAmount) ledBuffer[i].r -= fadeAmount;
    else ledBuffer[i].r = 0;
    
    if (ledBuffer[i].g > fadeAmount) ledBuffer[i].g -= fadeAmount;
    else ledBuffer[i].g = 0;
    
    if (ledBuffer[i].b > fadeAmount) ledBuffer[i].b -= fadeAmount;
    else ledBuffer[i].b = 0;
  }
}

void LEDInterface::rainbow(uint8_t initialHue, uint8_t deltaHue) {
  // Use FastLED's fill_rainbow function directly on the leds array
  fill_rainbow(leds, Config::NUM_LEDS, initialHue, deltaHue);
  
  // Update the ledBuffer to match the FastLED array
  for (int i = 0; i < Config::NUM_LEDS; i++) {
    ledBuffer[i].r = leds[i].r;
    ledBuffer[i].g = leds[i].g;
    ledBuffer[i].b = leds[i].b;
  }
}

void LEDInterface::rainbowBurst() {
  // Create a fast rainbow burst effect (used for CalmOffer gesture)
  // Start with high brightness
  uint8_t savedBrightness = brightness;
  FastLED.setBrightness(255);
  
  // Rapid rainbow animation
  for (int j = 0; j < 256; j += 8) {
    fill_rainbow(leds, Config::NUM_LEDS, j, 5);
    FastLED.show();
    delay(5); // Short delay for quick animation
  }
  
  // Restore original brightness
  FastLED.setBrightness(savedBrightness);
  brightness = savedBrightness;
  
  // Clear after effect
  clear();
}

void LEDInterface::pulse(const Color& color, uint8_t pulseCount, uint16_t pulseDuration) {
  uint8_t savedBrightness = brightness;
  
  for (int p = 0; p < pulseCount; p++) {
    // Fade in
    for (int b = 0; b < savedBrightness; b += 5) {
      setAllLEDs(color);
      FastLED.setBrightness(b);
      show();
      delay(pulseDuration / (savedBrightness / 5) / 2);
    }
    
    // Fade out
    for (int b = savedBrightness; b > 0; b -= 5) {
      setAllLEDs(color);
      FastLED.setBrightness(b);
      show();
      delay(pulseDuration / (savedBrightness / 5) / 2);
    }
  }
  
  // Restore original brightness
  FastLED.setBrightness(savedBrightness);
  brightness = savedBrightness;
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