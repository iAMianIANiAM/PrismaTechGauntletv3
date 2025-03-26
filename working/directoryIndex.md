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
  - **Status**: Implementation complete and tested on hardware

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
  - **Status**: Successfully tested on hardware, most positions detected

#### In Progress Implementation
- **Idle Mode** (`/src/modes/IdleMode.cpp`)
  - Position color visualization
  - Testing and feedback functionality
  - Position tracking
  - Performance monitoring
  - **Status**: Design completed, implementation pending

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

### Position Detection System
- `PositionDetector`: Position detection logic
  - **Status**: Implementation complete, tested on hardware
  - **Verification**: Successfully tested using SimplePositionTest
  - **Features**: 
    - Point Detection Model with threshold-based classification
    - 5-sample averaging for noise reduction
    - Dominant axis detection for six hand positions
    - TEST_MODE guards for test-specific functionality

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

### Position Detector Test
**File**: `/examples/component_tests/PositionDetectorTest.cpp`
**Environment**: env:postest
**Purpose**: More comprehensive testing of position detection system
**Features**:
- Advanced position debugging
- Transition analysis
- Duration measurements
- Extended serial output

### Hardware Manager Test
**File**: `/examples/HardwareManagerTest.cpp`
**Environment**: env:hwmtest
**Purpose**: Integration testing of all hardware components
**Features**:
- Hardware initialization verification
- Sensor data reading
- LED control demonstration
- Power state management

### Calibration Protocol
**File**: `/examples/CalibrationProtocol.cpp`
**Environment**: env:calibration
**Purpose**: Collecting calibration data for threshold generation
**Features**:
- Guided calibration sequence
- Position-specific data collection
- LED visual indicators
- Serial CSV output

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
- `/component_tests/NormalizedPositionTest.cpp`: Test application for normalized position detection
- `/component_tests/VectorOrientationTest.cpp`: Test application for the Hand-Cube Orientation Model

### /utils
Python utilities for calibration:
- `calibration_logger.py`: Serial data collection
- `analyze_calibration.py`: Statistical analysis of calibration data

### /logs
Data and analysis logs:
- `calibration_data_*.csv`: Raw calibration data
- `suggested_thresholds.txt`: Generated threshold values 

### /detection
- Contains position detection logic
- `PositionDetector.h`: Declares position detection interface
  - Defines `PositionDetector` class with methods for detecting hand positions
  - Implements threshold-based dominant axis detection
  - Provides normalization capabilities for robust detection
  - Implements Hand-Cube Orientation Model using vector-based detection
  - Supports dual detection modes with runtime switching
  - Includes comprehensive calibration methods for both approaches 

### Hand-Cube Orientation Model
- Vector-based position detection approach implemented in `PositionDetector.h/cpp`
- Test environment created in `env:vectortest`
- Test application in `/examples/component_tests/VectorOrientationTest.cpp`
- Features:
  - 3D orientation vector representation for hand positions
  - Reference vectors for six cube faces (hand positions)
  - Vector similarity calculation using dot product
  - Confidence calculation based on similarity scores
  - Interactive calibration process
  - Multiple debugging display modes
  - Real-time visual feedback through LEDs
- Current status: Implementation complete, awaiting hardware verification 