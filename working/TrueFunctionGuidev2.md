# PrismaTech Gauntlet - Functional Specification Guide (v2.0)

## Overview

The **PrismaTech Gauntlet** is a wearable gesture-based interface that enables users to perform “spellcasting” through specific hand orientations and transitions. Built around an IMU (Inertial Measurement Unit), it interprets hand poses and motion, producing LED-based visual feedback to reinforce interaction states.

The system features **three primary operational modes**:
1. **Idle Mode** – Passive monitoring and visual feedback
2. **Freecast Mode** – Dynamic, movement-driven effects
3. **Invocation Mode (TBD)** – Placeholder for future structured casting

> **Calibration Note**: All numeric thresholds in this guide are provisional and intended for illustrative purposes. Final values must be derived from hardware-specific calibration.

---

## Mode: Idle (Default State)

Idle Mode is the base operational state. It continuously:
- Tracks hand orientation
- Displays visual cues via LEDs
- Listens for gestures that trigger spells or mode changes

### LED Feedback:
Four key LEDs (indices 0, 3, 6, 9) are illuminated at 80% brightness, color-coded by hand position:

| Hand Position | Pose Description | LED Color (RGB) |
|---------------|------------------|-----------------|
| Offer         | Palm up          | (215, 0, 255)   |
| Calm          | Palm down        | (0, 0, 255)     |
| Oath          | Fingers up       | (255, 255, 0)   |
| Dig           | Fingers down     | (0, 255, 0)     |
| Shield        | Palm out to side | (255, 105, 180) |
| Null          | Palm in to side  | (255, 0, 0)     |
| Unclear       | Default/unknown  | (255, 255, 255) |

### Gesture Recognition Conventions:
- **Sequential Gestures**: Position1 → Position2 (e.g., `CalmOffer`)
- **Held Gestures**: Prefix `Long` + Position (e.g., `LongShield`)
- **Compound Gestures**: Future support for multi-sequence actions (e.g., `DigOathOffer`)

### QuickCast Spells:
Trigger via fast transitions (within 1000ms) from an “opening” position to its paired “closing” position.

| Gesture      | Trigger Sequence   | Spell Name       | Description |
|--------------|--------------------|------------------|-------------|
| CalmOffer    | Calm → Offer       | Rainbow Burst    | Accelerating outward rainbow
| DigOath      | Dig → Oath         | Lightning Blast  | Flash & crackle lightning
| NullShield   | Null → Shield      | Lumina           | Steady flashlight-style glow

**Timer Mechanics:**
- Each “opening” pose starts a 1000ms countdown
- Multiple timers may run concurrently
- Matching “closing” pose during active window triggers the effect
- Spell activation resets all active timers and returns to Idle

---

## Gesture: LongShield (Freecast Mode Entry/Exit)

Holding **Shield** pose for 5 seconds triggers Freecast Mode.

- **Visual Countdown**: After 3 seconds, LEDs begin flashing blue for remaining 2 seconds
- **Abort Condition**: Changing position during the countdown cancels trigger
- **Visual Confirmation**: Blue flash sequence on successful entry

---

## Gesture: LongOath (Invocation Mode Trigger – Not Yet Implemented)

Reserved for future use. Holding the **Oath** pose for 5 seconds will eventually initiate Invocation Mode.

- **Planned Feedback**: Yellow LED flash in final 2 seconds
- **Abort Condition**: Pose change before timeout cancels the attempt
- **Implementation Status**: Placeholder only

---

## Position Detection Logic

Uses a **Dominant Axis Detection Model** via the IMU’s accelerometer readings.

### Detection Method:
- Each pose is mapped to a single dominant axis that must exceed a threshold
- Non-dominant axes must fall below respective thresholds
- Shield pose includes additional Z-axis orientation validation
- Readings are averaged over 50–100ms to mitigate sensor noise
- Registered changes require:
  - **Debounce Time**: 100ms
  - **Confidence Score**: ≥60%

| Pose         | Dominant Axis        | Polarity  |
|--------------|----------------------|-----------|
| Offer        | Z                    | Positive  |
| Calm         | Z                    | Negative  |
| Oath         | Y                    | Negative  |
| Dig          | Y                    | Positive  |
| Shield       | X (+Z condition)     | Negative  |
| Null         | X                    | Positive  |
| Default      | —                    | No match  |

---

## QuickCast Spell Effects

### Rainbow Burst (CalmOffer)
- **Effect**: Radial rainbow spectrum that accelerates outward
- **Duration**: 7s
- **Feature**: Ends with all LEDs saturated in bold color burst

### Lightning Blast (DigOath)
- **Effect**: Flash white, then simulate lightning crackles with red, blue, purple
- **Duration**: 5s
- **Feature**: Randomized bright white “sparkle” pops

### Lumina (NullShield)
- **Effect**: Utility white light (6/12 LEDs), 80% brightness
- **Duration**: 20s
- **Feature**: Slowly fades out; usable as flashlight

---

## Freecast Mode – Dynamic Spellcasting

Allows continuous, expressive spellcasting based on motion. System loops between:
- **2s motion capture window**
- **2s visual playback window**

### Activation:
- Perform `LongShield`

### Core Loop:
1. **Capture Phase (2s)**:
   - Records accelerometer + gyroscope data
   - Subtle background animation indicates recording

2. **Pattern Generation (2s)**:
   - Translates motion data into LED animation
   - More intense motion → brighter/more active visuals

3. **Loop**: Repeats capture/generate cycle

### Motion Translations:
| Motion Type         | Visual Output                        |
|---------------------|--------------------------------------|
| High acceleration   | Brighter, more LEDs active           |
| Sudden spikes       | Bright flashes or white bursts       |
| Rapid rotation      | Fast transitions, directional shifts |
| Circular motion     | Spirals, waves                       |
| Steady motion       | Smooth gradients                     |
| Jerky motion/shake  | Random sparkles and flickers        |

### Exit Freecast Mode:
- Hold `Shield` for 5 seconds again (same as entry)
- Blue LED countdown begins at 3s
- Cancels if pose changes

### Freecast Timeline:
| Time       | Phase               | LEDs               | Notes                              |
|------------|---------------------|---------------------|-------------------------------------|
| 0.0        | Entry trigger        | Flash orange        | LongShield detected                 |
| 0.5–2.5s   | Capture Window #1    | Background pulse    | Collects IMU data                   |
| 2.5–4.5s   | Playback #1          | Custom pattern      | Based on prior motion              |
| 4.5–6.5s   | Capture Window #2    | Background pulse    | Continue movement                  |
| 6.5–8.5s   | Playback #2          | New pattern         | Reflects new data                  |
| Repeat     | Loop                 | Alternating states  | Cast → Feedback rhythm             |
| —          | Exit trigger         | Blue countdown flash| Hold LongShield to exit            |

---

## Invocation Mode (Placeholder)
- **Trigger**: LongOath (not yet functional)
- **Status**: TBD — to be redesigned in future version

---

## Technical Requirements

### 1. System Behavior
- Position recognition must meet timing + confidence thresholds
- Clear mode transitions and consistent visual states
- IMU data must feed cleanly into gesture and motion logic

### 2. Data Handling
- Maintain rolling 2s buffer for Freecast motion analysis
- Use both accelerometer and gyroscope data for all effects
- Gesture detection logic must support concurrent timers

### 3. Hardware Targets
- **MCU**: ESP32 or equivalent
- **IMU Sensor**: MPU-9250 (or compatible 9-axis unit)
- **LED Output**: WS2812 12-RGB LED ring
- **Power**: Portable rechargeable source

### 4. Software Structure
- Modular: Input → Detection → Output
- State machine governs mode and sub-phase transitions
- Visual feedback should map clearly to user actions

---

## LED Feedback Summary

### Idle Mode
- 4 LEDs (0,3,6,9): Position color (80%)
- Blue flashing: `LongShield` countdown
- Spell effects override temporarily, then reset

### Freecast Mode
- Orange flash: Enter mode
- Blue flash: Exit countdown
- Dynamic effects: Based on motion phase

### Invocation Mode
- TBD

---

## Summary

The **PrismaTech Gauntlet** blends intuitive gesture detection with immersive visual effects to simulate a spellcasting experience. With clearly defined modes, modular code structure, and robust input handling, it offers a reliable platform for magical interaction.

The defined system—**Idle**, **Freecast**, and placeholder **Invocation**—supports both structured and creative interactions. Core behaviors prioritize clarity for AI-based implementation and modular expansion.

