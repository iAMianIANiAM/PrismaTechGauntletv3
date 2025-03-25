#ifndef I2C_SCANNER_H
#define I2C_SCANNER_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @class I2CScanner
 * @brief Utility class for scanning and testing I2C connections
 *
 * This class provides methods to scan the I2C bus for connected devices,
 * test specific device addresses, and report diagnostic information.
 */
class I2CScanner {
public:
    /**
     * @brief Initialize the I2C scanner with the given pins and frequency
     * 
     * @param sdaPin SDA pin number
     * @param sclPin SCL pin number
     * @param frequency I2C bus frequency in Hz (default: 100000 = 100kHz)
     */
    static void begin(uint8_t sdaPin, uint8_t sclPin, uint32_t frequency = 100000) {
        Wire.begin(sdaPin, sclPin);
        Wire.setClock(frequency);
        
        Serial.println("\n=== I2C Scanner Initialized ===");
        Serial.printf("SDA Pin: %d\n", sdaPin);
        Serial.printf("SCL Pin: %d\n", sclPin);
        Serial.printf("Frequency: %d Hz\n", frequency);
    }
    
    /**
     * @brief Scan the I2C bus for all connected devices
     * 
     * @return int Number of devices found
     */
    static int scanBus() {
        int deviceCount = 0;
        
        Serial.println("\n=== I2C Bus Scan ===");
        Serial.println("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
        
        for (int baseAddr = 0; baseAddr <= 0x70; baseAddr += 0x10) {
            // Print row address
            Serial.printf("0x%01X0 ", baseAddr >> 4);
            
            // Scan addresses in this row
            for (int addr = 0; addr < 16; addr++) {
                int deviceAddr = baseAddr + addr;
                
                // Skip reserved addresses
                if (deviceAddr < 0x03 || deviceAddr > 0x77) {
                    Serial.print(" . ");
                    continue;
                }
                
                Wire.beginTransmission(deviceAddr);
                byte error = Wire.endTransmission();
                
                if (error == 0) {
                    // Device found
                    Serial.printf(" %02X", deviceAddr);
                    deviceCount++;
                } else {
                    // No device
                    Serial.print(" . ");
                }
            }
            Serial.println();
        }
        
        Serial.printf("\nScan complete. Found %d device(s)\n", deviceCount);
        return deviceCount;
    }
    
    /**
     * @brief Test if a device is present at the specified address
     * 
     * @param address Device I2C address
     * @return true Device is present
     * @return false Device is not present
     */
    static bool testAddress(uint8_t address) {
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();
        
        Serial.printf("Testing I2C address 0x%02X: ", address);
        
        if (error == 0) {
            Serial.println("Device FOUND");
            return true;
        } else {
            Serial.printf("NO device (error: %d)\n", error);
            return false;
        }
    }
    
    /**
     * @brief Try to read a register value from a device
     * 
     * @param address Device I2C address
     * @param reg Register address
     * @return int Register value or -1 if failed
     */
    static int readRegister(uint8_t address, uint8_t reg) {
        Wire.beginTransmission(address);
        Wire.write(reg);
        byte error = Wire.endTransmission(false);
        
        if (error != 0) {
            Serial.printf("Failed to set register address (error: %d)\n", error);
            return -1;
        }
        
        uint8_t bytesReceived = Wire.requestFrom(address, (uint8_t)1);
        if (bytesReceived != 1) {
            Serial.printf("Failed to read register (requested 1 byte, received %d)\n", bytesReceived);
            return -1;
        }
        
        int value = Wire.read();
        Serial.printf("Read register 0x%02X from device 0x%02X: 0x%02X\n", reg, address, value);
        return value;
    }
    
    /**
     * @brief Run a diagnostic test on a MPU9250/MPU6050 sensor
     * 
     * @param address Sensor I2C address (typically 0x68 or 0x69)
     * @return true Sensor passed basic diagnostic tests
     * @return false Sensor failed tests
     */
    static bool testMPUSensor(uint8_t address) {
        Serial.printf("\n=== MPU Sensor Test (Address: 0x%02X) ===\n", address);
        
        // Check if device is present
        if (!testAddress(address)) {
            Serial.println("Test FAILED: No device found at this address");
            return false;
        }
        
        // Read WHO_AM_I register (0x75)
        int whoAmI = readRegister(address, 0x75);
        if (whoAmI < 0) {
            Serial.println("Test FAILED: Could not read WHO_AM_I register");
            return false;
        }
        
        // Check if WHO_AM_I value is valid
        if (whoAmI == 0x68 || whoAmI == 0x71 || whoAmI == 0x73 || whoAmI == 0x70) {
            Serial.printf("Test PASSED: Valid WHO_AM_I value (0x%02X)\n", whoAmI);
            
            if (whoAmI == 0x68) {
                Serial.println("Detected MPU6050 sensor");
            } else if (whoAmI == 0x71 || whoAmI == 0x73) {
                Serial.println("Detected MPU9250 sensor");
            } else if (whoAmI == 0x70) {
                Serial.println("Detected MPU variant (possibly MPU6000)");
            }
        } else {
            Serial.printf("Test FAILED: Invalid WHO_AM_I value (0x%02X)\n", whoAmI);
            Serial.println("Expected: 0x68 (MPU6050), 0x71/0x73 (MPU9250), or 0x70 (MPU6000)");
            return false;
        }
        
        // Read power management register (0x6B)
        int pwrMgmt = readRegister(address, 0x6B);
        if (pwrMgmt < 0) {
            Serial.println("Test FAILED: Could not read power management register");
            return false;
        }
        
        Serial.printf("Power management value: 0x%02X\n", pwrMgmt);
        if (pwrMgmt & 0x40) { // Sleep bit set
            Serial.println("  - Device is in SLEEP mode");
        } else {
            Serial.println("  - Device is AWAKE");
        }
        
        Serial.println("=== Test Complete ===");
        return true;
    }
};

#endif // I2C_SCANNER_H 