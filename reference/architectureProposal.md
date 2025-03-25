# Refined PrismaTech Gauntlet 3.0 Architecture Proposal

## System Overview

A streamlined architecture balancing clear organization with minimal overhead, designed specifically for the ESP32 platform and the unique requirements of the PrismaTech Gauntlet.

## Core Architecture Components

### 1. System Controller

**GauntletController**: Lightweight central coordinator combining initialization, main loop, and critical timing.
- Uses non-blocking design with precise timing control
- Minimal memory footprint with explicit hardware interfacing
- Manages global system state using simple enumeration

```cpp
// Simple state tracking with minimal overhead
enum SystemMode {
  MODE_IDLE,
  MODE_INVOCATION,
  MODE_RESOLUTION,
  MODE_FREECAST
};
```

### 2. Hardware Interface Layer

**HardwareManager**: Direct hardware communication layer with explicit pin definitions.
- **MPU9250Interface**: Handles I2C communication with the IMU
- **LEDInterface**: Controls WS2812 LED ring with memory-efficient commands
- **PowerManager**: Manages power states, sleep modes, and battery optimization

### 3. Position & Gesture Detection

**PositionDetector**: Implements the Dominant Axis Detection model with calibration support.
- Uses fixed-point math where possible to increase performance
- Configurable thresholds stored in flash memory
- Includes self-calibration routine for initial setup

```cpp
// Memory-efficient position structure
struct PositionReading {
  uint8_t position;  // Using enum values to save memory
  uint8_t confidence;  // 0-100 scale
};
```

**GestureRecognizer**: Lightweight state machine for gesture detection.
- Detects CalmOffer and LongNull triggers with precise timing
- Uses circular buffer for recent position history
- Minimizes memory usage through efficient data structures

### 4. Mode Handlers

**ModeHandlers**: Simplified handler functions for each system mode.
- **handleIdleMode()**: Position monitoring and trigger detection
- **handleInvocationMode()**: Three-phase slot recording sequence
- **handleResolutionMode()**: Spell determination and animation
- **handleFreecastMode()**: Motion analysis and pattern generation

### 5. Animation System

**AnimationSystem**: Memory-efficient animation engine using program memory.
- Predefined animation data stored in flash memory (PROGMEM)
- Runtime interpolation for smooth transitions
- Optimized rendering pipeline for WS2812 LEDs

```cpp
// Flash-stored animation definition
const PROGMEM AnimationFrame idleAnimFrames[] = {
  // Frame data defined in program memory
};
```

**FreecastGenerator**: Specialized real-time pattern generator for Freecast mode.
- Ring buffer for motion data with fixed memory allocation
- Simple mapping algorithms from motion to visual patterns
- Efficient update strategy using incremental calculations

### 6. Configuration System

**ConfigSystem**: Persistent configuration with defaults and calibration.
- Stored in ESP32 flash with wear leveling
- Includes factory defaults with user calibration overlays
- Runtime parameter adjustment for testing and tuning

## Memory Management Strategy

1. **Static Allocation**: All buffers and objects pre-allocated at compile time
2. **Fixed Buffer Sizes**: Explicitly sized for worst-case scenarios
3. **Program Memory Usage**: Constant data stored in flash, not RAM
4. **Shared Buffers**: Reuse of memory between non-concurrent operations

```cpp
// Example of static allocation approach
static SensorBuffer sensorData;
static LEDBuffer ledValues;
```

## Hardware Specifications

1. **ESP32 Configuration**:
   - Core 0: Main application logic
   - Core 1: Reserved for sensor acquisition and LED updates
   - Sleep mode: Light sleep during idle periods

2. **MPU9250 Configuration**:
   - I2C connection (pins 21, 22)
   - Sampling rate: 50Hz for position detection, 100Hz for Freecast
   - DMP offloading for motion processing

3. **WS2812 Configuration**:
   - Data pin: GPIO 18 (hardware RMT peripheral)
   - 12 LEDs with explicit index mapping
   - Power control through GPIO 5

## Timing and Execution Model

1. **Event-Driven Core**:
   - Main loop runs at 100Hz for consistent timing
   - Sensor readings scheduled at precise intervals
   - Non-blocking design with state-based processing

2. **Interrupt Usage**:
   - Timer interrupts for precise animation timing
   - I2C completion interrupts for sensor data
   - Critical sections protected from interrupt disruption

3. **Power States**:
   - **Active**: Full operation mode (all features)
   - **Ready**: Sensors active, LEDs minimal brightness
   - **Low Power**: Reduced sampling rate, dimmed LEDs
   - **Sleep**: Motion detection only, wake on movement

## Directory Structure

```
/src
  /core
    GauntletController.h/cpp  # Main controller with minimal overhead
    SystemTypes.h             # Common enums and structures
    Config.h                  # Configurable parameters
  /hardware
    HardwareManager.h/cpp     # Hardware interface coordination
    MPU9250Interface.h/cpp    # IMU sensor interface
    LEDInterface.h/cpp        # WS2812 LED controller
    PowerManager.h/cpp        # Power state management
  /detection
    PositionDetector.h/cpp    # Hand position detection
    GestureRecognizer.h/cpp   # Gesture recognition logic
    CalibrationRoutine.h/cpp  # Sensor calibration
  /modes
    IdleMode.h/cpp            # Idle mode implementation
    InvocationMode.h/cpp      # Invocation sequence logic
    ResolutionMode.h/cpp      # Spell resolution and effects
    FreecastMode.h/cpp        # Motion-to-pattern system
  /animation
    AnimationSystem.h/cpp     # Animation engine
    AnimationData.h           # Predefined animation data (PROGMEM)
    FreecastGenerator.h/cpp   # Dynamic pattern generation
  /utils
    CircularBuffer.h          # Efficient ring buffer implementation
    FixedMath.h               # Fixed-point math utilities
    DebugTools.h/cpp          # Conditional debugging tools
```

## Testing and Verification

1. **Unit Testing Framework**:
   - Lightweight test harness for hardware-independent components
   - Mock objects for hardware interfaces
   - Regression tests for critical algorithms

2. **Hardware-in-the-Loop Testing**:
   - Verification mode for real-time debugging
   - Serial output of sensor and state data
   - Visual LED indicators for internal states

3. **Field Testing Protocol**:
   - Structured test scenarios for each operational mode
   - Performance metrics collection
   - Battery life measurement

## Implementation Phases

1. **Foundation Phase**:
   - Hardware interfaces
   - Core timing system
   - Sensor data acquisition

2. **Position Detection Phase**:
   - Implement and calibrate position detection
   - Basic LED feedback
   - Initial gesture recognition

3. **Mode Implementation Phase**:
   - Implement each operational mode
   - Basic animation system
   - Mode transitions

4. **Visual Effects & Polish Phase**:
   - Complete animation library
   - Fine-tune timing and transitions
   - Optimize power consumption

5. **Testing & Refinement Phase**:
   - Comprehensive testing
   - Performance optimization
   - Finalize calibration

## How This Architecture Addresses TrueFunctionGuide Requirements

1. **Four Operational Modes**: Implemented as dedicated handler functions with clean transitions
2. **Hand Position Detection**: Direct implementation of Dominant Axis Detection model
3. **Gesture Recognition**: Efficient trigger detection for CalmOffer and LongNull
4. **Visual Feedback**: Memory-efficient animation system with pre-defined patterns
5. **Freecast Motion Analysis**: Optimized motion-to-pattern algorithm with fixed memory allocation
6. **Timing Precision**: Event-driven core with precise timing control
7. **Power Efficiency**: Multiple power states with hardware-specific optimization 