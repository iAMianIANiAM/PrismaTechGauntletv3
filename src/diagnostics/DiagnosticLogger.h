/**
 * DiagnosticLogger.h
 * 
 * A lightweight logging component that provides configurable 
 * diagnostic output for debugging and troubleshooting.
 */

#ifndef DIAGNOSTIC_LOGGER_H
#define DIAGNOSTIC_LOGGER_H

#include <Arduino.h>

// Control whether diagnostic logging is enabled (set in platformio.ini)
#ifndef DIAG_LOGGING_ENABLED
#define DIAG_LOGGING_ENABLED true
#endif

// Diagnostic tags (subsystems)
#define DIAG_TAG_GESTURE    0x01
#define DIAG_TAG_ANIMATION  0x02
#define DIAG_TAG_MODE       0x03
#define DIAG_TAG_MOTION     0x04

// Diagnostic log levels
enum DiagnosticLogLevel {
  DIAG_LEVEL_CRITICAL = 1,
  DIAG_LEVEL_ERROR = 2,
  DIAG_LEVEL_WARNING = 3,
  DIAG_LEVEL_INFO = 4,
  DIAG_LEVEL_DEBUG = 5,
  DIAG_LEVEL_TRACE = 6
};

// Default minimum log level (only log messages with level <= this value)
#ifndef DIAG_LOG_LEVEL
#define DIAG_LOG_LEVEL DIAG_LEVEL_INFO
#endif

// Logging macros (conditionally compiled)
#if DIAG_LOGGING_ENABLED
  // Main logging macro with level and tag
  #define DIAG_LOG(level, tag, ...) \
    do { \
      if (DiagnosticLogger::isEnabled() && (level) <= DIAG_LOG_LEVEL) { \
        DiagnosticLogger::print("[%s:%s] ", \
          DiagnosticLogger::getLevelName(level), \
          DiagnosticLogger::getTagName(tag)); \
        DiagnosticLogger::print(__VA_ARGS__); \
        DiagnosticLogger::print("\n"); \
      } \
    } while(0)

  // Level-specific convenience macros
  #define DIAG_CRITICAL(tag, ...) DIAG_LOG(DIAG_LEVEL_CRITICAL, tag, __VA_ARGS__)
  #define DIAG_ERROR(tag, ...)    DIAG_LOG(DIAG_LEVEL_ERROR, tag, __VA_ARGS__)
  #define DIAG_WARNING(tag, ...)  DIAG_LOG(DIAG_LEVEL_WARNING, tag, __VA_ARGS__)
  #define DIAG_INFO(tag, ...)     DIAG_LOG(DIAG_LEVEL_INFO, tag, __VA_ARGS__)
  #define DIAG_DEBUG(tag, ...)    DIAG_LOG(DIAG_LEVEL_DEBUG, tag, __VA_ARGS__)
  #define DIAG_TRACE(tag, ...)    DIAG_LOG(DIAG_LEVEL_TRACE, tag, __VA_ARGS__)
#else
  // No-op versions when logging is disabled
  #define DIAG_LOG(level, tag, ...)
  #define DIAG_CRITICAL(tag, ...)
  #define DIAG_ERROR(tag, ...)
  #define DIAG_WARNING(tag, ...)
  #define DIAG_INFO(tag, ...)
  #define DIAG_DEBUG(tag, ...)
  #define DIAG_TRACE(tag, ...)
#endif

/**
 * Diagnostic Logger Class
 * 
 * Provides lightweight, configurable logging for tracking
 * system behavior and diagnosing issues.
 */
class DiagnosticLogger {
public:
  /**
   * Get the string representation of a log level
   */
  static const char* getLevelName(uint8_t level) {
    switch(level) {
      case DIAG_LEVEL_CRITICAL: return "CRIT";
      case DIAG_LEVEL_ERROR:    return "ERROR";
      case DIAG_LEVEL_WARNING:  return "WARN";
      case DIAG_LEVEL_INFO:     return "INFO";
      case DIAG_LEVEL_DEBUG:    return "DEBUG";
      case DIAG_LEVEL_TRACE:    return "TRACE";
      default:                  return "?";
    }
  }
  
  /**
   * Get the string representation of a diagnostic tag
   */
  static const char* getTagName(uint8_t tag);
  
  /**
   * Cross-platform formatted print function
   */
  static void print(const char* format, ...);
  
  /**
   * Enable or disable diagnostic output
   */
  static void setEnabled(bool enable);
  
  /**
   * Check if diagnostics are currently enabled
   */
  static bool isEnabled();
  
private:
  static bool _enabled;
};

#endif // DIAGNOSTIC_LOGGER_H 