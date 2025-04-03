/**
 * StateSnapshotCapture.cpp
 * 
 * Implementation of the StateSnapshotCapture class for the LUTT toolkit.
 */

#include "StateSnapshotCapture.h"
#include "DiagnosticLogger.h"

// Initialize static variables
StaticJsonDocument<512> StateSnapshotCapture::_snapshot;
bool StateSnapshotCapture::_hasContent = false;
unsigned long StateSnapshotCapture::_captureTime = 0;

/**
 * Initialize the state snapshot system
 */
void StateSnapshotCapture::init() {
  clear();
}

/**
 * Capture state snapshot if trigger matches filter
 */
void StateSnapshotCapture::capture(uint8_t triggerPoint, const char* context) {
  // Skip if trigger doesn't match filter
  if (!(SNAPSHOT_TRIGGER_FILTER & triggerPoint)) return;
  
  // Clear any previous snapshot
  clear();
  
  // Set base metadata
  _captureTime = millis();
  _snapshot["time"] = _captureTime;
  _snapshot["trigger"] = triggerPoint;
  _snapshot["context"] = context;
  _hasContent = true;
}

/**
 * Add a field to the current snapshot (integer variant)
 */
void StateSnapshotCapture::addField(const char* name, int value) {
  if (!_hasContent) return;
  _snapshot[name] = value;
}

/**
 * Add a field to the current snapshot (float variant)
 */
void StateSnapshotCapture::addField(const char* name, float value) {
  if (!_hasContent) return;
  _snapshot[name] = value;
}

/**
 * Add a field to the current snapshot (string variant)
 */
void StateSnapshotCapture::addField(const char* name, const char* value) {
  if (!_hasContent) return;
  _snapshot[name] = value;
}

/**
 * Add a field to the current snapshot (boolean variant)
 */
void StateSnapshotCapture::addField(const char* name, bool value) {
  if (!_hasContent) return;
  _snapshot[name] = value;
}

/**
 * Clear the snapshot buffer
 */
void StateSnapshotCapture::clear() {
  _snapshot.clear();
  _hasContent = false;
  _captureTime = 0;
}

/**
 * Output the snapshot to Serial
 */
void StateSnapshotCapture::printSnapshot() {
  if (!_hasContent) {
    DiagnosticLogger::print("No snapshot data available");
    return;
  }
  
  // Print metadata header
  DiagnosticLogger::print("--- SNAPSHOT [%lu] ---", _captureTime);
  
  // Serialize to JSON and print
  String jsonOutput;
  serializeJsonPretty(_snapshot, jsonOutput);
  Serial.println(jsonOutput);
  
  DiagnosticLogger::print("--- END SNAPSHOT ---");
} 