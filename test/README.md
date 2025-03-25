# PrismaTech Gauntlet 3.0 - Test Directory

This directory contains structured test files for the different components of the PrismaTech Gauntlet 3.0 project.

## Directory Structure

```
test/
├── mpu/                    - MPU9250 sensor test files
│   ├── MPU9250Test.cpp     - Test functions for the MPU sensor
│   └── MPU9250TestMain.cpp - Main entry point for MPU tests
├── led/                    - LED interface test files (future)
└── helpers/                - Utility files for testing
    └── dummy.cpp           - Arduino framework entry point helper
```

## Test Environments

The project includes several test environments defined in `platformio.ini`:

1. **[env:ledtest]** - Tests the LED interface implementation
   - Command: `pio run -e ledtest -t upload`

2. **[env:mpudiag]** - Runs the MPU diagnostic test from the examples directory
   - Command: `pio run -e mpudiag -t upload`

3. **[env:mputest]** - Runs the MPU unit tests from the test directory
   - Command: `pio run -e mputest -t upload`

## Running Tests

To run a test environment:

1. Connect the ESP32 to your computer
2. Select the appropriate test environment
3. Use PlatformIO to build and upload the test firmware
4. Monitor the serial output to see test results

Example:
```
pio run -e mputest -t upload
pio device monitor
```

## Adding New Tests

When adding new tests:

1. Create a specific subdirectory for the component being tested
2. Add both the test implementation and a main entry point file
3. Update platformio.ini with a new environment if needed
4. Document the new test in this README 