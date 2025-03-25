#include <Arduino.h>
#include "hardware/MPU9250Interface.h"
#include "utils/I2CScanner.h"
#include "core/Config.h"
#include "core/SystemTypes.h"

// Create global MPU sensor interface
MPU9250Interface mpuSensor;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);  // Allow time for serial connection to establish
    
    Serial.println("\n==================================");
    Serial.println("PrismaTech Gauntlet 3.0");
    Serial.println("==================================");
    
    // Initialize I2C scanner and scan for devices
    I2CScanner::begin(Config::I2C_SDA_PIN, Config::I2C_SCL_PIN, 100000);
    int devicesFound = I2CScanner::scanBus();
    
    if (devicesFound == 0) {
        Serial.println("\nWARNING: No I2C devices found. Check wiring connections.");
    }
    
    // Try both MPU addresses (0x68 is default, 0x69 if AD0 pin is HIGH)
    bool addrLowFound = I2CScanner::testAddress(0x68);
    bool addrHighFound = I2CScanner::testAddress(0x69);
    
    if (addrLowFound) {
        mpuSensor.setAddress(0x68);
    } else if (addrHighFound) {
        mpuSensor.setAddress(0x69);
    } else {
        Serial.println("\nWARNING: No MPU sensor found at standard addresses.");
    }
    
    // Initialize MPU sensor
    Serial.println("\nInitializing MPU sensor...");
    if (mpuSensor.init()) {
        Serial.println("MPU sensor initialized successfully.");
        
        // Perform calibration
        Serial.println("\nCalibrating sensor (keep device still)...");
        if (mpuSensor.calibrate()) {
            Serial.println("Calibration complete.");
        } else {
            Serial.println("Calibration failed. Using uncalibrated values.");
        }
    } else {
        Serial.println("Failed to initialize MPU sensor.");
    }
    
    // Additional setup code can be added here
    
    Serial.println("\nSetup complete. Beginning main loop.");
}

void loop() {
    // Create data object to hold sensor readings
    SensorData data;
    
    // Read from MPU sensor
    if (mpuSensor.readSensorData(&data)) {
        // Print raw data values with formatting to align columns
        Serial.printf("MPU Data: Accel(X=%6d Y=%6d Z=%6d) | Gyro(X=%6d Y=%6d Z=%6d)\n", 
                    data.accelX, data.accelY, data.accelZ, 
                    data.gyroX, data.gyroY, data.gyroZ);
    }
    
    // Additional application code can be added here
    
    // Control the output rate
    delay(100);
}