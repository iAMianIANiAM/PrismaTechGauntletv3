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
- **Rainbow Burst**: Multi-phase rainbow animation with increasing intensity
- **Lightning Blast**: Flash white with colored lightning crackles
- **Lumina**: Steady white light for utility purposes (flashlight)

### Freecast Mode
- Enter by holding the Shield position for 5 seconds
- Creates dynamic light patterns based on hand movement
- Alternates between 2-second motion capture and 2-second pattern display
- Exit by holding Shield again or using the ShakeCancel gesture

## Hardware Requirements

- ESP32-WROOM USB-C microcontroller
- MPU-9250 9-axis IMU sensor (or compatible)
- WS2812 12-LED RGB ring
- Portable 5V2A USB-C power source




## License

This project is licensed under the [Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)](https://creativecommons.org/licenses/by-nc/4.0/).

You are free to use, share, and adapt this work **for non-commercial purposes**, as long as you provide proper attribution. Commercial use of any kind is not permitted without explicit permission.

If you want to use this work commercially, contact me first. Otherwise—tinker away, remix, and enjoy!
