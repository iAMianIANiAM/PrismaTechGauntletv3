/**
 * MPU Filter Test
 * 
 * This example tests the enhanced data processing capabilities
 * of the MPU9250Interface, including:
 * - Filtered sensor data reading
 * - Data validation
 * - Motion magnitude calculation
 * - Maximum axis detection
 * 
 * The test displays both raw and filtered sensor data for comparison.
 */

#include <Arduino.h>
#include "../src/hardware/MPU9250Interface.h"
#include "../src/utils/DebugTools.h"

MPU9250Interface mpu;

// Motion threshold for detecting significant movement
const uint32_t MOTION_THRESHOLD = 500;

// Last time status was printed
unsigned long lastStatusTime = 0;
const unsigned long STATUS_INTERVAL = 250; // Status update interval in ms

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== MPU Filter Test ===");
  
  // Initialize MPU sensor
  if (!mpu.init()) {
    Serial.println("Failed to initialize MPU sensor!");
    return;
  }
  
  // Run diagnostics
  Serial.println("Running MPU diagnostics...");
  if (mpu.runDiagnostics()) {
    Serial.println("Diagnostics passed!");
  } else {
    Serial.println("Diagnostics failed!");
    if (mpu.recoverFromError()) {
      Serial.println("Recovery successful, continuing...");
    } else {
      Serial.println("Recovery failed, test may not function correctly.");
    }
  }
  
  // Perform calibration
  Serial.println("Calibrating MPU sensor...");
  Serial.println("Keep the device still for 2 seconds");
  if (mpu.calibrate()) {
    Serial.println("Calibration complete!");
  } else {
    Serial.println("Calibration failed!");
  }
  
  Serial.println("MPU Filter Test is ready!");
  Serial.println("Move the sensor to see the difference between raw and filtered data");
  Serial.println("-----------------------------------------------------");
}

void loop() {
  // Only update status at specified interval
  if (millis() - lastStatusTime >= STATUS_INTERVAL) {
    lastStatusTime = millis();
    
    // Read raw sensor data
    SensorData rawData;
    if (!mpu.readSensorData(&rawData)) {
      Serial.println("Error reading raw sensor data");
      return;
    }
    
    // Read filtered sensor data
    SensorData filteredData;
    if (!mpu.readFilteredData(&filteredData)) {
      Serial.println("Error reading filtered sensor data");
      return;
    }
    
    // Calculate motion magnitude for both raw and filtered data
    uint32_t rawMagnitude = mpu.calculateMotionMagnitude(rawData);
    uint32_t filteredMagnitude = mpu.calculateMotionMagnitude(filteredData);
    
    // Clear the terminal (ANSI escape sequence)
    Serial.print("\033[2J\033[H");
    
    // Print header
    Serial.println("=== MPU Filter Test ===");
    Serial.println("Comparing Raw vs Filtered Data");
    Serial.println();
    
    // Print raw data
    Serial.println("RAW DATA:");
    Serial.printf("Accel: X=%6d  Y=%6d  Z=%6d\n", rawData.accelX, rawData.accelY, rawData.accelZ);
    Serial.printf("Gyro:  X=%6d  Y=%6d  Z=%6d\n", rawData.gyroX, rawData.gyroY, rawData.gyroZ);
    Serial.printf("Motion Magnitude: %u", rawMagnitude);
    if (rawMagnitude > MOTION_THRESHOLD) {
      Serial.println(" (MOTION DETECTED)");
    } else {
      Serial.println();
    }
    
    // Print filtered data
    Serial.println("\nFILTERED DATA:");
    Serial.printf("Accel: X=%6d  Y=%6d  Z=%6d\n", filteredData.accelX, filteredData.accelY, filteredData.accelZ);
    Serial.printf("Gyro:  X=%6d  Y=%6d  Z=%6d\n", filteredData.gyroX, filteredData.gyroY, filteredData.gyroZ);
    Serial.printf("Motion Magnitude: %u", filteredMagnitude);
    if (filteredMagnitude > MOTION_THRESHOLD) {
      Serial.println(" (MOTION DETECTED)");
    } else {
      Serial.println();
    }
    
    // Validate data
    Serial.println("\nDATA VALIDATION:");
    if (mpu.validateSensorData(rawData)) {
      Serial.println("Raw data: VALID");
    } else {
      Serial.println("Raw data: INVALID");
    }
    
    if (mpu.validateSensorData(filteredData)) {
      Serial.println("Filtered data: VALID");
    } else {
      Serial.println("Filtered data: INVALID");
    }
    
    // Every 5 seconds, demonstrate max axis data collection
    if ((millis() / 5000) % 2 == 0 && (millis() % 5000) < 250) {
      Serial.println("\nCOLLECTING MAXIMUM AXIS DATA FOR 2 SECONDS...");
      Serial.println("Move the sensor in all directions!");
      
      // Get maximum axis values over 2 seconds
      SensorData maxData;
      if (mpu.getMaxAxisData(&maxData, 2000)) {
        Serial.println("\nMAXIMUM VALUES DETECTED:");
        Serial.printf("Accel: X=%6d  Y=%6d  Z=%6d\n", maxData.accelX, maxData.accelY, maxData.accelZ);
        Serial.printf("Gyro:  X=%6d  Y=%6d  Z=%6d\n", maxData.gyroX, maxData.gyroY, maxData.gyroZ);
      } else {
        Serial.println("Error collecting maximum axis data");
      }
    }
    
    Serial.println("\n-----------------------------------------------------");
    Serial.println("Move the sensor to see differences in data processing");
  }
  
  // Small delay
  delay(10);
} 