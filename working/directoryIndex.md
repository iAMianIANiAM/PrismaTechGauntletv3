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
/working        - Active working documents and current project state
/reference      - Technical specifications and documentation
/archive        - Historical project artifacts and archived documents
  /chronicles   - Archived chronicle documents
  /plans        - Archived planning documents 
  /thresholds   - Archived threshold configurations
  /workdocs     - Other archived working documents
```

## Implementation Status Indicators

This document uses the following status indicators:

- **[PLANNED]**: Designed but not yet implemented
- **[IN PROGRESS]**: Implementation started but not complete  
- **[IMPLEMENTED]**: Code exists and functions in test environments but not fully verified in system context
- **[VERIFIED]**: Implementation exists and has been fully tested in production context with all system integrations
- **[DEPRECATED]**: Was implemented but has been replaced

### Verification Standards
For consistency across the project, these verification distinctions are observed:
- **Test Environment Functionality**: Component works in isolation within a specific test environment
- **System Integration Verification**: Component functions correctly within the full system context
- **Production Verification**: Component meets all performance, reliability and user experience requirements in production environment

Only components that meet the full System Integration Verification criteria should be marked as [VERIFIED].

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
  - Hardware tested successfully in test environment
  
- **MPU9250 Data Processing** (`/src/hardware/MPU9250Interface.*`)
  - Simple filtering implementation with averaging
  - Data validation and consistency checks
  - Enhanced error recovery mechanisms
  - Motion magnitude calculation
  - Maximum axis data collection
  - Hardware tested successfully in test environment
  
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
  - Hardware tested successfully in test environment
  
- **Hardware Manager** (`/src/hardware/HardwareManager.*`)
  - Unified hardware initialization
  - Resource management
  - Power state control
  - Error recovery system
  - Hardware status monitoring
  - Hardware tested successfully in test environment
  
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
  - **Status**: [IMPLEMENTED] but [DEPRECATED] (Superseded by UBPD)
  - **Note**: This is a functional implementation but has been superseded by UltraBasicPositionDetector for production use.

- **Test Environments** (`platformio.ini`)
  - Main production environment (env:esp32dev)
  - Simple position test environment (env:simplepostest)
  - Position detector test environment (env:postest)
  - Hardware manager test environment (env:hardware_test)
  - Calibration environment (env:calibration)
  - Consistent build configurations and dependencies
  - **Status**: All environments build successfully and tested on hardware

- **Simple Position Test** (`/examples/component_tests/SimplePositionTest.cpp`)
  - Direct testing of position detection
  - Visual LED feedback for detected positions
  - Serial output of raw sensor data and detected positions
  - Uses calibrated thresholds from Config.h
  - **Status**: [IMPLEMENTED] (2025-03-27, Successfully tested in test environment)

#### In Progress Implementation
- **Idle Mode** (`/src/modes/IdleMode.cpp`)
  - Position color visualization
  - Testing and feedback functionality
  - Position tracking
  - Performance monitoring
  - **Status**: [IN PROGRESS] (Successfully tested with IdleModeTest.cpp)

## Key Implementations

### Ultra Basic Position Detection (UBPD) - PRIMARY DETECTION MODEL
- **Documentation**: Comprehensive documentation available in `reference/UBPD.md`
- **Implementation Files**: 
  - `src/detection/UltraBasicPositionDetector.h/cpp` - [IMPLEMENTED] (2025-03-25, Successfully tested with calibrated thresholds)
  - `examples/component_tests/UltraBasicPositionTest.cpp` - [IMPLEMENTED] (2025-03-25, Successfully tested with calibration output)
  - `env:ubpd_test` environment in platformio.ini - [IMPLEMENTED]
  - Main application integration in `src/main.cpp` - [IMPLEMENTED] (Successfully tested in test environment)
- **Purpose**:
  - This is the PRIMARY detection model for the entire device
  - Designed as a more optimized approach to position detection
  - Key feature is conversion of raw data to meaningful physical units (m/s²)
- **Key Features**:
  - Direct dominant axis detection with minimal processing
  - Physical units (m/s²) for intuitive thresholds and better debugging
  - Absolute threshold values with explicit definition in Config.h
  - Simplified, consistent processing pipeline
  - Enhanced calibration protocol with threshold output in copy-paste format
  - Position-specific threshold adjustments
  - Clear gravity orientation handling
  - Serial command-based calibration (type 'c' to enter calibration mode)
  - Threshold display command (type 't' during testing)
  - Quick threshold tuning through direct Config.h edits
- **Status**: [IMPLEMENTED] (2025-03-25, Successfully tested in test environment, pending full system verification)

### Threshold Management
- **Documentation**: Implementation details in chronicle_v1.md
- **Implementation Files**: 
  - `src/core/ThresholdManager.h/cpp` - [IMPLEMENTED] (Actively used in the codebase)
- **Purpose**:
  - Provides EEPROM-based persistent storage for calibration thresholds
  - Centralizes threshold management across the application
  - Support for backup/restore functionality via JSON files
- **Current Usage**:
  - Enabled via the USE_THRESHOLD_MANAGER build flag in platformio.ini
  - Used in main application and test environments
  - Provides failsafe mechanisms with default thresholds
- **Status**: [IMPLEMENTED] Active in test environments with build flag support, pending full system verification

### Experimental Detection Models (DEPRECATED)
The following detection models were experimental approaches that proved overly complex and less reliable in testing. These implementations have been deprecated as the UBPD model is the primary detection approach:

- **Hand-Cube Orientation Model**: A vector-based approach that was found to be unnecessarily complex.
  - **Status**: [DEPRECATED] (Will not be implemented)
  
- **Hybrid Position Detection**: A combined approach that added complexity without sufficient reliability improvements.
  - **Status**: [DEPRECATED] (Will not be implemented)

## Core Components

### Hardware Layer
- `MPU9250Interface`: Sensor communication and data reading
  - **Status**: [IMPLEMENTED] (2025-03-25, Successfully tested on hardware in test environment)
  - **Verification**: Successfully tested in test environment
- `I2CScanner`: I2C diagnostics and connection verification
  - **Status**: [IMPLEMENTED] (2025-03-25, Successfully tested on hardware in test environment)
  - **Verification**: Successfully tested in test environment
- `LEDInterface`: LED control and animation management
  - **Status**: [IMPLEMENTED] (2025-03-26, Successfully tested on hardware with all animations functioning)
  - **Verification**: Successfully tested in test environment with all animations functioning
- `HardwareManager`: Hardware resource coordination
  - **Status**: [IMPLEMENTED] (2025-03-26, Successfully tested with integration of MPU and LED components)
  - **Verification**: Successfully tested with component integration in test environment

### Position Detection System
- `PositionDetector`: Position detection logic
  - **Status**: [IMPLEMENTED] but [DEPRECATED] (Superseded by UBPD)
  - **Verification**: Successfully tested using SimplePositionTest
  - **Features**: 
    - Point Detection Model with threshold-based classification
    - 5-sample averaging for noise reduction
    - Dominant axis detection for six hand positions
    - TEST_MODE guards for test-specific functionality
  - **Note**: Has been superseded by UltraBasicPositionDetector for production use
  
- `UltraBasicPositionDetector`: Primary position detection implementation
  - **Status**: [IMPLEMENTED] (2025-03-25, Successfully tested with calibrated thresholds)
  - **Verification**: Successfully tested in test environment with all positions
  - **Features**: 
    - Physical unit conversion to m/s² implemented
    - Absolute thresholds in physical units implemented
    - 3-sample averaging for noise reduction implemented
    - Position-specific threshold management implemented
    - Serial command-based calibration support
    - Format-ready configuration output
  - **Note**: This is the PRIMARY detection model used in production

### Calibration and Analysis
- `CalibrationProtocol.cpp`: Calibration sequence implementation
  - **Status**: [IMPLEMENTED] (2025-03-26, Successfully collected calibration data for all positions)
  - **Verification**: Successfully tested in test environment
- `calibration_logger.py`: Data collection utility
  - **Status**: [IMPLEMENTED] (2025-03-26, Successfully logged calibration data)
  - **Verification**: Successfully tested with data collection
- `analyze_calibration.py`: Data analysis utility
  - **Status**: [IMPLEMENTED] (2025-03-26, Successfully generated threshold values from calibration data)
  - **Verification**: Successfully tested with threshold generation

### Core Systems
- `GauntletController`: System coordination
  - **Status**: [IMPLEMENTED] (Core functionality implemented, supports mode transitions)
  - **Verification**: Partially verified through IdleModeTest
- `GestureRecognizer`: Gesture recognition
  - **Status**: [PLANNED] (Header defined, implementation pending)
  - **Verification**: Not yet verified
- `AnimationSystem`: LED animation management
  - **Status**: [IN PROGRESS] (Header implemented, implementation pending)
  - **Verification**: Not yet verified
  - **Components**:
    - `AnimationSystem.h`: Main animation system coordination (header only)
    - `AnimationData.h`: Animation data structures and definitions (header only)
    - `FreecastGenerator.h`: Freecast mode animation generator (header only)
  - **Note**: Currently header-only implementations with design completed

## Operational Modes Implementation Status

### Idle Mode
- **File**: `src/modes/IdleMode.cpp`
- **Status**: [IN PROGRESS] (Successfully tested with IdleModeTest.cpp)
- **Features Implemented**:
  - Position color visualization (four LEDs at 80% brightness)
  - Color transition effects with interpolation
  - Position tracking and state management
  - Gesture detection (CalmOffer and LongNull)
  - Mode transition handling
- **Test Environment**: env:idlemode_test
- **Note**: Functions in isolation but not yet verified in full system context

### Invocation Mode
- **Status**: [PLANNED] (Design complete per TrueFunctionGuide, code not implemented)
- **Features Designed**:
  - Three-slot position recording
  - Four-phase slot filling (Indicator, Countdown, Checkpoint, Confirmation)
  - Visual feedback for slot states
  - Position color mapping
- **Note**: Placeholder exists in GauntletController for future implementation

### Resolution Mode
- **Status**: [PLANNED] (Design complete per TrueFunctionGuide, code not implemented)
- **Features Designed**:
  - Spell effect determination based on slot colors
  - Unique animations for each spell type
  - Fixed durations for definite spells
  - Return sequence to Idle Mode
- **Note**: Placeholder exists in GauntletController for future implementation

### Freecast Mode
- **Status**: [PLANNED] (Design complete per TrueFunctionGuide, code not implemented)
- **Features Designed**:
  - Motion-to-pattern translation
  - Alternating data collection and pattern generation
  - Motion intensity mapping to visual elements
  - LongNull exit gesture handling
- **Note**: Placeholder exists in GauntletController for future implementation

## Implementation Status

### Completed
The following components have been implemented and tested in test environments:
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
- Hardware testing verification in test environments
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
- Ultra Basic Position Detection (UBPD) implementation
  - Physical unit conversion (raw to m/s²)
  - Absolute thresholds in real-world units
  - Optimized calibration protocol
  - Simplified detection logic
- Test Environment Implementation
  - Multiple PlatformIO environments for different testing scenarios
  - Consistent build configurations
  - Hybrid approach for test and production code
  - Simple position test application
  - Hardware-tested test environments
- Config system implementation
  - Header-only implementation with inline constexpr definitions
  - System-wide configuration parameters
  - Calibrated position detection thresholds
  - LED color definitions
  - Timing parameters

All of these components have been successfully implemented and tested in appropriate test environments, but full system integration verification is still pending.

### In Progress
- Architecture Mapping
  - Documenting component relationships 
  - Analyzing data flow patterns
  - Mapping state transitions
- Idle Mode Implementation & Enhancement
  - Fine-tuning visual feedback
  - Implementing mode transitions
  - Testing gesture detection reliability
  - Ensuring full system integration
  - Resolving architecture mismatches

### Planned
- Invocation Mode Implementation
- Resolution Mode Implementation
- Freecast Mode Implementation
- Comprehensive testing framework

## Known Discrepancies

This section documents intentional or identified discrepancies between the reference documentation and the actual implementation.

### 1. Hand Position Order
- **TrueFunctionGuide Order**: The TrueFunctionGuide.md defines hand positions in this order: Offer, Calm, Oath, Dig, Shield, Null
- **Implementation Order**: The codebase (particularly in enumerations) often uses a different order 
- **Rationale**: This discrepancy is maintained for compatibility with existing code and to avoid extensive refactoring
- **Impact**: No functional impact, but developers should be aware when reading the code that the enumeration order may not match the documentation order
- **Future Plans**: May be harmonized in a future refactoring effort if deemed necessary

### 2. UltraBasicPositionDetector vs. PositionDetector
- **Documentation Clarity**: The DirectoryIndex previously did not clearly explain the relationship between these components
- **Actual Relationship**: UBPD is not a simpler version but rather an improved implementation that replaced the original PositionDetector
- **Key Enhancements**:
  - UBPD converts raw values to physical units (m/s²) for more intuitive thresholds
  - Provides better calibration capabilities with copy-paste ready configuration values
  - Offers more reliable position detection with position-specific threshold adjustments
  - Is the PRIMARY detection model used in production
- **Status**: PositionDetector is [IMPLEMENTED] but [DEPRECATED], while UBPD is [IMPLEMENTED] and actively used

### 3. Duplicate "examples" Directories
- **Issue**: Two separate directories named "examples" exist:
  - `/examples` (project root)
  - `/src/examples` (within source)
- **Current Approach**: Both directories are maintained for backward compatibility
- **Impact**: May cause confusion when searching for example files
- **Future Plans**: Add to backlog a comprehensive duplicate directory audit and consolidation plan
- **Note**: No immediate structure changes to avoid build breakage

### 4. Config.cpp Reference
- **Issue**: The `platformio.ini` refers to a non-existent `Config.cpp` file in multiple build environments
- **Actual Implementation**: `Config.h` is a header-only implementation with inline constexpr definitions
- **Impact**: Despite the non-existent file reference, builds complete successfully
- **Rationale**: The reference appears to be vestigial from an earlier implementation approach
- **Future Plans**: Add to backlog a low-priority cleanup task for when other build config changes are needed

## Test Environment Structure

### Simple Position Test
**File**: `/examples/component_tests/SimplePositionTest.cpp`
**Environment**: env:simplepostest (archived in platformio.ini)
**Purpose**: Basic testing of position detection with visual feedback
**Features**:
- Real-time sensor data output
- Position detection with LED feedback
- Uses calibrated thresholds from Config.h
- Clean TEST_MODE guards for isolation
**Status**: [IMPLEMENTED] (2025-03-27, Successfully tested in test environment)

### Position Detector Test
**File**: `/examples/component_tests/PositionDetectorTest.cpp`
**Environment**: env:postest (archived in platformio.ini)
**Purpose**: More comprehensive testing of position detection system
**Features**:
- Advanced position debugging
- Transition analysis
- Duration measurements
- Extended serial output
**Status**: [IMPLEMENTED] (2025-03-27, Successfully tested in test environment)

### Hardware Manager Test
**File**: `/examples/HardwareManagerTest.cpp`
**Environment**: env:hardware_test (active in platformio.ini)
**Purpose**: Integration testing of all hardware components
**Features**:
- Hardware initialization verification
- Sensor data reading
- LED control demonstration
- Power state management
**Status**: [IMPLEMENTED] (2025-03-26, Successfully tested in test environment)

### Calibration Protocol
**File**: `/examples/CalibrationProtocol.cpp`
**Environment**: env:calibration
**Purpose**: Collecting calibration data for threshold generation
**Features**:
- Guided calibration sequence
- Position-specific data collection
- LED visual indicators
- Serial CSV output
**Status**: [IMPLEMENTED] (2025-03-26, Successfully tested with data collection)

### Ultra Basic Position Test
**File**: `/examples/component_tests/UltraBasicPositionTest.cpp`
**Environment**: env:ubpd_test
**Purpose**: Testing of UBPD implementation with physical unit display
**Features**:
- Real-time sensor data in m/s²
- Position detection with LED feedback
- Serial command-based calibration capability
- Debug output of physical units
- Threshold display command support
- Copy-paste ready configuration output
**Status**: [IMPLEMENTED] (2025-03-25, Successfully tested in test environment)

### Idle Mode Test
**File**: `/examples/IdleModeTest.cpp`
**Environment**: env:idlemode_test
**Purpose**: Testing of Idle Mode functionality and mode transitions
**Features**:
- GauntletController integration
- Position tracking and visualization
- Color transition testing
- Command-based configuration
- Interactive serial interface
**Status**: [IMPLEMENTED] (2025-03-28, Successfully tested in test environment)

## Active Testing Environments

The project currently maintains the following PlatformIO environments:

- **env:esp32dev**: Main production environment
  - Full application build with UBPD integration
  - ThresholdManager support enabled
  - Production-ready configuration
  - **Status**: [IMPLEMENTED] (Build verified, pending full system verification)

- **env:ubpd_test**: Primary test environment for UBPD 
  - UltraBasicPositionTest application
  - Physical unit conversion and threshold visualization
  - Interactive calibration capabilities
  - **Status**: [IMPLEMENTED] (2025-03-25, Successfully tested in test environment)

- **env:calibration**: Calibration data collection environment
  - Structured position calibration sequence
  - CSV data output for threshold analysis
  - Visual feedback during calibration
  - **Status**: [IMPLEMENTED] (2025-03-26, Successfully tested with data collection)

- **env:hardware_test**: Hardware integration testing environment
  - Component initialization verification
  - Sensor data reading and validation
  - LED control capabilities testing
  - **Status**: [IMPLEMENTED] (2025-03-26, Successfully tested in test environment)

- **env:idlemode_test**: Idle mode testing environment
  - GauntletController and IdleMode integration
  - Visual feedback for hand positions
  - Gesture detection testing
  - Mode transition testing
  - **Status**: [IMPLEMENTED] (2025-03-28, Successfully tested in test environment)

All environments have been verified to build successfully and operate correctly on the target hardware within their respective test contexts. Full system integration verification is pending for all environments.

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
- `Config.h`: System-wide configuration parameters including calibrated thresholds (header-only implementation with inline constexpr definitions)
  - **Note**: Build configurations in platformio.ini reference a non-existent `Config.cpp` file as a vestigial artifact, but the header-only implementation is sufficient and the system builds correctly
- `SystemTypes.h`: Data structure definitions and enumerations
- `GauntletController.h/.cpp`: Main system controller (implementation in progress)
- `ThresholdManager.h/.cpp`: Threshold management and persistence system

### /src/hardware
Hardware abstraction layer components:
- `MPU9250Interface.h/.cpp`: Sensor interface implementation
- `LEDInterface.h/.cpp`: LED control with FastLED
- `HardwareManager.h/.cpp`: Unified hardware management
- `PowerManager.h/.cpp`: Power state control

### /src/detection
Position and gesture detection components:
- `PositionDetector.h/.cpp`: Hand position detection using dominant axis model
- `UltraBasicPositionDetector.h/.cpp`: UBPD implementation with physical unit conversion
- `GestureRecognizer.h`: Gesture detection based on position sequences (header only)
- `CalibrationRoutine.h`: Calibration procedure definitions (header only)

### /src/modes
Operational mode implementations:
- `IdleMode.h/.cpp`: Idle mode implementation with position visualization

### /src/animation
Animation system components:
- `AnimationSystem.h`: Main animation system coordination (header only)
- `AnimationData.h`: Animation data structures and definitions (header only)
- `FreecastGenerator.h`: Freecast mode animation generator (header only)

### /src/utils
Support utilities:
- `DebugTools.h/.cpp`: Debugging and logging utilities
- `I2CScanner.h`: I2C diagnostics and connection verification
- `FixedMath.h`: Fixed-point math utilities for embedded systems
- `CircularBuffer.h`: Efficient circular buffer implementation

### /examples
Standalone test applications:
- `/component_tests/SimplePositionTest.cpp`: Position detection test with visual feedback
- `/component_tests/PositionDetectorTest.cpp`: Comprehensive position detection testing
- `/component_tests/UltraBasicPositionTest.cpp`: UBPD testing with physical unit display
- `HardwareManagerTest.cpp`: Hardware integration test
- `CalibrationProtocol.cpp`: Calibration data collection application
- `IdleModeTest.cpp`: Idle mode testing application
- `LEDTest.cpp`: LED interface testing
- `MPUDiagnosticTest.cpp`: MPU sensor diagnostic testing
- `MPUFilterTest.cpp`: MPU data processing and filtering test

### /utils
Python utilities for calibration and development:
- `calibration_logger.py`: Serial data collection
- `analyze_calibration.py`: Statistical analysis of calibration data
- `CALIBRATION_GUIDE.md`: Comprehensive guide for the calibration process
- `pre-commit`: Git hook for pre-commit validation
- `verify_docs.py`: Documentation verification script
- `README.md`: Usage instructions for utility scripts

### /logs
Data and analysis logs:
- `calibration_data_*.csv`: Raw calibration data
- `suggested_thresholds.txt`: Generated threshold values 

### /src (Root)
Main application files:
- `main.cpp`: Primary application entry point
  - Initializes hardware components
  - Configures UltraBasicPositionDetector
  - Implements main control loop for position detection
  - Provides visual LED feedback for detected positions
  - **Status**: [IMPLEMENTED] (Successfully tested in test environment)