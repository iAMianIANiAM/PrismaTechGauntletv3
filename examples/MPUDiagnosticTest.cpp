#include <Arduino.h>
#include "../hardware/MPU9250Interface.h"
#include "../utils/I2CScanner.h"
#include "../core/Config.h"
#include "../core/SystemTypes.h"

// Create MPU interface instance
MPU9250Interface mpuSensor;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);  // Allow time for serial connection to establish
    
    Serial.println("\n==================================");
    Serial.println("MPU Sensor Diagnostic Test");
    Serial.println("==================================");
    
    // Initialize I2C scanner with the configured pins
    I2CScanner::begin(Config::I2C_SDA_PIN, Config::I2C_SCL_PIN, 100000);
    
    // Step 1: Scan I2C bus for all devices
    Serial.println("\nSTEP 1: Scanning I2C bus for all devices");
    int devicesFound = I2CScanner::scanBus();
    
    if (devicesFound == 0) {
        Serial.println("\n!! ERROR: No I2C devices found on the bus !!");
        Serial.println("Please check your wiring connections:");
        Serial.printf("- SDA pin connected to GPIO %d?\n", Config::I2C_SDA_PIN);
        Serial.printf("- SCL pin connected to GPIO %d?\n", Config::I2C_SCL_PIN);
        Serial.println("- Are pull-up resistors present on SDA/SCL?");
        Serial.println("- Is the sensor powered correctly (3.3V)?");
        Serial.println("\nProgram halted. Reset ESP32 after fixing connections.");
        while (true) {
            delay(1000); // Halt program execution
        }
    }
    
    // Step 2: Test addresses where MPU sensors are typically found
    Serial.println("\nSTEP 2: Testing standard MPU addresses");
    bool addrLowFound = I2CScanner::testMPUSensor(0x68); // AD0 pin LOW (default)
    bool addrHighFound = I2CScanner::testMPUSensor(0x69); // AD0 pin HIGH
    
    // Determine which address to use
    uint8_t addressToUse = 0;
    if (addrLowFound) {
        addressToUse = 0x68;
    } else if (addrHighFound) {
        addressToUse = 0x69;
    } else {
        Serial.println("\n!! ERROR: No MPU sensor found at standard addresses !!");
        Serial.println("Program halted. Reset ESP32 after fixing the issue.");
        while (true) {
            delay(1000); // Halt program execution
        }
    }
    
    // Set the correct address in the MPU interface
    mpuSensor.setAddress(addressToUse);
    
    // Step 3: Initialize the MPU sensor
    Serial.println("\nSTEP 3: Initializing MPU sensor");
    if (!mpuSensor.init()) {
        Serial.println("\n!! ERROR: Failed to initialize MPU sensor !!");
        Serial.println("Program halted. Reset ESP32 after fixing the issue.");
        while (true) {
            delay(1000); // Halt program execution
        }
    }
    
    // Step 4: Run the full diagnostics suite
    Serial.println("\nSTEP 4: Running comprehensive diagnostics");
    mpuSensor.runDiagnostics();
    
    // Step 5: Perform sensor calibration
    Serial.println("\nSTEP 5: Calibrating sensor");
    Serial.println("Keep the device perfectly still during calibration!");
    delay(2000); // Give user time to prepare
    
    if (!mpuSensor.calibrate()) {
        Serial.println("\n!! WARNING: Calibration failed, continuing with uncalibrated values !!");
    }
    
    Serial.println("\nDiagnostic test completed successfully!");
    Serial.println("Starting data reading loop...");
}

void loop() {
    // Create a data object to hold sensor readings
    SensorData data;
    
    // Read sensor data
    if (mpuSensor.readSensorData(&data)) {
        // Print raw data values
        Serial.printf("Accel: X=%6d Y=%6d Z=%6d | ", data.accelX, data.accelY, data.accelZ);
        Serial.printf("Gyro: X=%6d Y=%6d Z=%6d | ", data.gyroX, data.gyroY, data.gyroZ);
        Serial.printf("Time: %lu ms\n", data.timestamp);
    } else {
        Serial.println("Failed to read sensor data!");
    }
    
    // Slow down the output rate
    delay(100);
} 