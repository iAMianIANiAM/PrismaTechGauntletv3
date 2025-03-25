#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "../core/SystemTypes.h"
#include "MPU9250Interface.h"
#include "LEDInterface.h"
#include "PowerManager.h"

// Maximum number of motion samples to store
#define MAX_MOTION_SAMPLES 100

/**
 * @brief Manages all hardware interfaces and abstracts hardware access
 */
class HardwareManager {
public:
  /**
   * @brief Initialize all hardware components
   * @return True if initialization was successful
   */
  bool init();
  
  /**
   * @brief Update hardware state (read sensors, etc.)
   */
  void update();
  
  /**
   * @brief Get the latest sensor reading
   * @return Reference to the latest sensor data
   */
  const SensorData& getSensorData() const;
  
  /**
   * @brief Set the color of a specific LED
   * @param index LED index (0-11)
   * @param color RGB color
   */
  void setLED(uint8_t index, const Color& color);
  
  /**
   * @brief Set all LEDs to the same color
   * @param color RGB color
   */
  void setAllLEDs(const Color& color);
  
  /**
   * @brief Update the LED display
   */
  void updateLEDs();
  
  /**
   * @brief Start recording motion data for Freecast mode
   */
  void recordMotionData();
  
  /**
   * @brief Get the recorded motion data buffer
   * @return Pointer to the motion data array
   */
  SensorData* getMotionData();
  
  /**
   * @brief Get the number of motion samples recorded
   * @return Number of samples in the motion data buffer
   */
  uint8_t getMotionDataSize() const;
  
  /**
   * @brief Clear the motion data buffer
   */
  void clearMotionData();
  
  /**
   * @brief Set the system power state
   * @param active If true, use full power; if false, reduce power
   */
  void setPowerState(bool active);

private:
  // Hardware component instances
  MPU9250Interface imu;
  LEDInterface leds;
  PowerManager power;
  
  // Sensor data buffer
  SensorData latestSensorData;
  
  // Motion data buffer for Freecast mode
  SensorData motionData[MAX_MOTION_SAMPLES];
  uint8_t motionDataCount = 0;
  bool isRecordingMotion = false;
  
  // Internal helper methods
  void configurePins();
};

#endif // HARDWARE_MANAGER_H 