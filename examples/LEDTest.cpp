#include <Arduino.h>
#include "../src/hardware/LEDInterface.h"
#include "../src/core/Config.h"
#include "../src/core/SystemTypes.h"

// Create LED interface instance
LEDInterface ledInterface;

// Cycle through different test patterns
enum TestPattern {
  PATTERN_ALL_POSITIONS,
  PATTERN_RAINBOW,
  PATTERN_POSITION_PULSE,
  PATTERN_RAINBOW_BURST,
  PATTERN_FADING,
  PATTERN_SEQUENTIAL,
  NUM_PATTERNS
};

int currentPattern = 0;
unsigned long patternStartTime = 0;
const unsigned long PATTERN_DURATION = 5000; // 5 seconds per pattern

// Variables for pattern animations
uint8_t hue = 0;
int currentPosition = 0;
unsigned long lastPulseTime = 0;
unsigned long lastBurstTime = 0;
bool fadeDirection = true; // true = brightening, false = dimming
uint8_t fadeLevel = 20;
int currentLed = 0;
unsigned long lastLedChangeTime = 0;
Color fadeColor;
Color seqColor = {255, 255, 255}; // White

void printPattern(const char* patternName) {
  Serial.println("\n==================================");
  Serial.println(patternName);
  Serial.println("==================================");
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);  // Allow time for serial connection to establish
  
  Serial.println("\n==================================");
  Serial.println("LED Test Program");
  Serial.println("==================================");
  
  // Initialize the LED interface
  Serial.println("\nInitializing LED interface...");
  if (ledInterface.init()) {
    Serial.println("LED interface initialized successfully");
    Serial.printf("Using LED_DATA_PIN: %d\n", Config::LED_DATA_PIN);
    Serial.printf("Number of LEDs: %d\n", Config::NUM_LEDS);
    Serial.printf("Default brightness: %d\n", Config::DEFAULT_BRIGHTNESS);
  } else {
    Serial.println("Failed to initialize LED interface");
    while (true) {
      delay(1000);
    }
  }
  
  // Initialize pattern variables
  fadeColor = ledInterface.getColorForPosition(POS_OFFER); // Purple
  
  // Start the first pattern
  patternStartTime = millis();
  printPattern("Pattern: All Hand Positions");
}

void loop() {
  // Check if it's time to switch to the next pattern
  if (millis() - patternStartTime > PATTERN_DURATION) {
    // Move to next pattern
    currentPattern = (currentPattern + 1) % NUM_PATTERNS;
    patternStartTime = millis();
    
    // Clear LEDs before starting new pattern
    ledInterface.clear();
    
    // Print pattern information
    switch (currentPattern) {
      case PATTERN_ALL_POSITIONS:
        printPattern("Pattern: All Hand Positions");
        break;
      case PATTERN_RAINBOW:
        printPattern("Pattern: Rainbow Effect");
        break;
      case PATTERN_POSITION_PULSE:
        printPattern("Pattern: Position Color Pulses");
        break;
      case PATTERN_RAINBOW_BURST:
        printPattern("Pattern: Rainbow Burst (CalmOffer)");
        break;
      case PATTERN_FADING:
        printPattern("Pattern: Fading Effect");
        break;
      case PATTERN_SEQUENTIAL:
        printPattern("Pattern: Sequential LED Test");
        break;
    }
  }
  
  // Execute the current pattern
  switch (currentPattern) {
    case PATTERN_ALL_POSITIONS:
      // Show each hand position color on a segment of the ring
      for (int pos = 0; pos < 6; pos++) {
        Color posColor = ledInterface.getColorForPosition(pos);
        uint8_t ledsPerPos = Config::NUM_LEDS / 6;
        ledInterface.setLEDRange(pos * ledsPerPos, ledsPerPos, posColor);
      }
      // Handle the overflow (if any)
      if (Config::NUM_LEDS % 6 != 0) {
        Color unknownColor = ledInterface.getColorForPosition(POS_UNKNOWN);
        ledInterface.setLEDRange(6 * (Config::NUM_LEDS / 6), Config::NUM_LEDS % 6, unknownColor);
      }
      ledInterface.show();
      break;
      
    case PATTERN_RAINBOW:
      // Create a shifting rainbow pattern
      ledInterface.rainbow(hue, 255 / Config::NUM_LEDS);
      hue = (hue + 1) % 256; // Slowly cycle through hues
      ledInterface.show();
      delay(20);
      break;
      
    case PATTERN_POSITION_PULSE:
      // Pulse each position color
      if (millis() - lastPulseTime > 800) {
        lastPulseTime = millis();
        Color posColor = ledInterface.getColorForPosition(currentPosition);
        Serial.printf("Pulsing position %d\n", currentPosition);
        ledInterface.pulse(posColor, 1, 700);
        currentPosition = (currentPosition + 1) % 7; // Include POS_UNKNOWN
      }
      break;
      
    case PATTERN_RAINBOW_BURST:
      // Demonstrate the rainbow burst effect (used for CalmOffer gesture)
      if (millis() - lastBurstTime > 2000) {
        Serial.println("Triggering Rainbow Burst");
        ledInterface.rainbowBurst();
        lastBurstTime = millis();
      }
      break;
      
    case PATTERN_FADING:
      // Create a simple fading effect
      if (fadeDirection) {
        fadeLevel += 5;
        if (fadeLevel >= 200) fadeDirection = false;
      } else {
        fadeLevel -= 5;
        if (fadeLevel <= 20) fadeDirection = true;
      }
      
      ledInterface.setBrightness(fadeLevel);
      ledInterface.setAllLEDs(fadeColor);
      ledInterface.show();
      delay(30);
      break;
      
    case PATTERN_SEQUENTIAL:
      // Light each LED sequentially
      ledInterface.clear();
      ledInterface.setLED(currentLed, seqColor);
      ledInterface.show();
      
      // Move to next LED after a short delay
      if (millis() - lastLedChangeTime > 200) {
        lastLedChangeTime = millis();
        currentLed = (currentLed + 1) % Config::NUM_LEDS;
      }
      break;
  }
} 