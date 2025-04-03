#include "ShakeGestureDetector.h"
#include "../core/Config.h"
#include <Arduino.h>

// Diagnostic includes
#if DIAG_LOGGING_ENABLED
#include "../diagnostics/DiagnosticLogger.h"
#endif

ShakeGestureDetector::ShakeGestureDetector() 
  : mpuInterface(nullptr),
    wasAboveThreshold(false),
    crossingCount(0),
    lastCrossingTime(0),
    lastShakeTime(0),
    shakeDetected(false),
    lastShakeIntensity(0.0f)
{
}

bool ShakeGestureDetector::init(MPU9250Interface* mpuInterface) {
  if (!mpuInterface) {
    Serial.println(F("ERROR: ShakeGestureDetector requires valid MPU interface"));
    return false;
  }
  
  this->mpuInterface = mpuInterface;
  
  // Reset initial state
  reset();
  
  #if DIAG_LOGGING_ENABLED
  DIAG_INFO(DIAG_TAG_GESTURE, "ShakeGestureDetector initialized");
  #endif
  
  return true;
}

void ShakeGestureDetector::update() {
  // Skip if no MPU interface
  if (!mpuInterface) {
    return;
  }
  
  // Skip if in cooldown period
  if (isCoolingDown()) {
    return;
  }
  
  // Read current sensor data
  SensorData data;
  if (!mpuInterface->readSensorData(&data)) {
    #if DIAG_LOGGING_ENABLED
    DIAG_WARNING(DIAG_TAG_GESTURE, "Failed to read sensor data for shake detection");
    #endif
    return;
  }
  
  // Calculate acceleration magnitude
  uint32_t magnitude = calculateMagnitude(data);
  
  // Current time for timeout tracking
  uint32_t currentTime = millis();
  
  // Reset counter if too much time elapsed between crossings
  if (currentTime - lastCrossingTime > Config::ShakeDetection::SHAKE_MAX_CROSSING_INTERVAL_MS) {
    crossingCount = 0;
  }
  
  // State machine for crossing detection
  if (!wasAboveThreshold && magnitude > Config::ShakeDetection::SHAKE_HIGH_THRESHOLD) {
    wasAboveThreshold = true;
    crossingCount++;
    lastCrossingTime = currentTime;
    
    #if DIAG_LOGGING_ENABLED
    DIAG_DEBUG(DIAG_TAG_GESTURE, "Shake upward crossing: count=%d, mag=%lu", 
              crossingCount, magnitude);
    #endif
  } 
  else if (wasAboveThreshold && magnitude < Config::ShakeDetection::SHAKE_LOW_THRESHOLD) {
    wasAboveThreshold = false;
    crossingCount++;
    lastCrossingTime = currentTime;
    
    #if DIAG_LOGGING_ENABLED
    DIAG_DEBUG(DIAG_TAG_GESTURE, "Shake downward crossing: count=%d, mag=%lu", 
              crossingCount, magnitude);
    #endif
  }
  
  // Check if shake threshold reached
  if (crossingCount >= Config::ShakeDetection::SHAKE_REQUIRED_CROSSINGS) {
    // Record shake detection
    lastShakeTime = currentTime;
    lastShakeIntensity = magnitude / 10000.0f; // Normalize to 0.0-1.0 range
    shakeDetected = true;
    crossingCount = 0; // Reset for next detection
    
    #if DIAG_LOGGING_ENABLED
    DIAG_INFO(DIAG_TAG_GESTURE, "Shake gesture detected! Intensity: %.2f", 
             lastShakeIntensity);
    #endif
  }
}

bool ShakeGestureDetector::isShakeDetected() {
  if (shakeDetected) {
    shakeDetected = false; // Clear the flag once read
    return true;
  }
  return false;
}

void ShakeGestureDetector::reset() {
  wasAboveThreshold = false;
  crossingCount = 0;
  lastCrossingTime = 0;
  shakeDetected = false;
  
  #if DIAG_LOGGING_ENABLED
  DIAG_DEBUG(DIAG_TAG_GESTURE, "ShakeGestureDetector reset");
  #endif
}

uint32_t ShakeGestureDetector::calculateMagnitude(const SensorData& data) {
  // Simple sum of absolute values across all axes
  uint32_t magnitude = 0;
  
  // Add accelerometer components
  magnitude += abs(data.accelX);
  magnitude += abs(data.accelY);
  magnitude += abs(data.accelZ);
  
  return magnitude;
}

bool ShakeGestureDetector::isCoolingDown() const {
  return (millis() - lastShakeTime) < Config::ShakeDetection::SHAKE_COOLDOWN_MS;
} 