#include <Arduino.h>
#include "core/GauntletController.h"
#include "diagnostics/DiagnosticLogger.h"
#include "diagnostics/StateSnapshotCapture.h"
#include "diagnostics/VisualDebugIndicator.h"
#include "diagnostics/CommandLineInterface.h"

// Serial communication
#define SERIAL_BAUD_RATE 115200

// Update interval
#define UPDATE_INTERVAL 100  // ms

// Position names for debugging
const char* positionNames[] = {
  "Offer (Purple)",  // POS_OFFER
  "Calm (Blue)",     // POS_CALM
  "Oath (Yellow)",   // POS_OATH
  "Dig (Green)",     // POS_DIG
  "Shield (Pink)",   // POS_SHIELD
  "Null (Red)",      // POS_NULLPOS
  "Unknown (White)"  // POS_UNKNOWN
};

// Increase Arduino loop task stack size
#if CONFIG_ARDUINO_LOOP_STACK_SIZE < 16384
#undef CONFIG_ARDUINO_LOOP_STACK_SIZE
#define CONFIG_ARDUINO_LOOP_STACK_SIZE 16384
#endif

// Global GauntletController instance
GauntletController gauntletController;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  
  Serial.println(F("\n\n=== PrismaTech Gauntlet 3.0 ==="));
  Serial.println("Ultra Basic Position Detection System");
  
  // Initialize the GauntletController
  // This handles initialization of HardwareManager, PositionDetector, and all modes
  gauntletController.initialize(); 

  // Initialize LUTT components if enabled
  DiagnosticLogger::setEnabled(true);
  StateSnapshotCapture::init();
  // Get HardwareManager pointer from GauntletController
  VisualDebugIndicator::init(gauntletController.getHardwareManager());
  CommandLineInterface::init();
  
  DIAG_LOG(DIAG_LEVEL_INFO, DIAG_TAG_MODE, "System initialized successfully");

  // Optional: Add a ready indicator if desired (handled by controller init potentially)
  Serial.println(F("\nGauntlet ready."));
  Serial.println(F("------------------------------------------------------"));
}

void loop() {
  // Main loop simply updates the controller
  // The controller manages modes, timing, and hardware updates internally
  gauntletController.update();

  // Process LUTT diagnostics
  VisualDebugIndicator::process();
  CommandLineInterface::process();

  // No delay needed here, controller's maintainLoopTiming handles it
}