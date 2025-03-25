#include "DebugTools.h"
#include <Arduino.h>
#include <stdarg.h>

namespace DebugTools {
  // Timing variables
  static unsigned long timingStart = 0;
  static bool timingEnabled = false;
  static char timingSectionName[32] = {0};
  
  // Initialize debug tools
  void init(unsigned long baudRate) {
    #ifdef DEBUG_ENABLED
    Serial.begin(baudRate);
    delay(500); // Give serial time to connect
    Serial.println("Debug initialized");
    #endif
  }
  
  // Print debug message
  void print(const char* message) {
    #ifdef DEBUG_ENABLED
    Serial.print(message);
    #endif
  }
  
  // Print debug message with newline
  void println(const char* message) {
    #ifdef DEBUG_ENABLED
    Serial.println(message);
    #endif
  }
  
  // Print formatted debug message
  void printf(const char* format, ...) {
    #ifdef DEBUG_ENABLED
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print(buffer);
    #endif
  }
  
  // Print sensor data
  void printSensorData(const SensorData& data) {
    #ifdef DEBUG_ENABLED
    printf("Sensor: Accel[%d,%d,%d] Gyro[%d,%d,%d] Time:%lu\n", 
           data.accelX, data.accelY, data.accelZ,
           data.gyroX, data.gyroY, data.gyroZ,
           data.timestamp);
    #endif
  }
  
  // Print position reading
  void printPositionReading(const PositionReading& position) {
    #ifdef DEBUG_ENABLED
    printf("Position: %s (Confidence: %d%%) Time:%lu\n", 
           positionToString(position.position),
           position.confidence,
           position.timestamp);
    #endif
  }
  
  // Print current mode
  void printMode(uint8_t mode) {
    #ifdef DEBUG_ENABLED
    const char* modeName = "Unknown";
    
    switch (mode) {
      case 0: modeName = "IDLE"; break;
      case 1: modeName = "INVOCATION"; break;
      case 2: modeName = "RESOLUTION"; break;
      case 3: modeName = "FREECAST"; break;
    }
    
    printf("Mode: %s\n", modeName);
    #endif
  }
  
  // Enable/disable timing
  void setTimingEnabled(bool enabled) {
    timingEnabled = enabled;
  }
  
  // Start timing
  void startTiming(const char* sectionName) {
    #ifdef DEBUG_ENABLED
    if (timingEnabled) {
      timingStart = micros();
      strncpy(timingSectionName, sectionName, sizeof(timingSectionName) - 1);
      timingSectionName[sizeof(timingSectionName) - 1] = '\0';
    }
    #endif
  }
  
  // End timing and print
  void endTiming() {
    #ifdef DEBUG_ENABLED
    if (timingEnabled) {
      unsigned long elapsed = micros() - timingStart;
      printf("Timing [%s]: %lu us\n", timingSectionName, elapsed);
    }
    #endif
  }
  
  // Log memory usage
  void logMemoryUsage() {
    #ifdef DEBUG_ENABLED
    #ifdef ESP32
    printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
    #endif
    #endif
  }
  
  // Convert position to string
  const char* positionToString(uint8_t position) {
    switch (position) {
      case 0: return "OFFER";
      case 1: return "CALM";
      case 2: return "OATH";
      case 3: return "DIG";
      case 4: return "SHIELD";
      case 5: return "NULL";
      default: return "UNKNOWN";
    }
  }
} 