# PrismaTech Gauntlet 3.0 - Development Chronicle (v4)

> *Current Date: 2025-03-29*
>
> *This chronicle serves as the primary running journal of development activity and historical record for the PrismaTech Gauntlet 3.0 project. It contains ideas, test results, trial/error logs, decisions, and development plans.*

## 📋 Migration Summary (2025-03-29)

### Key Completed Components
- [VERIFIED] MPU9250 Sensor Initialization (followed ECHO reference implementation)
- [VERIFIED] I2C Communication with MPU9250
- [VERIFIED] Hardware detection of MPU sensor
- [IMPLEMENTED] Enhanced error recovery mechanisms for I2C communications

### Active Work Streams
- [IN PROGRESS] Sensor calibration and scaling correction
- [IN PROGRESS] Position detection confidence calculation
- [PLANNED] Integration with UBPD
- [PLANNED] Mode transition testing

### Critical Decisions
- Refactored MPU implementation to follow ECHO reference [v3:L850-L880]
- Confirmed issue is in calibration script data handling, not hardware [v3:L886-L890]

### Known Issues
- Sensor values not scaling correctly in detection mode [v3:L892-L893]
- Calibration script reports zero values despite functioning sensor [v3:L888-L890]

### Primary Reference Links
- [Chronicle v1](../archive/chronicles/chronicle_v1.md)
- [Chronicle v2](../archive/chronicles/chronicle_v2.md)
- [Chronicle v3](../archive/chronicles/chronicle_v3.md)
- [TrueFunctionGuide](../reference/TrueFunctionGuide.md)
- [ECHO_MPUInitialization](../reference/ECHO_MPUInitialization.md)

## Current MPU Sensor Status (2025-03-29)

🧠 INSIGHT: We have confirmed that the MPU sensor is working correctly in terms of hardware connection and initialization. The sensor responds to I2C communications and returns values in "Detection" mode. The data from the sensor shows responsiveness to movement, which confirms basic functionality.

⚠️ ISSUE: While the sensor is functional, the values it reports do not appear to be scaled correctly. This suggests an issue in either:
1. The unit conversion from raw sensor data to physical units (m/s²)
2. The calibration parameters being used to interpret the raw data
3. The actual data processing pipeline in the detection system

Our next steps will focus on analyzing and correcting the scaling issues to ensure accurate position detection.

## Investigation Plan for Scaling Issue

We will investigate the scaling issue through the following steps:

1. Compare the raw sensor values against expected ranges for the MPU9250
2. Verify the unit conversion calculations in the data processing pipeline
3. Trace the data flow from raw sensor readings to final position detection
4. Check for any post-processing applied to the data that might affect scaling
5. Review the calibration script to identify why it shows zeros despite functioning sensor
6. Test alternative calibration approaches if necessary

This investigation will help us determine whether the issue lies in hardware configuration, data processing, or calibration parameter handling. 

## 📓 Sensor Calibration Investigation & Repair Proposal (2025-03-29)

### 🧠 INSIGHT: MPU6500 Characteristics vs MPU9250 Configuration

Based on both user guidance and our codebase investigation, the sensor we're working with may need to be treated as an MPU6500 rather than an MPU9250 for proper operation. This is a common issue with these sensors, as they share hardware characteristics but have different WHO_AM_I register values and slight variations in initialization sequences. Our current implementation accepts multiple WHO_AM_I values (0x68, 0x71, 0x73, 0x70) but may need additional adjustments to fully support the MPU6500.

### Investigation Plan

I propose a systematic investigation of the calibration issue with the following steps:

1. **Sensor Identity Verification**
   - Log the actual WHO_AM_I register value returned by our sensor
   - Compare against expected values for both MPU9250 (0x71, 0x73) and MPU6500 (0x70) variants
   - Update initialization logic if needed to properly identify the specific sensor

2. **Scaling Factor Verification**
   - Review and verify the SCALING_FACTOR in UltraBasicPositionDetector (currently `4.0f * 9.81f / 32768.0f`)
   - Compare with reference implementation scaling for MPU6500/9250 (should be ~8192 LSB per g for ±4g range)
   - Test different scaling factors based on the sensor's specific characteristics

3. **Calibration Procedure Analysis**
   - Implement detailed logging during calibration to track raw vs. processed values
   - Verify that gravity is being properly accounted for in the Z-axis calibration
   - Test whether the calibration is properly capturing and storing offset values

4. **Raw Data Collection & Analysis**
   - Create a test mode that outputs both raw and calibrated values in real-time
   - Collect data for each position to identify any patterns in the raw readings
   - Graph the relationship between raw sensor values and expected physical units

5. **Zero-Value Investigation**
   - Specifically address the zero values reported in the calibration script
   - Check for potential timing issues or race conditions in the data collection
   - Verify that I2C communication is stable during the calibration process

6. **MPU6500-Specific Configuration**
   - Implement specific configuration parameters for MPU6500 if the sensor is correctly identified
   - Test different register settings that may be optimal for the specific sensor variant
   - Update the sensor interface to handle chip-specific variations

7. **Data Processing Pipeline Review**
   - Trace the complete data flow from raw sensor readings through calibration to position detection
   - Verify each transformation step using known test vectors
   - Identify any inconsistencies in the handling of calibration parameters

### Implementation Approach

We will implement this investigation in phases:

**Phase 1: Sensor Identity & Configuration**
- Enhance sensor debugging output to clearly identify the connected device
- Implement MPU6500-specific initialization if needed
- Test different WHO_AM_I validation approaches

**Phase 2: Calibration Debugging**
- Add enhanced logging throughout the calibration process
- Visualize raw vs. processed data during calibration
- Implement alternative gravity compensation approaches

**Phase 3: Scaling Correction**
- Test different scaling factors based on findings
- Compare with reference implementations from libraries like I2Cdevlib
- Implement sensor-specific scaling factors

**Phase 4: Recovery & Fallback Options**
- If direct fixes are unsuccessful, implement a versioned rollback to a known working calibration
- Isolate and extract the functional calibration logic from previous commits
- Integrate this proven calibration approach with current codebase

### Expected Outcomes

1. Identification of the specific cause of zero values in calibration
2. Correct scaling factors for our specific sensor hardware
3. Proper handling of MPU6500 vs MPU9250 differences
4. Working calibration procedure with reliable non-zero values
5. Documentation of sensor-specific behavior for future reference

This comprehensive approach should address all potential issues while providing multiple fallback options if direct fixes are unsuccessful. 

## 🛠️ Implementation: Phase 1 - Sensor Identity & Scaling (2025-03-29)

### ✅ RESOLUTION: Enhanced MPU Sensor Calibration

The initial phase of our investigation plan has been implemented with the following enhancements:

1. **Enhanced MPU9250 Calibration Diagnostics**
   - Added detailed sensor identity verification including WHO_AM_I register value logging
   - Implemented detection of specific sensor types (MPU6050, MPU6500, MPU9250)
   - Added dynamic scaling factor calculation based on accelerometer configuration
   - Enhanced zero reading detection with advanced diagnostics
   - Improved gravity compensation in Z-axis calibration
   - Added verification of calibration results with physical unit conversion

2. **Adaptive Scaling Factor System**
   - Modified UltraBasicPositionDetector to use configurable scaling factors
   - Added support for different scaling constants for various MPU sensor types
   - Implemented a statistical method to automatically determine the best scaling factor
   - Added runtime configurability of the scaling factor

3. **Comprehensive Test Application**
   - Created a dedicated MPUCalibrationTest application for diagnostics
   - Implemented step-by-step testing of scaling, calibration, and position detection
   - Added continuous monitoring mode for validation
   - Integrated zero reading detection and diagnosis

### 🔄 Next Steps

With these enhancements, we should be able to:
1. Determine if the sensor is truly an MPU6500 rather than MPU9250
2. Find the optimal scaling factor for our specific hardware
3. Diagnose and fix the zero value issue in the calibration script
4. Verify position detection with the corrected calibration

The test program (`examples/MPUCalibrationTest.cpp`) should be run to collect diagnostic data, which will inform our next phase of implementation focused on addressing any remaining issues discovered during testing.

### 📊 Expected Test Results

When running the test application, we expect to see:
1. Clear identification of the sensor type based on WHO_AM_I register
2. Diagnostic data showing raw vs. physical unit values
3. Statistical analysis of different scaling factors
4. Detection of any zero-reading issues and detailed register-level diagnostics

This data will be crucial in verifying our hypothesis about treating the sensor as an MPU6500 and will guide further refinement of the calibration process. 