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
  3. Color applied with smooth interpolation
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

4. **ShakeCancel**
   - Color: White (255, 255, 255)
   - Pattern: Three rapid flashes (50ms on, 50ms off)
   - Location: `GauntletController::playCancelAnimation()`
   - Configurable via `Config::ShakeDetection` namespace

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
   - **Effect**: Enhanced 4-phase rainbow animation with accelerating effects
   - **Duration**: 8 seconds (Config::Spells::RAINBOW_DURATION_MS)
   - **Colors**: Full spectrum rotation with white burst and color pops
   - **Implementation**: 
     - Phase 1 (0-2s): Slow pulsing (1Hz) and swirling (1 rotation/s)
     - Phase 2 (2-4s): Medium pulsing (2Hz) and swirling (2 rotations/s)
     - Phase 3 (4-6s): Fast pulsing (4Hz) and swirling (4 rotations/s)
     - Phase 4 (6-8s): White burst followed by fading and color pops

2. **Lightning Blast** (DigOath)
   - **Effect**: Flash white, then simulate lightning crackles
   - **Duration**: 5 seconds (Config::Spells::LIGHTNING_DURATION_MS)
   - **Colors**: White flashes with red, blue, and purple accents
   - **Implementation**: Random bright flashes with colored lightning effects

3. **Lumina** (NullShield)
   - **Effect**: Steady white light (flashlight functionality)
   - **Duration**: 60 seconds (Config::Spells::LUMINA_DURATION_MS)
   - **Colors**: Pure white at 80% brightness (Config::Spells::LUMINA_BRIGHTNESS)
   - **Implementation**: Stable illumination of 6/12 LEDs with slow fade out

### 5. FreeCast Mode Patterns
Dynamic patterns based on motion characteristics.

#### Implementation
- **Location**: `FreeCastMode.cpp`
- **Trigger**: Motion analysis during recording phase
- **Control Flow**:
  1. Motion data collected for 2 seconds (Config::FREECAST_COLLECTION_MS)
  2. Pattern type determined from motion characteristics
  3. Pattern parameters calculated
  4. Pattern rendered for 2 seconds (Config::FREECAST_DISPLAY_MS)
  5. Cycle repeats

#### Pattern Generation
- Based on motion characteristics:
  - High acceleration: Brighter, more LEDs active
  - Sudden spikes: Bright flashes
  - Rapid rotation: Fast transitions, directional shifts
  - Circular motion: Spirals, waves
  - Steady motion: Smooth gradients
  - Jerky motion: Random sparkles and flickers

#### Exit Methods
- **LongShield Exit**:
  - Hold Shield position for 5 seconds
  - Blue LED countdown begins at 3 seconds
  - Cancels if position changes during countdown
  
- **ShakeCancel Exit**:
  - Quick shake motion detected by ShakeGestureDetector
  - Immediate exit with brief white flash animation
  - Position-independent cancellation method

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
   - Update: Modify timing constants:
     - `RAINBOW_DURATION_MS`: 8000 (8 seconds)
     - `LIGHTNING_DURATION_MS`: 5000 (5 seconds)
     - `LUMINA_DURATION_MS`: 60000 (60 seconds)

2. **Gesture Recognition Timing**
   - Location: `Config.h`
   - Structure: Time constants (e.g., `QUICKCAST_WINDOW_MS`, `LONGSHIELD_TIME_MS`)
   - Update: Modify timing constants

3. **Freecast Cycle Timing**
   - Location: `Config.h`
   - Structure: `FREECAST_COLLECTION_MS` and `FREECAST_DISPLAY_MS` constants
   - Update: Modify timing values (currently both 2000ms)

4. **ShakeCancel Animation Timing**
   - Location: `Config.h` in `ShakeDetection` namespace
   - Structure: `CANCEL_FLASH_COUNT` and `CANCEL_FLASH_DURATION_MS` constants
   - Update: Modify flash count and duration

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

### 5. ShakeCancel Not Working
- **Issue**: Shake gesture not detected or not canceling current mode
- **Solution**:
  1. Check shake detection thresholds in `Config::ShakeDetection` namespace
  2. Verify `ShakeGestureDetector` is properly initialized
  3. Ensure current mode's cancellation method is called
  4. Test with different shake intensities

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
   - Test ShakeCancel in all applicable modes

---

> Last updated: 2025-04-05 