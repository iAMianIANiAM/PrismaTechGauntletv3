# PrismaTech Gauntlet 3.0

A wearable gesture-based interface that enables "spellcasting" through specific hand orientations and movement patterns.

## Overview

The PrismaTech Gauntlet 3.0 is a wearable electronic device that uses an IMU (Inertial Measurement Unit) to detect hand positions and gestures, providing visual feedback through an LED ring. The device creates an interactive spellcasting experience through position detection, gesture recognition, and dynamic LED animations.

## Features

- **Hand Position Detection**: Recognizes six distinct hand positions using dominant axis detection
- **Gesture Recognition**: Detects sequential and held gestures to trigger various effects
- **QuickCast Spells**: Trigger rapid visual effects through specific position transitions
- **Freecast Mode**: Generate dynamic, motion-responsive light patterns 
- **Universal Cancellation**: Simple shake gesture to exit any operational mode
- **Visual Feedback**: Rich LED animations that respond to user actions

## Operational Modes

### Idle Mode (Default)
- Continuously monitors hand position and displays corresponding color
- Detects QuickCast gestures and Freecast mode entry
- Provides smooth color transitions between positions

### QuickCast Spells Mode
Three predefined spell effects triggered by specific hand position transitions:
- **Rainbow Burst** (CalmOffer): Multi-phase rainbow animation with increasing intensity
- **Lightning Blast** (DigOath): Flash white with colored lightning crackles
- **Lumina** (NullShield): Steady white light for utility purposes (flashlight)

### Freecast Mode
- Enter by holding the Shield position for 5 seconds
- Creates dynamic light patterns based on hand movement
- Alternates between 2-second motion capture and 2-second pattern display
- Exit by holding Shield again or using the ShakeCancel gesture

## Hardware Requirements

- ESP32 microcontroller
- MPU-9250 9-axis IMU sensor (or compatible)
- WS2812 12-LED RGB ring
- Portable power source

## Project Structure

```
/
├── src/                  # Main firmware source code
│   ├── core/             # Core subsystem (Config, Controller, Types)
│   ├── modes/            # Operational modes implementation
│   ├── hardware/         # Hardware abstraction layer
│   ├── detection/        # Position and gesture detection
│   ├── animation/        # LED patterns and effects
│   ├── utils/            # Utility functions
│   └── diagnostics/      # System monitoring and troubleshooting
├── examples/             # Example applications and tests
├── test/                 # Test framework
├── reference/            # Reference documentation
├── working/              # Active working documents
└── platformio.ini        # PlatformIO configuration
```

## Key Components

- **GauntletController**: Main system controller managing mode transitions
- **UltraBasicPositionDetector**: Position detection using dominant axis algorithm
- **GestureTransitionTracker**: Tracks transitions between hand positions
- **ShakeGestureDetector**: Detects shake motion for universal cancellation
- **HardwareManager**: Central manager for all hardware interfaces
- **LEDInterface**: Controls the LED display patterns

## Getting Started

### Building and Uploading

```bash
# Main application
pio run -e esp32dev -t upload

# Calibration environment
pio run -e calibration -t upload

# Function test environment
pio run -e functionTest -t upload
```

### Monitor Serial Output

```bash
pio device monitor -p COM7 -b 115200
```

## Documentation

- **TrueFunctionGuide**: Authoritative specification for system behavior
- **LEDPatternMap**: Documentation of all LED patterns
- **DirectoryIndex**: Codebase structure and relationship documentation
- **Chronicle**: Development history and progress tracking

## License

© 2025 PrismaTech - All Rights Reserved 