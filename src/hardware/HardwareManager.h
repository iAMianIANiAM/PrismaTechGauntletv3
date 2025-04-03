#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "../core/SystemTypes.h"
#include "MPU9250Interface.h"
#include "LEDInterface.h"
#include "PowerManager.h"
#include "../detection/ShakeGestureDetector.h"

// Maximum number of motion samples to store
#define MAX_MOTION_SAMPLES 100

/**
 * @brief Hardware component enumeration for reset and self-test functions
 */
enum HardwareComponent {
  HW_COMPONENT_MPU,
  HW_COMPONENT_LED,
  HW_COMPONENT_ALL
};

/**
 * @brief Manages all hardware interfaces and abstracts hardware access
 */
class HardwareManager {
public:
  /**
   * @brief Get the singleton instance of HardwareManager
   * @return Pointer to the HardwareManager instance
   */
  static HardwareManager* getInstance();

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
   * @brief Set LED brightness
   * @param brightness Brightness value (0-255)
   */
  void setBrightness(uint8_t brightness);
  
  /**
   * @brief Start recording motion data for Freecast mode
   */
  void recordMotionData();
  
  /**
   * @brief Stop recording motion data
   */
  void stopRecordingMotion();
  
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
  
  /**
   * @brief Get the current power state
   * @return True if in active power state, false if in low power state
   */
  bool getPowerState() const;
  
  /**
   * @brief Reset a specific hardware component
   * @param component The component to reset
   * @return True if reset was successful
   */
  bool resetComponent(HardwareComponent component);
  
  /**
   * @brief Run a self-test on hardware components
   * @param component The component to test
   * @return True if test passed, false if failed
   */
  bool runSelfTest(HardwareComponent component);
  
  /**
   * @brief Get the ShakeGestureDetector instance
   * @return Pointer to the ShakeGestureDetector
   */
  ShakeGestureDetector* getShakeDetector() { return &shakeDetector; }
  
  /**
   * @brief Get the LEDInterface instance
   * @return Pointer to the LEDInterface
   */
  LEDInterface* getLEDInterface() { return &leds; }

private:
  // Private constructor for singleton pattern
  HardwareManager();
  
  // Hardware component instances
  MPU9250Interface imu;
  LEDInterface leds;
  PowerManager power;
  ShakeGestureDetector shakeDetector;
  
  // Sensor data buffer
  SensorData latestSensorData;
  
  // Motion data buffer for Freecast mode
  SensorData motionData[MAX_MOTION_SAMPLES];
  uint8_t motionDataCount;
  bool isRecordingMotion;
  
  // Timing variables
  unsigned long lastSensorUpdateTime;
  unsigned long lastLedUpdateTime;
  
  // State tracking
  bool isInitialized;
  bool isActive;
  
  // Internal helper methods
  void configurePins();
};

#endif // HARDWARE_MANAGER_H 