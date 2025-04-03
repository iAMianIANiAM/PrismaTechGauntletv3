/**
 * DiagnosticLogger.cpp
 * 
 * Implementation of the DiagnosticLogger class for the LUTT toolkit.
 */

#include "DiagnosticLogger.h"
#include <stdarg.h>

// Initialize static variables
bool DiagnosticLogger::_enabled = true;

/**
 * Get the name of a diagnostic tag as a string
 */
const char* DiagnosticLogger::getTagName(uint8_t tag) {
  switch(tag) {
    case DIAG_TAG_GESTURE:  return "GESTURE";
    case DIAG_TAG_ANIMATION: return "ANIMATION";
    case DIAG_TAG_MODE:     return "MODE";
    case DIAG_TAG_MOTION:   return "MOTION";
    default:                return "UNKNOWN";
  }
}

/**
 * Cross-platform formatted print function
 * (handles platforms where Serial.printf is unavailable)
 */
void DiagnosticLogger::print(const char* format, ...) {
  if (!_enabled) return;
  
  char buffer[128]; // Reasonable buffer size for diagnostic messages
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  
  Serial.print(buffer);
}

/**
 * Enable or disable all diagnostic output
 */
void DiagnosticLogger::setEnabled(bool enable) {
  _enabled = enable;
}

/**
 * Check if diagnostics are currently enabled
 */
bool DiagnosticLogger::isEnabled() {
  return _enabled;
} 