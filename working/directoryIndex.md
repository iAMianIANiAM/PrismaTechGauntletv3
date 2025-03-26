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
/utils          - Python utilities for data collection and analysis
/logs           - Data logs and generated thresholds
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
  
- **MPU9250 Data Processing** (`/src/hardware/MPU9250Interface.*`)
  - Simple filtering implementation with averaging
  - Data validation and consistency checks
  - Enhanced error recovery mechanisms
  - Motion magnitude calculation
  - Maximum axis data collection
  - Hardware verified with successful testing
  
- **I2C Scanner Utility** (`/src/utils/I2CScanner.h`)
  - I2C bus scanning
  - Device detection
  - MPU-specific diagnostic tools
  - Register reading capabilities
  
- **Diagnostic Test Example** (`/examples/MPUDiagnosticTest.cpp`)
  - Comprehensive MPU sensor testing
  - Connection diagnostics
  - Sensor data visualization
  
- **MPU Filter Test Example** (`/examples/MPUFilterTest.cpp`)
  - Raw vs. filtered data comparison
  - Data validation verification
  - Motion magnitude demonstration
  - Maximum axis data collection
  
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

### Phase 3: Basic Position Detection

#### Completed
- **Calibration Protocol** (`/examples/CalibrationProtocol.cpp`)
  - Structured calibration sequence with state machine
  - Position-specific data collection (30 seconds per position)
  - LED visual indicators for positions and transitions
  - Serial data output in CSV format for analysis
  - Successfully tested on hardware

- **Calibration Utilities** (`/utils/`)
  - Serial data logger script (`calibration_logger.py`)
  - Calibration data analysis script (`analyze_calibration.py`)
  - Threshold generation for position detection
  - Robust error handling for data capture
  - Detailed statistical analysis of sensor readings
  - Comprehensive calibration guide (`CALIBRATION_GUIDE.md`)
  - Sensor placement and orientation tracking

#### In Progress Implementation
- **Position Detection Thresholds**
  - Threshold determination from calibration data
  - Thresholds generated from latest calibration (calibration_data_20250326_011013.csv)
  - Thresholds stored in `logs/suggested_thresholds.txt`
  - Thresholds integrated into `src/core/Config.h` for system-wide access
  - Refactored for back-of-hand sensor placement
  - Documentation of threshold values and ranges
  - **Status**: Thresholds determined and integrated, awaiting implementation of detection logic

#### Pending Implementation
- **Position Detector Implementation** (`/src/detection/PositionDetector.cpp`)
  - Position Detector header exists (`/src/detection/PositionDetector.h`)
  - Point Detection Model implementation pending
  - Position classification based on calibration thresholds
  - Position state management
  - Integration with hardware manager

- **Idle Mode** (`/src/modes/IdleMode.cpp`)
  - Position color visualization
  - Testing and feedback functionality
  - Position tracking
  - Performance monitoring

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

### Calibration and Analysis
- `CalibrationProtocol.cpp`: Calibration sequence implementation
  - **Status**: Implementation complete and hardware-verified
  - **Verification**: Successfully collected calibration data for all positions
- `calibration_logger.py`: Data collection utility
  - **Status**: Implementation complete with robust error handling
  - **Verification**: Successfully logged calibration data
- `analyze_calibration.py`: Data analysis utility
  - **Status**: Implementation complete with statistical analysis capabilities
  - **Verification**: Successfully generated threshold values from calibration data

### Core Systems
- `GauntletController`: System coordination
  - **Status**: Header implemented, implementation file exists but functionality pending
  - **Verification**: Not yet verified
- `PositionDetector`: Position detection logic
  - **Status**: Header implemented, implementation file pending
  - **Verification**: Not yet verified
- `GestureRecognizer`: Gesture recognition
  - **Status**: Header defined, implementation pending
  - **Verification**: Not yet verified
- `AnimationSystem`: LED animation management
  - **Status**: Not yet implemented
  - **Verification**: Not yet verified

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
- MPU9250 Data Processing implementation
  - Data filtering mechanism
  - Data validation checks
  - Enhanced error recovery
  - Motion magnitude analysis
  - Maximum axis data collection
- I2C Scanner utility
  - Bus scanning
  - MPU-specific diagnostics
  - Connection troubleshooting
- Diagnostic test application 
  - Step-by-step sensor verification
  - Data visualization
  - Error reporting
- MPU Filter test application
  - Raw vs. filtered data comparison
  - Data validation verification
  - Motion analysis demonstration
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
  - MPU data processing verification
  - LED display verification
  - Hardware Manager integration verification
- Calibration Protocol implementation
  - State machine for sequenced calibration
  - Visual indicators for positions
  - Timed data collection for each position
  - CSV data output format
- Calibration Data Collection Utilities
  - Serial data logging
  - Robust error handling for binary data
  - CSV file generation with timestamps
  - Threshold analysis and extraction

### In Progress
- Position Detection System
  - Threshold validation and adjustment
  - Consideration for forearm sensor placement
  - Statistical analysis of position distinctiveness

### Pending
- Position detector implementation
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

### Calibration System
```
CalibrationProtocol (Implemented & Verified)
  └─┬─ calibration_logger.py (Implemented & Verified)
    └── analyze_calibration.py (Implemented & Verified)
         └── suggested_thresholds.txt (Generated)
```

### Core Systems
```
GauntletController
  ├── HardwareManager
  │     ├── MPU9250Interface (IMPLEMENTED & VERIFIED)
  │     ├── LEDInterface (IMPLEMENTED & VERIFIED)
  │     └── PowerManager (IMPLEMENTED & VERIFIED)
  ├── PositionDetector (HEADER IMPLEMENTED, CPP PENDING)
  ├── GestureRecognizer (HEADER IMPLEMENTED, CPP PENDING)
  └── AnimationSystem (HEADER IMPLEMENTED, CPP PENDING)
        └── FreecastGenerator (NOT YET IMPLEMENTED)
```

### Implemented Files
- **`src/hardware/MPU9250Interface.h/cpp`** - MPU9250 sensor interface (VERIFIED)
- **`src/utils/I2CScanner.h`** - I2C diagnostic utilities (VERIFIED)
- **`examples/MPUDiagnosticTest.cpp`** - Diagnostic application (VERIFIED)
- **`examples/MPUFilterTest.cpp`** - Data processing testing application (VERIFIED)
- **`src/core/SystemTypes.h`** - Data structures (IMPLEMENTED)
- **`src/core/Config.h`** - System configuration including position detection thresholds (IMPLEMENTED)
- **`src/main.cpp`** - Main application implementing sensor reading (IMPLEMENTED)
- **`src/hardware/LEDInterface.h/cpp`** - LED interface for WS2812 control (VERIFIED)
- **`src/hardware/HardwareManager.h/cpp`** - Hardware resource manager (VERIFIED)
- **`src/hardware/PowerManager.h/cpp`** - Power state management (VERIFIED)
- **`examples/LEDTest.cpp`** - LED testing application (VERIFIED)
- **`examples/HardwareManagerTest.cpp`** - Hardware integration test (VERIFIED)
- **`examples/CalibrationProtocol.cpp`** - Calibration protocol implementation (VERIFIED)
- **`utils/calibration_logger.py`** - Calibration data logging utility (VERIFIED)
- **`utils/analyze_calibration.py`** - Calibration data analysis utility (VERIFIED)
- **`logs/suggested_thresholds.txt`** - Generated threshold values (GENERATED)
- **`src/core/GauntletController.h/cpp`** - Main system controller (IMPLEMENTED, NOT VERIFIED)
- **`src/detection/PositionDetector.h`** - Position detection header (IMPLEMENTED, AWAITING CPP IMPLEMENTATION)
- **`src/detection/GestureRecognizer.h`** - Gesture recognition header (IMPLEMENTED, AWAITING CPP IMPLEMENTATION)
- **`src/detection/CalibrationRoutine.h`** - Calibration routine header (IMPLEMENTED, AWAITING CPP IMPLEMENTATION)
- **`src/utils/DebugTools.h/cpp`** - Debugging and diagnostic tools (IMPLEMENTED)
- **`src/utils/FixedMath.h`** - Fixed-point math utilities (IMPLEMENTED)
- **`src/utils/CircularBuffer.h`** - Generic circular buffer implementation (IMPLEMENTED)

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

### Calibration and Position Detection
- Adapt to current sensor placement on forearm
- Focus on statistical analysis for threshold determination
- Consider multi-axis criteria for position detection
- Prioritize reliability over sensitivity
- Implement clear debugging capabilities for position detection

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

### Position Detection Implementation
- Implement Point Detection Model with clear threshold boundaries
- Use simple averaging for noise reduction
- Avoid complex confidence calculations initially
- Prioritize clear visual feedback during testing
- Create distinct zones for positions with well-defined "unknown" areas
- Validate with thorough testing in Idle Mode

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
- **CalibrationProtocol.cpp** - Calibration protocol for position detection (IMPLEMENTED)
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
  ├── HardwareManager (IMPLEMENTED & VERIFIED)
  │     ├── MPU9250Interface (IMPLEMENTED & VERIFIED)
  │     ├── LEDInterface (IMPLEMENTED & VERIFIED)
  │     └── PowerManager (IMPLEMENTED & VERIFIED)
  ├── PositionDetector (HEADER IMPLEMENTED, CPP PENDING)
  ├── GestureRecognizer (HEADER IMPLEMENTED, CPP PENDING)
  └── AnimationSystem (HEADER IMPLEMENTED, CPP PENDING)
        └── FreecastGenerator (NOT YET IMPLEMENTED)
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

Foundation layer is complete with architecture in place. Hardware layer implementation is complete and verified. Calibration workflow is implemented and working successfully. Position detection thresholds have been generated from calibration data and integrated into Config.h. Next steps focus on implementing the PositionDetector.cpp file to leverage the thresholds for actual position detection functionality.

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

### Utils Directory (`/utils`)
Utilities for data processing and analysis.

- **calibration_logger.py** - Serial data logger for calibration (IMPLEMENTED)
- **analyze_calibration.py** - Analysis tool for calibration data (IMPLEMENTED)
- **CALIBRATION_GUIDE.md** - Step-by-step guide for calibration process (IMPLEMENTED)

## Logs Directory (`/logs`)
Log files for calibration data and generated thresholds.

- **calibration_data_20250326_011013.csv** - Most recent calibration data (back-of-hand placement) (VERIFIED)
- **suggested_thresholds.txt** - Generated threshold values from latest calibration (GENERATED)
- Other calibration data files from previous calibration runs 