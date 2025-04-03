#include <Arduino.h>
#include <Wire.h>
#include "hardware/HardwareManager.h"
#include "hardware/MPU9250Interface.h"
#include "core/Config.h"

// Pin configuration
#define LED_BUILTIN 2  // ESP32 built-in LED

// Constants for shake calibration
#define NORMAL_SAMPLES 50  // Samples for normal movement
#define SHAKE_SAMPLES 30   // Samples for shake movement
#define SAMPLING_DELAY 100 // 100ms between samples (10Hz)

// Buffers for collected data
SensorData normalData[NORMAL_SAMPLES];
SensorData shakeData[SHAKE_SAMPLES];

// HardwareManager instance
HardwareManager* hardwareManager = nullptr;
MPU9250Interface* mpuInterface = nullptr;

// Utility functions
uint32_t calculateMagnitude(const SensorData& data) {
  uint32_t magnitude = 0;
  magnitude += abs(data.accelX);
  magnitude += abs(data.accelY);
  magnitude += abs(data.accelZ);
  return magnitude;
}

uint32_t calculateMaxMagnitude(SensorData* buffer, uint16_t count) {
  if (count == 0) return 0;
  
  uint32_t maxVal = calculateMagnitude(buffer[0]);
  for (uint16_t i = 1; i < count; i++) {
    uint32_t mag = calculateMagnitude(buffer[i]);
    if (mag > maxVal) maxVal = mag;
  }
  return maxVal;
}

uint32_t calculateMinMagnitude(SensorData* buffer, uint16_t count) {
  if (count == 0) return 0;
  
  uint32_t minVal = calculateMagnitude(buffer[0]);
  for (uint16_t i = 1; i < count; i++) {
    uint32_t mag = calculateMagnitude(buffer[i]);
    if (mag < minVal) minVal = mag;
  }
  return minVal;
}

uint8_t countCrossings(SensorData* buffer, uint16_t count, uint32_t threshold) {
  if (count < 2) return 0;
  
  uint8_t crossings = 0;
  bool aboveThreshold = calculateMagnitude(buffer[0]) > threshold;
  
  for (uint16_t i = 1; i < count; i++) {
    uint32_t mag = calculateMagnitude(buffer[i]);
    bool currentAbove = mag > threshold;
    
    if (currentAbove != aboveThreshold) {
      crossings++;
      aboveThreshold = currentAbove;
    }
  }
  
  return crossings;
}

void collectSamples(SensorData* buffer, uint16_t count, const char* phase) {
  Serial.printf("Collecting %d samples for %s...\n", count, phase);
  
  // Initialize LED for user feedback
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initial delay with LED on
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Collect samples
  for (uint16_t i = 0; i < count; i++) {
    // Blink LED for each sample
    digitalWrite(LED_BUILTIN, (i % 2) ? HIGH : LOW);
    
    // Read sensor data
    if (!mpuInterface->readSensorData(&buffer[i])) {
      Serial.printf("Failed to read sample %d\n", i);
      buffer[i] = {0}; // Zero out failed samples
    }
    
    // Log progress
    if (i % 5 == 0) {
      Serial.printf("Sample %d/%d\n", i, count);
      
      // Print current values for debugging
      Serial.printf("  accelX: %d\n", buffer[i].accelX);
      Serial.printf("  accelY: %d\n", buffer[i].accelY);
      Serial.printf("  accelZ: %d\n", buffer[i].accelZ);
      Serial.printf("  magnitude: %lu\n", calculateMagnitude(buffer[i]));
    }
    
    // Wait for next sample
    delay(SAMPLING_DELAY);
  }
  
  digitalWrite(LED_BUILTIN, LOW);
  Serial.printf("Completed %s data collection\n", phase);
}

void analyzeSamples(SensorData* normalBuffer, uint16_t normalCount, 
                    SensorData* shakeBuffer, uint16_t shakeCount) {
  Serial.println("\n=== SHAKE CALIBRATION ANALYSIS ===");
  
  // Calculate key metrics
  uint32_t normalMaxMag = calculateMaxMagnitude(normalBuffer, normalCount);
  uint32_t shakeMinMag = calculateMinMagnitude(shakeBuffer, shakeCount);
  
  // Calculate a suitable midpoint threshold
  uint32_t midThreshold = (normalMaxMag + shakeMinMag) / 2;
  
  // Count typical crossings during shake
  uint8_t typicalCrossings = countCrossings(shakeBuffer, shakeCount, midThreshold);
  float crossingsPerSecond = float(typicalCrossings) * (1000.0f / SAMPLING_DELAY) / shakeCount;
  
  // Print findings
  Serial.println("\n=== SHAKE CALIBRATION RESULTS ===");
  Serial.printf("Normal movement max magnitude: %lu\n", normalMaxMag);
  Serial.printf("Shake movement min magnitude: %lu\n", shakeMinMag);
  Serial.printf("Recommended threshold midpoint: %lu\n", midThreshold);
  Serial.printf("Typical crossings in shake sample: %d\n", typicalCrossings);
  Serial.printf("Estimated crossings per second: %.2f\n", crossingsPerSecond);
  
  // Calculate and recommend thresholds
  // Add a buffer to the normal max magnitude
  uint32_t bufferAmount = (shakeMinMag > normalMaxMag) ? 
      (shakeMinMag - normalMaxMag) / 4 : 2000; // Buffer of 25% of difference or 2000 
  uint32_t highThreshold = normalMaxMag + bufferAmount;
  uint32_t lowThreshold = normalMaxMag - 1000;
  
  // Calculate sample count for detection window
  // Aiming for ~4 crossings to trigger
  uint8_t recommendedCrossings = max((uint8_t)4, uint8_t(typicalCrossings * 0.8));
  
  // Print configuration recommendations
  Serial.println("\n=== RECOMMENDED CONFIGURATION VALUES ===");
  Serial.printf("namespace ShakeDetection {\n");
  Serial.printf("  constexpr uint32_t SHAKE_HIGH_THRESHOLD = %lu;\n", highThreshold);
  Serial.printf("  constexpr uint32_t SHAKE_LOW_THRESHOLD = %lu;\n", lowThreshold);
  Serial.printf("  constexpr uint8_t SHAKE_REQUIRED_CROSSINGS = %d;\n", recommendedCrossings);
  Serial.printf("  constexpr uint16_t SHAKE_MAX_CROSSING_INTERVAL_MS = 300;\n");
  Serial.printf("  constexpr uint16_t SHAKE_COOLDOWN_MS = 1000;\n");
  Serial.printf("}\n");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== SHAKE CALIBRATION PROTOCOL ===");
  Serial.println("This utility will help calibrate shake detection thresholds");
  
  // Initialize I2C directly to ensure it's available for MPU interface
  Wire.begin(Config::I2C_SDA_PIN, Config::I2C_SCL_PIN);
  Wire.setClock(100000);
  delay(150);
  
  // Create a direct MPU interface for this test instead of using HardwareManager
  mpuInterface = new MPU9250Interface();
  if (!mpuInterface) {
    Serial.println("ERROR: Failed to create MPU interface");
    while (1) delay(1000);
  }
  
  // Try to initialize the MPU interface
  if (!mpuInterface->init()) {
    Serial.println("ERROR: Failed to initialize MPU interface");
    while (1) delay(1000);
  }
  
  // Wait for sensor stabilization
  delay(1000);
  
  // Calibration process
  Serial.println("\n=== CALIBRATION PROCESS STARTED ===");
  
  // Phase 1: Normal movement
  Serial.println("\n--- PHASE 1: NORMAL MOVEMENT ---");
  Serial.println("Please move the gauntlet normally for the next 5 seconds");
  Serial.println("This will establish baseline motion thresholds");
  Serial.println("Get ready...");
  delay(3000);
  collectSamples(normalData, NORMAL_SAMPLES, "normal movement");
  
  // Phase 2: Shake movement
  Serial.println("\n--- PHASE 2: SHAKE MOVEMENT ---");
  Serial.println("Please SHAKE the gauntlet vigorously for the next 3 seconds");
  Serial.println("This will establish shake detection thresholds");
  Serial.println("Get ready...");
  delay(3000);
  collectSamples(shakeData, SHAKE_SAMPLES, "shake movement");
  
  // Analyze results
  analyzeSamples(normalData, NORMAL_SAMPLES, shakeData, SHAKE_SAMPLES);
  
  Serial.println("\n=== CALIBRATION COMPLETE ===");
  Serial.println("Update Config.h with the recommended values");
  
  // Clean up
  delete mpuInterface;
}

void loop() {
  // Nothing to do here, calibration is a one-time process
  delay(1000);
  
  // Optional: Blink LED to indicate completion
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(LED_BUILTIN, ledState);
} 