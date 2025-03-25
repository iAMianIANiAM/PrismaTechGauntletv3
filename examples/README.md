# PrismaTech Gauntlet 3.0 Examples

This directory contains example applications that demonstrate specific functionality of the PrismaTech Gauntlet 3.0 system.

## MPU Sensor Diagnostic Test

The `MPUDiagnosticTest.cpp` file provides a comprehensive diagnostic application for the MPU9250/MPU6050 sensor. This example performs the following:

1. Initializes I2C communication on the configured pins
2. Scans the entire I2C bus for connected devices
3. Tests both common MPU addresses (0x68 and 0x69)
4. Performs a complete initialization of the detected sensor
5. Runs the diagnostic test suite
6. Calibrates the sensor
7. Continuously reads and displays sensor data

### How to Use

To use this example instead of the main application:

1. Temporarily rename `src/main.cpp` to something else (e.g., `src/main.cpp.bak`)
2. Copy this example to `src/main.cpp`
3. Compile and upload the project:
   ```
   pio run -t upload
   ```
4. Open the serial monitor to view diagnostic output:
   ```
   pio device monitor
   ```
5. Restore the original main file when done

### Expected Output

If everything is working correctly, you should see output similar to:

```
=== I2C Scanner Initialized ===
SDA Pin: 21
SCL Pin: 22
Frequency: 100000 Hz

=== I2C Bus Scan ===
     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
0x00  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
0x10  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
0x20  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
0x30  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
0x40  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
0x50  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
0x60  .  .  .  .  .  .  .  .  68  .  .  .  .  .  .  .
0x70  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

Scan complete. Found 1 device(s)

Testing I2C address 0x68: Device FOUND

=== MPU Sensor Diagnostics ===
Test 1: I2C Communication
PASS: Device responding on I2C bus
Test 2: Device Identity
WHO_AM_I = 0x68: PASS: Valid device ID
Test 3: Power Management
PWR_MGMT_1 = 0x01: PASS: Device is awake
Test 4: Data Reading
PASS: Successfully read sensor data
Accel: X=-123 Y=45 Z=8192
Gyro: X=5 Y=-3 Z=11
=== Diagnostics Complete ===

Calibration complete
Accel offsets: X=-124 Y=47 Z=0
Gyro offsets: X=5 Y=-3 Z=10

MPU Data: Accel(X=     0 Y=     0 Z=  8192) | Gyro(X=     0 Y=     0 Z=     0)
```

### Troubleshooting Tips

1. **No devices found**: Check the I2C wiring connections, verify SDA/SCL pins, and ensure the sensor is powered properly.
2. **Incorrect WHO_AM_I value**: If you see an unexpected WHO_AM_I value, this may indicate a sensor variant or a communication issue.
3. **Failed tests**: Check the specific test that failed and the detailed error message provided.
4. **Erratic readings**: Ensure the sensor is mounted securely and not subject to vibration or interference. 