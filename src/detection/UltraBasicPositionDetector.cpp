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
  // Convert raw values to physical units (m/s²) using the current scaling factor
  processed.accelX = raw.accelX * _currentScalingFactor;
  processed.accelY = raw.accelY * _currentScalingFactor;
  processed.accelZ = raw.accelZ * _currentScalingFactor;
  
  // Debug data flow every second to verify proper scaling
  static unsigned long lastDebugTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastDebugTime > 1000) {
    Serial.println("===== Data Flow Tracing =====");
    Serial.print("Raw Values: X=");
    Serial.print(raw.accelX);
    Serial.print(", Y=");
    Serial.print(raw.accelY);
    Serial.print(", Z=");
    Serial.println(raw.accelZ);
    
    Serial.print("Scaling Factor: ");
    Serial.println(_currentScalingFactor, 10); // Print with high precision to verify correct value
    
    Serial.print("Processed (m/s²): X=");
    Serial.print(processed.accelX, 2);
    Serial.print(", Y=");
    Serial.print(processed.accelY, 2);
    Serial.print(", Z=");
    Serial.println(processed.accelZ, 2);
    
    // Validate gravity reading when device is relatively flat
    if (abs(processed.accelX) < 2.0f && abs(processed.accelY) < 2.0f) {
      float gravityError = abs(abs(processed.accelZ) - 9.81f);
      float errorPercent = (gravityError / 9.81f) * 100.0f;
      
      Serial.print("Gravity Reading: ");
      Serial.print(processed.accelZ, 2);
      Serial.print(" m/s² (Error: ");
      Serial.print(errorPercent, 1);
      Serial.println("%)");
      
      if (errorPercent > 5.0f) {
        Serial.println("WARNING: Gravity reading error exceeds 5% - Scaling may be incorrect");
      }
    }
    
    Serial.println("=============================");
    lastDebugTime = currentTime;
  }
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
  // Load thresholds for each position from calibrated values if available
  
  // OFFER position
  _thresholds[POS_OFFER] = Config::Calibrated::OFFER_THRESHOLD;
  _dominantAxes[POS_OFFER] = Config::Calibrated::OFFER_AXIS;
  
  // CALM position
  _thresholds[POS_CALM] = Config::Calibrated::CALM_THRESHOLD;
  _dominantAxes[POS_CALM] = Config::Calibrated::CALM_AXIS;
  
  // OATH position
  _thresholds[POS_OATH] = Config::Calibrated::OATH_THRESHOLD;
  _dominantAxes[POS_OATH] = Config::Calibrated::OATH_AXIS;
  
  // DIG position
  _thresholds[POS_DIG] = Config::Calibrated::DIG_THRESHOLD;
  _dominantAxes[POS_DIG] = Config::Calibrated::DIG_AXIS;
  
  // SHIELD position
  _thresholds[POS_SHIELD] = Config::Calibrated::SHIELD_THRESHOLD;
  _dominantAxes[POS_SHIELD] = Config::Calibrated::SHIELD_AXIS;
  
  // NULL position
  _thresholds[POS_NULL] = Config::Calibrated::NULL_THRESHOLD;
  _dominantAxes[POS_NULL] = Config::Calibrated::NULL_AXIS;
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

void UltraBasicPositionDetector::setDominantAxis(uint8_t position, uint8_t axis) {
  if (position < 6 && axis < 3) {
    _dominantAxes[position] = axis;
  }
}

uint8_t UltraBasicPositionDetector::getDominantAxis(uint8_t position) const {
  return (position < 6) ? _dominantAxes[position] : 0;
}

void UltraBasicPositionDetector::printCalibrationData(Stream& serialOut) {
  serialOut.println("\n======= CALIBRATION DATA =======");
  serialOut.println("Position Detection Thresholds:");
  
  for (uint8_t pos = 0; pos < 6; pos++) {
    // Skip UNKNOWN position
    if (pos == POS_UNKNOWN) {
      continue;
    }
    
    const char* posName = getPositionName(pos);
    const char* axisName = getAxisName(_dominantAxes[pos]);
    float threshold = _thresholds[pos];
    
    serialOut.print("Position: ");
    serialOut.print(posName);
    serialOut.print("\n  Dominant Axis: ");
    serialOut.print(axisName);
    serialOut.print("\n  Threshold: ");
    serialOut.print(threshold);
    serialOut.println(" m/s²\n");
  }
  
  serialOut.println("===============================");
}

void UltraBasicPositionDetector::printConfigFormat(Stream& serialOut) {
  // Current date/time string in format YYYY-MM-DD
  char dateStr[11];
  sprintf(dateStr, "%d-%02d-%02d", 2025, 4, 2); // Hardcoded for now
  
  serialOut.println("\n// Position Detection Thresholds (calibrated on " + String(dateStr) + ")");
  
  for (uint8_t pos = 0; pos < 6; pos++) {
    // Skip UNKNOWN position
    if (pos == POS_UNKNOWN) {
      continue;
    }
    
    const char* posName = getPositionName(pos);
    const char* axisName = getAxisName(_dominantAxes[pos]);
    float threshold = _thresholds[pos];
    
    // Format: constexpr float OFFER_THRESHOLD = 5.21f;  // X-axis dominant
    serialOut.print("constexpr float ");
    serialOut.print(posName);
    serialOut.print("_THRESHOLD = ");
    serialOut.print(threshold);
    serialOut.print("f;  // ");
    serialOut.print(axisName);
    serialOut.println("-axis dominant");
  }
  
  serialOut.println("// NULL position uses special case detection");
}

const char* UltraBasicPositionDetector::getPositionName(uint8_t position) const {
  switch (position) {
    case POS_OFFER:  return "OFFER";
    case POS_CALM:   return "CALM";
    case POS_OATH:   return "OATH";
    case POS_DIG:    return "DIG";
    case POS_SHIELD: return "SHIELD";
    case POS_NULL:   return "NULL";
    default:         return "UNKNOWN";
  }
}

const char* UltraBasicPositionDetector::getAxisName(uint8_t axis) const {
  switch (axis) {
    case 0: return "X";
    case 1: return "Y";
    case 2: return "Z";
    default: return "?";
  }
}

float UltraBasicPositionDetector::calibrateScalingFactor(uint16_t testDuration) {
  Serial.println("\n=== ECHO Scaling Factor Validation ===");
  Serial.println("Validating ECHO reference scaling factor against gravity readings...");
  
  // Use the ECHO reference scaling factor
  _currentScalingFactor = DEFAULT_SCALING_FACTOR;
  
  // Variables for statistical analysis
  const int MAX_SAMPLES = 40;
  float zValues[MAX_SAMPLES];  // Store Z values for analysis
  float xValues[MAX_SAMPLES];  // Store X values for stability check
  float yValues[MAX_SAMPLES];  // Store Y values for stability check
  int numSamples = 0;
  float zSum = 0.0f;
  
  Serial.println("Hold the device flat and still for measurement...");
  delay(1000); // Give user time to stabilize
  
  // Collect samples for testDuration milliseconds
  uint32_t startTime = millis();
  uint32_t lastPrintTime = 0;
  
  while (millis() - startTime < testDuration && numSamples < MAX_SAMPLES) {
    SensorData rawData = _hardware->getSensorData();
    ProcessedData processedData;
    
    // Process data with ECHO scaling factor
    processRawData(rawData, processedData);
    
    // Store values for analysis
    xValues[numSamples] = processedData.accelX;
    yValues[numSamples] = processedData.accelY;
    zValues[numSamples] = processedData.accelZ;
    zSum += processedData.accelZ;
    
    // Print sample values every 500ms
    if (millis() - lastPrintTime > 500) {
      Serial.printf("Sample %d: X=%.2f Y=%.2f Z=%.2f m/s²\n", 
                  numSamples + 1, 
                  processedData.accelX, 
                  processedData.accelY, 
                  processedData.accelZ);
      lastPrintTime = millis();
    }
    
    numSamples++;
    delay(50); // 50ms between samples
  }
  
  // Calculate statistics
  float zMean = zSum / numSamples;
  float zVariance = 0.0f;
  float xVariance = 0.0f;
  float yVariance = 0.0f;
  
  // Calculate variance (stability measurement)
  for (int i = 0; i < numSamples; i++) {
    zVariance += (zValues[i] - zMean) * (zValues[i] - zMean);
    xVariance += xValues[i] * xValues[i]; // Variance from 0
    yVariance += yValues[i] * yValues[i]; // Variance from 0
  }
  zVariance /= numSamples;
  xVariance /= numSamples;
  yVariance /= numSamples;
  
  // Calculate error from expected gravity (9.81 m/s²)
  float gravityError = abs(zMean - 9.81f);
  float errorPercent = (gravityError / 9.81f) * 100.0f;
  
  Serial.println("\n=== Validation Results ===");
  Serial.printf("ECHO Scaling Factor: %.10f\n", DEFAULT_SCALING_FACTOR);
  Serial.printf("Gravity Reading: %.2f m/s² (Expected: 9.81 m/s²)\n", zMean);
  Serial.printf("Gravity Error: %.2f m/s² (%.1f%%)\n", gravityError, errorPercent);
  Serial.printf("Stability: X=%.4f Y=%.4f Z=%.4f\n", xVariance, yVariance, zVariance);
  
  if (errorPercent <= 5.0f) {
    Serial.println("✓ VALIDATION PASSED: Gravity reading within 5% of expected value");
    Serial.println("✓ ECHO reference scaling factor is appropriate for this device");
  } else {
    Serial.println("⚠ VALIDATION WARNING: Gravity reading outside 5% tolerance");
    Serial.println("Consider fine-tuning the scaling factor if detection issues persist");
  }
  
  Serial.println("=== Validation Complete ===\n");
  return DEFAULT_SCALING_FACTOR;
} 