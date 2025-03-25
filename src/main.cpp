#include <Arduino.h>
#include "hardware/MPU9250Interface.h"
#include "hardware/LEDInterface.h"
#include "utils/I2CScanner.h"
#include "core/Config.h"
#include "core/SystemTypes.h"

// Create global interfaces
MPU9250Interface mpuSensor;
LEDInterface ledInterface;

// Timer variables for non-blocking operation
unsigned long lastSensorReadTime = 0;
unsigned long lastLedUpdateTime = 0;
const unsigned long SENSOR_READ_INTERVAL = 20;    // 50Hz sensor reading
const unsigned long LED_UPDATE_INTERVAL = 50;     // 20Hz LED updates

// Variables to track orientation
int16_t maxAccelValue = 0;
uint8_t dominantAxis = 0;  // 0=X, 1=Y, 2=Z
bool isPositive = true;    // Direction of the axis

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
    
    // Try both MPU addresses and set the correct one
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
    
    // Initialize LED interface
    Serial.println("\nInitializing LED interface...");
    if (ledInterface.init()) {
        Serial.println("LED interface initialized successfully.");
        
        // Show a startup animation - pulse white three times
        Color white = {255, 255, 255};
        ledInterface.pulse(white, 3, 500);
    } else {
        Serial.println("Failed to initialize LED interface.");
    }
    
    Serial.println("\nSetup complete. Beginning main loop.");
}

void loop() {
    // Non-blocking sensor reading
    unsigned long currentMillis = millis();
    
    if (currentMillis - lastSensorReadTime >= SENSOR_READ_INTERVAL) {
        lastSensorReadTime = currentMillis;
        
        // Read from MPU sensor
        SensorData data;
        if (mpuSensor.readSensorData(&data)) {
            // Analyze the accelerometer data to determine orientation
            int16_t absX = abs(data.accelX);
            int16_t absY = abs(data.accelY);
            int16_t absZ = abs(data.accelZ);
            
            // Find the maximum absolute value
            maxAccelValue = max(absX, max(absY, absZ));
            
            // Determine the dominant axis
            if (maxAccelValue == absX) {
                dominantAxis = 0;  // X-axis
                isPositive = (data.accelX > 0);
            } else if (maxAccelValue == absY) {
                dominantAxis = 1;  // Y-axis
                isPositive = (data.accelY > 0);
            } else {
                dominantAxis = 2;  // Z-axis
                isPositive = (data.accelZ > 0);
            }
            
            // Print data with reduced frequency for readability
            static int printCounter = 0;
            if (++printCounter % 10 == 0) {  // Print every 10th reading
                Serial.printf("MPU Data: Accel(X=%6d Y=%6d Z=%6d) | ",
                            data.accelX, data.accelY, data.accelZ);
                Serial.printf("Gyro(X=%6d Y=%6d Z=%6d) | ",
                            data.gyroX, data.gyroY, data.gyroZ);
                
                // Print orientation info
                const char* axisNames[] = {"X", "Y", "Z"};
                Serial.printf("Dominant: %s%s (Value: %6d)\n", 
                            isPositive ? "+" : "-", 
                            axisNames[dominantAxis], 
                            maxAccelValue);
            }
        }
    }
    
    // Non-blocking LED updates
    if (currentMillis - lastLedUpdateTime >= LED_UPDATE_INTERVAL) {
        lastLedUpdateTime = currentMillis;
        
        // Use maxAccelValue to determine brightness
        // Map to range 20-180 to avoid too dim or too bright
        int mappedBrightness = 20 + (maxAccelValue * 160) / 16000;
        if (mappedBrightness > 180) mappedBrightness = 180;
        
        // Set brightness based on overall acceleration
        ledInterface.setBrightness(mappedBrightness);
        
        // Map orientation to hand positions (simplified mapping for now)
        uint8_t position;
        if (dominantAxis == 0) {           // X-axis
            position = isPositive ? POS_SHIELD : POS_NULL;
        } else if (dominantAxis == 1) {    // Y-axis
            position = isPositive ? POS_OFFER : POS_CALM;
        } else {                           // Z-axis
            position = isPositive ? POS_OATH : POS_DIG;
        }
        
        // Get the color for this position
        Color posColor = ledInterface.getColorForPosition(position);
        
        // Set all LEDs to the position color
        ledInterface.setAllLEDs(posColor);
        ledInterface.show();
    }
}