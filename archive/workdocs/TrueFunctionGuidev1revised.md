# PrismaTech Gauntlet - True Function Guide (v1.0)

## System Overview

The PrismaTech Gauntlet is a wearable electronic device that enables users to "cast spells" through specific hand positions and gestures. The system uses an IMU sensor to detect hand orientation and movement, providing visual feedback through an LED ring. The device follows a structured interaction flow consisting of three distinct operational modes: Idle Mode, Freecast Mode, and Invocation Mode (placeholder).

> **Note on Threshold Values:** All threshold values mentioned in this document are for illustrative and placeholder purposes only. Actual threshold values will be determined during development based on calibration tests with the specific hardware components used.

## Core Operational Modes

### 1. Idle Mode

Idle Mode is the default state of the gauntlet when not actively casting. It provides real-time visual feedback of the current hand position while monitoring for trigger gestures that activate other modes.

#### Key Characteristics
- **Visual Indicator**: Four evenly-spaced LEDs (0, 3, 6, and 9) display at 80% brightness in the color corresponding to the current hand position
- **Active Detection**: Continuously monitors for all hand positions for triggering different modes and QuickCast spells.
- **QuickCast Spell Triggers**: Detects several two-part gestures that trigger short LED patterns ("spell effects"), then return to Idle Mode.
   

#### Hand Position Visual Feedback
The four LEDs (0, 3, 6, and 9) change color based on the current hand position:
- **Offer** (Hand forward, Palm Up): Purple (RGB: 215, 0, 255)
- **Calm** (Hand forward, Palm Down): Blue (RGB: 0, 0, 255)
- **Oath** (Hand pointing upwards): Yellow (RGB: 255, 255, 0)
- **Dig** (Hand pointing downwards): Green (RGB: 0, 255, 0)
- **Shield** (Hand to the side, Palm Out): Pink (RGB: 255, 105, 180)
- **Null** (Hand to the side, Palm In): Red (RGB: 255, 0, 0)
- **Default/Unclear**: White (RGB: 255, 255, 255)

#### Gesture Naming Conventions
Gestures in the system are named according to a consistent nomenclature:
- **Sequential positions**: When a gesture involves moving from one position to another, the positions are combined in PascalCase (e.g., "CalmOffer")
- **Held positions**: When a gesture involves holding a position for an extended time, the position name is prefixed with "Long" (e.g., "LongShield")
- **Compound gestures**: Future complex gestures will follow the same convention, linking all involved positions in sequence

#### QuickCast Spell Triggers Requirements
- **Gesture Definition**: Three separate, distinct gestures that each trigger distinct spell effect: CalmOffer (Rainbow Burst), DigOath (Lightning Blast), NullShield (Lumina). Gesture involves a transition from "opening" position (Calm, Dig, or Null) to the "closing" position (Offer, Oath, or Shield) within 1000ms. Each "opening" position has an independent timer, and multiple "opening" timer windows may run simultaneously. Whenever any one of them detects its associated "closing" position within its 1000ms window, that spell effect is triggered and the gauntlet returns to Idle Mode with all timers reset.
- **Timing Window**: Must detect "closing" position within 1000ms of leaving "opening" position
- **Visual Feedback**: Associated "Spell Effect" per gesture combination.


#### Freecast Trigger Requirements (LongShield)
- **Gesture Definition**: Maintaining the "Shield" position (hand to side, palm out, thumb down) for 5000ms continuously
- **Countdown Feedback**: After 3000ms in the "Shield" position, the blue LEDs begin flashing for the remaining 2000ms
- **Cancellation**: Any change from the "Shield" position during the 5000ms window aborts the trigger sequence
- **Visual Feedback**: Blue flash animation when triggered

#### Invocation Trigger Requirements (LongOath) [FUTURE IMPLEMENTATION]
- **Gesture Definition**: Maintaining the "Oath" position (hand facing upward) for 5000ms continuously
- **Countdown Feedback**: After 3000ms in the "Oath" position, the yellow LEDs begin flashing for the remaining 2000ms
- **Cancellation**: Any change from the "Oath" position during the 5000ms window aborts the trigger sequence
- **Visual Feedback**: Yellow flash animation when triggered

#### Position Detection Details

The system uses accelerometer data to determine hand position at each checkpoint. The Dominant Axis Detection Model simplifies and improves reliability of position recognition.

##### Dominant Axis Detection Model

Each hand position has a primary axis that shows significantly higher values than the other axes:

1. **Offer (Purple)**: Z-axis dominant positive
2. **Calm (Blue)**: Z-axis dominant negative
3. **Oath (Yellow)**: Y-axis dominant negative
4. **Dig (Green)**: Y-axis dominant positive
5. **Shield (Pink)**: X-axis dominant negative
6. **Null (Red)**: X-axis dominant positive
7. **Default (White)**: When no dominant axis is detected

The detection algorithm operates on these principles:
- Each position has a dominant axis that must exceed a specific threshold
- Non-dominant axes must remain below their respective thresholds
- The Shield position has additional Z-axis requirements for proper palm orientation

The detection uses a brief average (50–100ms) of accelerometer readings to reduce sensor noise and improve reliability. Position changes require both proper timing (debounce time of 100ms) and confidence scoring (minimum 60%) to be registered.

#### QuickCast Spell Effects

1. **Rainbow Burst** (QuickCast Spell - CalmOffer Trigger)
   - **Visual Effect**: Rainbow pattern that accelerates outward
   - **Colors**: Full spectrum that shifts through RGB
   - **Movement**: Starts slow, accelerates to burst
   - **Duration**: 7 seconds
   - **Unique Feature**: Final "burst" effect that fills all LEDs with bold rainbow hues

2. **Lightning Blast** (QuickCast Spell - DigOath Trigger)
   - **Visual Effect**: Bright flash of faint-blue-almost-white light at 100% brightness, followed by a series of "crackles" of red, blue, and purple along with simmering "pops" of less-bright white lights.
   - **Colors**: Blue, light blue, red, purple, and white
   - **Movement**: Burst then fizzle
   - **Duration**: 5 seconds
   - **Unique Feature**: Occasional bright white "sparkles" that remain fixed

3. **Lumina** (QuickCast Spell - NullShield Trigger)
   - **Visual Effect**: 20 seconds of solid white light from 6/12 LEDs at 80% brightness. All LEDs fade back to black slowly.
   - **Colors**: Solid white
   - **Movement**: Solid color
   - **Duration**: 20 seconds
   - **Unique Feature**: Meant to be a utility mode, for occasionally using the gauntlet as a flashlight.

### Freecast Mode

Freecast Mode is a creative, freeform experience where users can generate unique light patterns by moving the gauntlet in various ways. Unlike the structured Invocation Mode, Freecast allows for more expressive and dynamic interactions.

#### Key Characteristics
- **Interaction Style**: Captures motion over a time window, then translates it into visual effects
- **Visual Feedback**: Rich, colorful patterns that respond to the intensity and style of movement
- **Duration**: Indefinite; continues until user deliberately exits the mode
- **Timing**: Two-second delay between movement and pattern generation

#### Freecast Operation

1. **Data Collection Phase**:
   - The system records accelerometer and gyroscope data over a two-second window
   - No visual feedback during data collection phase except for subtle background animation
   - System analyzes both the type and intensity of movement

2. **Pattern Generation Phase**:
   - After the two-second window, the gauntlet translates the recorded motion into a LED pattern
   - More rapid and extreme movements cause more dramatic visual effects
   - Pattern plays for two seconds before the next data collection phase begins

3. **Continuous Cycle**:
   - The system alternates between data collection and pattern generation indefinitely
   - Each new pattern is informed by the most recent two seconds of motion
   - The cycle creates a rhythm of "cast and response"

#### Motion-to-Pattern Translation

The system maps different aspects of motion to visual elements:

1. **Acceleration Magnitude**:
   - Higher acceleration → brighter colors, more LEDs active
   - Lower acceleration → softer colors, fewer LEDs active
   - Sudden spikes → bursts of bright white or color flashes

2. **Rotational Velocity**:
   - Faster rotation → quicker color transitions, faster movement patterns
   - Axis of rotation influences the direction of light movement
   - Smooth vs. jerky rotation creates different pattern textures

3. **Motion Types and Visual Mappings**:
   - **Quick Flicks**: Create shooting star effects or color trails
   - **Circular Motions**: Generate spinning or wave-like patterns
   - **Sharp Direction Changes**: Produce color shifts or sparkle effects
   - **Steady Movements**: Create gradual color blends and smooth transitions
   - **Shaking**: Generates random sparkles and flashes across all LEDs

#### Freecast Timeline

| Time (s) | System State | LED Activity | User Action | Technical Details |
|----------|--------------|--------------|-------------|-------------------|
| 0.0 | Idle → Freecast | All LEDs flash orange once | Complete LongShield gesture | System transitions after 5s in "Shield" position |
| 0.0-0.5 | Freecast Initialization | Sweeping orange pattern around ring | Prepare for first motion | System initializes data buffers |
| 0.5-2.5 | Data Collection 1 | Subtle pulsing background pattern | Move gauntlet creatively | System records 6-axis IMU data |
| 2.5-4.5 | Pattern Display 1 | First generated pattern based on motion | Observe effect | System processes recorded motion into pattern |
| 4.5-6.5 | Data Collection 2 | Subtle pulsing background pattern | Move gauntlet differently | System records new motion data |
| 6.5-8.5 | Pattern Display 2 | Second generated pattern | Observe effect | New pattern reflects different motion |
| Repeating | Continuous cycle | Alternating collection and display | Continuous interaction | System maintains 2s recording window |
| Variable | Exit Trigger Detected | All LEDs flash orange once | Perform LongShield gesture | System detects LongShield gesture |
| +0.0-0.5 | Blackout Period | All LEDs turn off completely | Await return | Brief transition before idle return |
| +0.5-3.0 | Return to Idle | Four idle LEDs fade in with position color | System ready | Gradual return to idle state |

#### Exiting Freecast Mode

To exit Freecast Mode, the user must:
- Perform the LongShield gesture: hold the "Shield" position (hand to side, palm out) for 5 seconds continuously
- After 3 seconds, the LEDs will flash blue for 2 seconds to indicate pending exit
- If "Shield" position is maintained for the full 5 seconds, the system returns to Idle Mode
- Any deviation from the "Shield" position during this time will cancel the exit sequence

### Invocation Mode

- **Status**: Placeholder — functionality is under redesign
- **Trigger Gesture**: LongOath (not implemented yet)
- **All other details**: TBD

## Technical Implementation Requirements

### 1. Critical System Requirements

1. **Position Detection Reliability**:
   - Using a dominant axis detection model improves position recognition
   - Position changes require both timing and confidence thresholds to be met
   - Debouncing and confidence scoring are critical for reliable detection

2. **Timing and State Management**:
   - Each mode must adhere to precise timing specifications
   - Clear visual indicators must signal state transitions
   - The system must maintain consistent operation across all modes

3. **Motion Analysis for Freecast Mode**:
   - Both accelerometer and gyroscope data must be processed
   - System must maintain a continuous 2-second data buffer
   - Motion characteristics must be mapped to visual parameters consistently

4. **Gesture Recognition**:
   - QuickCast gestures must reliably trigger associated spell effects
   - LongShield gesture must reliably trigger and exit Freecast Mode
   - Gesture recognition should follow the naming conventions for clarity and consistency

### 2. Hardware Requirements

- **Microcontroller**: ESP32 or compatible
- **Motion Sensor**: MPU-9250 9-axis IMU or compatible (must have both accelerometer and gyroscope)
- **LEDs**: WS2812 12-LED RGB ring
- **Power Source**: Portable battery solution

### 3. Implementation Approach

1. **Modular Architecture**:
   - Clear separation between sensor input, detection logic, and visual output
   - Well-defined interfaces between components
   - Consistent data representation throughout

2. **Robust State Management**:
   - Explicit tracking of current mode and phase
   - Well-defined transitions between states
   - Graceful error handling and recovery

3. **User Experience Focus**:
   - Consistent visual feedback for all user actions
   - Clear indication of system state at all times
   - Predictable timing and response

## Visual Feedback System

The LED ring provides detailed visual feedback throughout all three modes:

### 1. Idle Mode Visuals
- Four evenly-spaced LEDs (0, 3, 6, 9) at 80% brightness in the color matching current hand position
- Blue flashing during LongShield trigger countdown
- QuickCast Spell Effects

### 2. Freecast Mode Visuals
- Data Collection: Subtle background animation during recording phase
- Pattern Display: Dynamic, motion-responsive patterns during display phase
- Exit Countdown: Blue flashing animation during LongShield gesture countdown
- Return to Idle: Blackout followed by fade-in of idle LEDs

### 3. Invocation Mode Visuals
- TBD

## Conclusion

The PrismaTech Gauntlet creates a magical experience through a carefully designed interaction flow that combines gesture detection, position recognition, and visual feedback. By maintaining consistent state management and providing clear visual indicators, the system delivers a reliable and engaging user experience.

The three operational modes—Idle, Invocation (placeholder), and Freecast—work together to create a cohesive interaction system that balances structured spell casting with creative expression. The timing and visual feedback are critical components that contribute to the magical quality of the experience.

