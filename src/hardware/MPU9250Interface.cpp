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
    delay(150);
    
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
    delay(150);
    
    // Wake up the sensor (clear sleep bit and use X-axis gyro as clock reference)
    Serial.println("Waking up sensor...");
    if (!writeRegister(PWR_MGMT_1_REG, 0x01)) {
        Serial.println("Failed to wake up device");
        return false;
    }
    delay(100);
    
    // Configure sample rate divider (register 0x19)
    // Calculate: sample_rate = 1kHz / (1 + divider)
    // For 125Hz: 1000 / (1 + 7) = 125Hz
    Serial.println("Setting sample rate to 125Hz...");
    if (!writeRegister(SMPLRT_DIV_REG, 0x07)) {
        Serial.println("Failed to set sample rate");
        return false;
    }
    delay(10); // Add short delay between I2C operations
    
    // Configure accelerometer range to ±4g (register 0x1C)
    Serial.println("Configuring accelerometer to ±4g range...");
    if (!writeRegister(ACCEL_CONFIG_REG, 0x08)) {
        Serial.println("Failed to configure accelerometer");
        return false;
    }
    delay(10); // Add short delay between I2C operations
    
    // Configure gyroscope range to ±500 deg/s (register 0x1B)
    Serial.println("Configuring gyroscope to ±500 deg/s range...");
    if (!writeRegister(GYRO_CONFIG_REG, 0x08)) {
        Serial.println("Failed to configure gyroscope");
        return false;
    }
    delay(10); // Add short delay between I2C operations
    
    // Configure digital low pass filter
    Serial.println("Configuring digital low pass filter...");
    if (!writeRegister(CONFIG_REG, 0x03)) {
        Serial.println("Failed to configure DLPF");
        return false;
    }
    delay(10); // Add short delay between I2C operations
    
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
    // Try up to 3 times for reliability
    for (int attempt = 0; attempt < 3; attempt++) {
        Wire.beginTransmission(sensorAddress);
        Wire.write(reg);
        Wire.write(value);
        uint8_t error = Wire.endTransmission();
        if (error == 0) {
            return true;
        }
        
        // Log error and retry
        Serial.printf("I2C write error: %d (register 0x%02X, value 0x%02X, address 0x%02X) - attempt %d\n", 
                    error, reg, value, sensorAddress, attempt+1);
        delay(10); // Wait before retry
    }
    
    // Failed after multiple attempts
    return false;
}

uint8_t MPU9250Interface::readRegister(uint8_t reg) {
    for (int attempt = 0; attempt < 3; attempt++) {
        Wire.beginTransmission(sensorAddress);
        Wire.write(reg);
        uint8_t error = Wire.endTransmission(false);
        if (error != 0) {
            Serial.printf("I2C read error during address setup: %d (register 0x%02X, address 0x%02X) - attempt %d\n", 
                         error, reg, sensorAddress, attempt+1);
            delay(10);
            continue;
        }
        
        // Fix ambiguity by explicitly casting to the correct types
        uint8_t bytesReceived = Wire.requestFrom((uint8_t)sensorAddress, (uint8_t)1);
        if (bytesReceived != 1) {
            Serial.printf("I2C read error: requested 1 byte, received %d bytes - attempt %d\n", bytesReceived, attempt+1);
            delay(10);
            continue;
        }
        
        return Wire.read();
    }
    
    // Failed after multiple attempts
    return 0;
}

bool MPU9250Interface::readRegisters(uint8_t reg, uint8_t* buffer, uint8_t count) {
    for (int attempt = 0; attempt < 3; attempt++) {
        Wire.beginTransmission(sensorAddress);
        Wire.write(reg);
        uint8_t error = Wire.endTransmission(false);
        if (error != 0) {
            Serial.printf("I2C read error during multi-read address setup: %d (register 0x%02X, address 0x%02X) - attempt %d\n", 
                         error, reg, sensorAddress, attempt+1);
            delay(10);
            continue;
        }
        
        uint8_t bytesReceived = Wire.requestFrom((uint8_t)sensorAddress, (uint8_t)count);
        if (bytesReceived != count) {
            Serial.printf("I2C multi-read error: requested %d bytes, received %d bytes - attempt %d\n", count, bytesReceived, attempt+1);
            delay(10);
            continue;
        }
        
        for (uint8_t i = 0; i < count; i++) {
            buffer[i] = Wire.read();
        }
        
        return true;
    }
    
    // Failed after multiple attempts
    return false;
}

// Enhanced error handling and connection verification
bool MPU9250Interface::isConnected() {
    // Try to read the WHO_AM_I register
    Wire.beginTransmission(sensorAddress);
    Wire.write(WHO_AM_I_REG);
    uint8_t error = Wire.endTransmission(false);
    
    if (error != 0) {
        Serial.printf("I2C error during isConnected check: %d\n", error);
        
        // Try the alternative address if the first one fails
        if (sensorAddress == MPU9250_ADDRESS_AD0_LOW) {
            Serial.println("Trying alternative I2C address 0x69");
            sensorAddress = MPU9250_ADDRESS_AD0_HIGH;
        } else {
            Serial.println("Trying standard I2C address 0x68");
            sensorAddress = MPU9250_ADDRESS_AD0_LOW;
        }
        
        // Try again with the new address
        Wire.beginTransmission(sensorAddress);
        Wire.write(WHO_AM_I_REG);
        error = Wire.endTransmission(false);
        
        if (error != 0) {
            Serial.println("Device not responding on either address");
            return false;
        } else {
            Serial.printf("Device found at alternative address 0x%02X\n", sensorAddress);
        }
    }
    
    // Device responded to I2C communication
    return true;
}

bool MPU9250Interface::verifyConnection() {
    // First check if device is responding to I2C
    if (!isConnected()) {
        return false;
    }
    
    // Read WHO_AM_I register value
    uint8_t whoAmI = readRegister(WHO_AM_I_REG);
    
    // Verify the register value matches expected values for MPU6050/MPU9250
    // MPU6050: 0x68, MPU9250: 0x71 or 0x73, MPU6000: 0x70
    if (whoAmI == 0x68 || whoAmI == 0x71 || whoAmI == 0x73 || whoAmI == 0x70) {
        Serial.printf("WHO_AM_I register returned valid value: 0x%02X\n", whoAmI);
        return true;
    } else {
        Serial.printf("WHO_AM_I register returned unknown value: 0x%02X\n", whoAmI);
        return false;
    }
}

bool MPU9250Interface::resetDevice() {
    // Write the reset bit to PWR_MGMT_1 register
    Serial.println("Resetting MPU sensor...");
    if (!writeRegister(PWR_MGMT_1_REG, 0x80)) {
        Serial.println("Failed to reset device");
        return false;
    }
    
    // Wait for reset to complete
    delay(100);
    
    // Verify the device is responsive after reset
    if (!isConnected()) {
        Serial.println("Device not responding after reset");
        return false;
    }
    
    // Wake up the sensor
    if (!writeRegister(PWR_MGMT_1_REG, 0x01)) {
        Serial.println("Failed to wake up device after reset");
        return false;
    }
    
    return true;
}

bool MPU9250Interface::recoverFromError() {
    Serial.println("Attempting to recover MPU sensor from error state...");
    
    // Try basic reset first
    if (!resetDevice()) {
        Serial.println("Basic reset failed");
        
        // Try re-initializing I2C
        Wire.end();
        delay(50);
        Wire.begin(SDA_PIN, SCL_PIN);
        Wire.setClock(I2C_FREQUENCY);
        delay(50);
        
        // Try reset again
        if (!resetDevice()) {
            Serial.println("Re-initialization failed, recovery unsuccessful");
            return false;
        }
    }
    
    // Reset successful, try to re-initialize the device
    if (!init()) {
        Serial.println("Re-initialization failed after reset");
        return false;
    }
    
    // Reset error counters
    errorCount = 0;
    identicalReadings = 0;
    lastReadValid = true;
    
    Serial.println("MPU recovery successful");
    return true;
}

bool MPU9250Interface::runDiagnostics() {
    Serial.println("Running MPU sensor diagnostics...");
    
    // Check 1: Verify I2C connection
    if (!isConnected()) {
        Serial.println("Diagnostic failed: Device not responding on I2C");
        return false;
    }
    
    // Check 2: Verify device identity
    if (!verifyConnection()) {
        Serial.println("Diagnostic failed: Device identity check failed");
        return false;
    }
    
    // Check 3: Verify sensor data reading
    SensorData testData;
    if (!readSensorData(&testData)) {
        Serial.println("Diagnostic failed: Cannot read sensor data");
        return false;
    }
    
    // Check 4: Verify data validation
    if (!validateSensorData(testData)) {
        Serial.println("Diagnostic failed: Sensor data validation failed");
        return false;
    }
    
    // Check 5: Test register write and read
    uint8_t testValue = 0x03;  // DLPF setting
    if (!writeRegister(CONFIG_REG, testValue)) {
        Serial.println("Diagnostic failed: Cannot write to registers");
        return false;
    }
    
    uint8_t readValue = readRegister(CONFIG_REG);
    if (readValue != testValue) {
        Serial.printf("Diagnostic failed: Register read/write mismatch (wrote 0x%02X, read 0x%02X)\n",
                     testValue, readValue);
        return false;
    }
    
    Serial.println("All MPU sensor diagnostics passed successfully");
    return true;
}

bool MPU9250Interface::readFilteredData(SensorData* data) {
    // Check if a valid pointer was provided
    if (data == nullptr) {
        return false;
    }
    
    // Get raw sensor data first
    SensorData rawData;
    if (!readSensorData(&rawData)) {
        return false;
    }
    
    // If filtering isn't initialized, use raw data directly
    if (!filterInitialized) {
        // Initialize all filter slots with this sample
        for (int i = 0; i < FILTER_SAMPLE_COUNT; i++) {
            filterSamples[i] = rawData;
        }
        filterInitialized = true;
        *data = rawData;
        return true;
    }
    
    // Add the new data to the filter buffer
    addToFilterBuffer(rawData);
    
    // Calculate the filtered data
    calculateFilteredData(data);
    
    return true;
}

void MPU9250Interface::addToFilterBuffer(const SensorData& data) {
    // Store the new reading in the current index
    filterSamples[filterIndex] = data;
    
    // Move to the next slot (circular buffer)
    filterIndex = (filterIndex + 1) % FILTER_SAMPLE_COUNT;
}

void MPU9250Interface::calculateFilteredData(SensorData* output) {
    // Simple averaging filter
    int32_t accelXSum = 0, accelYSum = 0, accelZSum = 0;
    int32_t gyroXSum = 0, gyroYSum = 0, gyroZSum = 0;
    
    // Sum all values
    for (int i = 0; i < FILTER_SAMPLE_COUNT; i++) {
        accelXSum += filterSamples[i].accelX;
        accelYSum += filterSamples[i].accelY;
        accelZSum += filterSamples[i].accelZ;
        
        gyroXSum += filterSamples[i].gyroX;
        gyroYSum += filterSamples[i].gyroY;
        gyroZSum += filterSamples[i].gyroZ;
    }
    
    // Calculate average
    output->accelX = accelXSum / FILTER_SAMPLE_COUNT;
    output->accelY = accelYSum / FILTER_SAMPLE_COUNT;
    output->accelZ = accelZSum / FILTER_SAMPLE_COUNT;
    
    output->gyroX = gyroXSum / FILTER_SAMPLE_COUNT;
    output->gyroY = gyroYSum / FILTER_SAMPLE_COUNT;
    output->gyroZ = gyroZSum / FILTER_SAMPLE_COUNT;
    
    // Use the timestamp from the most recent reading
    output->timestamp = filterSamples[filterIndex > 0 ? filterIndex - 1 : FILTER_SAMPLE_COUNT - 1].timestamp;
}

bool MPU9250Interface::validateSensorData(const SensorData& data) {
    // Check for impossibly large values (likely hardware failure)
    if (abs(data.accelX) > MAX_ACCEL_VALUE || 
        abs(data.accelY) > MAX_ACCEL_VALUE || 
        abs(data.accelZ) > MAX_ACCEL_VALUE ||
        abs(data.gyroX) > MAX_GYRO_VALUE || 
        abs(data.gyroY) > MAX_GYRO_VALUE || 
        abs(data.gyroZ) > MAX_GYRO_VALUE) {
        
        Serial.println("Error: Sensor values exceed maximum possible range");
        errorCount++;
        lastReadValid = false;
        return false;
    }
    
    // Check for repeated identical readings (possible hardware failure)
    if (filterInitialized) {
        int lastIdx = (filterIndex == 0) ? FILTER_SAMPLE_COUNT - 1 : filterIndex - 1;
        
        if (data.accelX == filterSamples[lastIdx].accelX &&
            data.accelY == filterSamples[lastIdx].accelY &&
            data.accelZ == filterSamples[lastIdx].accelZ &&
            data.gyroX == filterSamples[lastIdx].gyroX &&
            data.gyroY == filterSamples[lastIdx].gyroY &&
            data.gyroZ == filterSamples[lastIdx].gyroZ) {
            
            identicalReadings++;
            
            if (identicalReadings > MAX_CONSECUTIVE_IDENTICAL) {
                Serial.println("Warning: Too many identical consecutive readings");
                errorCount++;
                lastReadValid = false;
                return false;
            }
        } else {
            // Reset the counter if readings change
            identicalReadings = 0;
        }
    }
    
    // Check for expected minimum variation in accelerometer 
    // (over multiple readings when device is handheld)
    if (filterInitialized) {
        bool hasVariation = false;
        for (int i = 0; i < FILTER_SAMPLE_COUNT - 1; i++) {
            if (abs(filterSamples[i].accelX - filterSamples[i+1].accelX) > MIN_ACCEL_VARIATION ||
                abs(filterSamples[i].accelY - filterSamples[i+1].accelY) > MIN_ACCEL_VARIATION ||
                abs(filterSamples[i].accelZ - filterSamples[i+1].accelZ) > MIN_ACCEL_VARIATION) {
                hasVariation = true;
                break;
            }
        }
        
        if (!hasVariation) {
            // No variation is suspicious but might be valid if device is stationary
            // Just log a warning but don't invalidate
            Serial.println("Note: Low variation in accelerometer readings");
        }
    }
    
    // Reset error count if we have a valid reading
    if (errorCount > 0) {
        errorCount--;
    }
    
    lastReadValid = true;
    return true;
}

bool MPU9250Interface::getMaxAxisData(SensorData* data, uint32_t durationMs) {
    if (data == nullptr) {
        return false;
    }
    
    // Initialize with minimum values
    data->accelX = data->accelY = data->accelZ = -32768;
    data->gyroX = data->gyroY = data->gyroZ = -32768;
    data->timestamp = millis();
    
    uint32_t startTime = millis();
    uint32_t endTime = startTime + durationMs;
    
    // Sample data for the specified duration
    while (millis() < endTime) {
        SensorData current;
        if (!readSensorData(&current)) {
            continue;  // Skip invalid readings
        }
        
        // Update maximum values (using absolute values)
        if (abs(current.accelX) > abs(data->accelX)) data->accelX = current.accelX;
        if (abs(current.accelY) > abs(data->accelY)) data->accelY = current.accelY;
        if (abs(current.accelZ) > abs(data->accelZ)) data->accelZ = current.accelZ;
        
        if (abs(current.gyroX) > abs(data->gyroX)) data->gyroX = current.gyroX;
        if (abs(current.gyroY) > abs(data->gyroY)) data->gyroY = current.gyroY;
        if (abs(current.gyroZ) > abs(data->gyroZ)) data->gyroZ = current.gyroZ;
        
        delay(10);  // Small delay between samples
    }
    
    // Update timestamp
    data->timestamp = millis();
    return true;
}

uint32_t MPU9250Interface::calculateMotionMagnitude(const SensorData& data) {
    // Simple sum of absolute values across all axes
    uint32_t magnitude = 0;
    
    // Add accelerometer components (scaled down)
    magnitude += (abs(data.accelX) / 10);
    magnitude += (abs(data.accelY) / 10);
    magnitude += (abs(data.accelZ) / 10);
    
    // Add gyroscope components (scaled down)
    magnitude += (abs(data.gyroX) / 5);
    magnitude += (abs(data.gyroY) / 5);
    magnitude += (abs(data.gyroZ) / 5);
    
    return magnitude;
} 