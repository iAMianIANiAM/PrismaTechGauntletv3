#ifndef CALIBRATION_ROUTINE_H
#define CALIBRATION_ROUTINE_H

#include "../hardware/HardwareManager.h"
#include "../hardware/LEDInterface.h"

/**
 * @brief Calibration state in the process
 */
enum CalibrationState {
  CAL_IDLE,             // Not calibrating
  CAL_STARTUP,          // Initial instructions
  CAL_POSITION_OFFER,   // Calibrating Offer position
  CAL_POSITION_CALM,    // Calibrating Calm position
  CAL_POSITION_OATH,    // Calibrating Oath position
  CAL_POSITION_DIG,     // Calibrating Dig position
  CAL_POSITION_SHIELD,  // Calibrating Shield position
  CAL_POSITION_NULL,    // Calibrating Null position
  CAL_PROCESSING,       // Processing calibration data
  CAL_COMPLETE,         // Calibration complete
  CAL_FAILED            // Calibration failed
};

/**
 * @brief Handles sensor calibration process
 */
class CalibrationRoutine {
public:
  /**
   * @brief Initialize the calibration routine
   * @param hardware Pointer to hardware manager
   * @return True if initialization successful
   */
  bool init(HardwareManager* hardware);
  
  /**
   * @brief Start the calibration process
   */
  void startCalibration();
  
  /**
   * @brief Update the calibration process
   * @return True if calibration is still in progress
   */
  bool update();
  
  /**
   * @brief Get the current calibration state
   * @return Current CalibrationState
   */
  CalibrationState getState() const;
  
  /**
   * @brief Check if calibration was successful
   * @return True if calibration completed successfully
   */
  bool isCalibrated() const;
  
  /**
   * @brief Cancel the calibration process
   */
  void cancel();

private:
  // Hardware reference
  HardwareManager* hardwareRef = nullptr;
  
  // Calibration state
  CalibrationState state = CAL_IDLE;
  bool calibrated = false;
  
  // Timing
  unsigned long stateStartTime = 0;
  unsigned long timePerPosition = 3000; // 3 seconds per position
  
  // Calibration data
  SensorData positionSamples[6][10]; // 10 samples per position
  uint8_t sampleCount[6] = {0};
  
  // Internal methods
  void advanceToNextState();
  void processCalibrationData();
  void showCalibrationPrompt();
  void collectSample();
  void saveCalibrationToFlash();
};

#endif // CALIBRATION_ROUTINE_H 