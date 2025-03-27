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
  /component_tests - Component-specific test applications
/utils          - Python utilities for data collection and analysis
/logs           - Data logs and generated thresholds
```

## IMPORTANT: Version Control & Implementation Status Notice

**There is currently a discrepancy between this directory index and the actual codebase.** Due to version control issues, several implementations documented here were lost or never fully committed to the repository. See the "Documented But Missing Implementations" section below for details.

We are in the process of reconciling documentation with the actual codebase state. The information in this document is being updated to reflect the actual files present in the repository.

## Implementation Status Indicators

This document uses the following status indicators:

- **[PLANNED]**: Designed but not yet implemented
- **[IN PROGRESS]**: Implementation started but not complete  
- **[IMPLEMENTED]**: Code exists but not fully verified
- **[VERIFIED]**: Implementation exists and has been tested on hardware
- **[DEPRECATED]**: Was implemented but has been replaced

## Current Implementation Focus

### Phase 2: Hardware Interface Foundation ✅

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

### Phase 3: Basic Position Detection ✅

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

- **Position Detection Thresholds**
  - Threshold determination from calibration data
  - Thresholds generated from latest calibration (calibration_data_20250326_011013.csv)
  - Thresholds stored in `logs/suggested_thresholds.txt`
  - Thresholds integrated into `src/core/Config.h` for system-wide access
  - Refactored for back-of-hand sensor placement
  - Documentation of threshold values and ranges
  - **Status**: Thresholds determined and integrated, successfully used in position detection

- **Position Detector Implementation** (`/src/detection/PositionDetector.cpp`)
  - Point Detection Model implementation complete
  - Position classification based on dominant axis thresholds
  - Simple averaging for noise reduction (5-sample circular buffer)
  - No hysteresis initially for clearer testing
  - No confidence metric per user preference
  - Integration with hardware manager
  - TEST_MODE preprocessor guards for separating test and production code
  - **Status**: [VERIFIED] (2025-03-27, Successfully tested using SimplePositionTest)
  - **Note**: This is a functional but not optimal implementation. UBPD is intended to replace this.

- **Test Environments** (`platformio.ini`)
  - Main production environment (env:esp32dev)
  - Simple position test environment (env:simplepostest)
  - Position detector test environment (env:postest)
  - Hardware manager test environment (env:hwmtest)
  - Calibration environment (env:calibration)
  - Consistent build configurations and dependencies
  - **Status**: All environments build successfully and tested on hardware

- **Simple Position Test** (`/examples/component_tests/SimplePositionTest.cpp`)
  - Direct testing of position detection
  - Visual LED feedback for detected positions
  - Serial output of raw sensor data and detected positions
  - Uses calibrated thresholds from Config.h
  - **Status**: [VERIFIED] (2025-03-27, Successfully tested on hardware)

#### In Progress Implementation
- **Idle Mode** (`/src/modes/IdleMode.cpp`)
  - Position color visualization
  - Testing and feedback functionality
  - Position tracking
  - Performance monitoring
  - **Status**: Design completed, implementation pending

## Documented But Missing Implementations

### Ultra Basic Position Detection (UBPD) - PRIMARY DETECTION MODEL
- **Documentation**: Comprehensive documentation available in `working/UBPD.md`
- **Implemented Files**: 
  - `UltraBasicPositionDetector.h/cpp` - [IMPLEMENTED] (Build successful, pending hardware validation)
  - `/examples/component_tests/UltraBasicPositionTest.cpp` - [IMPLEMENTED] (Build successful, pending hardware validation)
  - `env:ultrabasic` environment in platformio.ini - [IMPLEMENTED]
- **Purpose**:
  - This is the intended PRIMARY detection model for the entire device
  - Designed to replace the current PositionDetector implementation with a more optimized approach
  - Key feature is conversion of raw data to meaningful physical units (m/s²)
- **Key Features**:
  - Direct dominant axis detection with minimal processing
  - Physical units (m/s²) for intuitive thresholds and better debugging
  - Absolute threshold values rather than relative comparisons
  - Simplified, consistent processing pipeline
  - Enhanced calibration protocol with buffer time and sample discarding
  - Position-specific threshold adjustments
  - Clear gravity orientation handling
  - Serial command-based calibration (type 'c' to enter calibration mode)
- **Status**: [IMPLEMENTED] (2025-03-27, Build successful, pending hardware validation)

### Experimental Detection Models (NOT NEEDED)
The following detection models were experimental approaches that proved overly complex and less reliable in testing. These implementations do NOT need to be recovered, as the UBPD model is the intended final approach:

- **Hand-Cube Orientation Model**: A vector-based approach that was found to be unnecessarily complex.
  - **Status**: [DEPRECATED] (Will not be implemented)
  
- **Hybrid Position Detection**: A combined approach that added complexity without sufficient reliability improvements.
  - **Status**: [DEPRECATED] (Will not be implemented)

## Core Components

### Hardware Layer
- `MPU9250Interface`: Sensor communication and data reading
  - **Status**: [VERIFIED] (2025-03-25, Successfully tested on hardware)
  - **Verification**: Successfully tested on hardware
- `I2CScanner`: I2C diagnostics and connection verification
  - **Status**: [VERIFIED] (2025-03-25, Successfully tested on hardware)
  - **Verification**: Successfully tested on hardware
- `LEDInterface`: LED control and animation management
  - **Status**: [VERIFIED] (2025-03-26, Successfully tested on hardware with all animations functioning)
  - **Verification**: Successfully tested on hardware with all animations functioning
- `HardwareManager`: Hardware resource coordination
  - **Status**: [VERIFIED] (2025-03-26, Successfully tested with integration of MPU and LED components)
  - **Verification**: Successfully tested with integration of MPU and LED components

### Position Detection System
- `PositionDetector`: Position detection logic
  - **Status**: [VERIFIED] (2025-03-27, Successfully tested using SimplePositionTest)
  - **Verification**: Successfully tested using SimplePositionTest
  - **Features**: 
    - Point Detection Model with threshold-based classification
    - 5-sample averaging for noise reduction
    - Dominant axis detection for six hand positions
    - TEST_MODE guards for test-specific functionality
  - **Note**: Planned to be replaced by UltraBasicPositionDetector for production
  
- `UltraBasicPositionDetector`: Ultra Basic Position Detection implementation
  - **Status**: [IMPLEMENTED] (Implementation completed, build successful)
  - **Verification**: Not yet validated on hardware
  - **Features**: 
    - Physical unit conversion to m/s² implemented
    - Absolute thresholds in physical units implemented
    - 3-sample averaging for noise reduction implemented
    - Position-specific threshold management implemented

### Calibration and Analysis
- `CalibrationProtocol.cpp`: Calibration sequence implementation
  - **Status**: [VERIFIED] (2025-03-26, Successfully collected calibration data for all positions)
  - **Verification**: Successfully collected calibration data for all positions
- `calibration_logger.py`: Data collection utility
  - **Status**: [VERIFIED] (2025-03-26, Successfully logged calibration data)
  - **Verification**: Successfully logged calibration data
- `analyze_calibration.py`: Data analysis utility
  - **Status**: [VERIFIED] (2025-03-26, Successfully generated threshold values from calibration data)
  - **Verification**: Successfully generated threshold values from calibration data

### Core Systems
- `GauntletController`: System coordination
  - **Status**: [IN PROGRESS] (Header implemented, implementation file exists but functionality pending)
  - **Verification**: Not yet verified
- `GestureRecognizer`: Gesture recognition
  - **Status**: [PLANNED] (Header defined, implementation pending)
  - **Verification**: Not yet verified
- `AnimationSystem`: LED animation management
  - **Status**: [PLANNED] (Not yet implemented)
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
- Position Detection System
  - Position Detector implementation using dominant axis model
  - Simple averaging for noise reduction
  - Threshold-based detection for six hand positions
  - Memory-efficient implementation with circular buffer
  - TEST_MODE guards for test/production separation
- Test Environment Implementation
  - Multiple PlatformIO environments for different testing scenarios
  - Consistent build configurations
  - Hybrid approach for test and production code
  - Simple position test application
  - Hardware-verified test environments

### Priority Implementation Needed
- Ultra Basic Position Detection implementation (PRIMARY FOCUS)
  - Physical unit conversion (raw to m/s²)
  - Absolute thresholds in real-world units
  - Optimized calibration protocol
  - Simplified detection logic

### In Progress
- Position Detection Refinement
  - Fine-tuning thresholds based on testing feedback
  - Addressing detection stability issues
  - Improving transition behavior
- Idle Mode Implementation
  - Position visualization design
  - Real-time position feedback
  - LED animation integration

## Test Environment Structure

### Simple Position Test
**File**: `/examples/component_tests/SimplePositionTest.cpp`
**Environment**: env:simplepostest
**Purpose**: Basic testing of position detection with visual feedback
**Features**:
- Real-time sensor data output
- Position detection with LED feedback
- Uses calibrated thresholds from Config.h
- Clean TEST_MODE guards for isolation
**Status**: [VERIFIED] (2025-03-27, Successfully tested on hardware)

### Position Detector Test
**File**: `/examples/component_tests/PositionDetectorTest.cpp`
**Environment**: env:postest
**Purpose**: More comprehensive testing of position detection system
**Features**:
- Advanced position debugging
- Transition analysis
- Duration measurements
- Extended serial output
**Status**: [VERIFIED] (2025-03-27, Successfully tested on hardware)

### Hardware Manager Test
**File**: `/examples/HardwareManagerTest.cpp`
**Environment**: env:hwmtest
**Purpose**: Integration testing of all hardware components
**Features**:
- Hardware initialization verification
- Sensor data reading
- LED control demonstration
- Power state management
**Status**: [VERIFIED] (2025-03-26, Successfully tested on hardware)

### Calibration Protocol
**File**: `/examples/CalibrationProtocol.cpp`
**Environment**: env:calibration
**Purpose**: Collecting calibration data for threshold generation
**Features**:
- Guided calibration sequence
- Position-specific data collection
- LED visual indicators
- Serial CSV output
**Status**: [VERIFIED] (2025-03-26, Successfully collected calibration data)

### Ultra Basic Position Test (Planned)
**File**: `/examples/component_tests/UltraBasicPositionTest.cpp`
**Environment**: env:ultrabasic
**Purpose**: Testing of UBPD implementation with physical unit display
**Features**:
- Real-time sensor data in m/s²
- Position detection with LED feedback
- Simple calibration capability
- Debug output of physical units
**Status**: [PLANNED] (Implementation scheduled for Phase 2 of Hybrid Plan)

## Class Relationships

### Hardware Layer Relationships
```
HardwareManager
├── MPU9250Interface (sensor data acquisition)
├── LEDInterface (visual feedback)
└── PowerManager (power state control)
```

### Detection Layer Relationships
```
GauntletController
├── HardwareManager (hardware access)
├── PositionDetector (position detection)
│   └── HardwareManager* (sensor data access)
├── GestureRecognizer (gesture detection)
│   └── PositionReading[] (position history)
└── AnimationSystem (visual feedback)
    └── LEDInterface* (LED control)
```

### Calibration Relationships
```
CalibrationProtocol
├── HardwareManager (hardware access)
│   ├── MPU9250Interface (sensor data)
│   └── LEDInterface (visual feedback)
└── Serial (data output)

analyze_calibration.py
├── calibration_data_*.csv (input)
└── suggested_thresholds.txt (output)
```

## Directory Structure Details

### /src/core
Contains core system definitions, types, and configuration:
- `Config.h/.cpp`: System-wide configuration parameters including calibrated thresholds
- `SystemTypes.h`: Data structure definitions and enumerations
- `GauntletController.h/.cpp`: Main system controller (implementation in progress)

### /src/hardware
Hardware abstraction layer components:
- `MPU9250Interface.h/.cpp`: Sensor interface implementation
- `LEDInterface.h/.cpp`: LED control with FastLED
- `HardwareManager.h/.cpp`: Unified hardware management
- `PowerManager.h/.cpp`: Power state control

### /src/detection
Position and gesture detection components:
- `PositionDetector.h/.cpp`: Hand position detection using dominant axis model
- `GestureRecognizer.h`: Gesture detection based on position sequences (header only)
- `CalibrationRoutine.h`: Calibration procedure definitions (header only)

### /examples
Standalone test applications:
- `/component_tests/SimplePositionTest.cpp`: Position detection test with visual feedback
- `/component_tests/PositionDetectorTest.cpp`: Comprehensive position detection testing
- `HardwareManagerTest.cpp`: Hardware integration test
- `CalibrationProtocol.cpp`: Calibration data collection application
- `LEDTest.cpp`: LED interface testing
- `MPUDiagnosticTest.cpp`: MPU sensor diagnostic testing
- `MPUFilterTest.cpp`: MPU data processing and filtering test

### /utils
Python utilities for calibration:
- `calibration_logger.py`: Serial data collection
- `analyze_calibration.py`: Statistical analysis of calibration data

### /logs
Data and analysis logs:
- `calibration_data_*.csv`: Raw calibration data
- `suggested_thresholds.txt`: Generated threshold values 

## Recovery Plan

To address the missing implementations and restore the project to its intended state, we are following the Hybrid Plan outlined in currentplan_v2.md, which combines process improvements with UBPD implementation.

### Current Status
- **Phase 1: Immediate Process Stabilization**: [IN PROGRESS]
  - Git Backup Protocol finalized
  - Working documents being updated to reflect accurate status
  
- **Phase 2: UBPD Implementation**: [PLANNED]
  - Scheduled to begin after completion of Phase 1
  
- **Phase 3: Documentation and Integration**: [PLANNED]
  - Will follow successful UBPD implementation 