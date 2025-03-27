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
  
  // Set default thresholds and dominant axes if not calibrated
  // OFFER - Z axis positive
  _thresholds[POS_OFFER] = 7.0f;
  _dominantAxes[POS_OFFER] = 2; // Z-axis
  
  // CALM - Z axis negative
  _thresholds[POS_CALM] = -7.0f;
  _dominantAxes[POS_CALM] = 2; // Z-axis
  
  // OATH - Y axis positive
  _thresholds[POS_OATH] = 7.0f;
  _dominantAxes[POS_OATH] = 1; // Y-axis
  
  // DIG - Y axis negative
  _thresholds[POS_DIG] = -7.0f;
  _dominantAxes[POS_DIG] = 1; // Y-axis
  
  // SHIELD - X axis negative
  _thresholds[POS_SHIELD] = -7.0f;
  _dominantAxes[POS_SHIELD] = 0; // X-axis
  
  // NULL - X axis positive
  _thresholds[POS_NULL] = 7.0f;
  _dominantAxes[POS_NULL] = 0; // X-axis
  
  // Load calibration data if available, otherwise use defaults
  loadCalibration();
  
  return true;
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

bool UltraBasicPositionDetector::calibratePosition(uint8_t position, uint16_t samples) {
  // Validate position is in range
  if (position >= 6) return false;
  
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
      return false; // Invalid position
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
    
    // Visual feedback of progress (flash LED)
    if (i % 10 == 0) {
      _hardware->setAllLEDs(positionColor);
    } else if (i % 10 == 5) {
      _hardware->setAllLEDs({0, 0, 0});
    }
    _hardware->updateLEDs();
    
    delay(20);
  }
  
  // Calculate averages
  float avgX = sumX / samples;
  float avgY = sumY / samples;
  float avgZ = sumZ / samples;
  
  // Output debug information
  Serial.print("Position ");
  Serial.print(position);
  Serial.print(" calibration data: X=");
  Serial.print(avgX, 2);
  Serial.print(" Y=");
  Serial.print(avgY, 2);
  Serial.print(" Z=");
  Serial.println(avgZ, 2);
  
  // Determine which axis is dominant
  float absX = abs(avgX);
  float absY = abs(avgY);
  float absZ = abs(avgZ);
  
  if (absX >= absY && absX >= absZ) {
    // X is dominant
    _thresholds[position] = avgX * THRESHOLD_SCALE;
    _dominantAxes[position] = 0; // X-axis
    Serial.print("X axis dominant: ");
  } 
  else if (absY >= absX && absY >= absZ) {
    // Y is dominant
    _thresholds[position] = avgY * THRESHOLD_SCALE;
    _dominantAxes[position] = 1; // Y-axis
    Serial.print("Y axis dominant: ");
  }
  else {
    // Z is dominant
    _thresholds[position] = avgZ * THRESHOLD_SCALE;
    _dominantAxes[position] = 2; // Z-axis
    Serial.print("Z axis dominant: ");
  }
  
  Serial.print("Set threshold: ");
  Serial.print(_thresholds[position], 2);
  Serial.print(" for position ");
  Serial.println(position);
  
  // Visual feedback that calibration is complete
  for (int i = 0; i < 3; i++) {
    _hardware->setAllLEDs({0, 255, 0});
    _hardware->updateLEDs();
    delay(100);
    _hardware->setAllLEDs({0, 0, 0});
    _hardware->updateLEDs();
    delay(100);
  }
  
  return true;
}

bool UltraBasicPositionDetector::calibrateAllPositions(uint16_t samplesPerPosition) {
  // 1. Calibrate OFFER (showing purple color)
  Serial.println("\nCalibrating OFFER position (PURPLE):");
  Serial.println("Hold your hand in the OFFER position (palm up, fingers extended)");
  calibratePosition(POS_OFFER, samplesPerPosition);
  delay(2000); // Rest before next position
  
  // 2. Calibrate CALM (showing yellow color)
  Serial.println("\nCalibrating CALM position (YELLOW):");
  Serial.println("Hold your hand in the CALM position (palm down, fingers extended)");
  calibratePosition(POS_CALM, samplesPerPosition);
  delay(2000); // Rest before next position
  
  // 3. Calibrate OATH (showing red color)
  Serial.println("\nCalibrating OATH position (RED):");
  Serial.println("Hold your hand in the OATH position (hand vertical, palm facing inward)");
  calibratePosition(POS_OATH, samplesPerPosition);
  delay(2000); // Rest before next position
  
  // 4. Calibrate DIG (showing green color)
  Serial.println("\nCalibrating DIG position (GREEN):");
  Serial.println("Hold your hand in the DIG position (hand vertical, palm facing outward)");
  calibratePosition(POS_DIG, samplesPerPosition);
  delay(2000); // Rest before next position
  
  // 5. Calibrate SHIELD (showing blue color)
  Serial.println("\nCalibrating SHIELD position (BLUE):");
  Serial.println("Hold your hand in the SHIELD position (arm extended forward, palm facing forward)");
  calibratePosition(POS_SHIELD, samplesPerPosition);
  delay(2000); // Rest before next position
  
  // 6. Calibrate NULL (showing orange color)
  Serial.println("\nCalibrating NULL position (ORANGE):");
  Serial.println("Hold your hand in the NULL position (arm extended backward, palm facing back)");
  calibratePosition(POS_NULL, samplesPerPosition);
  
  // All positions calibrated
  Serial.println("\nAll positions have been calibrated!");
  Serial.println("Final calibration values:");
  
  // Display final calibration values for reference
  for (int i = 0; i < 6; i++) {
    Serial.print("Position ");
    Serial.print(i);
    Serial.print(": Axis ");
    Serial.print(_dominantAxes[i]);
    Serial.print(" Threshold ");
    Serial.println(_thresholds[i], 2);
  }
  
  // Save calibration
  saveCalibration();
  
  return true;
}

bool UltraBasicPositionDetector::saveCalibration() {
  // For now, just print values - would normally save to flash or EEPROM
  Serial.println("Calibration values for Config.h:");
  Serial.println("// Position detection thresholds (calibrated)");
  
  for (int i = 0; i < 6; i++) {
    // Generate a descriptive comment and threshold name
    const char* posNames[] = {"OFFER", "CALM", "OATH", "DIG", "SHIELD", "NULL"};
    const char* axisNames[] = {"X", "Y", "Z"};
    
    Serial.print("constexpr float THRESHOLD_");
    Serial.print(posNames[i]);
    Serial.print(" = ");
    Serial.print(_thresholds[i], 2);
    Serial.print("f;");
    Serial.print(" // ");
    Serial.print(axisNames[_dominantAxes[i]]);
    Serial.println("-axis threshold");
  }
  
  return true;
}

bool UltraBasicPositionDetector::loadCalibration() {
  // Would normally load from flash or EEPROM
  // For now, just use the default values set in init()
  return true;
} 