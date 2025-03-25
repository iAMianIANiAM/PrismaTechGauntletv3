#include <Arduino.h>

// This is a dummy file to help with build issues in test environments
// It provides the required Arduino framework entry points (setup/loop)
// that are missing when we try to build standalone test files

// Empty setup function
void setup() {
  // This will be overridden by the actual setup in the test file
}

// Empty loop function
void loop() {
  // This will be overridden by the actual loop in the test file
} 