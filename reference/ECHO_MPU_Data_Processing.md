# MPU Sensor Data Processing and Normalization

This document provides a comprehensive breakdown of how the PrismaTech Gauntlet processes and normalizes raw data from the MPU6050 sensor, transforming it into the meaningful values used for gesture detection and comparison against thresholds.

## Data Flow Overview

```
Raw Sensor Data → Binary Conversion → Physical Unit Scaling → Application-Specific Values → Threshold Comparison
```

## Stage 1: Raw Data Acquisition

### Hardware Configuration
- MPU6050 configured for:
  - ±4g accelerometer range (register 0x1C set to 0x08)
  - ±500 degrees/second gyroscope range (register 0x1B set to 0x08)
  - 125Hz sampling rate (register 0x19 set to 0x07)

### Register Reading
The device reads 14 bytes from the MPU6050's registers starting at 0x3B (ACCEL_XOUT_H):

```cpp
Wire.beginTransmission(MPU_ADDR);
Wire.write(ACCEL_XOUT_H);  // Starting register (0x3B)
Wire.endTransmission(false);
Wire.requestFrom(MPU_ADDR, 14);
```

## Stage 2: Binary Data Conversion

The raw bytes from the sensor are combined to form 16-bit signed integers:

```cpp
// Read accelerometer data (6 bytes)
int16_t accelX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
int16_t accelY = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
int16_t accelZ = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

// Skip temperature (2 bytes)
Wire.read();
Wire.read();

// Read gyroscope data (6 bytes)
int16_t gyroX = Wire.read() << 8 | Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
int16_t gyroY = Wire.read() << 8 | Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
int16_t gyroZ = Wire.read() << 8 | Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
```

These raw values are in the range of -32,768 to +32,767 (signed 16-bit integer).

## Stage 3: Scaling to Physical Units

The raw integer values are scaled to represent physical quantities:

### Accelerometer Scaling
For the ±4g range, the scaling calculation is:

```cpp
// Scale factor is 4g/32768 = 0.0001220703125 g/digit
// Multiply by 9.81 to convert to m/s²
const float accelScale = 0.0001220703125f * 9.81f;

accelData[0] = accelX * accelScale;  // X-axis in m/s²
accelData[1] = accelY * accelScale;  // Y-axis in m/s²
accelData[2] = accelZ * accelScale;  // Z-axis in m/s²
```

This gives accelerometer values in the range of approximately -10 to +10 m/s².

### Gyroscope Scaling
For the ±500 degrees/second range, the scaling calculation is:

```cpp
// Scale factor is 500/32768 = 0.01526 degrees/digit
// Convert to radians/second for consistency
const float gyroScale = 0.01526f * (PI/180.0f);

gyroData[0] = gyroX * gyroScale;  // X-axis in radians/second
gyroData[1] = gyroY * gyroScale;  // Y-axis in radians/second
gyroData[2] = gyroZ * gyroScale;  // Z-axis in radians/second
```

## Stage 4: Normalization Considerations

### Accelerometer at Rest
When the device is stationary and flat:
- X-axis and Y-axis should read close to 0 m/s²
- Z-axis should read approximately +9.81 m/s² (Earth's gravity)

### Important Scaling Note
As documented in Config.h:
```
* IMPORTANT: All threshold values in this file use the -10 to 10 scale,
* which matches what users see in the PuTTY serial debugging output.
* 
* The accelerometer data is already in this -10 to 10 range when received from
* MPUSensor.cpp, so NO additional scaling is needed when comparing values to 
* these thresholds in the detection code.
```

This is a critical design decision: values are pre-scaled to a standard range, making threshold comparisons intuitive.

## Stage 5: Threshold Comparisons and Detection Logic

The normalized data is used directly in detection functions:

### Palm Position Detection
```cpp
bool IdleMode::isPalmDown(float z) const {
    return z < Config::PALM_DOWN_THRESHOLD;  // Threshold = -7.0
}

bool IdleMode::isPalmUp(float z) const {
    return z > Config::PALM_UP_THRESHOLD;    // Threshold = +7.0
}
```

### Hand Position Detection
The hand position detector uses the normalized accelerometer data to determine which hand position is being held:

```cpp
// If dominant axis is X-axis with negative value (< -6.0)
// and other axes are below secondary threshold (< 4.0)
if (accelData[0] < -DOMINANT_AXIS_THRESHOLD &&
    abs(accelData[1]) < SECONDARY_AXIS_THRESHOLD && 
    abs(accelData[2]) < SECONDARY_AXIS_THRESHOLD) {
    return HandPosition::POSITION_OFFER;
}
```

## Visibility in Serial Monitor

The normalized data is displayed in the serial monitor every second:
```
Raw Accel: X=-0.23 Y=0.84 Z=9.67
```

This provides real-time feedback on the sensor values in their final normalized form.

## Scaling History Note

There was previously an issue with double-scaling:
```
* Previously, we incorrectly assumed a -1 to 1 range for raw values that needed
* a 10x scaling factor, but this was causing issues because the values were already
* at the correct scale, resulting in unwanted 100x effective scaling.
```

The current implementation ensures consistent scaling throughout the code with no redundant multipliers.

## Typical Value Ranges

| Situation | X Value | Y Value | Z Value | Interpretation |
|-----------|---------|---------|---------|----------------|
| Device flat on table | ~0 | ~0 | ~9.81 | Normal gravity on Z axis |
| Palm down | varies | varies | < -7.0 | Z acceleration strongly negative |
| Palm up | varies | varies | > +7.0 | Z acceleration strongly positive |
| Shield position | < 4.0 | < 4.0 | 5.0 to 8.0 | Z in specific range, X/Y subdued |

## Conclusion

The MPU6050 sensor data processing in the PrismaTech Gauntlet follows a well-defined pipeline that converts raw 16-bit integer values into physically meaningful measurements scaled to an intuitive -10 to +10 range. This consistent scaling simplifies gesture recognition logic and threshold comparisons throughout the codebase. 