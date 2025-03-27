#ifndef THRESHOLD_MANAGER_H
#define THRESHOLD_MANAGER_H

#include <Arduino.h>  // For Serial
#include <EEPROM.h>   // For EEPROM functions
#include <stdint.h>
#include <cstddef>  // For size_t
#include "SystemTypes.h"

/**
 * @brief Manages position detection thresholds with persistent storage
 * 
 * The ThresholdManager provides functions to load, save, and manage
 * position detection thresholds. It uses EEPROM for persistent storage
 * and provides methods for threshold validation and backup/restore.
 */
class ThresholdManager {
public:
    /**
     * @brief Initialize the threshold manager
     * 
     * @return true if initialization was successful
     * @return false if initialization failed
     */
    static bool init();

    /**
     * @brief Load thresholds from persistent storage
     * 
     * @return true if thresholds were successfully loaded
     * @return false if loading failed
     */
    static bool loadThresholds();

    /**
     * @brief Save thresholds to persistent storage
     * 
     * @return true if thresholds were successfully saved
     * @return false if saving failed
     */
    static bool saveThresholds();

    /**
     * @brief Update a specific position threshold
     * 
     * @param position Position to update (use Position enum from SystemTypes.h)
     * @param value New threshold value
     * @return true if threshold was updated
     * @return false if update failed
     */
    static bool updateThreshold(uint8_t position, float value);

    /**
     * @brief Get the threshold value for a specific position
     * 
     * @param position Position to get threshold for (use Position enum)
     * @return float Threshold value
     */
    static float getThreshold(uint8_t position);

    /**
     * @brief Backup thresholds to a JSON file
     * 
     * @param filename Name of the backup file
     * @return true if backup was successful
     * @return false if backup failed
     */
    static bool backupThresholds(const char* filename);

    /**
     * @brief Restore thresholds from a JSON file
     * 
     * @param filename Name of the backup file
     * @return true if restore was successful
     * @return false if restore failed
     */
    static bool restoreThresholds(const char* filename);

    /**
     * @brief Reset thresholds to default values
     * 
     * @return true if reset was successful
     * @return false if reset failed
     */
    static bool resetToDefaults();

    /**
     * @brief Validate current thresholds
     * 
     * @return true if all thresholds are valid
     * @return false if any threshold is invalid
     */
    static bool validateThresholds();
    
    /**
     * @brief Process any deferred operations
     * 
     * This method should be called regularly from the main loop
     * to handle operations that were deferred during initialization.
     */
    static void update();

private:
    static const uint16_t EEPROM_THRESHOLD_ADDRESS = 0;
    static const char* DEFAULT_BACKUP_FILENAME;
    static const float DEFAULT_THRESHOLDS[6];  // One per position

    static float thresholds[6];  // One per position
    static bool initialized;
    static bool needsSave;       // Flag for deferred EEPROM operations
    static uint8_t retryCount;   // Counter for operation retries

    // Private helper functions - updated to handle generic data types
    template<typename T>
    static bool writeToEEPROM(uint16_t address, const T* data, size_t size) {
        if (address + size > EEPROM.length()) {
            Serial.println("EEPROM write would exceed available space");
            return false;
        }
        
        const uint8_t* byteData = reinterpret_cast<const uint8_t*>(data);
        for (size_t i = 0; i < size; i++) {
            EEPROM.write(address + i, byteData[i]);
        }
        
        return true;
    }
    
    template<typename T>
    static bool readFromEEPROM(uint16_t address, T* data, size_t size) {
        if (address + size > EEPROM.length()) {
            Serial.println("EEPROM read would exceed available space");
            return false;
        }
        
        uint8_t* byteData = reinterpret_cast<uint8_t*>(data);
        for (size_t i = 0; i < size; i++) {
            byteData[i] = EEPROM.read(address + i);
        }
        
        return true;
    }
};

#endif // THRESHOLD_MANAGER_H 