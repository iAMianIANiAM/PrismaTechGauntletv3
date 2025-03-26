#ifndef POSITION_DETECTOR_H
#define POSITION_DETECTOR_H

#include "../core/SystemTypes.h"
#include "../hardware/HardwareManager.h"

// Number of samples to average for position detection
#define POSITION_AVERAGE_SAMPLES 5

/**
 * @brief Implements the Dominant Axis Detection model for hand position recognition
 */
class PositionDetector {
public:
  /**
   * @brief Initialize the position detector
   * @param hardware Pointer to the hardware manager
   * @return True if initialization was successful
   */
  bool init(HardwareManager* hardware);
  
  /**
   * @brief Detect the current hand position from sensor data
   * @param sensorData Current sensor reading
   * @return PositionReading containing position and confidence
   */
  PositionReading detectPosition(const SensorData& sensorData);
  
  /**
   * @brief Get the most recent position reading
   * @return Most recent position reading
   */
  PositionReading getCurrentPosition() const;
  
#ifdef TEST_MODE
  /**
   * @brief TESTING ONLY: Calibrate the position detection thresholds
   * @note This method is only for testing, not for production use.
   *       Calibration is performed via a separate calibration utility.
   * @return True if calibration was successful
   */
  bool calibrate();
  
  /**
   * @brief TESTING ONLY: Set custom thresholds for position detection
   * @note This method is only for testing, not for production use.
   * @param threshold Axis threshold value (deprecated - thresholds now in Config.h)
   */
  void setThreshold(uint16_t threshold);
#endif // TEST_MODE

private:
  // Hardware reference
  HardwareManager* hardwareRef = nullptr;
  
  // Latest position data
  PositionReading currentPosition;
  
  // Historical data for averaging
  SensorData recentSamples[POSITION_AVERAGE_SAMPLES];
  uint8_t sampleIndex = 0;
  
  // Internal processing methods
  uint8_t determinePositionFromAxes(const SensorData& averagedData);
  uint8_t calculateConfidence(const SensorData& sensorData, uint8_t position);
  SensorData calculateAveragedData();
  
  // Dominant axis detection helpers
  bool isXDominantPositive(const SensorData& data);
  bool isXDominantNegative(const SensorData& data);
  bool isYDominantPositive(const SensorData& data);
  bool isYDominantNegative(const SensorData& data);
  bool isZDominantPositive(const SensorData& data);
  bool isZDominantNegative(const SensorData& data);
};

#endif // POSITION_DETECTOR_H 