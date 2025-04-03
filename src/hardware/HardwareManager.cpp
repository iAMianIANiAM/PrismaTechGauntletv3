#include "HardwareManager.h"
#include "../core/Config.h"
#include "../utils/DebugTools.h"
#include <Wire.h>

// Singleton instance
static HardwareManager* _instance = nullptr;

/**
 * @brief Get the singleton instance of HardwareManager
 * @return Pointer to the HardwareManager instance
 */
HardwareManager* HardwareManager::getInstance() {
    if (_instance == nullptr) {
        _instance = new HardwareManager();
    }
    return _instance;
}

/**
 * @brief Constructor - initializes internal state
 */
HardwareManager::HardwareManager() 
    : motionDataCount(0)
    , isRecordingMotion(false)
    , lastSensorUpdateTime(0)
    , lastLedUpdateTime(0)
    , isInitialized(false)
    , isActive(true)
{
    // Initialize sensor data to zero
    memset(&latestSensorData, 0, sizeof(SensorData));
}

/**
 * @brief Initialize all hardware components
 * @return True if initialization was successful
 */
bool HardwareManager::init() {
    if (isInitialized) {
        DEBUG_PRINTLN("HardwareManager already initialized");
        return true;
    }

    DEBUG_PRINTLN("Initializing HardwareManager...");
    
    // Configure hardware pins
    configurePins();
    
    // CRITICAL FIX: Explicitly initialize I2C bus with correct parameters
    Wire.end(); // Ensure clean state
    delay(50);
    Wire.begin(Config::I2C_SDA_PIN, Config::I2C_SCL_PIN);
    Wire.setClock(100000); // 100kHz clock speed as specified in reference
    delay(150); // Wait for I2C to stabilize
    
    DEBUG_PRINTLN("Initializing MPU sensor...");
    
    // Try both possible MPU addresses
    const uint8_t mpuAddresses[] = {0x68, 0x69};
    bool sensorFound = false;
    
    for (uint8_t i = 0; i < 2; i++) {
        uint8_t addr = mpuAddresses[i];
        char addrStr[10];
        sprintf(addrStr, "0x%02X", addr);
        DEBUG_PRINT("Testing MPU at address ");
        DEBUG_PRINTLN(addrStr);
        
        // CRITICAL FIX: Check bus communication first
        Wire.beginTransmission(addr);
        byte error = Wire.endTransmission();
        
        if (error == 0) {
            DEBUG_PRINTLN("Device found on I2C bus");
            
            // Set the address in our interface
            imu.setAddress(addr);
            
            // CRITICAL FIX: Reset the device properly
            DEBUG_PRINTLN("Resetting MPU sensor...");
            // Reset power management register
            Wire.beginTransmission(addr);
            Wire.write(0x6B); // PWR_MGMT_1 register
            Wire.write(0x80); // Set reset bit
            error = Wire.endTransmission();
            if (error != 0) {
                DEBUG_PRINT("Reset transmission error: ");
                char errorStr[8];
                sprintf(errorStr, "%d", error);
                DEBUG_PRINTLN(errorStr);
                continue;
            }
            delay(150); // Wait for reset to complete
            
            // CRITICAL FIX: Wake up the device
            Wire.beginTransmission(addr);
            Wire.write(0x6B); // PWR_MGMT_1 register
            Wire.write(0x00); // Clear sleep bit
            error = Wire.endTransmission();
            if (error != 0) {
                DEBUG_PRINT("Wake transmission error: ");
                char errorStr[8];
                sprintf(errorStr, "%d", error);
                DEBUG_PRINTLN(errorStr);
                continue;
            }
            delay(150); // Wait for wake up
            
            // Now initialize the sensor through our interface
            if (imu.init()) {
                DEBUG_PRINTLN("MPU sensor initialized successfully");
                sensorFound = true;
                
                // Perform calibration after successful initialization
                DEBUG_PRINTLN("Calibrating MPU sensor...");
                if (!imu.calibrate()) {
                    DEBUG_PRINTLN("WARNING: MPU calibration failed. Using uncalibrated values.");
                    // Continue anyway - calibration failure is not fatal
                }
                
                break; // Exit the loop, we found a working sensor
            } else {
                DEBUG_PRINTLN("MPU initialization failed despite I2C connection");
            }
        } else {
            // Print the specific I2C error code for diagnostic purposes
            char errorMsg[50];
            sprintf(errorMsg, "I2C connection error: %d (address %s)", error, addrStr);
            DEBUG_PRINTLN(errorMsg);
        }
    }
    
    if (!sensorFound) {
        DEBUG_PRINTLN("ERROR: No MPU sensor found at standard addresses.");
        return false;
    }
    
    // Initialize LED interface
    DEBUG_PRINTLN("Initializing LED interface...");
    if (!leds.init()) {
        DEBUG_PRINTLN("ERROR: Failed to initialize LED interface");
        return false;
    }
    
    // Set default brightness
    leds.setBrightness(Config::DEFAULT_BRIGHTNESS);
    
    // Show startup animation (white pulse)
    Color white = {255, 255, 255};
    leds.pulse(white, 2, 500);
    
    // Initialize power manager
    DEBUG_PRINTLN("Initializing power management...");
    power.init();
    
    // Initialize shake detector
    DEBUG_PRINTLN("Initializing shake gesture detector...");
    if (!shakeDetector.init(&imu)) {
        DEBUG_PRINTLN("WARNING: Failed to initialize shake detector");
        // Continue anyway - not a critical component
    }
    
    // Set active power state
    setPowerState(true);
    
    isInitialized = true;
    DEBUG_PRINTLN("HardwareManager initialization complete");
    
    return true;
}

/**
 * @brief Update hardware state (read sensors, update LEDs)
 */
void HardwareManager::update() {
    if (!isInitialized) {
        return;
    }
    
    unsigned long currentMillis = millis();
    
    // Update sensor data at the configured interval (20ms default)
    const unsigned long SENSOR_UPDATE_INTERVAL = 20;
    if (currentMillis - lastSensorUpdateTime >= SENSOR_UPDATE_INTERVAL) {
        lastSensorUpdateTime = currentMillis;
        
        // Read sensor data
        if (!imu.readSensorData(&latestSensorData)) {
            DEBUG_PRINTLN("WARNING: Failed to read MPU sensor data");
            
            // Attempt recovery if multiple failures occur
            static uint8_t failureCount = 0;
            if (++failureCount >= 5) {
                DEBUG_PRINTLN("ERROR: Multiple sensor read failures, attempting reset");
                // Try to recover the sensor connection
                imu.recoverFromError();
                failureCount = 0;
            }
        }
        
        // Record motion data if enabled
        if (isRecordingMotion && motionDataCount < MAX_MOTION_SAMPLES) {
            motionData[motionDataCount++] = latestSensorData;
        }
        
        // Update shake detector
        shakeDetector.update();
    }
    
    // Update LEDs at the configured interval (50ms default)
    const unsigned long LED_UPDATE_INTERVAL = 50;
    if (currentMillis - lastLedUpdateTime >= LED_UPDATE_INTERVAL) {
        lastLedUpdateTime = currentMillis;
        
        // This only updates LEDs if there are pending changes
        leds.show();
    }
    
    // Update power management
    power.update();
}

/**
 * @brief Get the latest sensor reading
 * @return Reference to the latest sensor data
 */
const SensorData& HardwareManager::getSensorData() const {
    return latestSensorData;
}

/**
 * @brief Set the color of a specific LED
 * @param index LED index (0-11)
 * @param color RGB color
 */
void HardwareManager::setLED(uint8_t index, const Color& color) {
    if (isInitialized) {
        leds.setLED(index, color);
    }
}

/**
 * @brief Set all LEDs to the same color
 * @param color RGB color
 */
void HardwareManager::setAllLEDs(const Color& color) {
    if (isInitialized) {
        leds.setAllLEDs(color);
    }
}

/**
 * @brief Update the LED display
 */
void HardwareManager::updateLEDs() {
    if (isInitialized) {
        leds.show();
    }
}

/**
 * @brief Set LED brightness
 * @param brightness Brightness value (0-255)
 */
void HardwareManager::setBrightness(uint8_t brightness) {
    if (isInitialized) {
        leds.setBrightness(brightness);
    }
}

/**
 * @brief Start recording motion data for Freecast mode
 */
void HardwareManager::recordMotionData() {
    motionDataCount = 0;
    isRecordingMotion = true;
}

/**
 * @brief Stop recording motion data
 */
void HardwareManager::stopRecordingMotion() {
    isRecordingMotion = false;
}

/**
 * @brief Get the recorded motion data buffer
 * @return Pointer to the motion data array
 */
SensorData* HardwareManager::getMotionData() {
    return motionData;
}

/**
 * @brief Get the number of motion samples recorded
 * @return Number of samples in the motion data buffer
 */
uint8_t HardwareManager::getMotionDataSize() const {
    return motionDataCount;
}

/**
 * @brief Clear the motion data buffer
 */
void HardwareManager::clearMotionData() {
    motionDataCount = 0;
}

/**
 * @brief Set the system power state
 * @param active If true, use full power; if false, reduce power
 */
void HardwareManager::setPowerState(bool active) {
    isActive = active;
    
    if (active) {
        // Full power mode
        power.setLowPowerMode(false);
        leds.setBrightness(Config::DEFAULT_BRIGHTNESS);
    } else {
        // Power saving mode
        power.setLowPowerMode(true);
        leds.setBrightness(20); // Low power brightness
    }
}

/**
 * @brief Get the current power state
 * @return True if in active power state, false if in low power state
 */
bool HardwareManager::getPowerState() const {
    return isActive;
}

/**
 * @brief Reset a specific hardware component
 * @param component The component to reset
 * @return True if reset was successful
 */
bool HardwareManager::resetComponent(HardwareComponent component) {
    switch (component) {
        case HW_COMPONENT_MPU:
            // Use the proper reset method
            return imu.resetDevice();
        case HW_COMPONENT_LED:
            leds.clear();
            return leds.init();
        case HW_COMPONENT_ALL:
            bool mpuReset = imu.resetDevice();
            leds.clear();
            bool ledReset = leds.init();
            return mpuReset && ledReset;
    }
    return false;
}

/**
 * @brief Run a self-test on hardware components
 * @param component The component to test
 * @return True if test passed, false if failed
 */
bool HardwareManager::runSelfTest(HardwareComponent component) {
    switch (component) {
        case HW_COMPONENT_MPU:
            return imu.verifyConnection();
        case HW_COMPONENT_LED:
            // Simple LED test - flash red, green, blue
            {
                Color red = {255, 0, 0};
                Color green = {0, 255, 0};
                Color blue = {0, 0, 255};
                
                leds.setAllLEDs(red);
                leds.show();
                delay(200);
                
                leds.setAllLEDs(green);
                leds.show();
                delay(200);
                
                leds.setAllLEDs(blue);
                leds.show();
                delay(200);
                
                leds.clear();
                leds.show();
                return true;
            }
        case HW_COMPONENT_ALL:
            bool mpuTest = imu.verifyConnection();
            
            // Simple LED test
            Color white = {255, 255, 255};
            leds.setAllLEDs(white);
            leds.show();
            delay(200);
            leds.clear();
            leds.show();
            
            return mpuTest;
    }
    return false;
}

/**
 * @brief Configure hardware pins for all components
 */
void HardwareManager::configurePins() {
    // Configure LED power pin if defined
    #ifdef CONFIG_LED_POWER_PIN
    pinMode(Config::LED_POWER_PIN, OUTPUT);
    digitalWrite(Config::LED_POWER_PIN, HIGH);  // Enable LED power
    #endif
    
    // Additional pin configuration as needed
} 