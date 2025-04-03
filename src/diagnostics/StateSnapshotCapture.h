/**
 * StateSnapshotCapture.h
 * 
 * A simple state serialization utility to capture and report system state at key points.
 * Part of the Lightweight Universal Troubleshooting Toolkit (LUTT)
 */

#ifndef STATE_SNAPSHOT_CAPTURE_H
#define STATE_SNAPSHOT_CAPTURE_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Maximum number of fields in a snapshot
#define MAX_SNAPSHOT_FIELDS 10

// Snapshot trigger points (for filtering)
#define SNAPSHOT_TRIGGER_GESTURE_START  0x01
#define SNAPSHOT_TRIGGER_GESTURE_END    0x02
#define SNAPSHOT_TRIGGER_MODE_CHANGE    0x04
#define SNAPSHOT_TRIGGER_ANIMATION      0x08
#define SNAPSHOT_TRIGGER_FREECAST       0x10
#define SNAPSHOT_TRIGGER_ERROR          0x20
#define SNAPSHOT_TRIGGER_TEST           0x40  // Testing trigger
#define SNAPSHOT_TRIGGER_ALL            0xFF

#ifndef SNAPSHOT_TRIGGER_FILTER
#define SNAPSHOT_TRIGGER_FILTER SNAPSHOT_TRIGGER_ALL
#endif

class StateSnapshotCapture {
public:
  /**
   * Initialize the state snapshot system
   */
  static void init();
  
  /**
   * Capture state snapshot if trigger matches filter
   * @param triggerPoint One of the SNAPSHOT_TRIGGER_* constants
   * @param context String describing where the snapshot was triggered
   */
  static void capture(uint8_t triggerPoint, const char* context);
  
  /**
   * Add a field to the current snapshot (integer variant)
   * @param name Field name
   * @param value Integer value
   */
  static void addField(const char* name, int value);
  
  /**
   * Add a field to the current snapshot (float variant)
   * @param name Field name
   * @param value Float value
   */
  static void addField(const char* name, float value);
  
  /**
   * Add a field to the current snapshot (string variant)
   * @param name Field name
   * @param value String value
   */
  static void addField(const char* name, const char* value);
  
  /**
   * Add a field to the current snapshot (boolean variant)
   * @param name Field name
   * @param value Boolean value
   */
  static void addField(const char* name, bool value);
  
  /**
   * Clear the snapshot buffer
   */
  static void clear();
  
  /**
   * Output the snapshot to Serial
   */
  static void printSnapshot();

private:
  static StaticJsonDocument<512> _snapshot;
  static bool _hasContent;
  static unsigned long _captureTime;
};

#endif // STATE_SNAPSHOT_CAPTURE_H 