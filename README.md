## Diagnostic Tools

### MPU Sensor Calibration Diagnostic Tool

To help diagnose and fix issues with the MPU9250/MPU6500 sensor calibration, we've added a specialized diagnostic tool:

- **Location**: `examples/MPUCalibrationTest.cpp`
- **Purpose**: Helps identify sensor type, optimize scaling factors, and diagnose calibration issues
- **Features**:
  - Automatic sensor identification (MPU6050, MPU6500, MPU9250)
  - Scaling factor calibration with statistical analysis
  - Enhanced data logging with raw and physical unit conversion
  - Zero-value detection and diagnosis
  - Position testing with confidence metrics

To use this tool:

1. Upload the MPUCalibrationTest example to your ESP32
2. Open the serial monitor at 115200 baud
3. Follow the on-screen instructions for each test phase:
   - Scaling factor calibration
   - Sensor offset calibration
   - Position detection test
   - Continuous monitoring

The diagnostic output will help identify if the sensor should be treated as an MPU6500 rather than an MPU9250, and provide the optimal scaling factors for your specific hardware. 