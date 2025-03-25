#include <Arduino.h>
#include "../../src/hardware/MPU9250Interface.h"
#include "../../src/utils/DebugTools.h"

// Test instance
MPU9250Interface testSensor;

/**
 * @brief Set up the MPU9250 test
 */
void setupMPU9250Test() {
    // Initialize debug
    DebugTools::init(115200);
    DebugTools::println("Starting MPU9250 test...");
    
    // Initialize sensor
    DebugTools::println("Initializing sensor...");
    if (!testSensor.init()) {
        DebugTools::println("ERROR: Sensor initialization failed!");
        // Check connection
        if (!testSensor.isConnected()) {
            DebugTools::println("ERROR: Sensor not connected or I2C error.");
        } else {
            DebugTools::println("Sensor connected but initialization failed.");
        }
        return;
    }
    
    DebugTools::println("Sensor initialized successfully.");
    
    // Run calibration
    DebugTools::println("Calibrating sensor...");
    if (!testSensor.calibrate()) {
        DebugTools::println("WARNING: Sensor calibration failed.");
    } else {
        DebugTools::println("Sensor calibration complete.");
    }
}

/**
 * @brief Run the MPU9250 test
 */
void runMPU9250Test() {
    static const int SAMPLES = 10;
    
    DebugTools::println("\nReading sensor data...");
    
    for (int i = 0; i < SAMPLES; i++) {
        SensorData data;
        
        // Measure timing
        DebugTools::startTiming("SensorRead");
        bool success = testSensor.readSensorData(&data);
        DebugTools::endTiming();
        
        if (success) {
            DebugTools::printSensorData(data);
        } else {
            DebugTools::println("ERROR: Failed to read sensor data!");
            
            // Try recovery
            if (testSensor.recoverFromError()) {
                DebugTools::println("Recovery successful, continuing...");
            } else {
                DebugTools::println("Recovery failed, aborting test.");
                break;
            }
        }
        
        delay(100);
    }
    
    DebugTools::println("Test complete.");
}

/**
 * @brief Comprehensive MPU9250 sensor test
 */
void testMPU9250() {
    // Enable timing measurements
    DebugTools::setTimingEnabled(true);
    
    // Setup test
    setupMPU9250Test();
    
    // Run test
    runMPU9250Test();
    
    // Log memory usage
    DebugTools::logMemoryUsage();
} 