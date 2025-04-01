# PrismaTech Gauntlet 3.0 - Directory Index

> *This document serves as a high-level index of the codebase structure, module relationships, and key files for the PrismaTech Gauntlet 3.0 project. It is maintained as part of the working documentation set.*

## 🗺️ Top-Level Structure

```
/
├── src/                  # Source code for the main firmware
├── include/              # Header files for libraries and shared components
├── lib/                  # Third-party libraries and custom components
├── examples/             # Example applications and component tests
├── test/                 # Test cases and testing infrastructure
├── reference/            # Reference documentation for protocols and standards
├── working/              # Active working documents (chronicle, roadmap, etc.)
├── archive/              # Archived documents and deprecated code
└── platformio.ini        # PlatformIO project configuration
```

## 📂 Source Code Structure (src/)

### Core Subsystem (`src/core/`)

The core subsystem provides fundamental types, configuration, and system control.

| File | Description |
|------|-------------|
| `Config.h` | Global configuration parameters and settings |
| `SystemTypes.h` | Common data structures and type definitions |
| `GauntletController.h/cpp` | Main system controller managing mode transitions |
| `SystemStatus.h/cpp` | System status tracking and reporting |

### Mode Subsystem (`src/modes/`)

The mode subsystem implements the primary operational modes of the gauntlet.

| File | Description |
|------|-------------|
| `IdleMode.h/cpp` | Idle mode: position detection, Idle LED feedback, QuickCast gesture detection, Freecast mode trigger detection |
| `QuickCastSpellsMode.h/cpp` | Executes QuickCast spell visual effects (Rainbow, Lightning, Lumina) for a set duration |
| `FreecastMode.h/cpp` | Freecast mode: dynamic motion-to-pattern LED effects |

### Hardware Abstraction (`src/hardware/`)

The hardware subsystem abstracts physical components and peripherals.

| File | Description |
|------|-------------|
| `HardwareManager.h/cpp` | Central manager for all hardware interfaces |
| `MPU9250Interface.h/cpp` | Interface for the MPU9250 inertial measurement unit |
| `LEDInterface.h/cpp` | Interface for controlling NeoPixel LEDs |
| `PowerManager.h/cpp` | Power management and battery monitoring |

**MPU Data Flow:**
1. `MPU9250Interface.cpp` reads raw sensor data via I2C
2. Data is processed and stored in `HardwareManager`'s `latestSensorData`
3. `UltraBasicPositionDetector` accesses this data via `HardwareManager::getSensorData()`
4. Processed data is used to determine hand position in the UBPD

### Detection Subsystem (`src/detection/`)

The detection subsystem processes sensor data to determine hand positions and gestures.

| File | Description |
|------|-------------|
| `UltraBasicPositionDetector.h/cpp` | Basic position detection using dominant axis algorithm based on `TrueFunctionGuide` |
| `GestureTransitionTracker.h/cpp` | Tracks transitions between two hand positions within a time window (used for QuickCast spells) |

### Animation Subsystem (`src/animation/`)

This subsystem does not currently exist as separate files.
Animation logic is handled within specific modes (e.g., QuickCastSpellsMode)
and potentially within LEDInterface.

### Utility Functions (`src/utils/`)

The utilities subsystem provides helper functions and debugging tools.

| File | Description |
|------|-------------|
| `DebugTools.h/cpp` | Debugging, logging, and diagnostic functions |
| `SerialCommands.h/cpp` | Serial command parsing and execution |
| `TimeUtils.h/cpp` | Timing utilities and non-blocking delays |

## 📱 Example Applications (examples/)

### Component Tests

Individual component test applications for validation and debugging.

| File | Description |
|------|-------------|
| `MPUSensorTest.cpp` | Tests MPU9250 sensor data acquisition |
| `LEDTest.cpp` | Tests LED control and animations |
| `PositionDetectorTest.cpp` | Tests position detection algorithms |
| `IdleUBPDTest.cpp` | Tests idle mode with UBPD integration |

### Integrated Applications

Full integrated applications demonstrating the system capabilities.

| File | Description |
|------|-------------|
| `GauntletDemo.cpp` | Complete demonstration of all gauntlet features |
| `PositionVisualizerApp.cpp` | Real-time visualization of position detection |
| `GestureTrainer.cpp` | Interactive application for gesture training |

## 📚 References (reference/)

Reference documentation for system components and protocols.

| File | Description |
|------|-------------|
| `TrueFunctionGuide.md` | Authoritative specification for system behavior |
| `MPUCalibration.md` | MPU sensor calibration procedure |
| `ECHO_MPUInitialization.md` | Echo documentation for MPU initialization |
| `UBPD.md` | Ultra Basic Position Detection specification |
| `ChronicleTransitionProtocol.md` | Protocol for chronicle version transitions |

## 🧪 Test Framework (test/)

Test cases and testing infrastructure.

| File | Description |
|------|-------------|
| `test_position_detection.cpp` | Unit tests for position detection |
| `test_gesture_recognition.cpp` | Unit tests for gesture recognition |
| `test_hardware_interfaces.cpp` | Unit tests for hardware interfaces |

## 🔄 Dependency Map

```
GauntletController
├── Mode Controllers (Idle, QuickCastSpells, Freecast)
│   └── Detection Systems (UltraBasicPositionDetector, GestureTransitionTracker)
└── Hardware Manager
    ├── MPU9250 Interface
    ├── LED Interface
    └── Power Manager
```

## 🔄 Key Data Flow Paths

1. **Sensor Data Processing Flow:**
   ```
   MPU9250Interface → HardwareManager → UltraBasicPositionDetector → IdleMode
   ```

2. **Visual Feedback Flow:**
   ```
   Mode Controllers (Idle, QuickCastSpells, Freecast) → HardwareManager → LEDInterface → Physical LEDs
   ```

3. **Mode Transition Flow:**
   ```
   UltraBasicPositionDetector → IdleMode (using GestureTransitionTracker for spells) → GauntletController → Mode Activation
   ```

4. **Power Management Flow:**
   ```
   PowerManager → HardwareManager → Component Power States
   ```

## 🧠 Critical Code Relationships

1. **Position Detection System**
   - `UltraBasicPositionDetector` relies on `HardwareManager` for raw sensor data
   - Position detection thresholds are defined in `Config.h`
   - Physical unit conversion happens in `UltraBasicPositionDetector::processRawData()`

2. **Mode Transition System**
   - `GauntletController` manages mode lifecycle and transitions (IDLE <-> QUICKCAST, IDLE <-> FREECAST).
   - Mode transitions are triggered by specific gestures detected in `IdleMode` (`LongShield`, QuickCast gestures via `GestureTransitionTracker`).

3. **Hardware Abstraction System**
   - `HardwareManager` provides a unified interface to all hardware components
   - All hardware access should go through the `HardwareManager` singleton
   - I2C communication with the MPU sensor is handled by `MPU9250Interface`
   - LED control is abstracted by `LEDInterface`

## 📂 File Purpose Mapping

### Core Logic Files
- **System Initialization**: `main.cpp`
- **Configuration**: `Config.h`
- **Mode Management**: `GauntletController.cpp`
- **Type Definitions**: `SystemTypes.h`

### Hardware Interface Files
- **Sensor Access**: `MPU9250Interface.cpp`
- **LED Control**: `LEDInterface.cpp`
- **Power Management**: `PowerManager.cpp`
- **Hardware Abstraction**: `HardwareManager.cpp`

### Position & Gesture Detection Files
- **Basic Position Detection**: `UltraBasicPositionDetector.cpp`
- **QuickCast Gesture Logic**: `GestureTransitionTracker.cpp` (used by `IdleMode.cpp`)

### Mode Implementation Files
- **Idle Mode**: `IdleMode.cpp`
- **QuickCast Spell Mode**: `QuickCastSpellsMode.cpp`
- **Freecast Mode**: `FreecastMode.cpp`

## 📚 Working Document Cross-References

- **Chronicle**: [working/chronicle_v7.md](./chronicle_v7.md)
- **Roadmap**: [working/roadmap.md](./roadmap.md)
- **Glossary**: [working/glossary.md](./glossary.md)

## 🔄 System Architecture Update (2025-04-01)

After implementation of the QuickCast spells system and removal of deprecated Invocation/Resolution modes:

### Core Component Relationships

```
                   ┌──────────────────────┐
                   │ GauntletController   │
                   └──────────┬───────────┘
                              │ manages
        ┌─────────────────────┼─────────────────────────┐
        │                     │                         │
        ▼                     ▼                         ▼
┌───────────┐       ┌─────────────────────┐      ┌────────────────┐
│ IdleMode  ├───────► QuickCastSpellsMode │      │ FreecastMode   │
└─────┬─────┘       └─────────────────────┘      └────────────────┘
      │ uses                ▲ uses                      ▲ uses
      │                     │                           │
┌─────┴─────────────────────┴─────────┬─────────────────┘
│     uses                            │ uses
│   ┌──────────────────────────────┐  │  ┌─────────────────┐
│   │ UltraBasicPositionDetector │◄─┘  │ HardwareManager │
│   └──────────────────────────────┘     └───────┬─────────┘
│     uses                                       │ provides
└─► ┌──────────────────────────┐                 │
    │ GestureTransitionTracker │                 ▼
    └──────────────────────────┘        ┌────────────────┐
                                        │ LEDInterface   │
                                        └────────────────┘
                                                ▲
                                                │ uses
                                        ┌────────────────┐
                                        │ MPUInterface   │
                                        └────────────────┘
```

### Key Interface Points

1. `GauntletController` manages instances of `IdleMode`, `QuickCastSpellsMode`, `FreecastMode`.
2. `IdleMode` uses `UltraBasicPositionDetector` for position and `GestureTransitionTracker` for QuickCast detection.
3. `QuickCastSpellsMode` and `FreecastMode` use `HardwareManager` (primarily for `LEDInterface`) to render effects.
4. Transitions: IDLE -> QUICKCAST, IDLE -> FREECAST, QUICKCAST -> IDLE, FREECAST -> IDLE.

### Current System State

- QuickCast spells (Rainbow, Lightning, Lumina) implemented via `QuickCastSpellsMode`.
- `IdleMode` detects QuickCast gestures and `LongShield` Freecast trigger.
- Deprecated Invocation/Resolution modes removed.
- Animation logic is primarily within modes, using `HardwareManager`/`LEDInterface`.

---

> Last updated: 2025-04-01
> This document is maintained as part of the project's working documentation set.