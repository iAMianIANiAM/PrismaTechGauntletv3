#include "UltraBasicPositionDetector.h"
#include "../core/Config.h"
#include <Arduino.h>

bool UltraBasicPositionDetector::init(HardwareManager* hardware) {
  // Store hardware reference
  _hardware = hardware;
  
  // Verify hardware reference is valid
  if (!_hardware) {
    return false;
  }
  
  // Initialize MPU9250 through hardware manager
  if (!_hardware->init()) {
    return false;
  }
  
  // Initialize position data
  _currentPosition.position = POS_UNKNOWN;
  _currentPosition.confidence = 0.0f;
  _currentPosition.timestamp = millis();
  
  // Zero out the processed data structure
  _currentProcessedData.accelX = 0.0f;
  _currentProcessedData.accelY = 0.0f;
  _currentProcessedData.accelZ = 0.0f;
  
  // Clear sample buffer
  for (uint8_t i = 0; i < POSITION_AVERAGE_SAMPLES; i++) {
    _sampleBuffer[i] = {0};
  }
  _currentSampleIndex = 0;
  
  // Load default thresholds
  loadDefaultThresholds();
  
  return true;
}

PositionReading UltraBasicPositionDetector::update() {
  // Get sensor data from MPU via hardware manager
  SensorData rawData = _hardware->getSensorData();
  
  // Store in circular buffer
  _sampleBuffer[_currentSampleIndex] = rawData;
  _currentSampleIndex = (_currentSampleIndex + 1) % POSITION_AVERAGE_SAMPLES;
  
  // Calculate averaged data
  SensorData averagedData = calculateAveragedData();
  
  // Process raw averaged data
  processRawData(averagedData, _currentProcessedData);
  
  // Detect position based on processed data
  _currentPosition = detectPosition(_currentProcessedData);
  
  // Return the current position reading
  return _currentPosition;
}

PositionReading UltraBasicPositionDetector::getCurrentPosition() const {
  return _currentPosition;
}

ProcessedData UltraBasicPositionDetector::getProcessedData() const {
  return _currentProcessedData;
}

SensorData UltraBasicPositionDetector::calculateAveragedData() {
  SensorData result = {0};
  int32_t accelX = 0, accelY = 0, accelZ = 0;
  int32_t gyroX = 0, gyroY = 0, gyroZ = 0;
  
  // Sum all samples
  for (uint8_t i = 0; i < POSITION_AVERAGE_SAMPLES; i++) {
    accelX += _sampleBuffer[i].accelX;
    accelY += _sampleBuffer[i].accelY;
    accelZ += _sampleBuffer[i].accelZ;
    gyroX += _sampleBuffer[i].gyroX;
    gyroY += _sampleBuffer[i].gyroY;
    gyroZ += _sampleBuffer[i].gyroZ;
  }
  
  // Average the samples
  result.accelX = accelX / POSITION_AVERAGE_SAMPLES;
  result.accelY = accelY / POSITION_AVERAGE_SAMPLES;
  result.accelZ = accelZ / POSITION_AVERAGE_SAMPLES;
  result.gyroX = gyroX / POSITION_AVERAGE_SAMPLES;
  result.gyroY = gyroY / POSITION_AVERAGE_SAMPLES;
  result.gyroZ = gyroZ / POSITION_AVERAGE_SAMPLES;
  
  return result;
}

void UltraBasicPositionDetector::processRawData(const SensorData& raw, ProcessedData& processed) {
  // Convert raw values to physical units (m/s²)
  processed.accelX = raw.accelX * SCALING_FACTOR;
  processed.accelY = raw.accelY * SCALING_FACTOR;
  processed.accelZ = raw.accelZ * SCALING_FACTOR;
}

PositionReading UltraBasicPositionDetector::detectPosition(const ProcessedData& data) {
  PositionReading result;
  result.timestamp = millis();
  
  // Check each position by dominant axis and threshold
  float maxConfidence = 0.0f;
  uint8_t bestPosition = POS_UNKNOWN;
  
  // Check each defined position (excluding POS_UNKNOWN)
  for (uint8_t pos = 0; pos < 6; pos++) {
    float value = 0.0f;
    
    // Get value from the dominant axis for this position
    switch (_dominantAxes[pos]) {
      case 0: value = data.accelX; break;
      case 1: value = data.accelY; break;
      case 2: value = data.accelZ; break;
    }
    
    // Calculate confidence based on how close to the threshold
    float confidence = 0.0f;
    float threshold = _thresholds[pos];
    
    if (threshold != 0.0f) {
      confidence = (value / threshold) * 100.0f;
      
      // If confidence exceeds our minimum and is better than previous best
      if (confidence >= MIN_CONFIDENCE && confidence > maxConfidence) {
        maxConfidence = confidence;
        bestPosition = pos;
      }
    }
  }
  
  // Special case for NULL position (neutral/flat)
  // If no strong position detected and gravity along Z-axis
  if (maxConfidence < MIN_CONFIDENCE && 
      abs(data.accelX) < 2.0f && 
      abs(data.accelY) < 2.0f && 
      abs(data.accelZ - 9.81f) < 2.0f) {
    bestPosition = POS_NULL;
    maxConfidence = 100.0f;
  }
  
  // If still no position detected, it's unknown
  if (maxConfidence < MIN_CONFIDENCE) {
    bestPosition = POS_UNKNOWN;
    maxConfidence = 0.0f;
  }
  
  // Set result
  result.position = bestPosition;
  result.confidence = maxConfidence;
  
  return result;
}

void UltraBasicPositionDetector::loadDefaultThresholds() {
  // Load default thresholds for each position
  // Thresholds are in m/s² and represent approximately what
  // we expect to see in the dominant axis for each position
  
  // OFFER: X-axis dominant, negative (palm up)
  _thresholds[POS_OFFER] = 5.0f;
  _dominantAxes[POS_OFFER] = 0; // X-axis
  
  // CALM: Y-axis dominant, positive (palm inward)
  _thresholds[POS_CALM] = 5.0f;
  _dominantAxes[POS_CALM] = 1; // Y-axis
  
  // OATH: Y-axis dominant, negative (palm outward)
  _thresholds[POS_OATH] = -5.0f;
  _dominantAxes[POS_OATH] = 1; // Y-axis
  
  // DIG: X-axis dominant, positive (palm down)
  _thresholds[POS_DIG] = -5.0f;
  _dominantAxes[POS_DIG] = 0; // X-axis
  
  // SHIELD: Z-axis dominant, negative (forward)
  _thresholds[POS_SHIELD] = 5.0f;
  _dominantAxes[POS_SHIELD] = 2; // Z-axis
  
  // NULL: No threshold, special case
  _thresholds[POS_NULL] = 0.0f;
  _dominantAxes[POS_NULL] = 2; // Z-axis (gravity)
}

float UltraBasicPositionDetector::calibratePosition(uint8_t position, uint16_t samples) {
  // Only calibrate valid positions (not UNKNOWN or NULL)
  if (position >= 6) {
    return 0.0f;
  }
  
  // Zero out accumulators
  float accumX = 0.0f, accumY = 0.0f, accumZ = 0.0f;
  uint16_t validSamples = 0;
  
  // Collect samples
  for (uint16_t i = 0; i < samples; i++) {
    SensorData rawData = _hardware->getSensorData();
    ProcessedData processedData;
    
    processRawData(rawData, processedData);
    
    // Accumulate processed values
    accumX += processedData.accelX;
    accumY += processedData.accelY;
    accumZ += processedData.accelZ;
    validSamples++;
    
    delay(10); // Short delay between readings
  }
  
  // If we got some valid samples
  if (validSamples > 0) {
    // Calculate averages
    float avgX = accumX / validSamples;
    float avgY = accumY / validSamples;
    float avgZ = accumZ / validSamples;
    
    // Find the dominant axis (with highest absolute value)
    float absX = abs(avgX);
    float absY = abs(avgY);
    float absZ = abs(avgZ);
    
    if (absX >= absY && absX >= absZ) {
      // X is dominant
      _dominantAxes[position] = 0;
      _thresholds[position] = avgX;
    }
    else if (absY >= absX && absY >= absZ) {
      // Y is dominant
      _dominantAxes[position] = 1;
      _thresholds[position] = avgY;
    }
    else {
      // Z is dominant
      _dominantAxes[position] = 2;
      _thresholds[position] = avgZ;
    }
    
    // Scale threshold slightly for better detection reliability
    _thresholds[position] *= THRESHOLD_SCALE;
    
    return _thresholds[position];
  }
  
  return 0.0f; // Return zero if no valid samples were collected
}

bool UltraBasicPositionDetector::calibrateAllPositions(uint16_t samplesPerPosition) {
  // Calibrate each position in sequence
  for (uint8_t pos = 0; pos < 6; pos++) {
    // Skip NULL position which is special-cased
    if (pos == POS_NULL) {
      continue;
    }
    
    // Skip UNKNOWN position
    if (pos == POS_UNKNOWN) {
      continue;
    }
    
    // Calibrate this position
    if (calibratePosition(pos, samplesPerPosition) == 0.0f) {
      return false; // Calibration failed
    }
  }
  
  return true;
}

void UltraBasicPositionDetector::setThreshold(uint8_t position, float threshold) {
  if (position < 6) {
    _thresholds[position] = threshold;
  }
}

float UltraBasicPositionDetector::getThreshold(uint8_t position) const {
  return (position < 6) ? _thresholds[position] : 0.0f;
} 