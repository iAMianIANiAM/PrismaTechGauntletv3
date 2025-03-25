#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <stdint.h>

/**
 * @brief Power states for the system
 */
enum PowerState {
  POWER_ACTIVE,    // Full operation mode
  POWER_READY,     // Sensors active, reduced LED brightness
  POWER_LOW_POWER, // Reduced sample rate, minimal LEDs
  POWER_SLEEP      // Motion detection only, wake on movement
};

/**
 * @brief Manages power states and sleep modes
 */
class PowerManager {
public:
  /**
   * @brief Initialize the power manager
   * @return True if initialization was successful
   */
  bool init();
  
  /**
   * @brief Update power management (call periodically)
   */
  void update();
  
  /**
   * @brief Set the current power state
   * @param state PowerState to transition to
   */
  void setPowerState(PowerState state);
  
  /**
   * @brief Set low power mode
   * @param lowPower True to enter low power, false for normal power
   */
  void setLowPowerMode(bool lowPower);
  
  /**
   * @brief Get the current power state
   * @return Current PowerState
   */
  PowerState getCurrentState() const;
  
  /**
   * @brief Enter sleep mode for power conservation
   * @param durationMs How long to sleep in milliseconds, 0 for indefinite
   */
  void sleep(uint32_t durationMs = 0);
  
  /**
   * @brief Wake from sleep mode
   */
  void wake();
  
  /**
   * @brief Check if the system should enter low power mode
   * @param idleTimeMs How long the system has been idle
   * @return True if should enter low power
   */
  bool shouldEnterLowPower(uint32_t idleTimeMs) const;

private:
  // Current power state
  PowerState currentState = POWER_ACTIVE;
  
  // Configure ESP32 for the specified power state
  void configureForState(PowerState state);
  
  // Configure deep sleep wake sources
  void configureWakeSources();
};

#endif // POWER_MANAGER_H 