/**
 * @file PositionDetector.cpp
 * @brief Implementation of the Dominant Axis Detection model for hand position recognition
 */

#include "PositionDetector.h"
#include "../core/Config.h"
#include <Arduino.h>

bool PositionDetector::init(HardwareManager* hardware) {
  if (!hardware) return false;
  
  // Store hardware reference
  hardwareRef = hardware;
  
  // Initialize position to unknown
  currentPosition.position = POS_UNKNOWN;
  currentPosition.confidence = 0;
  currentPosition.timestamp = millis();
  
  // Clear sample buffer
  for (int i = 0; i < POSITION_AVERAGE_SAMPLES; i++) {
    recentSamples[i].accelX = 0;
    recentSamples[i].accelY = 0;
    recentSamples[i].accelZ = 0;
    recentSamples[i].timestamp = 0;
  }
  
  // Reset sample index
  sampleIndex = 0;
  
  return true;
}

PositionReading PositionDetector::detectPosition(const SensorData& sensorData) {
  // Add new sample to buffer (circular buffer implementation)
  recentSamples[sampleIndex] = sensorData;
  sampleIndex = (sampleIndex + 1) % POSITION_AVERAGE_SAMPLES;
  
  // Calculate averaged data
  SensorData averagedData = calculateAveragedData();
  
  // Determine position
  uint8_t detectedPosition = determinePositionFromAxes(averagedData);
  
  // Update current position
  currentPosition.position = detectedPosition;
  // NOTE: Setting confidence to 100 as per user's preference to avoid confidence calculation
  currentPosition.confidence = 100;
  currentPosition.timestamp = millis();
  
  return currentPosition;
}

PositionReading PositionDetector::getCurrentPosition() const {
  return currentPosition;
}

#ifdef TEST_MODE
bool PositionDetector::calibrate() {
  // This is a stub method and is NOT meant to be called during runtime
  // Calibration is performed separately via dedicated calibration routines
  return false;
}

void PositionDetector::setThreshold(uint16_t threshold) {
  // This method is no longer used as thresholds are now managed by Config.h
}
#endif // TEST_MODE

uint8_t PositionDetector::determinePositionFromAxes(const SensorData& averagedData) {
  // Implement Point Detection Model logic
  // Check each position-specific condition in sequence
  if (isZDominantPositive(averagedData)) {
    return POS_OFFER;
  } else if (isZDominantNegative(averagedData)) {
    return POS_CALM;
  } else if (isYDominantNegative(averagedData)) {
    return POS_OATH;
  } else if (isYDominantPositive(averagedData)) {
    return POS_DIG;
  } else if (isXDominantNegative(averagedData)) {
    return POS_SHIELD;
  } else if (isXDominantPositive(averagedData)) {
    return POS_NULL;
  }
  
  // If no position is detected
  return POS_UNKNOWN;
}

uint8_t PositionDetector::calculateConfidence(const SensorData& sensorData, uint8_t position) {
  // Per user's request, we are avoiding confidence calculations
  // This is left as a stub method to maintain interface compatibility
  return 100;
}

SensorData PositionDetector::calculateAveragedData() {
  SensorData result;
  result.accelX = 0;
  result.accelY = 0;
  result.accelZ = 0;
  result.gyroX = 0;
  result.gyroY = 0;
  result.gyroZ = 0;
  result.timestamp = millis();
  
  // Sum all samples
  for (int i = 0; i < POSITION_AVERAGE_SAMPLES; i++) {
    result.accelX += recentSamples[i].accelX;
    result.accelY += recentSamples[i].accelY;
    result.accelZ += recentSamples[i].accelZ;
    result.gyroX += recentSamples[i].gyroX;
    result.gyroY += recentSamples[i].gyroY;
    result.gyroZ += recentSamples[i].gyroZ;
  }
  
  // Calculate average
  result.accelX /= POSITION_AVERAGE_SAMPLES;
  result.accelY /= POSITION_AVERAGE_SAMPLES;
  result.accelZ /= POSITION_AVERAGE_SAMPLES;
  result.gyroX /= POSITION_AVERAGE_SAMPLES;
  result.gyroY /= POSITION_AVERAGE_SAMPLES;
  result.gyroZ /= POSITION_AVERAGE_SAMPLES;
  
  return result;
}

// Dominant axis detection helpers
// These implement the Point Detection Model where only specific regions
// of the sensor space are assigned to positions

bool PositionDetector::isXDominantPositive(const SensorData& data) {
  // X-axis is dominant and positive (POS_NULL - Orange)
  return (data.accelX > Config::THRESHOLD_NULL) && 
         (abs(data.accelX) > abs(data.accelY)) && 
         (abs(data.accelX) > abs(data.accelZ));
}

bool PositionDetector::isXDominantNegative(const SensorData& data) {
  // X-axis is dominant and negative (POS_SHIELD - Blue)
  return (data.accelX < Config::THRESHOLD_SHIELD) && 
         (abs(data.accelX) > abs(data.accelY)) && 
         (abs(data.accelX) > abs(data.accelZ));
}

bool PositionDetector::isYDominantPositive(const SensorData& data) {
  // Y-axis is dominant and positive (POS_DIG - Green)
  return (data.accelY > Config::THRESHOLD_DIG) && 
         (abs(data.accelY) > abs(data.accelX)) && 
         (abs(data.accelY) > abs(data.accelZ));
}

bool PositionDetector::isYDominantNegative(const SensorData& data) {
  // Y-axis is dominant and negative (POS_OATH - Red)
  return (data.accelY < Config::THRESHOLD_OATH) && 
         (abs(data.accelY) > abs(data.accelX)) && 
         (abs(data.accelY) > abs(data.accelZ));
}

bool PositionDetector::isZDominantPositive(const SensorData& data) {
  // Z-axis is dominant and positive (POS_OFFER - Purple)
  return (data.accelZ > Config::THRESHOLD_OFFER) && 
         (abs(data.accelZ) > abs(data.accelX)) && 
         (abs(data.accelZ) > abs(data.accelY));
}

bool PositionDetector::isZDominantNegative(const SensorData& data) {
  // Z-axis is dominant and negative (POS_CALM - Yellow)
  return (data.accelZ < Config::THRESHOLD_CALM) && 
         (abs(data.accelZ) > abs(data.accelX)) && 
         (abs(data.accelZ) > abs(data.accelY));
} 