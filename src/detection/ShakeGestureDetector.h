#ifndef SHAKE_GESTURE_DETECTOR_H
#define SHAKE_GESTURE_DETECTOR_H

#include "../core/SystemTypes.h"
#include "../hardware/MPU9250Interface.h"

/**
 * @brief Detects shake gestures using accelerometer data
 * 
 * This class monitors accelerometer data to detect deliberate shake
 * motions based on rapid threshold crossings. It is designed to be
 * a simple, reliable way to detect when the user is shaking the device
 * as a universal cancellation gesture.
 */
class ShakeGestureDetector {
public:
  /**
   * @brief Constructor
   */
  ShakeGestureDetector();
  
  /**
   * @brief Initialize the shake detector
   * @param mpuInterface Pointer to the MPU interface for sensor access
   * @return True if initialization was successful
   */
  bool init(MPU9250Interface* mpuInterface);
  
  /**
   * @brief Update the detector state
   * 
   * This should be called regularly (in the main loop) to update
   * the internal state of the shake detector.
   */
  void update();
  
  /**
   * @brief Check if a shake gesture has been detected
   * @return True if a shake gesture was detected since the last check
   */
  bool isShakeDetected();
  
  /**
   * @brief Reset the detector state
   * 
   * Resets all internal counters and flags, useful when changing modes
   * or after handling a detected shake.
   */
  void reset();
  
  /**
   * @brief Get normalized intensity of the last detected shake
   * @return Shake intensity from 0.0 to 1.0
   */
  float getLastShakeIntensity() const { return lastShakeIntensity; }

private:
  // MPU interface for sensor access
  MPU9250Interface* mpuInterface;
  
  // Detection state
  bool wasAboveThreshold;
  uint8_t crossingCount;
  uint32_t lastCrossingTime;
  uint32_t lastShakeTime;
  bool shakeDetected;
  float lastShakeIntensity;
  
  // Helper methods
  uint32_t calculateMagnitude(const SensorData& data);
  bool isCoolingDown() const;
};

#endif // SHAKE_GESTURE_DETECTOR_H 