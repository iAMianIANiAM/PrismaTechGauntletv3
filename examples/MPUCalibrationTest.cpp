/**
 * MPU Calibration Test
 * 
 * This test program helps diagnose issues with the MPU9250/MPU6500 calibration
 * by providing detailed diagnostic output and testing different scaling factors.
 * 
 * It can be used to:
 * 1. Verify sensor identity and connectivity
 * 2. Test calibration with enhanced debugging
 * 3. Visualize raw vs. calibrated data
 * 4. Experiment with different scaling approaches
 */

#include <Arduino.h>
#include "../src/hardware/MPU9250Interface.h"
#include "../src/utils/DebugTools.h"
#include "../src/detection/UltraBasicPositionDetector.h"
#include "../src/hardware/HardwareManager.h"

// Test instances
MPU9250Interface mpuSensor;
HardwareManager hardware;
UltraBasicPositionDetector detector;

// Test states
enum TestState {
  STATE_INIT,
  STATE_SCALING_CALIBRATION,
  STATE_CALIBRATION,
  STATE_POSITION_TEST,
  STATE_CONTINUOUS_MONITORING
};

TestState currentState = STATE_INIT;
unsigned long stateChangeTime = 0;
unsigned long lastOutputTime = 0;
const unsigned long OUTPUT_INTERVAL = 200; // 200ms between readings

// For tracking issues
bool hadZeroReadings = false;
int consecutiveZeroReadings = 0;

// Function prototypes
void setupTest();
void runCalibration();
void testPositionDetection();
void monitorContinuously();
void printSensorData(const SensorData& data, bool withConversion = true);
void printPhysicalUnits(const SensorData& data);
void checkForZeroReadings(const SensorData& data);
void calibrateScalingFactor();

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(2000); // Wait for serial to initialize
  
  Serial.println("\n\n===== MPU Calibration Diagnostic Tool =====");
  Serial.println("This tool will help diagnose MPU sensor calibration issues");
  
  // Start the test sequence
  setupTest();
}

void loop() {
  unsigned long currentTime = millis();
  
  // State machine for the test sequence
  switch (currentState) {
    case STATE_INIT:
      // Initialization phase - wait 3 seconds before calibration
      if (currentTime - stateChangeTime > 3000) {
        Serial.println("\n>> Starting scaling factor calibration");
        Serial.println(">> Place the device flat on a stable surface");
        delay(3000);
        
        currentState = STATE_SCALING_CALIBRATION;
        stateChangeTime = currentTime;
      }
      break;
    
    case STATE_SCALING_CALIBRATION:
      // Calibrate the scaling factor first
      calibrateScalingFactor();
      
      Serial.println("\n>> Starting sensor calibration in 3 seconds...");
      Serial.println(">> Please keep the device still on a flat surface");
      delay(3000);
      
      currentState = STATE_CALIBRATION;
      stateChangeTime = currentTime;
      break;
    
    case STATE_CALIBRATION:
      // Run the enhanced calibration procedure
      runCalibration();
      
      // Move to position testing
      Serial.println("\n>> Calibration complete. Starting position detection test...");
      currentState = STATE_POSITION_TEST;
      stateChangeTime = currentTime;
      break;
    
    case STATE_POSITION_TEST:
      // Test position detection for 10 seconds
      testPositionDetection();
      
      // After 10 seconds, switch to continuous monitoring
      if (currentTime - stateChangeTime > 10000) {
        Serial.println("\n>> Switching to continuous monitoring mode");
        Serial.println(">> Move the sensor to verify calibration values");
        currentState = STATE_CONTINUOUS_MONITORING;
        stateChangeTime = currentTime;
      }
      break;
    
    case STATE_CONTINUOUS_MONITORING:
      // Continuously monitor sensor data
      monitorContinuously();
      break;
  }
}

void setupTest() {
  Serial.println("\n----- Test Initialization -----");
  
  // Try to initialize the MPU sensor
  Serial.println("Initializing MPU sensor...");
  if (!mpuSensor.init()) {
    Serial.println("ERROR: Failed to initialize MPU sensor!");
    
    // Check if connected but failed to initialize
    if (mpuSensor.isConnected()) {
      Serial.println("Sensor is connected but initialization failed");
      Serial.println("Trying alternate initialization approach...");
      
      // Try to recover
      if (mpuSensor.recoverFromError()) {
        Serial.println("Recovery successful!");
      } else {
        Serial.println("Recovery failed. Please check hardware connections.");
        while (1) delay(1000); // Stop here
      }
    } else {
      Serial.println("Sensor not connected. Please check wiring.");
      while (1) delay(1000); // Stop here
    }
  }
  
  // Initialize hardware manager
  hardware.init();
  
  // Initialize position detector
  if (!detector.init(&hardware)) {
    Serial.println("ERROR: Failed to initialize position detector!");
    while (1) delay(1000); // Stop here
  }
  
  Serial.println("Initialization complete!");
  stateChangeTime = millis();
}

void runCalibration() {
  Serial.println("\n----- Starting Enhanced Calibration -----");
  
  // Run the enhanced MPU calibration
  if (!mpuSensor.calibrate()) {
    Serial.println("ERROR: Calibration failed!");
    return;
  }
  
  // Verify calibration with some test readings
  Serial.println("\nTaking test readings after calibration...");
  for (int i = 0; i < 5; i++) {
    SensorData data;
    if (mpuSensor.readSensorData(&data)) {
      Serial.printf("Reading %d: ", i);
      printSensorData(data);
      printPhysicalUnits(data);
      checkForZeroReadings(data);
    }
    delay(200);
  }
}

void testPositionDetection() {
  // Only output every OUTPUT_INTERVAL ms
  unsigned long currentTime = millis();
  if (currentTime - lastOutputTime < OUTPUT_INTERVAL) {
    return;
  }
  lastOutputTime = currentTime;
  
  // Get raw sensor data
  SensorData rawData = hardware.getSensorData();
  
  // Get processed position data
  PositionReading posReading = detector.update();
  ProcessedData processedData = detector.getProcessedData();
  
  // Output raw sensor data
  Serial.printf("Raw: ");
  printSensorData(rawData, false);
  
  // Output processed data and position
  Serial.printf("Processed (m/s²): X=%.2f Y=%.2f Z=%.2f | ", 
               processedData.accelX, processedData.accelY, processedData.accelZ);
  
  // Output position information
  Serial.printf("Position: %s (Confidence: %.1f%%)\n", 
               getPositionName(posReading.position), posReading.confidence);
  
  // Check for zero values
  checkForZeroReadings(rawData);
}

void monitorContinuously() {
  // Only output every OUTPUT_INTERVAL ms
  unsigned long currentTime = millis();
  if (currentTime - lastOutputTime < OUTPUT_INTERVAL) {
    return;
  }
  lastOutputTime = currentTime;
  
  // Get raw sensor data
  SensorData rawData;
  if (!mpuSensor.readSensorData(&rawData)) {
    Serial.println("Error reading sensor data");
    return;
  }
  
  // Print data
  Serial.print("Continuous: ");
  printSensorData(rawData);
  printPhysicalUnits(rawData);
  
  // Check for zero values
  checkForZeroReadings(rawData);
}

void printSensorData(const SensorData& data, bool withConversion) {
  Serial.printf("accelX=%d, accelY=%d, accelZ=%d", 
               data.accelX, data.accelY, data.accelZ);
  
  if (withConversion) {
    Serial.printf(" | gyroX=%d, gyroY=%d, gyroZ=%d", 
                 data.gyroX, data.gyroY, data.gyroZ);
  }
}

void printPhysicalUnits(const SensorData& data) {
  // Use the detector's current scaling factor
  float scalingFactor = detector.getScalingFactor();
  
  float accelX = data.accelX * scalingFactor;
  float accelY = data.accelY * scalingFactor;
  float accelZ = data.accelZ * scalingFactor;
  
  Serial.printf(" | Physical (m/s²): X=%.2f Y=%.2f Z=%.2f\n", 
               accelX, accelY, accelZ);
}

void checkForZeroReadings(const SensorData& data) {
  if (data.accelX == 0 && data.accelY == 0 && data.accelZ == 0) {
    consecutiveZeroReadings++;
    
    if (consecutiveZeroReadings > 2 && !hadZeroReadings) {
      Serial.println("WARNING: Multiple consecutive zero readings detected!");
      hadZeroReadings = true;
    }
  } else {
    consecutiveZeroReadings = 0;
  }
}

void calibrateScalingFactor() {
  Serial.println("\n----- Scaling Factor Calibration -----");
  
  // Run the scaling factor calibration
  float bestFactor = detector.calibrateScalingFactor(3000);
  
  // Update our test program's SCALING_FACTOR constant to match
  Serial.printf("Updating test program scaling factor to: %.8f\n", bestFactor);
  
  // Take test readings with the new scaling factor
  Serial.println("\nTaking test readings with calibrated scaling factor...");
  for (int i = 0; i < 5; i++) {
    SensorData rawData = hardware.getSensorData();
    ProcessedData processedData;
    
    // Use the detector to process the data with calibrated scaling
    detector.processRawData(rawData, processedData);
    
    Serial.printf("Reading %d: Raw(X=%d, Y=%d, Z=%d) → ", 
                 i, rawData.accelX, rawData.accelY, rawData.accelZ);
    
    Serial.printf("Physical(X=%.2f, Y=%.2f, Z=%.2f m/s²)\n", 
                 processedData.accelX, processedData.accelY, processedData.accelZ);
    
    delay(200);
  }
}

// Helper function to get position name
const char* getPositionName(uint8_t position) {
  switch (position) {
    case POS_OFFER: return "OFFER";
    case POS_CALM: return "CALM";
    case POS_OATH: return "OATH";
    case POS_DIG: return "DIG";
    case POS_SHIELD: return "SHIELD";
    case POS_NULL: return "NULL";
    case POS_UNKNOWN: return "UNKNOWN";
    default: return "INVALID";
  }
} 