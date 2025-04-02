# LED Pattern Map - PrismaTech Gauntlet 3.0

## Overview

This document maps all LED patterns in the system, their triggers, and implementation details. It serves as a reference for understanding and modifying the visual feedback system.

## Pattern Categories

### 1. Position-Based Patterns
These patterns directly reflect the current hand position.

#### Implementation
- **Location**: `IdleMode.cpp` (normal operation)
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
- **Location**: `IdleMode.cpp`
- **Trigger**: Gesture detection system during countdown
- **Control Flow**:
  1. Gesture progress calculated (0.0 to 1.0)
  2. Color determined by gesture type
  3. Pattern generated based on progress
  4. Applied via `HardwareManager` methods

#### Pattern Types
1. **LongShield Countdown**
   - Color: Shield pink (255, 105, 180)
   - Pattern: 2Hz flash during countdown
   - Duration: 3000ms (warning) to 5000ms (trigger)

2. **QuickCast Gesture Detection**
   - No visual feedback during gesture detection
   - Only triggered LEDs on successful detection

### 3. Mode Transition Patterns
Visual feedback during mode changes.

#### Implementation
- **Location**: 
  - `GauntletController.cpp`: Mode transition handling
  - `AnimationData.h`: Predefined transition animations
- **Trigger**: Mode transition detection
- **Control Flow**:
  1. Transition detected
  2. Animation sequence loaded
  3. Frames played in sequence
  4. Mode change completed

#### Pattern Types
1. **FreeCast Mode Entry**
   - Color: Shield pink (255, 105, 180)
   - Pattern: Triple flash (100ms on, 50ms off)
   - Location: `FreecastMode.cpp` initialization

2. **FreeCast Mode Exit**
   - Color: Shield pink (255, 105, 180)
   - Pattern: Triple flash (100ms on, 50ms off)
   - Location: `FreecastMode.cpp` exit method

3. **QuickCast Mode Entry**
   - Immediate transition to spell effect
   - No dedicated transition animation

### 4. QuickCast Spell Effects
Visual effects triggered by specific gesture combinations.

#### Implementation
- **Location**: `QuickCastSpellsMode.cpp`
- **Trigger**: Specific QuickCast gestures (CalmOffer, DigOath, NullShield)
- **Control Flow**:
  1. Gesture detected in `IdleMode` via `GestureTransitionTracker`
  2. Mode change to `QuickCastSpellsMode`
  3. Specific spell effect initiated based on gesture type
  4. Effect plays for its defined duration
  5. Return to `IdleMode`

#### Spell Effects
1. **Rainbow Burst** (CalmOffer)
   - **Effect**: Radial rainbow spectrum that accelerates outward
   - **Duration**: 7 seconds
   - **Colors**: Full spectrum rotation
   - **Implementation**: Progressive circular pattern with increasing speed

2. **Lightning Blast** (DigOath)
   - **Effect**: Flash white, then simulate lightning crackles
   - **Duration**: 5 seconds
   - **Colors**: White flashes with blue/purple accents
   - **Implementation**: Random bright flashes with decay patterns

3. **Lumina** (NullShield)
   - **Effect**: Steady white light (flashlight functionality)
   - **Duration**: 20 seconds
   - **Colors**: Pure white at 80% brightness
   - **Implementation**: Stable illumination of 6/12 LEDs

### 5. FreeCast Mode Patterns
Dynamic patterns based on motion characteristics.

#### Implementation
- **Location**: `FreeCastMode.cpp`
- **Trigger**: Motion analysis during recording phase
- **Control Flow**:
  1. Motion data collected for 2 seconds
  2. Pattern type determined from motion characteristics
  3. Pattern parameters calculated
  4. Pattern rendered for 2 seconds
  5. Cycle repeats

#### Pattern Generation
- Based on motion characteristics:
  - High acceleration: Brighter, more LEDs active
  - Sudden spikes: Bright flashes
  - Rapid rotation: Fast transitions, directional shifts
  - Circular motion: Spirals, waves
  - Steady motion: Smooth gradients
  - Jerky motion: Random sparkles and flickers

### 6. System Status Patterns
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

2. **Error Indicator**
   - Color: Red
   - Pattern: Fast blink (100ms on, 100ms off)
   - Location: Error handling sections

## Pattern Modification Guide

### 1. Color Changes
1. **System-Wide Colors**
   - Location: `Config.h` in `Colors` namespace
   - Affects: All position-based patterns
   - Update: Modify color constants

2. **QuickCast Spell Colors**
   - Location: `QuickCastSpellsMode.cpp` in spell rendering methods
   - Affects: Spell visual effects
   - Update: Modify color values in rendering code

3. **Freecast Colors**
   - Location: `FreeCastMode.cpp` in pattern generation code
   - Affects: Motion-based patterns
   - Update: Modify color calculation algorithms

### 2. Timing Changes
1. **Spell Durations**
   - Location: `Config.h` in `Spells` namespace
   - Structure: Duration constants
   - Update: Modify timing constants

2. **Gesture Recognition Timing**
   - Location: `Config.h`
   - Structure: Time constants (e.g., `QUICKCAST_WINDOW_MS`, `LONGSHIELD_TIME_MS`)
   - Update: Modify timing constants

3. **Freecast Cycle Timing**
   - Location: `Config.h`
   - Structure: `FREECAST_COLLECTION_MS` and `FREECAST_DISPLAY_MS` constants
   - Update: Modify timing values (currently both 2000ms)

## Common LED Pattern Issues and Solutions

### 1. Color Inconsistency
- **Issue**: Position colors don't match expected values
- **Solution**: Verify position color constants in `Config.h` are correct and consistent with `TrueFunctionGuide.md`

### 2. Gesture Detection Problems
- **Issue**: QuickCast gestures not properly triggering spells
- **Solution**: 
  1. Verify `GestureTransitionTracker` intermediate position handling
  2. Check gesture time windows in `Config.h`
  3. Enable debug output in `GestureTransitionTracker.cpp`

### 3. LED State After Spell Completion
- **Issue**: LEDs not returning to proper state after QuickCast spell completes
- **Solution**: 
  1. Verify `QuickCastSpellsMode::exit()` correctly resets brightness
  2. Ensure `IdleMode::initialize()` properly sets up LED state

### 4. Pattern Rendering Inconsistency
- **Issue**: Patterns not displaying consistently across modes
- **Solution**: 
  1. Ensure each mode class implements `renderLEDs()` method
  2. Verify `GauntletController` properly calls render methods
  3. Check `HardwareManager::updateLEDs()` is called consistently

## Best Practices

1. **Color Consistency**
   - Use `Config::Colors` for system-wide colors
   - Maintain consistent color naming and values
   - Avoid hardcoded color values

2. **Rendering Pipeline**
   - Each mode should own its LED rendering logic
   - All modes should provide consistent rendering interface
   - Use `HardwareManager::updateLEDs()` for final application

3. **Performance**
   - Batch LED updates where possible
   - Minimize calculation complexity during rendering
   - Use timing parameters from `Config.h` for consistency

4. **Testing**
   - Verify each pattern individually
   - Test transitions between patterns
   - Check LED state persistence between mode changes

---

> Last updated: 2025-04-02 