#include <Arduino.h>
#include "../src/hardware/HardwareManager.h"
#include "../src/detection/UltraBasicPositionDetector.h"
#include "../src/core/Config.h"

// Test harness for color position tests
HardwareManager hardwareManager;
UltraBasicPositionDetector ubpDetector;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=================================");
  Serial.println("PrismaTech Gauntlet: Color Test");
  Serial.println("=================================");
  
  // Initialize hardware
  if (!hardwareManager.init()) {
    Serial.println("Failed to initialize hardware manager!");
    while (1) { delay(1000); } // Halt
  }
  
  // Initialize position detector
  if (!ubpDetector.init(&hardwareManager)) {
    Serial.println("Failed to initialize position detector!");
    while (1) { delay(1000); } // Halt
  }
  
  Serial.println("Initialization complete!");
  Serial.println("Starting position color test in 3 seconds...");
  delay(3000);
}

void runPositionColorTest() {
  // Display each position with its color for 1 second
  const HandPosition positions[] = {
    POS_OFFER, POS_CALM, POS_OATH, POS_DIG, POS_SHIELD, POS_NULLPOS
  };
  
  // Print position details to serial
  Serial.println("\nPosition Color Test");
  Serial.println("------------------");
  
  for (int i = 0; i < 6; i++) {
    HandPosition pos = positions[i];
    
    // Get position name
    const char* posName = ubpDetector.getPositionName(pos);
    Serial.print("Position: ");
    Serial.print(posName);
    
    // Get RGB values for the position
    Color color = hardwareManager.setLEDsForPosition(pos);
    Serial.print(" | RGB: (");
    Serial.print(color.r);
    Serial.print(", ");
    Serial.print(color.g);
    Serial.print(", ");
    Serial.print(color.b);
    Serial.println(")");
    
    // Display position color on LEDs
    hardwareManager.renderLEDs();
    delay(1000);
  }
}

void runColorTransitionTest() {
  // Test transitions between positions to verify visual feedback
  Serial.println("\nPosition Transition Test");
  Serial.println("---------------------");
  
  // Test transitions between all gesture pairs
  const HandPosition transitions[][2] = {
    {POS_CALM, POS_OFFER},   // CalmOffer gesture
    {POS_DIG, POS_OATH},     // DigOath gesture
    {POS_NULLPOS, POS_SHIELD} // NullShield gesture
  };
  
  for (int i = 0; i < 3; i++) {
    const HandPosition* transition = transitions[i];
    
    Serial.print("Testing transition: ");
    Serial.print(ubpDetector.getPositionName(transition[0]));
    Serial.print(" → ");
    Serial.println(ubpDetector.getPositionName(transition[1]));
    
    // Display first position
    Serial.print("Position 1: ");
    Serial.println(ubpDetector.getPositionName(transition[0]));
    hardwareManager.setLEDsForPosition(transition[0]);
    hardwareManager.renderLEDs();
    delay(1000);
    
    // Display second position
    Serial.print("Position 2: ");
    Serial.println(ubpDetector.getPositionName(transition[1]));
    hardwareManager.setLEDsForPosition(transition[1]);
    hardwareManager.renderLEDs();
    delay(1000);
    
    // Reset
    hardwareManager.setAllLEDs({0,0,0});
    hardwareManager.renderLEDs();
    delay(500);
  }
}

void runBrightnessTest() {
  // Test colors at different brightness levels
  Serial.println("\nBrightness Test");
  Serial.println("--------------");
  
  const HandPosition positions[] = {
    POS_OFFER, POS_CALM, POS_OATH, POS_DIG, POS_SHIELD, POS_NULLPOS
  };
  
  const uint8_t brightnessLevels[] = {255, 128, 64, 32, 16, 8};
  
  for (int i = 0; i < 6; i++) {
    HandPosition pos = positions[i];
    const char* posName = ubpDetector.getPositionName(pos);
    
    Serial.print("Testing position: ");
    Serial.println(posName);
    
    for (int b = 0; b < 6; b++) {
      uint8_t brightness = brightnessLevels[b];
      Serial.print("  Brightness: ");
      Serial.println(brightness);
      
      // Set brightness
      hardwareManager.setBrightness(brightness);
      
      // Display position color
      hardwareManager.setLEDsForPosition(pos);
      hardwareManager.renderLEDs();
      delay(500);
    }
    
    // Reset brightness
    hardwareManager.setBrightness(Config::DEFAULT_BRIGHTNESS);
    
    // Clear LEDs between positions
    hardwareManager.setAllLEDs({0,0,0});
    hardwareManager.renderLEDs();
    delay(300);
  }
}

void loop() {
  // Run all tests in sequence
  Serial.println("\n\n=================================");
  Serial.println("Starting Color Test Sequence");
  Serial.println("=================================");
  
  // Test position colors
  runPositionColorTest();
  
  // Test transitions
  runColorTransitionTest();
  
  // Test brightness levels
  runBrightnessTest();
  
  Serial.println("\n=================================");
  Serial.println("Color Test Sequence Complete!");
  Serial.println("Press reset to run again.");
  Serial.println("=================================");
  
  // End of test - wait forever
  while (1) { delay(1000); }
} 