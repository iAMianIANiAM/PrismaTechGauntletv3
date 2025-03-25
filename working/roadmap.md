# PrismaTech Gauntlet 3.0 - Development Roadmap

This document tracks the project goals and timelines in a checklist format.

## Phase 1: Hardware Setup and Initialization
- [ ] Plan wiring and power details
- [ ] Physical prototype assembly (to be done by Ian)
- [ ] Implement WiFi protocols for OTA updates
  - [ ] Research and select appropriate OTA library
  - [ ] Implement basic OTA functionality
  - [ ] Test OTA updates on the device

## Phase 2: Idle Mode Development
- [ ] Develop calibration protocol script
  - [ ] Create serial monitor interface for gathering sensor data
  - [ ] Collect and analyze real-world data from prototype
  - [ ] Define initial position thresholds based on sensor data
- [ ] Position Detection System
  - [ ] Implement dominant axis detection logic
  - [ ] Map sensor readings to defined hand positions
  - [ ] Implement LED feedback for current hand position
  - [ ] Test and refine position detection reliability
- [ ] Gesture Recognition System
  - [ ] Implement CalmOffer gesture detection
  - [ ] Implement LongNull gesture detection
  - [ ] Add visual feedback animations for gesture recognition
  - [ ] Test gesture detection reliability

## Phase 3: Freecast Mode Development
- [ ] Design data collection and analysis algorithms
- [ ] Implement motion-to-pattern translation system
- [ ] Create visual effects based on motion characteristics
- [ ] Implement mode entry/exit functionality
- [ ] Test and refine motion response

## Phase 4: Invocation Mode Development
- [ ] Implement slot-based position recording system
- [ ] Create timing and animation sequences for each phase:
  - [ ] Indicator Phase
  - [ ] Countdown Phase
  - [ ] Checkpoint Phase
  - [ ] Confirmation Phase
- [ ] Implement position imprinting functionality
- [ ] Test the full invocation sequence

## Phase 5: Resolution Mode Development
- [ ] Implement spell determination logic
- [ ] Create unique LED animations for each spell effect
- [ ] Implement mode transitions and timing
- [ ] Test the full resolution sequence

## Phase 6: Integration and Final Testing
- [ ] Integrate all operational modes
- [ ] Optimize power consumption
- [ ] Conduct comprehensive reliability testing
- [ ] Final adjustments and improvements 