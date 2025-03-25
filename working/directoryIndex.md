# PrismaTech Gauntlet 3.0 - Directory Index

## Overview

This document provides a comprehensive index of the PrismaTech Gauntlet 3.0 project structure, including directories, key files, and their relationships.

## Top-Level Directory Structure

- **/src** - Main source code for the application
- **/include** - External library header files
- **/lib** - External libraries
- **/reference** - Reference documents and design specifications
- **/working** - Working documents and project notes

## Source Code Organization

The source code follows a layered architecture focused on embedded efficiency:

### Core Layer (`/src/core`)

Central system organization and state management.

- **GauntletController.h/cpp** - Main system controller coordinating all subsystems
- **SystemTypes.h** - Common data structures and enumerations
- **Config.h** - System-wide configuration parameters

### Hardware Layer (`/src/hardware`)

Hardware interfaces and abstractions.

- **HardwareManager.h** - Coordinates all hardware components (header only)
- **MPU9250Interface.h** - IMU sensor interface (header only)
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

- **CircularBuffer.h** - Generic circular buffer implementation
- **FixedMath.h** - Fixed-point math utilities for efficient calculations
- **DebugTools.h/cpp** - Debugging and diagnostic tools

## Key Class Relationships

```
GauntletController
  ├── HardwareManager
  │     ├── MPU9250Interface
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