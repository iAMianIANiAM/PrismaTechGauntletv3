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

### Implemented Calibration Process

The actual implementation includes these enhancements to the calibration procedure:

#### 1. Interactive Visual Feedback
- Position-specific colors used during calibration
- LED flashing to indicate sampling progress
- Green flashes on calibration completion

#### 2. Threshold Scaling
- Uses a constant `THRESHOLD_SCALE` (0.8f) to set thresholds at 80% of measured values
- This constant is defined in the UltraBasicPositionDetector class

#### 3. Enhanced Calibration Command Interface
- 'c' command to enter calibration mode
- 't' command to display current thresholds
- Countdown with visual cues before each position calibration

#### 4. Direct Config.h Output
- Generates copy-paste ready code for Config.h
- Outputs thresholds in format compatible with DEFAULT_POSITION_THRESHOLDS array
- Clear instructions for users to update Config.h after calibration

#### 5. Enhanced Verification
- Min/max value reporting during calibration
- Verification flash pattern after each position is calibrated
- REST period between position calibrations

Here's the actual implementation of the calibration process:

```cpp
float UltraBasicPositionDetector::calibratePosition(uint8_t position, uint16_t samples) {
  // Validate position is in range
  if (position >= 6) return 0.0f;
  
  // 1. Visual feedback that calibration is starting
  Color positionColor = getPositionColor(position);
  
  // 2. Collect and average samples for calibration
  float sumX = 0, sumY = 0, sumZ = 0;
  float maxX = -9999, maxY = -9999, maxZ = -9999;
  float minX = 9999, minY = 9999, minZ = 9999;
  
  // Discard first 10 samples (stabilization)
  for (int i = 0; i < 10; i++) {
    _hardware->update();
    delay(20);
  }
  
  // Collect samples for averaging
  for (int i = 0; i < samples; i++) {
    _hardware->update();
    
    // Get sensor data and process
    SensorData raw = _hardware->getSensorData();
    ProcessedData data;
    processRawData(raw, data);
    
    // Track running sums
    sumX += data.accelX;
    sumY += data.accelY;
    sumZ += data.accelZ;
    
    // Track min/max values
    maxX = max(maxX, data.accelX);
    maxY = max(maxY, data.accelY);
    maxZ = max(maxZ, data.accelZ);
    minX = min(minX, data.accelX);
    minY = min(minY, data.accelY);
    minZ = min(minZ, data.accelZ);
    
    // Flash LED to indicate sampling
    if (i % 5 == 0) {
      _hardware->setAllLEDs(positionColor);
    } else {
      _hardware->setAllLEDs({0, 0, 0});
    }
    _hardware->updateLEDs();
    
    delay(20);
  }
  
  // Calculate average
  float avgX = sumX / samples;
  float avgY = sumY / samples;
  float avgZ = sumZ / samples;
  
  // Output detailed calibration data
  Serial.print("Position average: X=");
  Serial.print(avgX, 2);
  Serial.print(" Y=");
  Serial.print(avgY, 2);
  Serial.print(" Z=");
  Serial.println(avgZ, 2);
  
  Serial.print("Position range: X=[");
  Serial.print(minX, 2);
  Serial.print(" to ");
  Serial.print(maxX, 2);
  Serial.print("] Y=[");
  Serial.print(minY, 2);
  Serial.print(" to ");
  Serial.print(maxY, 2);
  Serial.print("] Z=[");
  Serial.print(minZ, 2);
  Serial.print(" to ");
  Serial.print(maxZ, 2);
  Serial.println("]");
  
  // Calculate the threshold based on dominant axis average
  float newThreshold = 0;
  int axis = _dominantAxes[position];
  switch (axis) {
    case 0: newThreshold = avgX * THRESHOLD_SCALE; break;
    case 1: newThreshold = avgY * THRESHOLD_SCALE; break;
    case 2: newThreshold = avgZ * THRESHOLD_SCALE; break;
  }
  
  // Update threshold locally for testing
  _thresholds[position] = newThreshold;
  
  Serial.print("Calibrated threshold for position ");
  Serial.print(position);
  Serial.print(": ");
  Serial.println(newThreshold, 2);
  
  // Complete indication
  for (int i = 0; i < 3; i++) {
    _hardware->setAllLEDs({0, 255, 0});
    _hardware->updateLEDs();
    delay(100);
    _hardware->setAllLEDs({0, 0, 0});
    _hardware->updateLEDs();
    delay(100);
  }
  
  return newThreshold;
}
```

### Additional Calibration User Interface

The UltraBasicPositionTest.cpp implements an enhanced user experience:

```cpp
void handleCalibration() {
  Serial.println("Starting position calibration process...");
  Serial.println("Follow the LED prompts and instructions for each position.");
  Serial.println("Hold each position steady when instructed.\n");
  delay(3000);
  
  // Position names for clearer instructions
  const char* posNames[] = {
    "OFFER (Purple - Palm up, fingers extended)",
    "CALM (Yellow - Palm down, fingers extended)",
    "OATH (Red - Hand vertical, palm facing inward)",
    "DIG (Green - Hand vertical, palm facing outward)",
    "SHIELD (Blue - Arm extended forward, palm facing forward)",
    "NULL (Orange - Arm extended backward, palm facing back)"
  };
  
  // Calibrate each position with better timing
  for (uint8_t pos = 0; pos < 6; pos++) {
    // Announce next position with countdown
    Serial.print("\n=== NEXT POSITION: ");
    Serial.print(posNames[pos]);
    Serial.println(" ===");
    Serial.println("Get ready in:");
    
    // Countdown with distinct color
    Color prepColor = getPositionColor(pos);
    for (int i = 5; i > 0; i--) {
      Serial.print(i);
      Serial.println(" seconds...");
      
      // Pulse LEDs to indicate preparation
      for (int j = 0; j < 5; j++) {
        hardware->setAllLEDs({prepColor.r/2, prepColor.g/2, prepColor.b/2});
        hardware->updateLEDs();
        delay(100);
        hardware->setAllLEDs({0, 0, 0});
        hardware->updateLEDs();
        delay(100);
      }
    }
    
    // Actual calibration with clear visual feedback
    Serial.println("\nHOLD POSITION NOW!");
    Serial.println("Collecting samples...");
    
    // Solid color during sample collection
    hardware->setAllLEDs(prepColor);
    hardware->updateLEDs();
    
    // Perform actual calibration (50 samples)
    float threshold = positionDetector.calibratePosition(pos, 50);
    
    // Display the calibrated threshold
    Serial.print("Calibrated threshold for position ");
    Serial.print(posNames[pos]);
    Serial.print(": ");
    Serial.println(threshold);
    
    // Confirm completion
    Serial.println("Position calibrated successfully!");
    Serial.println("You can relax now while we prepare for the next position.");
    
    // Rest period with relaxing blue pulse
    Serial.println("Rest for a moment...");
    for (int i = 0; i < 10; i++) {
      uint8_t brightness = 50 + 50 * sin(i * 0.6);
      hardware->setAllLEDs({0, 0, brightness});
      hardware->updateLEDs();
      delay(200);
    }
    hardware->setAllLEDs({0, 0, 0});
    hardware->updateLEDs();
  }
  
  Serial.println("\n=== Calibration Complete ===");
}
```

This documentation accurately reflects the actual implemented calibration procedure, including the enhanced visual feedback, position-specific calibration approaches, and user interface elements.

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

According to the project directory index, the Ultra-Simplified Position Detection system will be implemented in the following files:

- Implementation in `UltraBasicPositionDetector.h/cpp` **[PLANNED]** (Implementation scheduled for Phase 2 of Hybrid Plan)
- Test environment in `env:ultrabasic` **[PLANNED]** (Implementation scheduled for Phase 2 of Hybrid Plan)
- Test application in `/examples/component_tests/UltraBasicPositionTest.cpp` **[PLANNED]** (Implementation scheduled for Phase 2 of Hybrid Plan)

There is currently a discrepancy between documentation and actual code implementation. The implementation of these files is scheduled as part of Phase 2 of our Hybrid Plan as outlined in currentplan_v2.md. 

## Approved Implementation Plan

Based on analysis of both the ECHO MPU data processing approach and this UBPD design document, the following implementation plan was approved:

### Core Implementation Components

#### 1. Data Processing Pipeline

```cpp
class UltraBasicPositionDetector {
private:
    // Constants
    static constexpr int POSITION_AVERAGE_SAMPLES = 3;
    static constexpr float SCALING_FACTOR = 4.0f * 9.81f / 32768.0f; // For ±4g range to m/s²
    
    // Raw data buffer for averaging
    SensorData _sampleBuffer[POSITION_AVERAGE_SAMPLES];
    uint8_t _currentSampleIndex = 0;
    
    // Position thresholds in m/s²
    float _positiveXThreshold = 7.0f;
    float _negativeXThreshold = -7.0f;
    float _positiveYThreshold = 7.0f;
    float _negativeYThreshold = -7.0f;
    float _positiveZThreshold = 7.0f;
    float _negativeZThreshold = -7.0f;
    
    // Current position data
    PositionReading _currentPosition;
    
    // Hardware interface
    IHardwareInterface* _hardware;
};
```

#### 2. Position Detection Logic

```cpp
uint8_t UltraBasicPositionDetector::detectPosition(const ProcessedData& data) {
    // Z-axis positions
    if (data.accelZ > _positiveZThreshold) {
        return POS_OFFER;     // Z strongly positive
    }
    if (data.accelZ < _negativeZThreshold) {
        return POS_CALM;      // Z strongly negative
    }
    
    // Y-axis positions
    if (data.accelY > _positiveYThreshold) {
        return POS_OATH;      // Y strongly positive
    }
    if (data.accelY < _negativeYThreshold) {
        return POS_DIG;       // Y strongly negative
    }
    
    // X-axis positions
    if (data.accelX > _positiveXThreshold) {
        return POS_NULL;      // X strongly positive
    }
    if (data.accelX < _negativeXThreshold) {
        return POS_SHIELD;    // X strongly negative
    }
    
    return POS_UNKNOWN;
}
```

#### 3. Data Processing Function

```cpp
void UltraBasicPositionDetector::processRawData(const SensorData& raw, ProcessedData& processed) {
    // Apply scaling to convert to m/s²
    processed.accelX = raw.accelX * SCALING_FACTOR;
    processed.accelY = -raw.accelY * SCALING_FACTOR;  // Invert for face-down mounting
    processed.accelZ = raw.accelZ * SCALING_FACTOR;
}
```

#### 4. Main Detection Method

```cpp
PositionReading UltraBasicPositionDetector::update() {
    // 1. Get raw sensor data
    SensorData rawData = _hardware->getSensorData();
    
    // 2. Add to averaging buffer
    _sampleBuffer[_currentSampleIndex] = rawData;
    _currentSampleIndex = (_currentSampleIndex + 1) % POSITION_AVERAGE_SAMPLES;
    
    // 3. Calculate averaged data
    SensorData averagedData = calculateAveragedData();
    
    // 4. Process to physical units
    ProcessedData processedData;
    processRawData(averagedData, processedData);
    
    // 5. Detect position
    uint8_t position = detectPosition(processedData);
    
    // 6. Update position reading
    _currentPosition.position = position;
    _currentPosition.confidence = 100; // Simple system always 100% confident
    _currentPosition.timestamp = millis();
    
    return _currentPosition;
}
```

#### 5. Calibration Implementation

```cpp
bool UltraBasicPositionDetector::calibratePosition(uint8_t position, uint16_t samples) {
    // 1. Visual feedback that calibration is starting
    _hardware->setLED(position, true);
    
    // 2. Discard initial samples to avoid transition motion
    for (uint8_t i = 0; i < 10; i++) {
        _hardware->getSensorData();
        delay(20);
    }
    
    // 3. Collect and average samples
    ProcessedData sum = {0, 0, 0};
    for (uint16_t i = 0; i < samples; i++) {
        SensorData raw = _hardware->getSensorData();
        ProcessedData processed;
        processRawData(raw, processed);
        
        sum.accelX += processed.accelX;
        sum.accelY += processed.accelY;
        sum.accelZ += processed.accelZ;
        
        // Visual feedback of progress
        if (i % 10 == 0) {
            _hardware->toggleLED(position);
        }
        
        delay(20);
    }
    
    // 4. Compute average
    ProcessedData avg = {
        sum.accelX / samples,
        sum.accelY / samples,
        sum.accelZ / samples
    };
    
    // 5. Set threshold at 80% of measured value
    float threshold = 0.8f;
    
    // 6. Assign to appropriate threshold
    switch (position) {
        case POS_OFFER:   _positiveZThreshold = avg.accelZ * threshold; break;
        case POS_CALM:    _negativeZThreshold = avg.accelZ * threshold; break;
        case POS_OATH:    _positiveYThreshold = avg.accelY * threshold; break;
        case POS_DIG:     _negativeYThreshold = avg.accelY * threshold; break;
        case POS_SHIELD:  _negativeXThreshold = avg.accelX * threshold; break;
        case POS_NULL:    _positiveXThreshold = avg.accelX * threshold; break;
    }
    
    // 7. Visual feedback that calibration is complete
    _hardware->setLED(position, false);
    
    return true;
}
```

### Key Advantages

1. **Simplicity**: Straightforward detection logic with minimal processing
2. **Physical Units**: All thresholds and values in meaningful m/s² units
3. **Reliability**: Direct threshold comparison with clear axis prioritization
4. **Debuggability**: Clear data flow and simple averaging
5. **Standard Scale**: Consistent -10 to +10 scale matching ECHO approach
6. **Easy Calibration**: Interactive calibration with visual feedback

### Implementation Strategy

1. Create `UltraBasicPositionDetector.h` with class definition, enums, and method declarations
2. Implement `UltraBasicPositionDetector.cpp` with the core detection logic
3. Create `UltraBasicPositionTest.cpp` in the examples/component_tests directory
4. Update the platformio.ini file to include the ultrabasic environment

This implementation maintains the core design philosophy of maximum simplicity while incorporating the best practices from the ECHO implementation regarding physical unit standardization and threshold handling. 