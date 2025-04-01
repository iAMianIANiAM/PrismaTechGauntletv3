# PrismaTech Gauntlet - True Function Guide (v1.0)

## System Overview

The PrismaTech Gauntlet is a wearable electronic device that enables users to "cast spells" through specific hand positions and gestures. The system uses an IMU sensor to detect hand orientation and movement, providing visual feedback through an LED ring. The device follows a structured interaction flow consisting of four distinct operational modes: Idle Mode, Invocation Mode, Resolution Mode, and Freecast Mode.

> **Note on Threshold Values:** All threshold values mentioned in this document are for illustrative and placeholder purposes only. Actual threshold values will be determined during development based on calibration tests with the specific hardware components used.

## Core Operational Modes

### 1. Idle Mode

Idle Mode is the default state of the gauntlet when not actively casting. It provides real-time visual feedback of the current hand position while monitoring for trigger gestures that activate other modes.

#### Key Characteristics
- **Visual Indicator**: Four evenly-spaced LEDs (0, 3, 6, and 9) display at 80% brightness in the color corresponding to the current hand position
- **Active Detection**: Continuously monitors for palm position and the "Null" position for triggering different modes
- **Transition**: Executes a transition animation when a trigger gesture is detected
- **Power State**: Low-energy operation to conserve battery

#### Hand Position Visual Feedback
The four LEDs (0, 3, 6, and 9) change color based on the current hand position:
- **Offer** (Hand forward, Palm Up): Purple (RGB: 128, 0, 128)
- **Calm** (Hand forward, Palm Down): Yellow (RGB: 255, 255, 0)
- **Oath** (Hand pointing upwards): Red (RGB: 255, 0, 0)
- **Dig** (Hand pointing downwards): Green (RGB: 0, 255, 0)
- **Shield** (Hand to the side, Palm Out): Blue (RGB: 0, 0, 255)
- **Null** (Hand to the side, Palm In): Orange (RGB: 255, 165, 0)
- **Default/Unclear**: White (RGB: 255, 255, 255)

#### Gesture Naming Conventions
Gestures in the system are named according to a consistent nomenclature:
- **Sequential positions**: When a gesture involves moving from one position to another, the positions are combined in PascalCase (e.g., "CalmOffer")
- **Held positions**: When a gesture involves holding a position for an extended time, the position name is prefixed with "Long" (e.g., "LongNull")
- **Compound gestures**: Future complex gestures will follow the same convention, linking all involved positions in sequence

#### Invocation Trigger Requirements (CalmOffer)
- **Gesture Definition**: A transition from "Calm" position (palm-down) to "Offer" position (palm-up)
- **Timing Window**: Must detect "Offer" position within 1000ms of leaving "Calm" position
- **Visual Feedback**: Purple flash animation when triggered

#### Freecast Trigger Requirements (LongNull)
- **Gesture Definition**: Maintaining the "Null" position (hand to side, palm in) for 5000ms continuously
- **Countdown Feedback**: After 3000ms in the "Null" position, the orange LEDs begin flashing for the remaining 2000ms
- **Cancellation**: Any change from the "Null" position during the 5000ms window aborts the trigger sequence
- **Visual Feedback**: Orange flash animation when triggered

#### Idle Mode Timeline

| Time (s) | System State | LED Activity | User Action | Technical Details |
|----------|--------------|--------------|-------------|-------------------|
| Continuous | Idle Monitoring | Four LEDs (0,3,6,9) at 80% brightness in position color | Normal activity | System refreshes at 50Hz, constantly checking hand position |
| Variable | "Calm" Detected | Four LEDs display yellow at 80% brightness | Hold palm down | System recognizes palm-down orientation |
| +0.0 | "Calm" Exit | LEDs change to match new position | Move hand | System detects palm is no longer down |
| +0.0-1.0 | CalmOffer Trigger Window | LEDs update to current position color | Move to "Offer" | System monitors for palm-up within 1000ms |
| +Variable | "Offer" Detected | All LEDs flash purple once | Complete gesture | System recognizes palm-up orientation within window |
| +0.0 | Idle → Invocation | All LEDs flash purple once | Observe transition | System transitions to Invocation Mode |
| *or* |  |  |  |  |
| Continuous | "Null" Position Monitoring | Four LEDs display orange at 80% brightness | Hold "Null" position | System detects palm-in, hand to side orientation |
| +3.0 | LongNull Countdown Start | Orange LEDs begin flashing (500ms on/off) | Maintain "Null" position | System continues monitoring for position changes |
| +5.0 | LongNull Trigger Complete | All LEDs flash orange once | Observe transition | System transitions to Freecast Mode |

### 2. Invocation Mode

Invocation Mode is a structured sequence where the user's hand positions are recorded into three distinct "slots" to determine the resulting spell effect.

#### Key Concepts

1. **Invocation Slots**: Three distinct slots, each represented by 4 LEDs on the ring, which record a position-based imprint.

2. **Hand Positions**: Six defined positions that map to different colors:
   - **Offer** (Hand forward, Palm Up): Purple imprint
   - **Calm** (Hand forward, Palm Down): Yellow imprint
   - **Oath** (Hand pointing upwards): Red imprint
   - **Dig** (Hand pointing downwards): Green imprint
   - **Shield** (Hand to the side, Palm Out): Blue imprint
   - **Null** (Hand to the side, Palm In): Orange imprint
   - **Default/Unclear**: White imprint (when position not clearly detected)

3. **Phases**: The system operates in distinct phases:
   - **Indicator Phase**: Signals which slot is currently active (1 second)
   - **Countdown Phase**: Visual timer showing when position will be recorded (2 seconds)
   - **Checkpoint Phase**: Brief moment when hand position is detected (200ms)
   - **Confirmation Phase**: Visual feedback of the detected position (1 second)

#### Invocation Timeline

| Time (s) | System State | LED Activity | User Action | Technical Details |
|----------|--------------|--------------|-------------|-------------------|
| 0.0 | Idle Mode | Four LEDs display in position color at 80% brightness | Hold "Calm" position | System awaiting CalmOffer gesture |
| 0.0 | Trigger Detected | All LEDs flash purple once | Complete CalmOffer gesture | System detects transition from palm-down to palm-up |
| 0.0-1.0 | Slot 1 Indicator | Slot 1 LEDs (0-3) flash | Prepare for first position | `runIndicatorPhase()` activates |
| 1.0-3.0 | Slot 1 Countdown | Single white LED rotates around ring; all other LEDs off | Move to desired position | `runCountdownPhase()` creates moving LED effect |
| 3.0-3.2 | Slot 1 Checkpoint | All 12 LEDs flash briefly like camera flash | Hold position steady | `runCheckpointPhase()` captures accelerometer data |
| 3.2-4.2 | Slot 1 Confirmation | Slot 1 LEDs light up in detected color | Observe feedback | Detected position mapped to color and displayed |
| 4.2-5.2 | Slot 2 Indicator | Slot 2 LEDs (4-7) flash | Prepare for second position | Previous slot imprint remains visible |
| 5.2-7.2 | Slot 2 Countdown | Single white LED rotates around ring; all other LEDs off | Move to desired position | Previously imprinted slots temporarily turn off |
| 7.2-7.4 | Slot 2 Checkpoint | All 12 LEDs flash briefly like camera flash | Hold position steady | System captures second position |
| 7.4-8.4 | Slot 2 Confirmation | Slot 2 LEDs light up in detected color | Observe feedback | Both slot 1 and 2 imprints now visible |
| 8.4-9.4 | Slot 3 Indicator | Slot 3 LEDs (8-11) flash | Prepare for third position | Previous slot imprints remain visible |
| 9.4-11.4 | Slot 3 Countdown | Single white LED rotates around ring; all other LEDs off | Move to desired position | Previously imprinted slots temporarily turn off |
| 11.4-11.6 | Slot 3 Checkpoint | All 12 LEDs flash briefly like camera flash | Hold position steady | System captures third position |
| 11.6-12.6 | Slot 3 Confirmation | Slot 3 LEDs light up in detected color | Observe feedback | All three slot imprints now visible |
| 12.6-14.1 | Resolution Indicator | Imprints flash at 0.5s & 1.0s, spell core color flashes at 1.5s | Observe transition | System determines which spell effect to activate |

#### Position Detection Details

The system uses accelerometer data to determine hand position at each checkpoint. The Dominant Axis Detection Model simplifies and improves reliability of position recognition.

##### Dominant Axis Detection Model

Each hand position has a primary axis that shows significantly higher values than the other axes:

1. **Offer (Purple)**: Z-axis dominant positive
2. **Calm (Yellow)**: Z-axis dominant negative
3. **Oath (Red)**: Y-axis dominant negative
4. **Dig (Green)**: Y-axis dominant positive
5. **Shield (Blue)**: X-axis dominant negative
6. **Null (Orange)**: X-axis dominant positive
7. **Default (White)**: When no dominant axis is detected

The detection algorithm operates on these principles:
- Each position has a dominant axis that must exceed a specific threshold
- Non-dominant axes must remain below their respective thresholds
- The Shield position has additional Z-axis requirements for proper palm orientation

The detection uses a brief average (50-100ms) of accelerometer readings to reduce sensor noise and improve reliability. Position changes require both proper timing (debounce time of 100ms) and confidence scoring (minimum 60%) to be registered.

### 3. Resolution Mode

Resolution Mode is the culmination of the invocation sequence, where the system evaluates the three imprinted positions and activates a corresponding spell effect.

#### Key Characteristics
- **Effect Determination**: Based on the colors/positions recorded in the three slots
- **Visual Feedback**: Rich animated effects unique to each spell
- **Duration Control**: Fixed durations for definite spells, cancelable for channeled spells
- **Return Sequence**: Structured return to Idle Mode after effect completion

#### Spell Determination Rules

After all three slots are imprinted, the system counts the occurrence of each color to determine which spell effect to activate:

- **Inferno**: Red majority (Red, Red, Any)
- **Blizzard**: Blue majority (Blue, Blue, Any)
- **Wild Growth**: Green majority (Green, Green, Any)
- **Detect Magic**: Purple only (Purple, Purple, Purple)
- **Rainbow Burst**: RGB mix (Red, Green, Blue in any order)
- **Rainbow Pop**: Default when no other pattern matches

#### Resolution Mode Timeline

| Time (s) | System State | LED Activity | User Action | Technical Details |
|----------|--------------|--------------|-------------|-------------------|
| 12.6-13.1 | Resolution Preparation | All current imprints flash once | Observe | System evaluates slot colors |
| 13.1-13.6 | Spell Selection | All current imprints flash again | Observe | System determines spell effect |
| 13.6-14.1 | Spell Core Indication | Center LEDs flash in spell's core color | Observe | Visual indication of selected spell |
| 14.1-19.1 | Definite Spell Effect | Unique animation for selected spell | Observe effect | Short-duration spells (5s) |
| *or* |  |  |  |  |
| 14.1-29.1 | Extended Spell Effect | Unique animation for selected spell | Observe effect | Long-duration spells (15s) |
| +0.0-0.5 | Blackout Period | All LEDs turn off completely | Await return | Brief transition before idle return |
| +0.5-3.0 | Return to Idle | Four idle LEDs fade in with position color | System ready | Gradual return to idle state |

#### Spell Effect Descriptions

1. **Inferno** (Red Majority)
   - **Visual Effect**: Fire-like pattern spiraling clockwise
   - **Colors**: Red, orange, yellow with occasional white flickers
   - **Movement**: Medium-fast rotation with upward intensity fluctuations
   - **Duration**: 5 seconds
   - **Unique Feature**: Occasional bright white "sparks" that rapidly fade

2. **Blizzard** (Blue Majority)
   - **Visual Effect**: Swirling ice pattern moving counter-clockwise
   - **Colors**: Deep blue, light blue, and white
   - **Movement**: Smooth, continuous rotation with pulsing intensity
   - **Duration**: 5 seconds
   - **Unique Feature**: Occasional bright white "sparkles" that remain fixed

3. **Wild Growth** (Green Majority)
   - **Visual Effect**: Pulsing heartbeat-like pattern
   - **Colors**: Multiple green shades with occasional yellow accents
   - **Movement**: Expanding and contracting pattern from center
   - **Duration**: 5 seconds
   - **Unique Feature**: Growth "waves" that expand from center outward

4. **Detect Magic** (Triple Purple)
   - **Visual Effect**: Scanning pattern rotating full circle
   - **Colors**: Purple with bright white scanning beam
   - **Movement**: Slow rotation with bright scanning beam
   - **Duration**: 15 seconds
   - **Unique Feature**: "Scanning beam" that highlights different sections

5. **Rainbow Burst** (RGB Mix)
   - **Visual Effect**: Rainbow pattern that accelerates outward
   - **Colors**: Full spectrum that shifts through RGB
   - **Movement**: Starts slow, accelerates to burst
   - **Duration**: 5 seconds
   - **Unique Feature**: Final "burst" effect that fills all LEDs with white

6. **Rainbow Pop** (Default/Fallback)
   - **Visual Effect**: Random color pops around the ring
   - **Colors**: Random bright colors on black background
   - **Movement**: Random positions with varying durations
   - **Duration**: 5 seconds
   - **Unique Feature**: Unpredictable pattern that never repeats

### 4. Freecast Mode

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
| 0.0 | Idle → Freecast | All LEDs flash orange once | Complete LongNull gesture | System transitions after 5s in "Null" position |
| 0.0-0.5 | Freecast Initialization | Sweeping orange pattern around ring | Prepare for first motion | System initializes data buffers |
| 0.5-2.5 | Data Collection 1 | Subtle pulsing background pattern | Move gauntlet creatively | System records 6-axis IMU data |
| 2.5-4.5 | Pattern Display 1 | First generated pattern based on motion | Observe effect | System processes recorded motion into pattern |
| 4.5-6.5 | Data Collection 2 | Subtle pulsing background pattern | Move gauntlet differently | System records new motion data |
| 6.5-8.5 | Pattern Display 2 | Second generated pattern | Observe effect | New pattern reflects different motion |
| Repeating | Continuous cycle | Alternating collection and display | Continuous interaction | System maintains 2s recording window |
| Variable | Exit Trigger Detected | All LEDs flash orange once | Perform LongNull gesture | System detects LongNull gesture |
| +0.0-0.5 | Blackout Period | All LEDs turn off completely | Await return | Brief transition before idle return |
| +0.5-3.0 | Return to Idle | Four idle LEDs fade in with position color | System ready | Gradual return to idle state |

#### Exiting Freecast Mode

To exit Freecast Mode, the user must:
- Perform the LongNull gesture: hold the "Null" position (hand to side, palm in) for 5 seconds continuously
- After 3 seconds, the LEDs will flash orange for 2 seconds to indicate pending exit
- If "Null" position is maintained for the full 5 seconds, the system returns to Idle Mode
- Any deviation from the "Null" position during this time will cancel the exit sequence

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
   - CalmOffer gesture must reliably trigger Invocation Mode
   - LongNull gesture must reliably trigger or exit Freecast Mode
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

The LED ring provides detailed visual feedback throughout all four modes:

### 1. Idle Mode Visuals
- Four evenly-spaced LEDs (0, 3, 6, 9) at 80% brightness in the color matching current hand position
- Orange flashing during LongNull trigger countdown
- Mode transition animations (purple flash for Invocation, orange flash for Freecast)

### 2. Invocation Mode Visuals
- Slot Indicator: Four LEDs in current slot flash
- Countdown: Single white LED rotates around ring
- Checkpoint: Brief "camera flash" effect with all LEDs
- Confirmation: Current slot LEDs illuminate in position color
- Previous slots remain illuminated in their detected colors

### 3. Resolution Mode Visuals
- Resolution Indicator: Three-step sequence signaling transition
- Spell Effect: Unique animation patterns for each spell
- Return to Idle: Blackout followed by fade-in of idle LEDs

### 4. Freecast Mode Visuals
- Data Collection: Subtle background animation during recording phase
- Pattern Display: Dynamic, motion-responsive patterns during display phase
- Exit Countdown: Orange flashing animation during LongNull gesture countdown
- Return to Idle: Blackout followed by fade-in of idle LEDs

## Conclusion

The PrismaTech Gauntlet creates a magical experience through a carefully designed interaction flow that combines gesture detection, position recognition, and visual feedback. By maintaining consistent state management and providing clear visual indicators, the system delivers a reliable and engaging user experience.

The four operational modes—Idle, Invocation, Resolution, and Freecast—work together to create a cohesive interaction system that balances structured spell casting with creative expression. The timing and visual feedback are critical components that contribute to the magical quality of the experience. 