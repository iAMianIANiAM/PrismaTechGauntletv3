#include "ThresholdManager.h"
#include "Config.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>

// Initialize static variables
bool ThresholdManager::initialized = false;
bool ThresholdManager::needsSave = false;
uint8_t ThresholdManager::retryCount = 0;
float ThresholdManager::thresholds[6] = {0.0f};
const char* ThresholdManager::DEFAULT_BACKUP_FILENAME = "/thresholds.json";

// Default thresholds from Config.h
const float ThresholdManager::DEFAULT_THRESHOLDS[6] = {
    Config::THRESHOLD_NULL,
    Config::THRESHOLD_SHIELD,
    Config::THRESHOLD_OFFER,
    Config::THRESHOLD_OATH,
    Config::THRESHOLD_DIG,
    Config::THRESHOLD_CALM
};

bool ThresholdManager::init() {
    if (initialized) return true;
    
    // Initialize EEPROM
    EEPROM.begin(512);
    delay(10); // Allow EEPROM to stabilize
    
    // Initialize SPIFFS separately
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS initialization failed, continuing with defaults");
        // Don't return false here, continue with defaults
    }
    delay(10); // Ensure separation between flash operations
    
    // Set initialized flag BEFORE loading thresholds
    initialized = true;
    
    // Try to load thresholds
    bool loadSuccess = loadThresholds();
    if (!loadSuccess) {
        Serial.println("Failed to load thresholds, using defaults");
        // Set default values
        for (int i = 0; i < 6; i++) {
            thresholds[i] = DEFAULT_THRESHOLDS[i];
        }
        
        // Schedule save for later rather than doing it immediately
        needsSave = true;
    }
    
    return true; // Return success regardless of load operation
}

void ThresholdManager::update() {
    if (!initialized) return;
    
    // Check if we need to save thresholds
    if (needsSave) {
        bool success = saveThresholds();
        if (success) {
            needsSave = false;
            retryCount = 0;
            Serial.println("Deferred threshold save completed successfully");
        } else {
            retryCount++;
            if (retryCount >= 3) {
                Serial.println("Maximum retries reached for threshold save, giving up");
                needsSave = false;
                retryCount = 0;
            }
        }
    }
}

bool ThresholdManager::loadThresholds() {
    if (!initialized) {
        return false; // Cannot load if not initialized
    }
    
    // Read magic number with error handling
    uint32_t magicNumber = 0;
    if (!readFromEEPROM(EEPROM_THRESHOLD_ADDRESS, &magicNumber, sizeof(magicNumber))) {
        return false;
    }
    
    if (magicNumber != 0x50544854) {  // 'PTHT' in hex
        Serial.println("No valid thresholds found in EEPROM");
        return false;
    }
    
    // Read thresholds with safe temporary buffer
    float tempThresholds[6] = {0};
    bool success = readFromEEPROM(EEPROM_THRESHOLD_ADDRESS + sizeof(uint32_t), 
                                 tempThresholds, sizeof(tempThresholds));
    
    // Only update main thresholds if read was successful
    if (success) {
        memcpy(thresholds, tempThresholds, sizeof(thresholds));
    }
    
    return success;
}

bool ThresholdManager::saveThresholds() {
    if (!initialized) return false;
    
    // Write magic number to identify valid thresholds
    uint32_t magicNumber = 0x50544854;  // 'PTHT' in hex
    EEPROM.put(EEPROM_THRESHOLD_ADDRESS, magicNumber);
    
    // Write thresholds
    bool success = writeToEEPROM(EEPROM_THRESHOLD_ADDRESS + sizeof(uint32_t), thresholds, sizeof(thresholds));
    
    if (success) {
        // Commit changes to EEPROM
        success = EEPROM.commit();
    }
    
    return success;
}

bool ThresholdManager::updateThreshold(uint8_t position, float value) {
    if (!initialized) init();
    
    if (position >= 6) {
        Serial.println("Invalid position for threshold update");
        return false;
    }
    
    thresholds[position] = value;
    // Schedule the save for later instead of doing it immediately
    needsSave = true;
    return true;
}

float ThresholdManager::getThreshold(uint8_t position) {
    if (!initialized) init();
    
    if (position >= 6) {
        Serial.println("Invalid position for threshold retrieval");
        return 0.0f;
    }
    
    return thresholds[position];
}

bool ThresholdManager::backupThresholds(const char* filename) {
    if (!initialized) return false;
    
    const char* backupFile = (filename == nullptr) ? DEFAULT_BACKUP_FILENAME : filename;
    
    File file = SPIFFS.open(backupFile, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    
    // Create JSON document on heap instead of stack
    DynamicJsonDocument doc(1024);
    
    // Add timestamp
    doc["timestamp"] = millis();
    
    // Add thresholds
    JsonArray thresholdArray = doc.createNestedArray("thresholds");
    for (int i = 0; i < 6; i++) {
        thresholdArray.add(thresholds[i]);
    }
    
    // Serialize JSON to file
    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write to file");
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

bool ThresholdManager::restoreThresholds(const char* filename) {
    if (!initialized) return false;
    
    const char* backupFile = (filename == nullptr) ? DEFAULT_BACKUP_FILENAME : filename;
    
    if (!SPIFFS.exists(backupFile)) {
        Serial.println("Backup file does not exist");
        return false;
    }
    
    File file = SPIFFS.open(backupFile, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return false;
    }
    
    // Parse JSON - use heap instead of stack
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        return false;
    }
    
    // Extract thresholds to temporary buffer first
    float tempThresholds[6] = {0};
    JsonArray thresholdArray = doc["thresholds"];
    
    if (thresholdArray.size() != 6) {
        Serial.println("Invalid threshold count in backup file");
        return false;
    }
    
    for (int i = 0; i < 6; i++) {
        tempThresholds[i] = thresholdArray[i];
    }
    
    // Validate before copying
    for (int i = 0; i < 6; i++) {
        if (abs(tempThresholds[i]) > 30000.0f) {
            Serial.println("Invalid threshold value in backup file");
            return false;
        }
    }
    
    // Copy to real thresholds
    memcpy(thresholds, tempThresholds, sizeof(thresholds));
    
    // Schedule save for later rather than doing it immediately
    needsSave = true;
    return true;
}

bool ThresholdManager::resetToDefaults() {
    if (!initialized) init();
    
    for (int i = 0; i < 6; i++) {
        thresholds[i] = DEFAULT_THRESHOLDS[i];
    }
    
    // Schedule save for later rather than doing it immediately
    needsSave = true;
    return true;
}

bool ThresholdManager::validateThresholds() {
    if (!initialized) init();
    
    // Check if thresholds are within expected ranges
    // This is application-specific validation
    
    // For example, we can check that values are not zero or extremely large
    for (int i = 0; i < 6; i++) {
        // Check for unreasonable values based on our -10 to 10 expected scale
        if (abs(thresholds[i]) > 30000.0f || thresholds[i] == 0.0f) {
            return false;
        }
    }
    
    // Additional validation can be added here
    
    return true;
} 