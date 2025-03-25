#include "MPU9250Interface.h"
#include "../utils/DebugTools.h"
#include "../core/Config.h"

// I2C configuration - reduced from 400kHz to 100kHz for better reliability
static const uint8_t SDA_PIN = Config::I2C_SDA_PIN;  // I2C SDA pin on ESP32
static const uint8_t SCL_PIN = Config::I2C_SCL_PIN;  // I2C SCL pin on ESP32
static const uint32_t I2C_FREQUENCY = 100000;  // 100 kHz I2C clock for reliability

// Address-related methods
void MPU9250Interface::setAddress(uint8_t address) {
    // Only allow valid MPU addresses
    if (address == MPU9250_ADDRESS_AD0_LOW || address == MPU9250_ADDRESS_AD0_HIGH) {
        sensorAddress = address;
        Serial.printf("MPU sensor address set to 0x%02X\n", sensorAddress);
    } else {
        Serial.printf("Invalid MPU address: 0x%02X (must be 0x68 or 0x69)\n", address);
    }
}

uint8_t MPU9250Interface::getAddress() const {
    return sensorAddress;
}

bool MPU9250Interface::init() {
    // Initialize I2C communication
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(I2C_FREQUENCY);
    
    // Log connection attempt
    Serial.printf("Initializing MPU sensor on I2C address 0x%02X\n", sensorAddress);
    
    // Delay for stable power-up
    delay(100);
    
    // Check if sensor is responding
    if (!isConnected()) {
        Serial.println("MPU sensor not responding to I2C communication");
        return false;
    }
    
    // Check WHO_AM_I register
    uint8_t whoAmI = readRegister(WHO_AM_I_REG);
    Serial.printf("MPU WHO_AM_I register value: 0x%02X\n", whoAmI);
    
    // Accept a wider range of valid WHO_AM_I values based on Echo insights
    if (whoAmI == 0x68 || whoAmI == 0x71 || whoAmI == 0x73 || whoAmI == 0x70) {
        Serial.println("Valid MPU sensor identified");
        
        if (whoAmI == 0x68) {
            Serial.println("Detected MPU6050 sensor");
        } else if (whoAmI == 0x71 || whoAmI == 0x73) {
            Serial.println("Detected MPU9250 sensor");
        } else if (whoAmI == 0x70) {
            Serial.println("Detected MPU variant (possibly MPU6000)");
        }
    } else {
        Serial.printf("WARNING: Unknown WHO_AM_I value: 0x%02X\n", whoAmI);
        Serial.println("Possible issues:");
        Serial.println("1. I2C connection problem (wiring issue)");
        Serial.println("2. Wrong I2C address");
        Serial.println("3. Sensor not powered properly");
        return false;
    }
    
    // Reset the device
    Serial.println("Resetting MPU sensor...");
    if (!writeRegister(PWR_MGMT_1_REG, 0x80)) {
        Serial.println("Failed to reset device");
        return false;
    }
    
    // Wait for reset to complete
    delay(100);
    
    // Wake up the sensor (clear sleep bit and use X-axis gyro as clock reference)
    Serial.println("Waking up sensor...");
    if (!writeRegister(PWR_MGMT_1_REG, 0x01)) {
        Serial.println("Failed to wake up device");
        return false;
    }
    delay(50);
    
    // Configure sample rate divider (register 0x19)
    // Calculate: sample_rate = 1kHz / (1 + divider)
    // For 125Hz: 1000 / (1 + 7) = 125Hz
    Serial.println("Setting sample rate to 125Hz...");
    if (!writeRegister(SMPLRT_DIV_REG, 0x07)) {
        Serial.println("Failed to set sample rate");
        return false;
    }
    
    // Configure accelerometer range to ±4g (register 0x1C)
    Serial.println("Configuring accelerometer to ±4g range...");
    if (!writeRegister(ACCEL_CONFIG_REG, 0x08)) {
        Serial.println("Failed to configure accelerometer");
        return false;
    }
    
    // Configure gyroscope range to ±500 deg/s (register 0x1B)
    Serial.println("Configuring gyroscope to ±500 deg/s range...");
    if (!writeRegister(GYRO_CONFIG_REG, 0x08)) {
        Serial.println("Failed to configure gyroscope");
        return false;
    }
    
    // Configure digital low pass filter
    Serial.println("Configuring digital low pass filter...");
    if (!writeRegister(CONFIG_REG, 0x03)) {
        Serial.println("Failed to configure DLPF");
        return false;
    }
    
    Serial.println("MPU sensor initialization complete");
    return true;
}

bool MPU9250Interface::readSensorData(SensorData* data) {
    // Check if a valid pointer was provided
    if (data == nullptr) {
        return false;
    }
    
    // Read all data at once (accelerometer, temperature, gyroscope)
    uint8_t buffer[14];
    
    // Start from ACCEL_XOUT_H register (0x3B)
    if (!readRegisters(ACCEL_XOUT_H_REG, buffer, 14)) {
        Serial.println("Error reading sensor data");
        return false;
    }
    
    // Extract raw data values from buffer
    data->accelX = (int16_t)((buffer[0] << 8) | buffer[1]) - accelOffsetX;
    data->accelY = (int16_t)((buffer[2] << 8) | buffer[3]) - accelOffsetY;
    data->accelZ = (int16_t)((buffer[4] << 8) | buffer[5]) - accelOffsetZ;
    
    // Skip temperature (bytes 6-7)
    
    data->gyroX = (int16_t)((buffer[8] << 8) | buffer[9]) - gyroOffsetX;
    data->gyroY = (int16_t)((buffer[10] << 8) | buffer[11]) - gyroOffsetY;
    data->gyroZ = (int16_t)((buffer[12] << 8) | buffer[13]) - gyroOffsetZ;
    
    // Set timestamp
    data->timestamp = millis();
    
    return true;
}

void MPU9250Interface::setSampleRate(uint16_t rate) {
    // Calculate divider: sample_rate = 1000 / (1 + divider)
    uint8_t divider = 1000 / rate - 1;
    
    // Ensure valid range
    if (divider > 255) divider = 255;
    
    // Set sample rate divider
    writeRegister(SMPLRT_DIV_REG, divider);
    
    Serial.printf("Sample rate set to approximately %d Hz\n", 1000 / (1 + divider));
}

void MPU9250Interface::setLowPowerMode(bool lowPower) {
    if (lowPower) {
        // Enable low power mode by setting CYCLE bit (5) and disabling TEMP_DIS bit (3)
        writeRegister(PWR_MGMT_1_REG, 0x20);
        
        // Set low power wake-up frequency to 1.25 Hz
        writeRegister(0x1D, 0x40);
        Serial.println("Low power mode enabled");
    } else {
        // Disable low power mode
        writeRegister(PWR_MGMT_1_REG, 0x01);  // Use gyro X as clock reference
        Serial.println("Normal power mode enabled");
    }
}

bool MPU9250Interface::calibrate() {
    const int sampleCount = 100;
    const int calmDelay = 2000; // Time to keep device still
    
    Serial.println("MPU calibration starting...");
    Serial.println("Keep the device still for 2 seconds");
    delay(calmDelay); // Give user time to keep the device still
    
    // Reset offsets
    accelOffsetX = accelOffsetY = accelOffsetZ = 0;
    gyroOffsetX = gyroOffsetY = gyroOffsetZ = 0;
    
    int32_t accelXSum = 0, accelYSum = 0, accelZSum = 0;
    int32_t gyroXSum = 0, gyroYSum = 0, gyroZSum = 0;
    
    // Collect samples
    for (int i = 0; i < sampleCount; i++) {
        SensorData data;
        if (!readSensorData(&data)) {
            Serial.println("Error during calibration data collection");
            return false;
        }
        
        accelXSum += data.accelX;
        accelYSum += data.accelY;
        accelZSum += data.accelZ;
        
        gyroXSum += data.gyroX;
        gyroYSum += data.gyroY;
        gyroZSum += data.gyroZ;
        
        delay(10);  // Short delay between samples
    }
    
    // Calculate average offsets
    accelOffsetX = accelXSum / sampleCount;
    accelOffsetY = accelYSum / sampleCount;
    // Keep gravity in Z axis (8192 is approximately 1g in the ±4g range)
    accelOffsetZ = (accelZSum / sampleCount) - 8192;
    
    gyroOffsetX = gyroXSum / sampleCount;
    gyroOffsetY = gyroYSum / sampleCount;
    gyroOffsetZ = gyroZSum / sampleCount;
    
    Serial.println("Calibration complete");
    Serial.printf("Accel offsets: X=%d Y=%d Z=%d\n", accelOffsetX, accelOffsetY, accelOffsetZ);
    Serial.printf("Gyro offsets: X=%d Y=%d Z=%d\n", gyroOffsetX, gyroOffsetY, gyroOffsetZ);
    
    return true;
}

// I2C helper methods
bool MPU9250Interface::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(sensorAddress);
    Wire.write(reg);
    Wire.write(value);
    uint8_t error = Wire.endTransmission();
    if (error != 0) {
        Serial.printf("I2C write error: %d (register 0x%02X, value 0x%02X, address 0x%02X)\n", 
                     error, reg, value, sensorAddress);
        return false;
    }
    return true;
}

uint8_t MPU9250Interface::readRegister(uint8_t reg) {
    Wire.beginTransmission(sensorAddress);
    Wire.write(reg);
    uint8_t error = Wire.endTransmission(false);
    if (error != 0) {
        Serial.printf("I2C read error during address setup: %d (register 0x%02X, address 0x%02X)\n", 
                     error, reg, sensorAddress);
        return 0;  // Error condition
    }
    
    // Fix ambiguity by explicitly casting to the correct types
    uint8_t bytesReceived = Wire.requestFrom((uint8_t)sensorAddress, (uint8_t)1);
    if (bytesReceived != 1) {
        Serial.printf("I2C read error: requested 1 byte, received %d bytes\n", bytesReceived);
        return 0;  // Error condition
    }
    
    return Wire.read();
}

bool MPU9250Interface::readRegisters(uint8_t reg, uint8_t* buffer, uint8_t count) {
    Wire.beginTransmission(sensorAddress);
    Wire.write(reg);
    uint8_t error = Wire.endTransmission(false);
    if (error != 0) {
        Serial.printf("I2C read error during multi-read address setup: %d (register 0x%02X, address 0x%02X)\n", 
                     error, reg, sensorAddress);
        return false;  // Error condition
    }
    
    uint8_t bytesReceived = Wire.requestFrom((uint8_t)sensorAddress, (uint8_t)count);
    if (bytesReceived != count) {
        Serial.printf("I2C multi-read error: requested %d bytes, received %d bytes\n", count, bytesReceived);
        return false;  // Error condition
    }
    
    for (uint8_t i = 0; i < count; i++) {
        buffer[i] = Wire.read();
    }
    
    return true;
}

// Enhanced error handling and connection verification
bool MPU9250Interface::isConnected() {
    Wire.beginTransmission(sensorAddress);
    uint8_t error = Wire.endTransmission();
    
    if (error != 0) {
        Serial.printf("I2C connection error: %d (address 0x%02X)\n", error, sensorAddress);
        return false;
    }
    
    return true;
}

bool MPU9250Interface::verifyConnection() {
    // Check if device responds to I2C communication
    if (!isConnected()) {
        Serial.println("MPU sensor not responding to I2C communication");
        return false;
    }
    
    // Check WHO_AM_I register
    uint8_t whoAmI = readRegister(WHO_AM_I_REG);
    
    // Accept a wider range of valid WHO_AM_I values
    bool validID = (whoAmI == 0x68 || whoAmI == 0x71 || whoAmI == 0x73 || whoAmI == 0x70);
    
    if (!validID) {
        Serial.printf("Invalid WHO_AM_I value: 0x%02X\n", whoAmI);
    }
    
    return validID;
}

bool MPU9250Interface::resetDevice() {
    Serial.println("Resetting MPU device...");
    // Reset the device by setting bit 7 of PWR_MGMT_1 register
    return writeRegister(PWR_MGMT_1_REG, 0x80);
}

bool MPU9250Interface::recoverFromError() {
    Serial.println("Attempting to recover from error state...");
    
    // First, try resetting the device
    if (!resetDevice()) {
        Serial.println("Reset failed");
        return false;
    }
    
    delay(100);  // Wait for reset
    
    // Reinitialize
    return init();
}

bool MPU9250Interface::runDiagnostics() {
    Serial.println("\n=== MPU Sensor Diagnostics ===");
    
    // Test 1: I2C Communication
    Serial.println("Test 1: I2C Communication");
    if (!isConnected()) {
        Serial.println("FAIL: Device not responding on I2C bus");
        return false;
    }
    Serial.println("PASS: Device responding on I2C bus");
    
    // Test 2: Device Identity
    Serial.println("Test 2: Device Identity");
    uint8_t whoAmI = readRegister(WHO_AM_I_REG);
    Serial.printf("WHO_AM_I = 0x%02X: ", whoAmI);
    
    if (whoAmI == 0x68 || whoAmI == 0x71 || whoAmI == 0x73 || whoAmI == 0x70) {
        Serial.println("PASS: Valid device ID");
    } else {
        Serial.println("FAIL: Unknown device ID");
        return false;
    }
    
    // Test 3: Power Management
    Serial.println("Test 3: Power Management");
    uint8_t pwrMgmt = readRegister(PWR_MGMT_1_REG);
    Serial.printf("PWR_MGMT_1 = 0x%02X: ", pwrMgmt);
    
    if (pwrMgmt & 0x40) { // Check if sleep bit is set
        Serial.println("FAIL: Device is in sleep mode");
        return false;
    } else {
        Serial.println("PASS: Device is awake");
    }
    
    // Test 4: Data Reading
    Serial.println("Test 4: Data Reading");
    SensorData testData;
    if (!readSensorData(&testData)) {
        Serial.println("FAIL: Could not read sensor data");
        return false;
    }
    
    Serial.println("PASS: Successfully read sensor data");
    Serial.printf("Accel: X=%d Y=%d Z=%d\n", testData.accelX, testData.accelY, testData.accelZ);
    Serial.printf("Gyro: X=%d Y=%d Z=%d\n", testData.gyroX, testData.gyroY, testData.gyroZ);
    
    Serial.println("=== Diagnostics Complete ===");
    return true;
} 