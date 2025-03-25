#include <Arduino.h>
#include "../../src/utils/DebugTools.h"

// Forward declaration of test function
extern void testMPU9250();

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  Serial.println("PrismaTech Gauntlet 3.0 - MPU9250 Test");
  
  // Run MPU9250 test
  testMPU9250();
}

void loop() {
  // Just delay in the main loop
  delay(1000);
} 