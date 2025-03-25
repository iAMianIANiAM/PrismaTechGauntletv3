#ifndef MPU9250_INTERFACE_H
#define MPU9250_INTERFACE_H

#include "../core/SystemTypes.h"
#include <Wire.h>

// MPU9250 I2C address
#define MPU9250_ADDRESS 0x68

// MPU9250 register addresses
#define MPU9250_ACCEL_XOUT_H    0x3B
#define MPU9250_ACCEL_XOUT_L    0x3C
#define MPU9250_ACCEL_YOUT_H    0x3D
#define MPU9250_ACCEL_YOUT_L    0x3E
#define MPU9250_ACCEL_ZOUT_H    0x3F
#define MPU9250_ACCEL_ZOUT_L    0x40
#define MPU9250_GYRO_XOUT_H     0x43
#define MPU9250_GYRO_XOUT_L     0x44
#define MPU9250_GYRO_YOUT_H     0x45
#define MPU9250_GYRO_YOUT_L     0x46
#define MPU9250_GYRO_ZOUT_H     0x47
#define MPU9250_GYRO_ZOUT_L     0x48
#define MPU9250_PWR_MGMT_1      0x6B
#define MPU9250_WHO_AM_I        0x75

/**
 * @brief Interface to the MPU9250 IMU sensor
 */
class MPU9250Interface {
public:
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

private:
  // Sensor calibration offsets
  int16_t accelOffsetX = 0;
  int16_t accelOffsetY = 0;
  int16_t accelOffsetZ = 0;
  int16_t gyroOffsetX = 0;
  int16_t gyroOffsetY = 0;
  int16_t gyroOffsetZ = 0;
  
  // I2C helper methods
  bool writeRegister(uint8_t reg, uint8_t value);
  uint8_t readRegister(uint8_t reg);
  bool readRegisters(uint8_t reg, uint8_t* buffer, uint8_t count);
};

#endif // MPU9250_INTERFACE_H 