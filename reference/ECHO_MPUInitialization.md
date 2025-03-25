# MPU Sensor Initialization Guide

## Hardware Configuration

The PrismaTech Gauntlet uses an MPU6050 6-axis (gyroscope + accelerometer) sensor connected via I2C. Here are the specific configuration details:

### Pin Assignments
- **SDA Pin**: 21 (Default ESP32 I2C data pin)
- **SCL Pin**: 22 (Default ESP32 I2C clock pin)
- **I2C Address**: 0x68 (Standard MPU6050 address)
- **I2C Clock Speed**: 100kHz (100000Hz)

## Initialization Process

### 1. Required Libraries
```cpp
#include <Wire.h>       // Arduino I2C library
#include "I2Cdev.h"     // I2C communication library
#include "MPU6050_6Axis_MotionApps20.h" // MPU6050 specific library
```

### 2. Initialization Sequence

The initialization process follows these steps:

```cpp
bool initializeMPU(uint8_t addr = 0x68, uint8_t sda = 21, uint8_t scl = 22) {
    // Configure I2C bus
    Wire.begin(sda, scl);
    Wire.setClock(100000); // 100kHz clock speed
    
    // Verify device presence
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    if (error != 0) {
        Serial.println("ERROR: MPU6050 not found on I2C bus");
        return false;
    }
    
    // Reset the device
    writeMPURegister(addr, 0x6B, 0x80); // Set reset bit in PWR_MGMT_1 register
    delay(100); // Wait for reset to complete
    
    // Wake up the device
    writeMPURegister(addr, 0x6B, 0x00); // Clear sleep bit in PWR_MGMT_1 register
    delay(100);
    
    // Verify device identity
    uint8_t whoAmI;
    readMPURegister(addr, 0x75, &whoAmI, 1); // WHO_AM_I register
    
    // Accept 0x70, 0x71, 0x73 as valid WHO_AM_I values
    if (whoAmI != 0x71 && whoAmI != 0x73 && whoAmI != 0x70) {
        Serial.print("WARNING: Unknown WHO_AM_I value: 0x");
        Serial.println(whoAmI, HEX);
        // Continue anyway for compatibility with some MPU variants
    }
    
    // Configure sampling rate (register 0x19)
    writeMPURegister(addr, 0x19, 0x07); // 1kHz / (1 + 7) = 125Hz sample rate
    
    // Configure gyroscope range (register 0x1B)
    writeMPURegister(addr, 0x1B, 0x08); // ±500 degrees/second range
    
    // Configure accelerometer range (register 0x1C)
    writeMPURegister(addr, 0x1C, 0x08); // ±4g range
    
    // Enable data ready interrupt (register 0x38)
    writeMPURegister(addr, 0x38, 0x01);
    
    Serial.println("MPU6050 initialized successfully");
    return true;
}
```

### 3. Helper Functions for Register Access

```cpp
// Write a byte to a register
void writeMPURegister(uint8_t addr, uint8_t reg, uint8_t data) {
    Wire.beginTransmission(addr);
    Wire.write(reg);   // Register address
    Wire.write(data);  // Data byte
    Wire.endTransmission(true);
}

// Read bytes from a register
void readMPURegister(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t count) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission(false);  // Send repeated start
    Wire.requestFrom(addr, count);
    
    for (uint8_t i = 0; i < count && Wire.available(); i++) {
        buffer[i] = Wire.read();
    }
}
```

## Reading Sensor Data

### 1. Data Retrieval Process

To read accelerometer and gyroscope data, follow this process:

```cpp
void readMPUData(uint8_t addr, float* accelData, float* gyroData) {
    // Start reading from the accelerometer data registers (0x3B)
    Wire.beginTransmission(addr);
    Wire.write(0x3B);  // ACCEL_XOUT_H register
    byte result = Wire.endTransmission(false);  // Keep connection active
    
    if (result != 0) {
        Serial.print("Error reading from MPU: ");
        Serial.println(result);
        return;
    }
    
    // Request 14 bytes (6 for accel, 2 for temp, 6 for gyro)
    Wire.requestFrom(addr, (uint8_t)14);
    
    if (Wire.available() == 14) {
        // Read accelerometer data (registers 0x3B-0x40)
        int16_t accelX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
        int16_t accelY = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
        int16_t accelZ = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
        
        // Skip temperature data (registers 0x41-0x42)
        Wire.read();
        Wire.read();
        
        // Read gyroscope data (registers 0x43-0x48)
        int16_t gyroX = Wire.read() << 8 | Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
        int16_t gyroY = Wire.read() << 8 | Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
        int16_t gyroZ = Wire.read() << 8 | Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
        
        // Apply scaling factors
        // For ±4g range (set with register 0x1C = 0x08)
        // Scale factor is 4g/32768 = 0.0001220703125 g/digit
        // Multiply by 9.81 m/s² to convert to proper units
        const float accelScale = 0.0001220703125f * 9.81f;
        
        // For ±500 dps range (set with register 0x1B = 0x08)
        // Scale factor is 500/32768 = 0.01526 degrees/digit
        // Convert to radians/s for consistency
        const float gyroScale = 0.01526f * (PI/180.0f);
        
        // Scale raw values to proper units
        accelData[0] = accelX * accelScale;  // X-axis, m/s²
        accelData[1] = accelY * accelScale;  // Y-axis, m/s²
        accelData[2] = accelZ * accelScale;  // Z-axis, m/s²
        
        gyroData[0] = gyroX * gyroScale;     // X-axis, rad/s
        gyroData[1] = gyroY * gyroScale;     // Y-axis, rad/s
        gyroData[2] = gyroZ * gyroScale;     // Z-axis, rad/s
    }
}
```

### 2. Understanding the Data Scale

The accelerometer and gyroscope data are scaled to provide consistent and interpretable values:

- **Accelerometer data** is scaled to be in the range of approximately -10 to 10 m/s² when the sensor is at rest, with Earth's gravity causing approximately +/-9.81 m/s² on the axis aligned with gravity.

- **Gyroscope data** is scaled to radians per second, which provides better compatibility with most physics calculations.

### 3. Typical Serial Monitor Output

When properly configured, your serial monitor output for sensor data should look like:

```
Raw Accel: X=-0.23 Y=0.84 Z=9.67
```

When the device is at rest on a flat surface, you should see:
- X and Y values close to 0 (with some noise)
- Z value close to +9.81 (Earth's gravity)

## Integration Into Main Loop

Here's how to integrate the MPU sensor into your main program loop:

```cpp
void setup() {
    Serial.begin(115200);
    delay(1000);  // Give serial a moment to start
    
    Serial.println("Initializing MPU sensor...");
    if (!initializeMPU(0x68, 21, 22)) {
        Serial.println("ERROR: Failed to initialize MPU sensor!");
    } else {
        Serial.println("MPU sensor initialized successfully");
    }
}

void loop() {
    float accelData[3]; // [x, y, z]
    float gyroData[3];  // [x, y, z]
    
    readMPUData(0x68, accelData, gyroData);
    
    // Print values periodically (e.g., every second)
    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastPrintTime > 1000) {
        Serial.print("Accel: X=");
        Serial.print(accelData[0], 2);
        Serial.print(" Y=");
        Serial.print(accelData[1], 2);
        Serial.print(" Z=");
        Serial.print(accelData[2], 2);
        
        Serial.print(" | Gyro: X=");
        Serial.print(gyroData[0], 2);
        Serial.print(" Y=");
        Serial.print(gyroData[1], 2);
        Serial.print(" Z=");
        Serial.println(gyroData[2], 2);
        
        lastPrintTime = currentTime;
    }
    
    delay(10); // Small delay for stability
}
```

## Common Issues and Troubleshooting

1. **No sensor detected on I2C bus**
   - Verify power connections (3.3V and GND)
   - Check I2C pins (SDA and SCL) are correctly connected
   - Verify pull-up resistors are present (4.7kΩ typically)
   - Try reducing I2C clock speed (e.g., to 50kHz)

2. **Incorrect WHO_AM_I value**
   - This may indicate a different MPU variant is being used
   - If basic functionality works, this can often be ignored

3. **Noisy or drifting readings**
   - At rest, there will always be some noise in readings
   - For high-precision applications, implement a digital filter (e.g., low-pass or complementary)
   - Calibration routines can help offset any constant bias

4. **Scale factor issues**
   - If values seem too large or small, double-check the scale factors
   - Verify the configuration registers are set correctly for desired ranges

## Conclusion

This guide provides a comprehensive approach to initializing and reading data from the MPU6050 sensor using the PrismaTech Gauntlet hardware configuration. By following these steps, you can reliably integrate MPU sensor functionality into your ESP32 projects while ensuring proper data scaling and interpretation. 