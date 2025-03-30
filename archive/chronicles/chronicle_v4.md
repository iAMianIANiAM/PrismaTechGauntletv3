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

## 🔍 Calibration Environment Analysis (2025-03-29)

### 📌 INSIGHT: Overlooked Functional Calibration Implementation

During a previous session, we discovered an overlooked yet functional calibration routine present in the project that was not fully accounted for in our initial analysis. This functional calibration implementation is in the "calibration" environment defined in platformio.ini, using the `/examples/CalibrationProtocol.cpp` file.

### 🧠 RECOLLECTION: The Missed Calibration Routine

I previously overlooked this file during our analysis, likely due to the impact of the OneDrive synchronization on file indexing. When the user discovered, uploaded, and successfully ran the calibration routine, it changed our perspective on the issue. The routine worked correctly, suggesting that our core scaling and detection problems were not necessarily with the sensor hardware or fundamental communication, but rather with the specific implementation approaches in our newer code.

### ✅ ANALYSIS: Two Distinct Calibration Approaches

Comparing the original `CalibrationProtocol.cpp` with our newer `UBPDCalibrationProtocol.cpp` reveals several critical differences:

1. **Data Collection Strategy**:
   - **Original Approach**: Collects raw sensor data (SensorData) directly from hardware in its native format
   - **UBPD Approach**: Collects processed data (ProcessedData) after scaling transformation

2. **Threshold Calculation**:
   - **Original Approach**: Outputs raw calibration data to CSV for external processing
   - **UBPD Approach**: Performs statistical analysis internally using 85% of mean values

3. **Position Detection**:
   - **Original Approach**: Directly associates each position with a dominant axis
   - **UBPD Approach**: Dynamically determines dominant axis based on statistical analysis

4. **Scaling Approach**:
   - **Original Approach**: Preserves raw values throughout the process
   - **UBPD Approach**: Applies scaling factors to convert to physical units (m/s²)

5. **Data Validation**:
   - **Original Approach**: Simple collection without validation
   - **UBPD Approach**: Includes variance calculation and statistical validation

### 🔬 Key Technical Details

1. The functioning `CalibrationProtocol.cpp` collects 300 samples per position and preserves the raw accelerometer values in CSV format:
   ```cpp
   void collectSensorData() {
     SensorData data = hardware->getSensorData();
     
     // Record in our array if space available
     if (sampleIndex < MAX_SAMPLES) {
       positionSamples[sampleIndex++] = data;
     }
     
     // Output to serial in CSV format
     uint8_t position = getPositionForState(currentState);
     outputSampleCSV(data, position);
   }
   ```

2. The CSV output contains timestamp, position ID, and raw accelerometer/gyroscope values:
   ```cpp
   void outputSampleCSV(const SensorData& sample, uint8_t position) {
     Serial.print(sample.timestamp);
     Serial.print(",");
     Serial.print(position);
     Serial.print(",");
     Serial.print(sample.accelX);
     // ...etc
   }
   ```

3. In contrast, our `UBPDCalibrationProtocol.cpp` uses a statistical approach with processed data:
   ```cpp
   void collectSensorData() {
     // Get processed sensor data
     ProcessedData data = detector->getProcessedData();
     
     // Accumulate data for this position
     positionData[position].accumX += data.accelX;
     // ...etc
   }
   ```

4. The threshold calculation is fundamentally different:
   ```cpp
   void calculateThresholds() {
     // Calculate means
     float meanX = positionData[position].accumX / positionData[position].validSamples;
     // ...
     
     // Set threshold based on dominant axis mean with safety margin
     float threshold = dominantValue * 0.85; // 85% of measured value
   }
   ```

### ⚠️ REVISED INSIGHT: Root Cause of Scaling Issues

After deeper analysis and clarification, it's now evident that the scaling issues are not due to a fundamental mismatch in approach between raw values and physical units. Rather, the issues stem from:

1. The conversion to m/s² is an intentional and necessary component of the UBPD design
2. This conversion provides more intuitive, manageable values against which to set meaningful thresholds
3. The issue lies in the specific scaling factor or conversion process that's being applied
4. The "Ultra Basic Position Detection" name reflects its simplified yet effective approach: raw data → physical units → threshold comparison

The core problem appears to be that our current implementation is either:
- Using an incorrect scaling factor for the m/s² conversion
- Applying the scaling factor inconsistently
- Not accounting for calibration offset properly

### 📌 DECISION: Conversion to m/s² is Critical and Required

The conversion to physical units (m/s²) is a vital feature of the UBPD system, not an optional or undesirable complexity. The design philosophy of "no more complex than absolutely necessary to function" correctly includes this conversion as an essential component.

*Note: The previous "Hybrid Approach" decision is hereby superseded by the "Synthesized Calibration and Scaling Plan" below.*

## 📋 Synthesized Calibration and Scaling Plan (2025-03-29)

*This plan supersedes all previous plans regarding calibration and scaling in the chronicle.*

### 🎯 Core Objectives

1. **Correctly implement the m/s² conversion** in the Ultra Basic Position Detector
2. **Determine the optimal scaling factor** for our specific sensor configuration
3. **Establish reliable threshold values** in physical units
4. **Ensure consistent data flow** from raw sensor readings to final position detection

### 🔬 Technical Implementation Plan

1. **Scaling Factor Validation**
   - Create a comprehensive test to determine the correct scaling factor
   - Compare the three existing scaling factors in `UltraBasicPositionDetector.h`:
     ```cpp
     static constexpr float DEFAULT_SCALING_FACTOR = 4.0f * 9.81f / 32768.0f; // Default for ±4g range
     static constexpr float MPU6500_SCALING_FACTOR = 9.81f / 8192.0f;         // MPU6500 at ±4g
     static constexpr float ALT_SCALING_FACTOR = 9.81f / 4096.0f;             // Higher sensitivity option
     ```
   - Validate each against known physical motions (e.g., measuring 1g at rest)
   - Extract information from successful calibration runs to inform factor selection

2. **Data Flow Tracing**
   - Implement detailed logging throughout the entire data path
   - Trace raw values → conversion → processed values → position detection
   - Identify any inconsistencies or data corruption points
   - Ensure proper unit propagation throughout the system

3. **Enhanced Calibration Procedure**
   - Refine `UBPDCalibrationProtocol.cpp` to ensure accurate m/s² conversion
   - Implement real-time validation of the scaling process during calibration
   - Add visualization of both raw and scaled values for immediate feedback
   - Preserve a record of both raw and processed values for analysis

4. **Threshold Validation**
   - Create a test mode that applies thresholds to both raw and processed data
   - Compare detection accuracy between approaches
   - Verify that m/s² thresholds provide better discernibility between positions
   - Calibrate statistical calculations to account for sensor noise in physical units

5. **Sensor-Specific Adaptation**
   - Determine if our hybrid MPU6050/6500/9250 sensor requires special handling
   - Test various scaling factors based on sensor type auto-detection
   - Document the specific quirks of our sensor configuration
   - Implement automatic sensor calibration at startup if needed

### 📊 Expected Benefits

1. Improved position detection reliability with the correct scaling factor
2. More intuitive threshold values expressed in physical units
3. Better diagnostics and troubleshooting capability
4. Consistent behavior across different device instances

### ⏱️ Implementation Timeline

1. **Phase 1: Scaling Factor Validation** (1 day)
   - Implement the scaling factor test application
   - Validate against known physical motions
   - Select the optimal scaling factor

2. **Phase 2: Enhanced Calibration** (2 days)
   - Refine the calibration protocol
   - Implement visualization of conversion process
   - Add validation checks

3. **Phase 3: System Integration** (2 days)
   - Apply the validated scaling factor to the main application
   - Ensure consistent behavior between test and production environments
   - Document final threshold values

### 📝 Success Criteria

The implementation will be considered successful when:
1. Position detection shows high confidence levels (65-95%, not >100%)
2. Thresholds in m/s² correctly identify all six hand positions
3. Calibration process produces consistent, usable values
4. System behaves predictably across multiple calibration/detection cycles

This plan addresses the root cause of our scaling issues while maintaining fidelity to the core UBPD design principle of converting raw values to physical units for more effective threshold-based detection.

## 🚨 Project Directory Migration (2025-03-29)

### 📌 DECISION: Project Directory Relocation

The entire project directory has been migrated from its previous location within a OneDrive-synchronized folder to a new location that is not affected by cloud synchronization services:

- **Previous Path**: `[OneDrive path]/PrismaTech Gauntlet 3.0`
- **New Path**: `C:\ProgrammingProjects\PlatformIO\Projects\PrismaTech Gauntlet 3.0`

### ⚠️ ISSUE: OneDrive Interference

OneDrive synchronization was causing several issues across multiple projects, including:
- File locking conflicts during builds
- Source control conflicts
- Intermittent I/O errors during development
- Increased build times due to cloud synchronization overhead

### ✅ RESOLUTION: Migration and Path Dependency Fixes

The following actions were taken to ensure a smooth transition:

1. **Build Environment Regeneration**:
   - PlatformIO environment was reinitialized to regenerate all path-dependent configuration files
   - `.vscode/c_cpp_properties.json` and `launch.json` updated with new project paths

2. **Script Updates**:
   - Fixed hardcoded Python path in `utils/pre-commit` hook to use system PATH for better portability
   - Verified all Python utility scripts use relative paths or dynamic path resolution:
     - `verify_docs.py` (verified)
     - `calibration_logger.py` (verified)
     - `analyze_calibration.py` (verified)

3. **Build Cache Cleaning**:
   - Cleaned build artifacts to ensure no old paths are referenced in compiled code
   - Rebuilt all environments to verify successful compilation with new path structure

### 🧠 INSIGHT: Path Handling Best Practices

This migration highlights the importance of:
1. Using relative paths wherever possible
2. Using path libraries (like Python's `pathlib` or `os.path`) rather than string concatenation
3. Dynamically determining project root rather than hardcoding absolute paths
4. Keeping development projects outside of cloud-synchronized directories

The project should now function correctly in its new location with no lingering path dependencies.

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

## 📘 Chronicle State and Structure (202503291749)

The current chronicle v4 page has become difficult to follow due to several organizational issues. New entries were inappropriately inserted in the middle of the document rather than being appended chronologically at the end, creating confusion about the sequence of events. This was compounded by the use of the same date timestamp (2025-03-29) across multiple entries without specific times, making it impossible to determine their temporal relationship. Additionally, multiple planning sections with differing approaches were scattered throughout the document, some of which were later superseded by newer plans, further obscuring the current project direction. This structure violation has compromised the chronicle's function as a clear historical record of development activities and decisions. 