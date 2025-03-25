#include <Arduino.h>
#include "core/GauntletController.h"
#include "utils/DebugTools.h"

// Global controller instance
GauntletController controller;

void setup() {
  // Initialize debug tools
  DEBUG_INIT(115200);
  DEBUG_PRINTLN("PrismaTech Gauntlet 3.0 starting...");
  
  // Initialize controller
  if (!controller.init()) {
    DEBUG_PRINTLN("Controller initialization failed!");
    while (1) {
      // Blink rapidly to indicate initialization error
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }
  
  DEBUG_PRINTLN("Initialization complete");
}

void loop() {
  // Call controller update
  controller.update();
}