#ifndef MPU9250_INTERFACE_H
#define MPU9250_INTERFACE_H

#include "../core/SystemTypes.h"
#include <Wire.h>

// MPU Register Definitions
#define WHO_AM_I_REG       0x75  // Device identity register
#define PWR_MGMT_1_REG     0x6B  // Power management register
#define ACCEL_XOUT_H_REG   0x3B  // First accelerometer data register
#define GYRO_XOUT_H_REG    0x43  // First gyroscope data register
#define CONFIG_REG         0x1A  // Configuration register
#define GYRO_CONFIG_REG    0x1B  // Gyroscope configuration register
#define ACCEL_CONFIG_REG   0x1C  // Accelerometer configuration register
#define SMPLRT_DIV_REG     0x19  // Sample rate divider register
#define INT_ENABLE_REG     0x38  // Interrupt enable register

// MPU9250/MPU6050 I2C addresses
#define MPU9250_ADDRESS_AD0_LOW  0x68  // Default address when AD0 is grounded
#define MPU9250_ADDRESS_AD0_HIGH 0x69  // Alternative address when AD0 is connected to VCC

// Default to AD0_LOW unless changed through setAddress()
#define MPU9250_ADDRESS MPU9250_ADDRESS_AD0_LOW

// Number of samples for filtering
#define FILTER_SAMPLE_COUNT 5

/**
 * @brief Interface to the MPU9250/MPU6050 IMU sensor
 * 
 * Provides methods to initialize, read data from, and calibrate the
 * MPU9250 or MPU6050 sensor via I2C communication.
 */
class MPU9250Interface {
public:
  /**
   * @brief Set the I2C address for the MPU sensor
   * @param address The I2C address (0x68 or 0x69)
   */
  void setAddress(uint8_t address);
  
  /**
   * @brief Get the current I2C address being used
   * @return The current I2C address
   */
  uint8_t getAddress() const;
  
  /**
   * @brief Initialize the IMU sensor
   * @return True if initialization was successful
   */
  bool init();
  
  /**
   * @brief Read sensor data
   * @param data Pointer to SensorData structure to fill
   * @return True if read was successful
   */
  bool readSensorData(SensorData* data);
  
  /**
   * @brief Read sensor data with filtering
   * @param data Pointer to SensorData structure to fill with filtered data
   * @return True if read was successful
   */
  bool readFilteredData(SensorData* data);
  
  /**
   * @brief Validate the sensor data for consistency
   * @param data The sensor data to validate
   * @return True if data appears valid
   */
  bool validateSensorData(const SensorData& data);
  
  /**
   * @brief Get the maximum axis values detected within a time period
   * @param data Pointer to SensorData to store maximum values
   * @param durationMs Duration in milliseconds to monitor
   * @return True if operation was successful
   */
  bool getMaxAxisData(SensorData* data, uint32_t durationMs);
  
  /**
   * @brief Calculate the magnitude of motion across all axes
   * @param data Sensor data to analyze
   * @return Total motion magnitude (unitless, relative)
   */
  uint32_t calculateMotionMagnitude(const SensorData& data);
  
  /**
   * @brief Set the sensor sample rate
   * @param rate Sample rate in Hz (50-1000)
   */
  void setSampleRate(uint16_t rate);
  
  /**
   * @brief Set the sensor to low power mode
   * @param lowPower If true, enable low power mode
   */
  void setLowPowerMode(bool lowPower);
  
  /**
   * @brief Calibrate the sensor
   * @return True if calibration was successful
   */
  bool calibrate();
  
  /**
   * @brief Check if the sensor is connected
   * @return True if connected
   */
  bool isConnected();
  
  /**
   * @brief Verify sensor connection with ID check
   * @return True if the correct sensor is connected
   */
  bool verifyConnection();
  
  /**
   * @brief Reset the sensor
   * @return True if reset was successful
   */
  bool resetDevice();
  
  /**
   * @brief Attempt to recover from an error state
   * @return True if recovery was successful
   */
  bool recoverFromError();
  
  /**
   * @brief Run diagnostics on the sensor
   * @return True if all diagnostic tests pass
   */
  bool runDiagnostics();

private:
  // Current sensor address (default to AD0_LOW)
  uint8_t sensorAddress = MPU9250_ADDRESS_AD0_LOW;
  
  // Sensor calibration offsets
  int16_t accelOffsetX = 0;
  int16_t accelOffsetY = 0;
  int16_t accelOffsetZ = 0;
  int16_t gyroOffsetX = 0;
  int16_t gyroOffsetY = 0;
  int16_t gyroOffsetZ = 0;
  
  // Filtering data
  SensorData filterSamples[FILTER_SAMPLE_COUNT];
  uint8_t filterIndex = 0;
  bool filterInitialized = false;
  
  // Data validation thresholds
  static const int32_t MAX_ACCEL_VALUE = 32767;  // Max possible 16-bit value
  static const int32_t MAX_GYRO_VALUE = 32767;   // Max possible 16-bit value
  static const int32_t MIN_ACCEL_VARIATION = 10; // Minimum expected variation
  static const int32_t MAX_CONSECUTIVE_IDENTICAL = 5; // Max identical readings
  
  // Sensor health tracking
  uint8_t errorCount = 0;
  uint8_t identicalReadings = 0;
  bool lastReadValid = true;
  
  // I2C helper methods
  bool writeRegister(uint8_t reg, uint8_t value);
  uint8_t readRegister(uint8_t reg);
  bool readRegisters(uint8_t reg, uint8_t* buffer, uint8_t count);
  
  // Filtering helper methods
  void addToFilterBuffer(const SensorData& data);
  void calculateFilteredData(SensorData* output);
};

#endif // MPU9250_INTERFACE_H 