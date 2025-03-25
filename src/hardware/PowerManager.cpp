#include "PowerManager.h"
#include "../core/Config.h"
#include "../utils/DebugTools.h"
#include <Arduino.h>
#include <esp_sleep.h>
#include <esp_wifi.h>

/**
 * @brief Initialize the power manager
 * @return True if initialization was successful
 */
bool PowerManager::init() {
    DEBUG_PRINTLN("Initializing PowerManager");
    
    // Start in active mode
    currentState = POWER_ACTIVE;
    
    // Configure wake sources
    configureWakeSources();
    
    return true;
}

/**
 * @brief Update power management (call periodically)
 */
void PowerManager::update() {
    // Placeholder for periodic power management tasks
    // Could implement idle detection and automatic power mode transitions
}

/**
 * @brief Set the current power state
 * @param state PowerState to transition to
 */
void PowerManager::setPowerState(PowerState state) {
    if (state == currentState) {
        return;  // Already in this state
    }
    
    DEBUG_PRINTLN("PowerManager: Changing state");
    
    // Convert state to descriptive string for debug output
    const char* stateStr = "";
    switch (currentState) {
        case POWER_ACTIVE: stateStr = "ACTIVE"; break;
        case POWER_READY: stateStr = "READY"; break;
        case POWER_LOW_POWER: stateStr = "LOW_POWER"; break;
        case POWER_SLEEP: stateStr = "SLEEP"; break;
    }
    DEBUG_PRINT("From: ");
    DEBUG_PRINTLN(stateStr);
    
    switch (state) {
        case POWER_ACTIVE: stateStr = "ACTIVE"; break;
        case POWER_READY: stateStr = "READY"; break;
        case POWER_LOW_POWER: stateStr = "LOW_POWER"; break;
        case POWER_SLEEP: stateStr = "SLEEP"; break;
    }
    DEBUG_PRINT("To: ");
    DEBUG_PRINTLN(stateStr);
    
    currentState = state;
    configureForState(state);
}

/**
 * @brief Set low power mode
 * @param lowPower True to enter low power, false for normal power
 */
void PowerManager::setLowPowerMode(bool lowPower) {
    if (lowPower) {
        setPowerState(POWER_LOW_POWER);
    } else {
        setPowerState(POWER_ACTIVE);
    }
}

/**
 * @brief Get the current power state
 * @return Current PowerState
 */
PowerState PowerManager::getCurrentState() const {
    return currentState;
}

/**
 * @brief Enter sleep mode for power conservation
 * @param durationMs How long to sleep in milliseconds, 0 for indefinite
 */
void PowerManager::sleep(uint32_t durationMs) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Entering sleep for %lu ms", (unsigned long)durationMs);
    DEBUG_PRINTLN(buffer);
    
    // Save state before sleeping
    setPowerState(POWER_SLEEP);
    
    // Flush any pending serial output
    Serial.flush();
    
    if (durationMs > 0) {
        // Set wakeup timer if duration is specified
        esp_sleep_enable_timer_wakeup(durationMs * 1000ULL); // Convert to microseconds
    }
    
    // Enter deep sleep
    esp_deep_sleep_start();
    
    // Execution resumes from setup() after waking up
}

/**
 * @brief Wake from sleep mode
 */
void PowerManager::wake() {
    DEBUG_PRINTLN("PowerManager: Waking from sleep");
    
    // Restore active state
    setPowerState(POWER_ACTIVE);
}

/**
 * @brief Check if the system should enter low power mode
 * @param idleTimeMs How long the system has been idle
 * @return True if should enter low power
 */
bool PowerManager::shouldEnterLowPower(uint32_t idleTimeMs) const {
    // Enter low power mode after 30 seconds of inactivity by default
    return (idleTimeMs > 30000);
}

/**
 * @brief Configure ESP32 for the specified power state
 * @param state PowerState to configure for
 */
void PowerManager::configureForState(PowerState state) {
    switch (state) {
        case POWER_ACTIVE:
            // Full power mode
            setCpuFrequencyMhz(240);  // Full CPU speed
            
            // Turn on WiFi if needed
            // esp_wifi_start();
            break;
            
        case POWER_READY:
            // Ready mode - reduced power but still responsive
            setCpuFrequencyMhz(160);  // Reduced CPU speed
            break;
            
        case POWER_LOW_POWER:
            // Low power mode - minimum power while still running
            setCpuFrequencyMhz(80);   // Minimum CPU speed
            
            // Turn off WiFi if enabled
            // esp_wifi_stop();
            break;
            
        case POWER_SLEEP:
            // Sleep mode - configure for sleep
            // Note: actual sleep happens in sleep() method
            break;
    }
}

/**
 * @brief Configure deep sleep wake sources
 */
void PowerManager::configureWakeSources() {
    // Enable wakeup on external pin (e.g., motion detection)
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1); // Example: GPIO33, high level

    // Enable wakeup on timer (backup) - 1 hour max sleep
    // Use unsigned long long (ULL) to avoid integer overflow
    esp_sleep_enable_timer_wakeup(3600ULL * 1000ULL * 1000ULL); // 1 hour in microseconds
} 