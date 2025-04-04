# 📘 PrismaTech Gauntlet 3.0 Project Glossary

> This glossary defines key terms, concepts, and protocols for the PrismaTech Gauntlet 3.0 project. Terms are organized by functional domain for easier reference and navigation.

## 📋 Table of Contents

1. [System Architecture & Core Concepts](#system-architecture--core-concepts)
2. [Hardware Components](#hardware-components)
3. [Detection & Recognition](#detection--recognition)
4. [Visual Effects & Feedback](#visual-effects--feedback)
5. [Operational Modes](#operational-modes)
6. [Development Process](#development-process)
7. [Configuration & Calibration](#configuration--calibration)
8. [Protocols](#protocols)
9. [Status Indicators](#status-indicators)
10. [Alphabetical Index](#alphabetical-index)

---

## System Architecture & Core Concepts

This section defines the core components, relationships, and architectural patterns that form the foundation of the PrismaTech Gauntlet system.

### Components

| Term | Status | Definition |
|------|--------|------------|
| **Config** | [IMPLEMENTED] | Header-only implementation containing system-wide constants and configuration values. Serves as the central location for all configurable parameters, ensuring consistency across the system. |
| **GauntletController** | [IMPLEMENTED] | The central controller class that coordinates all subsystems, manages state transitions between operational modes, and processes user input from sensors. Acts as the main orchestrator for the entire system. |
| **LEDInterface** | [IMPLEMENTED] | Hardware abstraction layer for controlling the LED system. Provides a clean API for pattern generation, color management, and animation effects regardless of the underlying LED hardware. |
| **MPU9250Interface** | [IMPLEMENTED] | Hardware abstraction layer for the motion processing unit. Handles sensor initialization, data acquisition, and preprocessing of raw sensor data before position detection. |
| **OperationalModes** | [IMPLEMENTED] | The distinct states of the PrismaTech Gauntlet system: `IdleMode`, `QuickCastSpellsMode`, and `FreecastMode`. Each mode has specific behaviors, visual feedback patterns, and interaction models. |
| **ThresholdManager** | [IMPLEMENTED] | Component for centralizing management of detection thresholds. Stores, retrieves, and provides access to calibrated threshold values that determine hand position detection. |
| **UBPD** | [IMPLEMENTED] | Ultra Basic Position Detection - The primary detection model for the device that converts raw accelerometer data to physical units (m/s²) and uses simplified dominant axis detection with explicit thresholds to identify hand positions. |

### Architectural Patterns

| Term | Status | Definition |
|------|--------|------------|
| **DominantAxisDetectionModel** | [IMPLEMENTED] | The algorithm used for position recognition where each hand position has a primary axis that shows significantly higher values than other axes. The system detects which axis has the strongest acceleration and compares it to calibrated thresholds. |
| **Non-blocking Architecture** | [IMPLEMENTED] | Design pattern where operations are structured to avoid blocking the main program flow. Ensures the system remains responsive regardless of ongoing operations. Critical for maintaining consistent timing in animation effects and sensor polling. |

---

## Hardware Components

This section defines the physical components and hardware interfaces used in the PrismaTech Gauntlet.

| Term | Status | Definition |
|------|--------|------------|
| **ESP32** | [IMPLEMENTED] | The primary microcontroller used in the PrismaTech Gauntlet. Features dual-core processor, built-in WiFi and Bluetooth, and sufficient GPIO pins for all required peripherals. |
| **Gauntlet** | [IMPLEMENTED] | The complete physical device, including all hardware and embedded software components that make up the wearable system. |
| **MPU9250** | [IMPLEMENTED] | The motion sensor used in the gauntlet for position and gesture detection. Provides 9-axis sensing (3-axis accelerometer, 3-axis gyroscope, 3-axis magnetometer) with digital output through I2C or SPI. |
| **NeoPixel Ring** | [IMPLEMENTED] | Addressable RGB LED ring used for visual feedback. Typically configured with 12 or 16 LEDs in a circular arrangement, controlled via a single data pin using WS2812 protocol. |

---

## Detection & Recognition

This section defines terms related to position detection, gesture recognition, and the interpretation of sensor data.

### Hand Positions

| Term | Status | Definition |
|------|--------|------------|
| **CalmPosition** | [VERIFIED] | Hand forward, palm down orientation. Represented by blue color (RGB: 0, 0, 255). Detected via Z-axis dominant negative acceleration. |
| **DigPosition** | [VERIFIED] | Hand pointing downwards. Represented by green color (RGB: 0, 255, 0). Detected via Y-axis dominant positive acceleration. |
| **HandPositions** | [VERIFIED] | The six defined orientations that the gauntlet can detect: Offer (palm up), Calm (palm down), Oath (hand pointing up), Dig (hand pointing down), Shield (hand to side, palm out), and Null (hand to side, palm in). Each position has a corresponding color and is detected via the dominant axis model. |
| **NullPosition** | [VERIFIED] | Hand to the side, palm in. Represented by red color (RGB: 255, 0, 0). Detected via X-axis dominant positive acceleration. |
| **OathPosition** | [VERIFIED] | Hand pointing upwards. Represented by yellow color (RGB: 255, 255, 0). Detected via Y-axis dominant negative acceleration. |
| **OfferPosition** | [VERIFIED] | Hand forward, palm up orientation. Represented by purple color (RGB: 128, 0, 255). Detected via Z-axis dominant positive acceleration. |
| **ShieldPosition** | [VERIFIED] | Hand to the side, palm out. Represented by pink color (RGB: 255, 105, 180). Detected via X-axis dominant negative acceleration. |

### Gestures

| Term | Status | Definition |
|------|--------|------------|
| **CalmOffer** | [VERIFIED] | A transition gesture from "Calm" position (palm-down) to "Offer" position (palm-up) within a 1000ms window. This gesture triggers the Rainbow Burst QuickCast spell. |
| **DigOath** | [VERIFIED] | A transition gesture from "Dig" position (fingers down) to "Oath" position (fingers up) within a 1000ms window. This gesture triggers the Lightning Blast QuickCast spell. |
| **GestureTransitionTracker** | [IMPLEMENTED] | Component that tracks transitions between two hand positions within a specified time window. Used to detect gesture combinations that trigger QuickCast spells. |
| **LongShield** | [VERIFIED] | A gesture where the "Shield" position (hand to side, palm out) is maintained continuously for 5000ms. This gesture triggers the transition to or exit from Freecast Mode. |
| **NullShield** | [VERIFIED] | A transition gesture from "Null" position (palm in) to "Shield" position (palm out) within a 1000ms window. This gesture triggers the Lumina QuickCast spell. |
| **ShakeCancel** | [VERIFIED] | A motion-based universal cancellation gesture that allows exiting any non-idle mode by performing a quick shake of the Gauntlet. Provides a consistent, position-independent way to cancel operations. |

### Detection Concepts

| Term | Status | Definition |
|------|--------|------------|
| **Activation Energy** | [IMPLEMENTED] | The amount of motion required to trigger a position change. Helps prevent accidental position changes due to minor movements or sensor noise. |
| **CalibrationThreshold** | [IMPLEMENTED] | The specific value that defines the boundary between different hand positions. Determined during the calibration process and stored for runtime use. |
| **Physical Units** | [IMPLEMENTED] | Sensor readings converted to standard physical units (e.g., m/s², gauss) rather than raw values. Ensures consistent interpretation regardless of specific sensor hardware. |
| **Threshold Boundaries** | [IMPLEMENTED] | The values that define the transition between different hand positions. Used by the position detection system to determine the current hand orientation. |

---

## Visual Effects & Feedback

This section defines terms related to the visual output, animations, and LED patterns used in the system.

### QuickCast Spell Effects

| Term | Status | Definition |
|------|--------|------------|
| **LightningBlast** | [VERIFIED] | A QuickCast spell effect triggered by the DigOath gesture. Features flash white, then simulated lightning crackles with red, blue, and purple, lasting for 5 seconds (Config::Spells::LIGHTNING_DURATION_MS). |
| **Lumina** | [VERIFIED] | A QuickCast spell effect triggered by the NullShield gesture. Produces a utility white light using 6/12 LEDs at 80% brightness for 60 seconds (Config::Spells::LUMINA_DURATION_MS). Functions as a flashlight. |
| **RainbowBurst** | [VERIFIED] | A QuickCast spell effect triggered by the CalmOffer gesture. Features a 4-phase radial rainbow spectrum that accelerates outward, lasting for 8 seconds (Config::Spells::RAINBOW_DURATION_MS). |
| **SpellEffects** | [VERIFIED] | The visual animations triggered by QuickCast gestures. Current implementations include Rainbow Burst, Lightning Blast, and Lumina, each with unique animations, colors, and durations. |

### Visual Feedback Patterns

| Term | Status | Definition |
|------|--------|------------|
| **Freecast Cycle** | [VERIFIED] | The alternating sequence in Freecast Mode consisting of a 2-second Data Collection Phase (recording motion) followed by a 2-second Pattern Generation Phase (displaying motion-based visuals). |

---

## Operational Modes

This section defines the different operational states and modes of the PrismaTech Gauntlet system.

| Term | Status | Definition |
|------|--------|------------|
| **FreecastMode** | [VERIFIED] | A creative, freeform experience where users can generate unique light patterns by moving the gauntlet in various ways. Activated via `LongShield` and deactivated via either `LongShield` or `ShakeCancel`. |
| **IdleMode** | [VERIFIED] | The default state of the gauntlet when not actively casting. It provides real-time visual feedback of the current hand position while monitoring for QuickCast trigger gestures (`CalmOffer`, `DigOath`, `NullShield`) and the Freecast mode trigger (`LongShield`). |
| **QuickCastSpellsMode** | [VERIFIED] | Operational mode responsible for executing the visual effects of triggered QuickCast spells (`RainbowBurst`, `LightningBlast`, `Lumina`) for their defined durations. |

### Deprecated Modes

| Term | Status | Definition |
|------|--------|------------|
| **InvocationMode** | [DEPRECATED] | Originally planned as a structured sequence mode. Replaced by `QuickCastSpellsMode`. |
| **InvocationPhases** | [DEPRECATED] | The four sequential stages of Invocation Mode: Indicator Phase, Countdown Phase, Checkpoint Phase, and Confirmation Phase. |
| **InvocationSlots** | [DEPRECATED] | Three distinct slots in Invocation Mode, each represented by 4 LEDs on the ring, which record position-based imprints to determine the resulting spell effect. |
| **ResolutionMode** | [DEPRECATED] | Originally planned as the spell effect execution mode following Invocation. Spell effects are now handled by `QuickCastSpellsMode`. |

---

## Development Process

This section defines terms related to project management, development workflow, documentation standards, and testing.

### Documentation Standards

| Term | Status | Definition |
|------|--------|------------|
| **Chronicle** | [IMPLEMENTED] | Primary running journal of development activity and historical record. Located at `working/chronicle_v#.md`. |
| **DirectoryIndex** | [IMPLEMENTED] | High-level index of codebase structure, module relationships, and key files. Located at `working/directoryIndex.md`. |
| **Glossary** | [IMPLEMENTED] | Shared term definitions: protocols, project-specific terms, system acronyms. Located at `working/glossary.md`. |
| **WTEMP** | [IMPLEMENTED] | Prefix for "Working Temporary" files. These files contain information pertinent to a particular task or implementation and are intended to be discarded after use. Before deletion, they must be reviewed for valuable information, which is then summarized in a chronicle entry indicating the file has completed its purpose and been deleted. |

### Date Standards

| Term | Status | Definition |
|------|--------|------------|
| **Date Format** | [IMPLEMENTED] | All dates must use ISO 8601 format: YYYY-MM-DD (e.g., 2025-03-28) |
| **Document Alignment** | [IMPLEMENTED] | All documents referencing the same event must use identical dates |
| **Future References** | [IMPLEMENTED] | References to future dates must be clearly marked as estimates or targets |
| **Timezone Reference** | [IMPLEMENTED] | All dates and times refer to Eastern Standard Time (EST) unless explicitly stated otherwise |

### Git Workflow

| Term | Status | Definition |
|------|--------|------------|
| **chore** | [IMPLEMENTED] | Commit type for maintenance tasks. |
| **docs** | [IMPLEMENTED] | Commit type for documentation changes. |
| **feat** | [IMPLEMENTED] | Commit type for new features or components. |
| **fix** | [IMPLEMENTED] | Commit type for bug fixes. |
| **Full Git Commit** | [IMPLEMENTED] | A comprehensive git operation that captures the complete state of the project at a specific point in time, ensuring the ability to restore the entire codebase to exactly this state if needed. |
| **refactor** | [IMPLEMENTED] | Commit type for code improvements with no functional changes. |
| **test** | [IMPLEMENTED] | Commit type for test additions or changes. |

---

## Configuration & Calibration

This section defines terms related to system configuration, calibration processes, and settings management.

| Term | Status | Definition |
|------|--------|------------|
| **Calibration Mode** | [IMPLEMENTED] | Special system mode for adjusting sensor thresholds. |
| **Calibration Protocol** | [IMPLEMENTED] | Structured process for capturing sensor data to generate detection thresholds. |

---

## Protocols

This section defines standardized procedures and protocols used throughout the development process.

### Core Protocols

| Protocol | Description |
|----------|-------------|
| **Analyze** | Root cause investigation protocol for unexpected issues or behavior. Uses a structured four-step approach:<br>1. Review code, chronicle, and glossary<br>2. Identify likely failure points<br>3. Gather context and trace root causes<br>4. Report findings and recommend next steps |
| **Double-Check** | Post-implementation verification protocol. Uses checklists to ensure structural, functional, and build integrity:<br>1. Generate Verification Checklist (Build, Structural, Functional)<br>2. Build Assumption Checklist<br>3. Audit implementation line-by-line<br>4. Report findings and identify flaws |
| **Proposal** | Protocol for suggesting new solutions. Requires user approval before implementation:<br>1. Analyze context via documentation<br>2. Draft a detailed solution path<br>3. Await explicit user approval<br>4. On approval, elevate to Plan and implement<br>5. If unsure about approval, ask for clarification |
| **Report** | Protocol for information-only responses. No code or action included. Provides well-organized explanations without implementation. |

### Project-Specific Protocols

| Protocol | Description |
|----------|-------------|
| **Audit** | Systematic process for identifying and resolving documentation-code discrepancies:<br>1. Initial investigation to identify discrepancies<br>2. List all discrepancies in numbered format<br>3. Methodical investigation of each discrepancy<br>4. Gather evidence through targeted tool usage<br>5. Implement specific fixes for each issue<br>6. Perform verification check after all fixes<br>7. Final report on addressed issues |
| **Chronicle Transition** | A systematic process for transitioning between chronicle versions when a document reaches its 1000-line threshold:<br>1. Create new chronicle with proper header<br>2. Generate migration summary<br>3. Archive old chronicle<br>4. Update references across documents<br>For full details, see [ChronicleTransitionProtocol.md](../reference/ChronicleTransitionProtocol.md) |
| **Document Review (DocRev)** | A comprehensive review methodology to ensure working documents are accurate, current, and conform to user expectations:<br>1. Analyze document and identify source of truth<br>2. Address documentation review symbols by priority<br>3. Assess content against source of truth<br>4. Reconcile document to conform with source of truth<br>5. Provide detailed report of changes made |
| **Documentation Maintenance** | Process for keeping documentation aligned with codebase changes:<br>1. Code-First Updates: Always update code before documentation<br>2. Concurrent Documentation: Update related documentation in same commit<br>3. Cross-Reference Verification: Ensure consistency across documentation<br>4. Reference Path Validation: Verify all referenced file paths exist |

### Documentation Review Symbols

| Symbol | Meaning |
|--------|---------|
| **!!!** | Critical Review Required - Indicates a section requiring immediate attention and correction. Must be specifically addressed in the next report. |
| **$$$** | General Review Required - All-purpose flag for sections needing review, revision, or adjustment. Specific handling instructions will be provided in the review directive. |
| **???** | Questions/Elaboration Required - Indicates sections needing further explanation or clarification. May require additional research or cross-referencing. |

---

## Status Indicators

This section defines the status indicators used throughout the documentation to track implementation progress.

### Implementation Status

| Status | Definition |
|--------|------------|
| **[PLANNED]** | Designed but not yet implemented. Documentation exists, but code does not. |
| **[IN PROGRESS]** | Implementation started but not complete. Code may exist in partial form. |
| **[IMPLEMENTED]** | Code exists and functions in test environments but not fully verified in system context. |
| **[VERIFIED]** | Implementation exists and has been fully tested in production context with all system integrations. Include verification date and method. |
| **[DEPRECATED]** | Was implemented but has been replaced by a newer approach. |

### Verification Levels

| Level | Definition |
|-------|------------|
| **Test Environment Functionality** | Component works in isolation within a specific test environment |
| **System Integration Verification** | Component functions correctly within the full system context |
| **Production Verification** | Component meets all performance, reliability and user experience requirements in production environment |

---

## Project Guiding Principles

| Principle | Definition |
|-----------|------------|
| **DRY** | Don't Repeat Yourself - be vigilant against redundancy. |
| **Explicit Over Implicit** | Be explicit about status, intentions, and dependencies. |
| **Implementation Before Documentation** | Never document features as complete before they exist in code. |
| **KISS** | Keep It Simple Stupid - add no more complexity than necessary. |
| **Recover Before Extending** | Fix and verify existing functionality before adding new features. |
| **Small Changes, Frequent Commits** | Prefer small, focused changes with clear documentation. |
| **Verification Is Required** | All implementations must be verified before being considered complete. |
| **YAGNI** | You Ain't Gonna Need It - implement functionality only when it is truly necessary. |

---

## Alphabetical Index

- [Activation Energy](#detection-concepts)
- [Analyze Protocol](#core-protocols)
- [Audit Protocol](#project-specific-protocols)
- [Calibration Mode](#configuration--calibration)
- [Calibration Protocol](#configuration--calibration)
- [CalibrationThreshold](#detection-concepts)
- [CalmOffer](#gestures)
- [CalmPosition](#hand-positions)
- [chore](#git-workflow)
- [Chronicle](#documentation-standards)
- [Chronicle Transition Protocol](#project-specific-protocols)
- [Config](#components)
- [Date Format](#date-standards)
- [DigOath](#gestures)
- [DigPosition](#hand-positions)
- [DirectoryIndex](#documentation-standards)
- [docs](#git-workflow)
- [Document Alignment](#date-standards)
- [Document Review (DocRev)](#project-specific-protocols)
- [Documentation Maintenance Protocol](#project-specific-protocols)
- [DominantAxisDetectionModel](#architectural-patterns)
- [Double-Check Protocol](#core-protocols)
- [DRY](#project-guiding-principles)
- [ESP32](#hardware-components)
- [Explicit Over Implicit](#project-guiding-principles)
- [feat](#git-workflow)
- [fix](#git-workflow)
- [Freecast Cycle](#visual-feedback-patterns)
- [FreecastMode](#operational-modes)
- [Full Git Commit](#git-workflow)
- [Future References](#date-standards)
- [Gauntlet](#hardware-components)
- [GauntletController](#components)
- [GestureTransitionTracker](#gestures)
- [Glossary](#documentation-standards)
- [HandPositions](#hand-positions)
- [IdleMode](#operational-modes)
- [Implementation Before Documentation](#project-guiding-principles)
- [InvocationMode](#deprecated-modes)
- [InvocationPhases](#deprecated-modes)
- [InvocationSlots](#deprecated-modes)
- [KISS](#project-guiding-principles)
- [LEDInterface](#components)
- [LightningBlast](#quickcast-spell-effects)
- [LongShield](#gestures)
- [Lumina](#quickcast-spell-effects)
- [MPU9250](#hardware-components)
- [MPU9250Interface](#components)
- [NeoPixel Ring](#hardware-components)
- [Non-blocking Architecture](#architectural-patterns)
- [NullPosition](#hand-positions)
- [NullShield](#gestures)
- [OathPosition](#hand-positions)
- [OfferPosition](#hand-positions)
- [OperationalModes](#components)
- [Physical Units](#detection-concepts)
- [Proposal Protocol](#core-protocols)
- [QuickCastSpellsMode](#operational-modes)
- [RainbowBurst](#quickcast-spell-effects)
- [Recover Before Extending](#project-guiding-principles)
- [refactor](#git-workflow)
- [Report Protocol](#core-protocols)
- [ResolutionMode](#deprecated-modes)
- [ShakeCancel](#gestures)
- [ShieldPosition](#hand-positions)
- [Small Changes, Frequent Commits](#project-guiding-principles)
- [SpellEffects](#quickcast-spell-effects)
- [test](#git-workflow)
- [Threshold Boundaries](#detection-concepts)
- [ThresholdManager](#components)
- [Timezone Reference](#date-standards)
- [UBPD](#components)
- [Verification Is Required](#project-guiding-principles)
- [Verification Levels](#verification-levels)
- [WTEMP](#documentation-standards)
- [YAGNI](#project-guiding-principles)

---

> Last updated: 2025-04-05

> Note: This glossary will continue to evolve as the project progresses. Terms will be added, updated, or deprecated as needed to maintain an accurate reflection of the current state of the project.
