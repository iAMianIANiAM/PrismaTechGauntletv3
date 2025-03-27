# MPU Data Processing Documentation

## Raw Sensor Data Characteristics

### MPU9250/6050 Accelerometer Basics
- **Measurement Range**: Configurable to ±2g, ±4g, ±8g, or ±16g
- **ADC Resolution**: 16-bit signed integers (-32,768 to +32,767)
- **Current Configuration**: ±2g range (our implementation)
- **Sensitivity**: At ±2g range, ~16,384 ADC units per g (gravitational acceleration)

### Physical Meaning of Raw Values
- Raw data represents proper acceleration (including gravity) along each axis
- At rest on Earth's surface, one axis will read ~16,384 units (1g)
- "True zero" would only be achieved in free-fall (absence of acceleration)
- When tilted, gravity component is distributed across multiple axes

### Sensor Orientation on Gauntlet
- Mounted on back of hand
- In neutral position:
  - Z-axis points upward (perpendicular to back of hand)
  - X-axis points along the hand toward fingers
  - Y-axis points across the hand from thumb to little finger

## Current Data Processing Steps

### Step 1: Raw ADC Reading
- 16-bit signed integers read directly from MPU registers
- Range depends on sensor configuration (currently ±2g = roughly ±32,768)
- Values at this stage represent raw ADC counts without any processing

### Step 2: Initial Calibration (Applied at Startup)
- **Process**: During the first 2 seconds after initialization, the sensor calculates offsets
- **Purpose**: Reduces bias errors and provides baseline correction
- **Implementation**: Averages multiple readings and calculates offset values for each axis
- **Output Effect**: All subsequent readings have these offsets subtracted
- **Documentation Gap**: This step was not previously clearly documented

### Step 3: Threshold Comparison (Current Implementation)
- Raw values (with initial calibration applied) are compared directly to thresholds
- Current thresholds were derived from calibration data that already included the startup calibration effect
- Current implementation uses absolute thresholds without normalization or scaling

## Current Position Detection Thresholds (Config.h)
```cpp
// Position detection thresholds (calibrated for back-of-hand mounting)
constexpr float THRESHOLD_OFFER = 18900.09f;    // accelZ > threshold
constexpr float THRESHOLD_CALM = 2014.40f;      // accelZ < threshold
constexpr float THRESHOLD_OATH = -12475.42f;    // accelY < threshold
constexpr float THRESHOLD_DIG = 3106.71f;       // accelY > threshold
constexpr float THRESHOLD_SHIELD = -14224.77f;  // accelX < threshold
constexpr float THRESHOLD_NULL = 1281.05f;      // accelX > threshold
```

These thresholds were derived from statistical analysis of calibration data collected on 2025-03-26.

## Proposed Data Processing Steps

### Step 1: Raw ADC Reading [Unchanged]
- Read 16-bit integers directly from MPU registers
- Document and display these values as "True Raw"

### Step 2: Optional Initial Calibration [Configurable]
- Make the 2-second startup calibration optional/configurable
- Document when it's used and exact offset values calculated
- Display both pre-calibration and post-calibration values when used

### Step 3: Neutral Position Normalization [New]
- During calibration, measure neutral position reading for each axis
- Calculate average values in neutral position
- Subtract these from each reading to center data around zero for neutral position
- Document normalization offsets and process

### Step 4: Scaling Normalization [New/Optional]
- Determine range of motion for each axis during calibration
- Apply scaling to equalize the influence of each axis
- Document scaling factors and application

### Step 5: Threshold Comparison [Enhanced]
- Use normalized/scaled values for more consistent comparison
- Add secondary dominance checks where applicable
- Document exact comparison logic

## Threshold Calculation Documentation

For each set of thresholds, this document will record:
1. Date and time of calibration data collection
2. Sensor mounting position and orientation
3. Exact data processing steps applied before threshold calculation
4. Statistical method used to derive thresholds
5. Raw threshold values from statistical analysis
6. Any manual adjustments made based on testing
7. Final threshold values implemented in Config.h

## Change Log

| Date | Change | Description | Author |
|------|--------|-------------|--------|
| 2025-03-XX | Document Creation | Initial documentation of data processing steps | Claude/User |
| | | | |

This document will be updated any time changes are made to how MPU data is processed or thresholds are calculated. 