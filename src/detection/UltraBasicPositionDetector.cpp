#include "UltraBasicPositionDetector.h"
#include "../core/Config.h"
#include <Arduino.h>

bool UltraBasicPositionDetector::init(HardwareManager* hardware) {
  if (!hardware) {
    return false;
  }
  
  _hardware = hardware;
  
  // Initialize current position to unknown
  _currentPosition.position = POS_UNKNOWN;
  _currentPosition.confidence = 100;
  _currentPosition.timestamp = millis();
  
  // Initialize processed data
  _currentProcessedData = {0.0f, 0.0f, 0.0f};
  
  // Initialize sample buffer
  for (int i = 0; i < POSITION_AVERAGE_SAMPLES; i++) {
    _sampleBuffer[i] = {0, 0, 0, 0, 0, 0, 0};
  }
  
  // Initialize dominant axes
  _dominantAxes[POS_OFFER] = 2;  // Z-axis
  _dominantAxes[POS_CALM] = 2;   // Z-axis
  _dominantAxes[POS_OATH] = 1;   // Y-axis
  _dominantAxes[POS_DIG] = 1;    // Y-axis
  _dominantAxes[POS_SHIELD] = 0; // X-axis
  _dominantAxes[POS_NULL] = 0;   // X-axis
  
  // Load thresholds from defaults
  loadDefaultThresholds();
  
  return true;
}

void UltraBasicPositionDetector::loadDefaultThresholds() {
  // Load default thresholds from Config.h
  for (int i = 0; i < 6; i++) {
    _thresholds[i] = Config::DEFAULT_POSITION_THRESHOLDS[i];
  }
  Serial.println("Using default thresholds from Config.h");
}

PositionReading UltraBasicPositionDetector::update() {
  // 1. Get raw sensor data
  SensorData rawData = _hardware->getSensorData();
  
  // 2. Add to averaging buffer
  _sampleBuffer[_currentSampleIndex] = rawData;
  _currentSampleIndex = (_currentSampleIndex + 1) % POSITION_AVERAGE_SAMPLES;
  
  // 3. Calculate averaged data
  SensorData averagedData = calculateAveragedData();
  
  // 4. Process to physical units
  processRawData(averagedData, _currentProcessedData);
  
  // 5. Detect position
  _currentPosition = detectPosition(_currentProcessedData);
  
  return _currentPosition;
}

PositionReading UltraBasicPositionDetector::getCurrentPosition() const {
  return _currentPosition;
}

ProcessedData UltraBasicPositionDetector::getProcessedData() const {
  return _currentProcessedData;
}

void UltraBasicPositionDetector::processRawData(const SensorData& raw, ProcessedData& processed) {
  // Apply scaling to convert to m/s²
  processed.accelX = raw.accelX * SCALING_FACTOR;
  processed.accelY = -raw.accelY * SCALING_FACTOR; // Invert for face-down mounting
  processed.accelZ = raw.accelZ * SCALING_FACTOR;
}

PositionReading UltraBasicPositionDetector::detectPosition(const ProcessedData& data) {
  PositionReading reading;
  reading.position = POS_UNKNOWN;
  reading.confidence = 0;
  reading.timestamp = millis();
  
  // Store values for easier access
  float values[3] = {data.accelX, data.accelY, data.accelZ};
  
  // Debug output
  Serial.print("Detection values: X=");
  Serial.print(data.accelX, 2);
  Serial.print(" Y=");
  Serial.print(data.accelY, 2);
  Serial.print(" Z=");
  Serial.print(data.accelZ, 2);
  
  // Check each position based on its dominant axis and threshold
  float maxConfidence = 0;
  int bestPosition = POS_UNKNOWN;
  
  for (int pos = 0; pos < 6; pos++) {
    int axis = _dominantAxes[pos];
    float threshold = _thresholds[pos];
    float value = values[axis];
    
    // Calculate how well this matches (distance from threshold)
    float confidence = 0;
    
    // If threshold is positive, we check if value > threshold
    // If threshold is negative, we check if value < threshold
    if ((threshold >= 0 && value > threshold) || 
        (threshold < 0 && value < threshold)) {
      // Calculate confidence based on how far over threshold
      confidence = 100.0f * (abs(value) / (abs(threshold) * 1.5f));
      confidence = min(confidence, 100.0f); // Cap at 100%
      
      Serial.print(" | Pos ");
      Serial.print(pos);
      Serial.print(": ");
      Serial.print(confidence, 0);
      Serial.print("%");
      
      // Track best match
      if (confidence > maxConfidence) {
        maxConfidence = confidence;
        bestPosition = pos;
      }
    }
  }
  
  Serial.print(" | Best: ");
  Serial.println(bestPosition);
  
  // Set the best position if confidence is reasonable
  if (maxConfidence > MIN_CONFIDENCE) {
    reading.position = bestPosition;
    reading.confidence = maxConfidence;
  }
  
  return reading;
}

SensorData UltraBasicPositionDetector::calculateAveragedData() {
  SensorData result = {0, 0, 0, 0, 0, 0, 0};
  
  // Sum all samples
  for (int i = 0; i < POSITION_AVERAGE_SAMPLES; i++) {
    result.accelX += _sampleBuffer[i].accelX;
    result.accelY += _sampleBuffer[i].accelY;
    result.accelZ += _sampleBuffer[i].accelZ;
    result.gyroX += _sampleBuffer[i].gyroX;
    result.gyroY += _sampleBuffer[i].gyroY;
    result.gyroZ += _sampleBuffer[i].gyroZ;
  }
  
  // Divide by number of samples
  result.accelX /= POSITION_AVERAGE_SAMPLES;
  result.accelY /= POSITION_AVERAGE_SAMPLES;
  result.accelZ /= POSITION_AVERAGE_SAMPLES;
  result.gyroX /= POSITION_AVERAGE_SAMPLES;
  result.gyroY /= POSITION_AVERAGE_SAMPLES;
  result.gyroZ /= POSITION_AVERAGE_SAMPLES;
  result.timestamp = millis();
  
  return result;
}

float UltraBasicPositionDetector::calibratePosition(uint8_t position, uint16_t samples) {
  // Validate position is in range
  if (position >= 6) return 0.0f;
  
  // 1. Visual feedback that calibration is starting
  Color positionColor;
  switch (position) {
    case POS_OFFER:
      positionColor = {Config::Colors::OFFER_COLOR[0], Config::Colors::OFFER_COLOR[1], Config::Colors::OFFER_COLOR[2]};
      break;
    case POS_CALM:
      positionColor = {Config::Colors::CALM_COLOR[0], Config::Colors::CALM_COLOR[1], Config::Colors::CALM_COLOR[2]};
      break;
    case POS_OATH:
      positionColor = {Config::Colors::OATH_COLOR[0], Config::Colors::OATH_COLOR[1], Config::Colors::OATH_COLOR[2]};
      break;
    case POS_DIG:
      positionColor = {Config::Colors::DIG_COLOR[0], Config::Colors::DIG_COLOR[1], Config::Colors::DIG_COLOR[2]};
      break;
    case POS_SHIELD:
      positionColor = {Config::Colors::SHIELD_COLOR[0], Config::Colors::SHIELD_COLOR[1], Config::Colors::SHIELD_COLOR[2]};
      break;
    case POS_NULL:
      positionColor = {Config::Colors::NULL_COLOR[0], Config::Colors::NULL_COLOR[1], Config::Colors::NULL_COLOR[2]};
      break;
    default:
      return 0.0f; // Invalid position
  }
  
  // 2. Collect and average samples for calibration
  float sumX = 0, sumY = 0, sumZ = 0;
  float maxX = -9999, maxY = -9999, maxZ = -9999;
  float minX = 9999, minY = 9999, minZ = 9999;
  
  // Discard first 10 samples (stabilization)
  for (int i = 0; i < 10; i++) {
    _hardware->update();
    delay(20);
  }
  
  // Collect samples for averaging
  for (int i = 0; i < samples; i++) {
    _hardware->update();
    
    // Get sensor data and process
    SensorData raw = _hardware->getSensorData();
    ProcessedData data;
    processRawData(raw, data);
    
    // Track running sums
    sumX += data.accelX;
    sumY += data.accelY;
    sumZ += data.accelZ;
    
    // Track min/max values
    maxX = max(maxX, data.accelX);
    maxY = max(maxY, data.accelY);
    maxZ = max(maxZ, data.accelZ);
    minX = min(minX, data.accelX);
    minY = min(minY, data.accelY);
    minZ = min(minZ, data.accelZ);
    
    // Flash LED to indicate sampling
    if (i % 5 == 0) {
      _hardware->setAllLEDs(positionColor);
    } else {
      _hardware->setAllLEDs({0, 0, 0});
    }
    _hardware->updateLEDs();
    
    delay(20);
  }
  
  // Calculate average
  float avgX = sumX / samples;
  float avgY = sumY / samples;
  float avgZ = sumZ / samples;
  
  Serial.print("Position average: X=");
  Serial.print(avgX, 2);
  Serial.print(" Y=");
  Serial.print(avgY, 2);
  Serial.print(" Z=");
  Serial.println(avgZ, 2);
  
  Serial.print("Position range: X=[");
  Serial.print(minX, 2);
  Serial.print(" to ");
  Serial.print(maxX, 2);
  Serial.print("] Y=[");
  Serial.print(minY, 2);
  Serial.print(" to ");
  Serial.print(maxY, 2);
  Serial.print("] Z=[");
  Serial.print(minZ, 2);
  Serial.print(" to ");
  Serial.print(maxZ, 2);
  Serial.println("]");
  
  // 3. Calculate the threshold based on dominant axis average
  float newThreshold = 0;
  int axis = _dominantAxes[position];
  switch (axis) {
    case 0: newThreshold = avgX * THRESHOLD_SCALE; break;
    case 1: newThreshold = avgY * THRESHOLD_SCALE; break;
    case 2: newThreshold = avgZ * THRESHOLD_SCALE; break;
  }
  
  // 4. Update threshold locally for testing
  _thresholds[position] = newThreshold;
  
  Serial.print("Calibrated threshold for position ");
  Serial.print(position);
  Serial.print(": ");
  Serial.println(newThreshold, 2);
  
  // 5. Complete indication
  for (int i = 0; i < 3; i++) {
    _hardware->setAllLEDs({0, 255, 0});
    _hardware->updateLEDs();
    delay(100);
    _hardware->setAllLEDs({0, 0, 0});
    _hardware->updateLEDs();
    delay(100);
  }
  
  return newThreshold;
}

bool UltraBasicPositionDetector::calibrateAllPositions(uint16_t samplesPerPosition) {
  // Calibrate each position in sequence
  for (uint8_t pos = 0; pos < 6; pos++) {
    float threshold = calibratePosition(pos, samplesPerPosition);
    if (threshold == 0.0f) {
      return false;
    }
    delay(1000); // Pause between positions
  }
  
  return true;
}

void UltraBasicPositionDetector::setThreshold(uint8_t position, float threshold) {
  if (position < 6) {
    _thresholds[position] = threshold;
  }
}

float UltraBasicPositionDetector::getThreshold(uint8_t position) const {
  if (position < 6) {
    return _thresholds[position];
  }
  return 0.0f;
} 