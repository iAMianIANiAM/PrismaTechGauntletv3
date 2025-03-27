# Ultra Basic Position Detection (UBPD) System

## Overview

The Ultra Basic Position Detection (UBPD) system was implemented as a minimalist approach to position detection in the PrismaTech Gauntlet 3.0 project. This system was designed to prioritize reliability and simplicity over sophistication, based on the principle that a simpler system that works consistently is better than a complex system that works intermittently.

## Core Design Philosophy

1. **Maximum Simplicity**: 
   - Direct dominant axis detection with minimal processing
   - 3-sample averaging only for basic noise reduction
   - No complex vector calculations or confidence metrics
   - No hysteresis or state tracking
   - Clear, straightforward data path from sensor to position

2. **Clear Implementation**:
   - Single detection function that identifies the dominant axis
   - Simple threshold comparison for each axis/position
   - Direct mapping from axis to hand position
   - Basic LED feedback for visual confirmation

3. **Simple Calibration**:
   - Straightforward 5-second calibration per position
   - Calculates thresholds at 80% of the measured values
   - No complex normalization or vector calculations
   - Interactive process with clear visual guidance

## Initial Implementation Components

1. **UltraBasicPositionDetector Class**:
   - Focused on one job: finding the dominant axis
   - Uses calibrated thresholds from existing configuration
   - Implements minimal data averaging (3 samples)
   - Clear mapping from axis readings to positions

2. **Test Application**:
   - Interactive calibration sequence
   - Real-time position display
   - Raw sensor data visualization
   - Threshold adjustment and display
   - Color-coded LED feedback

3. **Data Processing Flow**:
   - Raw acceleration → Simple averaging → Dominant axis detection → Position assignment
   - No intermediate calculations or complex processing stages
   - Direct thresholds applied to find dominant axis

## Initial Expected Benefits

- **Improved Reliability**: By removing complex calculations and relying on direct sensor readings
- **Better Debuggability**: Clear data path makes it easier to identify issues
- **Faster Response**: Minimal processing reduces latency
- **Less Code**: Smaller implementation with fewer potential points of failure
- **Easier Calibration**: Straightforward process that's easy to understand and repeat

## Redesigned Ultra-Basic Position Detection System

Based on analysis of the initial implementation and insights from the ECHO system, a redesigned approach for position detection was developed that prioritizes reliability and simplicity while addressing key issues encountered in testing.

### Core Design Philosophy of Redesign

1. Standardized physical units for intuitive thresholds
2. Absolute threshold values rather than percentage-based comparisons
3. Simplified, consistent processing pipeline
4. Minimalist calibration approach
5. Clear gravity orientation handling

### Implementation Components of Redesign

#### 1. Sensor Data Processing

```cpp
struct ProcessedData {
  float accelX, accelY, accelZ;  // in m/s²
};

void processRawData(const SensorData& raw, ProcessedData& processed) {
  // Convert to m/s² (standard gravity units)
  // Scale factor: ±4g range means 32768 = 4g
  // 1g = 9.81 m/s²
  const float scale = 4.0f * 9.81f / 32768.0f;
  
  // Apply scale and handle sensor orientation (face-down mounting)
  processed.accelX = raw.accelX * scale;
  processed.accelY = -raw.accelY * scale;  // Invert for face-down mounting
  processed.accelZ = raw.accelZ * scale;
}
```

#### 2. Position Detection Logic

```cpp
uint8_t detectPosition(const ProcessedData& data) {
  // Check each position using absolute thresholds in m/s²
  
  // Z-axis positions (handling gravity orientation)
  if (data.accelZ > positiveZThreshold) {
    return POS_OFFER;     // Z strongly positive
  }
  if (data.accelZ < negativeZThreshold) {
    return POS_CALM;      // Z strongly negative
  }
  
  // Y-axis positions
  if (data.accelY > positiveYThreshold) {
    return POS_OATH;      // Y strongly positive
  }
  if (data.accelY < negativeYThreshold) {
    return POS_DIG;       // Y strongly negative
  }
  
  // X-axis positions
  if (data.accelX > positiveXThreshold) {
    return POS_NULL;      // X strongly positive
  }
  if (data.accelX < negativeXThreshold) {
    return POS_SHIELD;    // X strongly negative
  }
  
  return POS_UNKNOWN;
}
```

#### 3. Calibration Approach

```cpp
bool calibratePosition(uint8_t position, uint16_t samples) {
  // 1. Discard initial samples to avoid transition motion
  for (uint8_t i = 0; i < 10; i++) {
    _hardware->getSensorData();
    delay(20);
  }
  
  // 2. Collect samples and compute average in m/s²
  ProcessedData sum = {0, 0, 0};
  for (uint16_t i = 0; i < samples; i++) {
    SensorData raw = _hardware->getSensorData();
    ProcessedData processed;
    processRawData(raw, processed);
    
    sum.accelX += processed.accelX;
    sum.accelY += processed.accelY;
    sum.accelZ += processed.accelZ;
    delay(20);
  }
  
  // 3. Compute average
  ProcessedData avg = {
    sum.accelX / samples,
    sum.accelY / samples,
    sum.accelZ / samples
  };
  
  // 4. Set appropriate threshold based on position
  // Use 80% of detected value as threshold for stability
  switch (position) {
    case POS_OFFER:   positiveZThreshold = avg.accelZ * 0.8f; break;
    case POS_CALM:    negativeZThreshold = avg.accelZ * 0.8f; break;
    case POS_OATH:    positiveYThreshold = avg.accelY * 0.8f; break;
    case POS_DIG:     negativeYThreshold = avg.accelY * 0.8f; break;
    case POS_SHIELD:  negativeXThreshold = avg.accelX * 0.8f; break;
    case POS_NULL:    positiveXThreshold = avg.accelX * 0.8f; break;
  }
  
  return true;
}
```

#### 4. Default Thresholds

```cpp
// Initial thresholds (in m/s²), will be updated during calibration
// These match ECHO approach of -10 to +10 scale
float positiveXThreshold = 7.0f;
float negativeXThreshold = -7.0f;
float positiveYThreshold = 7.0f;
float negativeYThreshold = -7.0f;
float positiveZThreshold = 7.0f;
float negativeZThreshold = -7.0f;
```

#### 5. Main Detection Pipeline

```cpp
PositionReading detectPosition(const SensorData& sensorData) {
  // 1. Basic data smoothing (keep simple 3-sample average)
  _sampleBuffer[_currentSampleIndex] = sensorData;
  _currentSampleIndex = (_currentSampleIndex + 1) % POSITION_AVERAGE_SAMPLES;
  SensorData averagedData = calculateAveragedData();
  
  // 2. Convert to physical units
  ProcessedData processedData;
  processRawData(averagedData, processedData);
  
  // 3. Detect position using absolute thresholds
  uint8_t position = detectPosition(processedData);
  
  // 4. Update position reading
  _currentPosition.position = position;
  _currentPosition.confidence = 100;
  _currentPosition.timestamp = millis();
  
  return _currentPosition;
}
```

### Key Differences from Previous Implementation

1. **Physical Units**: Data converted to meaningful m/s² units instead of percentage-based normalization
2. **Fixed Detection Logic**: Consistent use of absolute thresholds throughout
3. **Calibration & Detection Alignment**: Calibration directly updates the thresholds used in detection
4. **Gravity Orientation**: Clear handling of gravity's constant effect

### Improved Calibration Protocol

The calibration protocol was enhanced with:

1. **Buffer time between positions**: A 3-second countdown to give the user time to get into position
2. **Initial sample discarding**: Discards the first 10 samples to avoid capturing transition motion
3. **Improved user guidance**: Better visual and serial feedback during calibration
4. **Position-specific threshold adjustment**: Thresholds are stored separately for each axis direction

### Implementation Strategy

1. Update UltraBasicPositionDetector.h/.cpp with the new design
2. Modify UltraBasicPositionTest.cpp to display physical unit values
3. Maintain the same simple 3-sample averaging for noise reduction
4. Provide clear documentation about the conversion to m/s² units

### Expected Benefits of Redesign

1. **Improved Position Detection**: More reliable position detection with clear physical thresholds
2. **Intuitive Debugging**: Physical units make it easier to understand sensor readings
3. **Consistent Thresholds**: Using absolute thresholds provides more consistent detection
4. **Better Stability**: Clear handling of gravity and sensor orientation

## Verification Plan

To properly verify the Ultra-Simplified Position Detection implementation, the following steps were planned:

1. **Build and Upload**: Compile and upload the `env:ultrabasic` environment to the physical device
2. **Run Calibration**: Complete the calibration process for all six hand positions
3. **Test Detection**: Test all hand positions to verify detection accuracy
4. **Compare Modes**: Compare performance with the original Dominant Axis detection mode
5. **Document Findings**: Document the results including:
   - Detection accuracy for each position
   - Stability of detection during transitions
   - Any problematic positions or edge cases
6. **Adjust Parameters**: Fine-tune parameters based on testing results if needed

## Current Status

According to the project directory index, the Ultra-Simplified Position Detection system appears to be implemented in the following files:

- Implementation in `UltraBasicPositionDetector.h/cpp`
- Test environment in `env:ultrabasic` 
- Test application in `/examples/component_tests/UltraBasicPositionTest.cpp`

However, there appears to be a discrepancy between documentation and actual code implementation. The files referenced in the directory index may not actually exist in the codebase, indicating a potential issue with version control or backup procedures. The current status is "Implementation complete, awaiting hardware verification" according to the directory index, but actual file verification is needed 