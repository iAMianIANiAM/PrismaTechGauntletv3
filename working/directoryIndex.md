# Directory Index - PrismaTech Gauntlet 3.0

## Project Structure Overview

```
/src
  /core         - System management and core structures
  /hardware     - Hardware interfaces for sensors and LEDs
  /detection    - Position and gesture detection logic
  /modes        - Mode-specific implementation handlers
  /animation    - LED animation system
  /utils        - Support utilities
/examples       - Standalone example applications
```

## Current Implementation Focus

### Phase 2: Hardware Interface Foundation

#### Completed
- **MPU9250 Core Interface** (`/src/hardware/MPU9250Interface.*`)
  - I2C communication setup
  - Sensor initialization
  - Raw data reading
  - Error handling and diagnostics
  - Debug logging
  - Auto-detection of device address
  - Calibration routines
  - Connection verification
  - Hardware testing with successful validation
  
- **I2C Scanner Utility** (`/src/utils/I2CScanner.h`)
  - I2C bus scanning
  - Device detection
  - MPU-specific diagnostic tools
  - Register reading capabilities
  
- **Diagnostic Test Example** (`/examples/MPUDiagnosticTest.cpp`)
  - Comprehensive MPU sensor testing
  - Connection diagnostics
  - Sensor data visualization

- **LED Interface** (`/src/hardware/LEDInterface.*`)
  - WS2812 driver initialization using FastLED
  - Color control and mapping
  - Animation framework implementation
  - Position color mapping
  - Advanced animations (rainbow, pulse)
  - Power state management
  - Hardware verified with successful testing
  
- **Hardware Manager** (`/src/hardware/HardwareManager.*`)
  - Unified hardware initialization
  - Resource management
  - Power state control
  - Error recovery system
  - Hardware status monitoring
  - Hardware verified with successful testing
  
- **LED Test Example** (`/examples/LEDTest.cpp`)
  - LED functionality demonstration
  - Animation testing
  - Color mapping verification
  
- **Hardware Manager Test** (`/examples/HardwareManagerTest.cpp`)
  - Integration testing of all hardware components
  - Power state transition testing
  - Error recovery demonstration
  - Sensor data visualization with LED feedback

#### Pending Implementation
- **MPU9250 Data Processing** (`/src/hardware/MPU9250Interface.*`)
  - Simple filtering implementation
  - Data validation checks
  - Performance optimization
  - Raw data threshold determination

## Core Components

### Hardware Layer
- `MPU9250Interface`: Sensor communication and data reading
  - **Status**: Core implementation complete and hardware-verified
  - **Verification**: Successfully tested on hardware
- `I2CScanner`: I2C diagnostics and connection verification
  - **Status**: Implementation complete
  - **Verification**: Successfully tested on hardware
- `LEDInterface`: LED control and animation management
  - **Status**: Implementation complete and hardware-verified
  - **Verification**: Successfully tested on hardware with all animations functioning
- `HardwareManager`: Hardware resource coordination
  - **Status**: Implementation complete and hardware-verified
  - **Verification**: Successfully tested with integration of MPU and LED components

### Core Systems
- `GauntletController`: System coordination
- `PositionDetector`: Position detection logic
- `GestureRecognizer`: Gesture recognition
- `AnimationSystem`: LED animation management

## Implementation Status

### Completed
- Core architecture design
- Directory structure
- Basic data structures
- Interface definitions
- Debug infrastructure
- MPU9250 Core Interface implementation
  - I2C communication
  - Sensor initialization
  - Data reading
  - Error handling
  - Calibration routine
  - Hardware validation
- I2C Scanner utility
  - Bus scanning
  - MPU-specific diagnostics
  - Connection troubleshooting
- Diagnostic test application 
  - Step-by-step sensor verification
  - Data visualization
  - Error reporting
- LED Interface implementation
  - WS2812 driver with FastLED
  - Color control and position mapping
  - Animation framework
  - Power state management
- Hardware Manager implementation
  - Unified hardware initialization
  - Resource management
  - Power state control
  - Error recovery system
  - Hardware status monitoring
- Hardware testing and verification
  - MPU sensor function verification
  - LED display verification
  - Hardware Manager integration verification

### In Progress
- Testing and verification of Calibration Protocol
- Analysis of calibration data
- Determination of position thresholds
- Position detection algorithm refinement

### Pending
- Position detection implementation
- Gesture recognition
- Mode implementation
- Animation system enhancements

## File Relationships

### Hardware Layer
```
HardwareManager (Implemented & Verified)
  ├── MPU9250Interface (Implemented & Verified)
  └── LEDInterface (Implemented & Verified)
```

### Utilities
```
Utils
  ├── I2CScanner (Implemented & Verified)
  ├── DebugTools
  └── CircularBuffer
```

### Core Systems
```
GauntletController
  ├── HardwareManager
  ├── PositionDetector
  ├── GestureRecognizer
  └── AnimationSystem
```

### Implemented Files
- **`src/hardware/MPU9250Interface.h/cpp`** - MPU9250 sensor interface (VERIFIED)
- **`src/utils/I2CScanner.h`** - I2C diagnostic utilities (VERIFIED)
- **`examples/MPUDiagnosticTest.cpp`** - Diagnostic application (VERIFIED)
- **`src/core/SystemTypes.h`** - Data structures
- **`src/core/Config.h`** - System configuration
- **`src/main.cpp`** - Main application implementing sensor reading
- **`src/hardware/LEDInterface.h/cpp`** - LED interface for WS2812 control (VERIFIED)
- **`src/hardware/HardwareManager.h/cpp`** - Hardware resource manager (VERIFIED)
- **`examples/LEDTest.cpp`** - LED testing application (VERIFIED)
- **`examples/HardwareManagerTest.cpp`** - Hardware integration test (VERIFIED)
- **`src/hardware/PowerManager.h/cpp`** - Power state management (VERIFIED)

## Current Development Guidelines

### Hardware Interface Implementation
- Focus on robust error handling
- Maintain raw integer data throughout sensor processing
- Document hardware configurations carefully
- Avoid complex scaling operations
- Prioritize reliability over optimization

### MPU Sensor Data Handling
- Work with raw integer data to avoid scaling issues
- Implement simple filtering where needed
- Document all data transformations explicitly
- Use relative comparisons rather than absolute values
- Implement hysteresis for threshold detection

### Code Organization
- Keep hardware-specific code isolated
- Maintain clear interface boundaries
- Document all public interfaces
- Include comprehensive error handling
- Add detailed debug logging

### Testing Requirements
- Unit tests for all components
- Integration tests for hardware communication
- Error condition testing
- Performance validation
- Memory usage monitoring

## Top-Level Directory Structure

- **/src** - Main source code for the application
- **/include** - External library header files
- **/lib** - External libraries
- **/reference** - Reference documents and design specifications
- **/working** - Working documents and project notes
- **/examples** - Standalone example applications for testing components
- **/test** - Structured test files for unit and integration testing

## Source Code Organization

The source code follows a layered architecture focused on embedded efficiency:

### Core Layer (`/src/core`)

Central system organization and state management.

- **GauntletController.h/cpp** - Main system controller coordinating all subsystems
- **SystemTypes.h** - Common data structures and enumerations (IMPLEMENTED)
- **Config.h** - System-wide configuration parameters (IMPLEMENTED)

### Hardware Layer (`/src/hardware`)

Hardware interfaces and abstractions.

- **HardwareManager.h** - Coordinates all hardware components (header only)
- **MPU9250Interface.h/cpp** - IMU sensor interface (IMPLEMENTED)
- **LEDInterface.h** - WS2812 LED control interface (header only)
- **PowerManager.h** - Power state management (header only)

### Detection Layer (`/src/detection`)

Position and gesture detection algorithms.

- **PositionDetector.h** - Hand position detection using Dominant Axis model (header only)
- **GestureRecognizer.h** - Gesture detection for mode triggers (header only)
- **CalibrationRoutine.h** - Sensor calibration procedure (header only)

### Mode Handlers (`/src/modes`)

Implementation of the four operational modes.

- Directory created but files not yet implemented
- Planned files include:
  - **IdleMode.h/cpp** - Default monitoring mode
  - **InvocationMode.h/cpp** - Three-slot position recording mode
  - **ResolutionMode.h/cpp** - Spell determination and effect mode
  - **FreecastMode.h/cpp** - Motion-based pattern generation mode

### Animation System (`/src/animation`)

Visual feedback system for LED patterns.

- **AnimationSystem.h** - Animation coordination and rendering (header only)
- **AnimationData.h** - Predefined animation data stored in program memory
- **FreecastGenerator.h** - Dynamic pattern generation from motion data (header only)

### Utilities (`/src/utils`)

Support utilities and helper functions.

- **I2CScanner.h** - I2C bus scanner and diagnostic tools (IMPLEMENTED)
- **CircularBuffer.h** - Generic circular buffer implementation
- **FixedMath.h** - Fixed-point math utilities for efficient calculations
- **DebugTools.h/cpp** - Debugging and diagnostic tools

### Examples (`/examples`)
Standalone test applications for verifying component functionality.

- **MPUDiagnosticTest.cpp** - Diagnostic application for MPU sensor (IMPLEMENTED)
- **LEDTest.cpp** - Test application for LED interface (IMPLEMENTED)
- (Additional example files to be created as needed)

### Test Directory (`/test`)
Structured test files organized by component.

- **`/mpu`** - MPU sensor test files
  - **MPU9250Test.cpp** - Test functions for the MPU sensor
  - **MPU9250TestMain.cpp** - Main entry point for MPU tests
- **`/led`** - LED interface test files (future)
- **`/helpers`** - Utility files for testing
  - **dummy.cpp** - Arduino framework entry point helper

## Key Class Relationships

```
GauntletController
  ├── HardwareManager
  │     ├── MPU9250Interface (IMPLEMENTED)
  │     ├── LEDInterface
  │     └── PowerManager
  ├── PositionDetector
  ├── GestureRecognizer
  └── AnimationSystem
        └── FreecastGenerator
```

## Data Flow

1. **Input Flow**:
   - MPU9250Interface → HardwareManager → PositionDetector → GestureRecognizer → Mode Handlers

2. **State Management Flow**:
   - GauntletController manages system state and mode transitions
   - Each mode uses specialized handlers for timing and behavior

3. **Output Flow**:
   - Mode Handlers → AnimationSystem → LEDInterface → Physical LEDs

## Key Resource Usage

- **Memory Usage**: Static allocation with fixed buffer sizes
- **Processing**: Event-driven core with non-blocking design
- **Power Management**: Multiple power states to optimize battery life

## Configuration Parameters

Key system parameters are defined in `Config.h` including:

- Hardware pin assignments
- Timing parameters for state transitions
- Threshold values for position detection
- Color definitions for visual feedback

## Current Development Stage

Foundation layer is complete with architecture in place. We have implemented header files for most components with skeleton structures and are now proceeding with the implementation of hardware interfaces and the calibration protocol. Our next steps focus on the MPU9250 interface and calibration data collection via USB (COM7).

## PlatformIO Project Structure

This project follows the PlatformIO standard directory structure for ESP32 development:

- **`src/`**: Contains the main application code
- **`include/`**: Contains header files that should be accessible across the entire project
- **`lib/`**: Contains project-specific libraries or modified third-party libraries
- **`test/`**: Contains organized test files for components
- **`examples/`**: Contains standalone example applications
- **`platformio.ini`**: Configuration file for PlatformIO that specifies build parameters, board details, framework, and library dependencies

## Namespace Organization

Will be fully implemented as we develop the core components.

## Key Directories and Contents

### Reference Directory
- **`TrueFunctionGuide.md`**: Comprehensive documentation of system functionality
- **`LED Reference/`**: Documentation for WS2812 LED Ring
- **`GY-521_MPU6050 Reference/`**: Documentation for the MPU6050 sensor
- **`ESP32 Reference/`**: Documentation for the ESP32-S2 mini controller
- **`hardwareSpecifications.md`**: Detailed hardware specifications and wiring information

### Working Directory
- **`roadmap.md`**: Project goals and timelines in checklist format
- **`currentplan.md`**: Detailed outline of current work
- **`directoryIndex.md`**: This file - comprehensive index of the project directory
- **`UserScratchpad.md`**: Notes and ideas for the development process

### Source Directory
- **`main.cpp`**: Application entry point
- **`core/`**: Core system components
- **`hardware/`**: Hardware interface components
- **`detection/`**: Position and gesture detection components
- **`modes/`**: To be populated with mode handlers
- **`animation/`**: Animation system components
- **`utils/`**: Utility functions and tools 