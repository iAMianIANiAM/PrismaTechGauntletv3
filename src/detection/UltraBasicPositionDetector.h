#ifndef ULTRA_BASIC_POSITION_DETECTOR_H
#define ULTRA_BASIC_POSITION_DETECTOR_H

#include "../core/SystemTypes.h"
#include "../hardware/HardwareManager.h"
#include "../core/Config.h"

// Number of samples to average for position detection
#define POSITION_AVERAGE_SAMPLES 3

/**
 * @brief Processed accelerometer data structure (in m/s²)
 */
struct ProcessedData {
  float accelX, accelY, accelZ;
};

/**
 * @brief Ultra Basic Position Detection system implementing simplified 
 * position detection with physical unit thresholds
 */
class UltraBasicPositionDetector {
public:
  /**
   * @brief Initialize the position detector
   * @param hardware Pointer to the hardware manager
   * @return True if initialization was successful
   */
  bool init(HardwareManager* hardware);
  
  /**
   * @brief Update the detector and get the current hand position
   * @return PositionReading containing position and timestamp
   */
  PositionReading update();
  
  /**
   * @brief Get the most recent position reading
   * @return Most recent position reading
   */
  PositionReading getCurrentPosition() const;
  
  /**
   * @brief Get the most recent processed data (for debugging)
   * @return Most recent processed acceleration data
   */
  ProcessedData getProcessedData() const;
  
  /**
   * @brief Calibrate the position detection thresholds for a specific position
   * @param position Position to calibrate (from HandPosition enum)
   * @param samples Number of samples to collect for calibration
   * @return The calibrated threshold value
   */
  float calibratePosition(uint8_t position, uint16_t samples = 50);
  
  /**
   * @brief Calibrate all positions in sequence
   * @param samplesPerPosition Number of samples to collect for each position
   * @return True if calibration was successful
   */
  bool calibrateAllPositions(uint16_t samplesPerPosition = 50);
  
  /**
   * @brief Set a threshold for a specific position
   * @param position Position to set threshold for
   * @param threshold New threshold value
   */
  void setThreshold(uint8_t position, float threshold);
  
  /**
   * @brief Get the threshold for a specific position
   * @param position Position to get threshold for
   * @return Threshold value for the specified position
   */
  float getThreshold(uint8_t position) const;

private:
  // Constants
  static constexpr float SCALING_FACTOR = 4.0f * 9.81f / 32768.0f; // For ±4g range to m/s²
  static constexpr float THRESHOLD_SCALE = 0.8f; // 80% scale for more reliable detection
  static constexpr float MIN_CONFIDENCE = 30.0f; // Minimum confidence for position detection
  
  // Hardware reference
  HardwareManager* _hardware = nullptr;
  
  // Position thresholds in m/s² with dominant axis tracking
  float _thresholds[6] = {0.0f}; // One threshold per position
  uint8_t _dominantAxes[6] = {0}; // Which axis is dominant for each position (0=X, 1=Y, 2=Z)
  
  // Current position and processed data
  PositionReading _currentPosition;
  ProcessedData _currentProcessedData;
  
  // Raw data buffer for averaging
  SensorData _sampleBuffer[POSITION_AVERAGE_SAMPLES];
  uint8_t _currentSampleIndex = 0;
  
  // Internal processing methods
  void processRawData(const SensorData& raw, ProcessedData& processed);
  PositionReading detectPosition(const ProcessedData& data);
  SensorData calculateAveragedData();
  
  // Load thresholds from defaults
  void loadDefaultThresholds();
};

#endif // ULTRA_BASIC_POSITION_DETECTOR_H 