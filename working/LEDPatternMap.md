# LED Pattern Map - PrismaTech Gauntlet 3.0

## Overview

This document maps all LED patterns in the system, their triggers, and implementation details. It serves as a reference for understanding and modifying the visual feedback system.

## Pattern Categories

### 1. Position-Based Patterns
These patterns directly reflect the current hand position.

#### Implementation
- **Location**: `main.cpp` (fallback mode) and `IdleMode.cpp` (normal operation)
- **Trigger**: Position detection update cycle
- **Control Flow**:
  1. Position detected by `UltraBasicPositionDetector`
  2. Position mapped to color via `Config::Colors::*_COLOR`
  3. Color applied to all LEDs via `HardwareManager::setAllLEDs()`
  4. Update applied via `HardwareManager::updateLEDs()`

#### Position-to-Color Mapping
- **Offer**: Purple (128, 0, 255)
- **Calm**: Blue (0, 0, 255)
- **Oath**: Yellow (255, 255, 0)
- **Dig**: Green (0, 255, 0)
- **Shield**: Pink (255, 105, 180)
- **Null**: Red (255, 0, 0)
- **Unknown**: White (255, 255, 255)

### 2. Gesture Countdown Patterns
Visual feedback during gesture detection countdowns.

#### Implementation
- **Location**: `IdleMode.cpp` and `FreeCastMode.cpp`
- **Trigger**: Gesture detection system during countdown
- **Control Flow**:
  1. Gesture progress calculated (0.0 to 1.0)
  2. Color determined by gesture type
  3. Pattern generated based on progress
  4. Applied via `HardwareManager` methods

#### Pattern Types
1. **LongShield Countdown**
   - Color: Shield blue
   - Pattern: 2Hz flash during countdown
   - Duration: 3000ms (warning) to 5000ms (trigger)

2. **CalmOffer Countdown**
   - Color: Purple
   - Pattern: Progressive fill
   - Duration: 1000ms

### 3. Mode Transition Patterns
Visual feedback during mode changes.

#### Implementation
- **Location**: 
  - `main.cpp`: Direct LED control in `handleModeTransition()`
  - `AnimationData.h`: Predefined transition animations
- **Trigger**: Mode transition detection
- **Control Flow**:
  1. Transition detected
  2. Animation sequence loaded
  3. Frames played in sequence
  4. Mode change completed

#### Pattern Types
1. **FreeCast Mode Entry**
   - Color: Shield blue
   - Pattern: Triple flash (100ms on, 50ms off)
   - Location: `handleModeTransition()` in `main.cpp`

2. **FreeCast Mode Exit**
   - Color: Shield blue
   - Pattern: Triple flash (100ms on, 50ms off)
   - Location: `handleModeTransition()` in `main.cpp`

3. **Invocation Mode Entry**
   - Color: Purple
   - Pattern: Triple flash (100ms on, 50ms off)
   - Location: `handleModeTransition()` in `main.cpp`

### 4. FreeCast Mode Patterns
Dynamic patterns based on motion characteristics.

#### Implementation
- **Location**: `FreeCastMode.cpp`
- **Trigger**: Motion analysis during recording phase
- **Control Flow**:
  1. Motion data collected
  2. Pattern type determined
  3. Pattern parameters calculated
  4. Pattern rendered

#### Pattern Generation
- Based on motion characteristics
- Uses `HardwareManager` for LED control
- Patterns defined in `AnimationData.h`

### 5. System Status Patterns
Startup and error indication patterns.

#### Implementation
- **Location**: `main.cpp` in `setup()`
- **Trigger**: System initialization
- **Control Flow**:
  1. System state determined
  2. Status pattern selected
  3. Pattern played
  4. Normal operation begins

#### Pattern Types
1. **Ready Indicator**
   - Color: Green
   - Pattern: Triple flash (200ms on, 200ms off)
   - Location: `setup()` in `main.cpp`

## Pattern Modification Guide

### 1. Color Changes
1. **System-Wide Colors**
   - Location: `Config.h` in `Colors` namespace
   - Affects: All position-based patterns
   - Update: Modify color constants

2. **Mode-Specific Colors**
   - Location: `AnimationData.h` in `AnimColors` namespace
   - Affects: Transition animations
   - Update: Modify color constants

3. **Runtime Colors**
   - Location: `main.cpp` in `handleModeTransition()`
   - Affects: Mode transitions
   - Update: Modify color variables

### 2. Pattern Changes
1. **Animation Sequences**
   - Location: `AnimationData.h`
   - Structure: `AnimationFrame` array
   - Update: Modify frame data and durations

2. **Dynamic Patterns**
   - Location: Mode-specific files (e.g., `FreeCastMode.cpp`)
   - Structure: Pattern generation code
   - Update: Modify pattern generation logic

### 3. Timing Changes
1. **Frame Durations**
   - Location: `AnimationData.h`
   - Structure: `duration` field in `AnimationFrame`
   - Update: Modify duration values

2. **Gesture Timings**
   - Location: `Config.h`
   - Structure: Time constants
   - Update: Modify timing constants

## Best Practices

1. **Color Consistency**
   - Use `Config::Colors` for system-wide colors
   - Use `AnimColors` for animation-specific colors
   - Avoid hardcoded color values

2. **Pattern Organization**
   - Keep related patterns together
   - Use clear naming conventions
   - Document pattern relationships

3. **Performance**
   - Use PROGMEM for static patterns
   - Minimize runtime calculations
   - Batch LED updates

4. **Maintainability**
   - Document pattern triggers
   - Use consistent timing units
   - Maintain clear separation of concerns

## Common Pitfalls

1. **Color Inconsistency**
   - Mixing different color definitions
   - Using hardcoded values
   - Inconsistent color application

2. **Timing Issues**
   - Inconsistent timing units
   - Missing delay calls
   - Pattern overlap

3. **Performance Problems**
   - Too frequent LED updates
   - Complex runtime calculations
   - Missing PROGMEM usage

## Testing Guidelines

1. **Visual Verification**
   - Test each pattern individually
   - Verify timing accuracy
   - Check color consistency

2. **Integration Testing**
   - Test pattern transitions
   - Verify mode changes
   - Check gesture feedback

3. **Performance Testing**
   - Monitor update frequency
   - Check memory usage
   - Verify smooth transitions 