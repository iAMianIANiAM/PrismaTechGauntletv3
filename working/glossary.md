# 📘 PrismaTech Gauntlet 3.0 Project Glossary

> This glossary defines key terms, concepts, and protocol references for this project. It will expand as the project progresses. Claude should update this document whenever a new domain-specific term is introduced.

---

## 🔧 Protocols

| Protocol | Description |
|----------|-------------|
| **Analyze** | Root cause investigation protocol for unexpected issues or behavior. Uses a structured four-step approach. |
| **Proposal** | Protocol for suggesting new solutions. Requires user approval before implementation. |
| **Double-Check** | Post-implementation verification protocol. Uses checklists to ensure structural, functional, and build integrity. |
| **Report** | Protocol for information-only responses. No code or action included. |

### Project Specific Protocols

| Protocol | Description |
|----------|-------------|
| **Audit** | Systematic process for identifying and resolving documentation-code discrepancies. Steps: 1) Initial investigation to identify all discrepancies between documentation and actual code; 2) Listing all discrepancies in a numbered format; 3) Methodical investigation of each discrepancy, one at a time; 4) For each discrepancy, gathering evidence through targeted tool usage (reading files, directory listings, grep searches); 5) Implementing a specific fix for each identified issue; 6) Performing a verification check after all fixes to ensure accuracy; 7) Final report on all discrepancies addressed and any remaining issues. Used primarily for reconciling documentation with codebase reality. |
| **Chronicle Transition** | A systematic process for transitioning between chronicle versions when a document reaches its 1000-line threshold. This protocol requires explicit user authorization before execution. The protocol defines the process for archiving the current chronicle to archive/chronicles/, creating a new version with appropriate summary information, and maintaining cross-references to preserve project history. For full details, see [ChronicleTransitionProtocol.md](../reference/ChronicleTransitionProtocol.md) |

---

## 📅 Date Standards

| Standard | Definition |
|----------|------------|
| **Date Format** | All dates must use ISO 8601 format: YYYY-MM-DD (e.g., 2025-03-28) |
| **Timezone Reference** | All dates and times refer to Eastern Standard Time (EST) unless explicitly stated otherwise |
| **Chronicle Dating** | Each chronicle entry must be timestamped with the actual calendar date of entry creation |
| **Roadmap Sync** | The roadmap "Last Updated" date must be synchronized with the most recent chronicle entry |
| **Document Alignment** | All documents referencing the same event must use identical dates |
| **Future References** | References to future dates must be clearly marked as estimates or targets |

---

## 🧠 Plan vs. Proposal

| Term | Definition |
|------|------------|
| **Proposal** | A detailed suggestion presented for review. Must not be implemented until explicitly approved. |
| **Plan** | An approved proposal. Must be recorded in `chronicle.md` and is eligible for implementation. |
| **Elevate to Plan** | The act of promoting a proposal to a plan. Claude must confirm elevation before proceeding. |
| **Approval Phrases** | Terms such as "approve," "greenlight," "go ahead," or "make it a plan." Ambiguous language must trigger clarification. |

---

## 🏷️ Chronicle Tags

| Tag | Use |
|-----|-----|
| `🧠 INSIGHT:` | Highlight implementation or design insights. |
| `⚠️ ISSUE:` | Identify bugs, blockers, or risks. |
| `✅ RESOLUTION:` | Document how an issue was resolved. |
| `📌 DECISION:` | Flag finalized design or architectural choices. |
| `🔍 TBD:` | Indicate incomplete or open-ended topics. |
| `📊 GUIDE-ALIGNED:` | Indicate implementation that follows TrueFunctionGuide specifications. |

---

## 📁 File References (Initial)

| File | Description |
|------|-------------|
| `chronicle_v#.md` | Running journal of work sessions and insights. Archive at ~1000 lines. |
| `roadmap.md` | Mid-weight directional guide for project goals and milestones. |
| `directoryIndex.md` | Project structure, file purpose mapping, and key relationships. |
| `glossary.md` | This document. Expanded as the project evolves. |

---

## 📊 Implementation Status Tracking

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

Only components that meet the full System Integration Verification criteria should be marked as [VERIFIED].

---

## 🔁 Git Workflow Terms

| Term | Definition |
|------|------------|
| **feat** | Commit type for new features or components. |
| **fix** | Commit type for bug fixes. |
| **docs** | Commit type for documentation changes. |
| **test** | Commit type for test additions or changes. |
| **refactor** | Commit type for code improvements with no functional changes. |
| **chore** | Commit type for maintenance tasks. |
| **recovery-phase1** | Deprecated development branch for recovery efforts. |
| **End-of-Session Procedure** | Set of steps to follow before ending a development session (update docs, commit, push). |

---

## 🧭 Project Guiding Principles

| Principle | Definition |
|-----------|------------|
| **Implementation Before Documentation** | Never document features as complete before they exist in code. |
| **Verification Is Required** | All implementations must be verified before being considered complete. |
| **Small Changes, Frequent Commits** | Prefer small, focused changes with clear documentation. |
| **Recover Before Extending** | Fix and verify existing functionality before adding new features. |
| **Explicit Over Implicit** | Be explicit about status, intentions, and dependencies. |
| **KISS** | Keep It Simple Stupid - add no more complexity than necessary. |
| **DRY** | Don't Repeat Yourself - be vigilant against redundancy. |
| **YAGNI** | You Ain't Gonna Need It - implement functionality only when it is truly necessary. |

---

## 🔬 Project-Specific Terms

| Term | Definition |
|------|------------|
| **CalmOffer** | A transition gesture from "Calm" position (palm-down) to "Offer" position (palm-up) within a 1000ms window. This gesture triggers the Rainbow Burst QuickCast spell. |
| **CalmPosition** | Hand forward, palm down orientation. Represented by blue color (RGB: 0, 0, 255). Detected via Z-axis dominant negative. |
| **Calibration Protocol** | Structured process for capturing sensor data to generate detection thresholds. |
| **Calibration Mode** | Special system mode for adjusting sensor thresholds. |
| **CalibrationThreshold** | The value that defines the transition between different hand positions. |
| **CalibrationValue** | The value that defines the transition between different hand positions. |
| **Config** | Header-only implementation containing system-wide constants and configuration values |
| **DigOath** | A transition gesture from "Dig" position (fingers down) to "Oath" position (fingers up) within a 1000ms window. This gesture triggers the Lightning Blast QuickCast spell. |
| **DominantAxisDetectionModel** | The algorithm used for position recognition where each hand position has a primary axis that shows significantly higher values than other axes. The system detects which axis has the strongest acceleration and compares it to calibrated thresholds. |
| **Double-Check** | Post-implementation verification protocol. Uses checklists to ensure structural, functional, and build integrity. |
| **Freecast Cycle** | The alternating sequence in Freecast Mode consisting of a 2-second Data Collection Phase (recording motion) followed by a 2-second Pattern Generation Phase (displaying motion-based visuals). |
| **FreecastMode** | A creative, freeform experience where users can generate unique light patterns by moving the gauntlet in various ways. Activated/Deactivated via `LongShield`. |
| **HandPositions** | The six defined orientations that the gauntlet can detect: Offer (palm up), Calm (palm down), Oath (hand pointing up), Dig (hand pointing down), Shield (hand to side, palm out), and Null (hand to side, palm in). Each position has a corresponding color and is detected via the dominant axis model. |
| **IdleMode** | The default state of the gauntlet when not actively casting. It provides real-time visual feedback of the current hand position while monitoring for QuickCast trigger gestures (`CalmOffer`, `DigOath`, `NullShield`) and the Freecast mode trigger (`LongShield`). |
| **InvocationMode** | **⚫ DEPRECATED:** Originally planned as a structured sequence mode. Replaced by `QuickCastSpellsMode`. |
| **InvocationPhases** | **⚫ DEPRECATED:** The four sequential stages of Invocation Mode: Indicator Phase, Countdown Phase, Checkpoint Phase, and Confirmation Phase. |
| **InvocationSlots** | **⚫ DEPRECATED:** Three distinct slots in Invocation Mode, each represented by 4 LEDs on the ring, which record position-based imprints to determine the resulting spell effect. |
| **LightningBlast** | A QuickCast spell effect triggered by the DigOath gesture. Features flash white, then simulated lightning crackles with red, blue, and purple, lasting for 5 seconds. |
| **LongShield** | A gesture where the "Shield" position (hand to side, palm out) is maintained continuously for 5000ms. This gesture triggers the transition to or exit from Freecast Mode. Replaced the deprecated LongNull gesture. |
| **LongOffer** | **⚫ DEPRECATED:** Extended duration in Offer position. Not currently used in the system. |
| **Lumina** | A QuickCast spell effect triggered by the NullShield gesture. Produces a utility white light using 6/12 LEDs at 80% brightness for 20 seconds. Functions as a flashlight. |
| **MPU9250** | The motion sensor used in the gauntlet for position and gesture detection. |
| **NullPosition** | Hand to the side, palm in. Represented by red color (RGB: 255, 0, 0). Detected via X-axis dominant positive. |
| **NullShield** | A transition gesture from "Null" position (palm in) to "Shield" position (palm out) within a 1000ms window. This gesture triggers the Lumina QuickCast spell. |
| **OfferPosition** | Hand forward, palm up orientation. Represented by purple color (RGB: 128, 0, 255). Detected via Z-axis dominant positive. |
| **OperationalModes** | The distinct states of the PrismaTech Gauntlet: `IdleMode`, `QuickCastSpellsMode`, and `FreecastMode`. |
| **OathPosition** | Hand pointing upwards. Represented by yellow color (RGB: 255, 255, 0). Detected via Y-axis dominant negative. |
| **QuickCastSpell** | An effect pattern triggered from Idle Mode via a two-position gesture combination (`CalmOffer`, `DigOath`, `NullShield`), which returns to Idle Mode upon completion of the spell effect. Examples: `RainbowBurst`, `LightningBlast`, `Lumina`. |
| **QuickCastSpellsMode** | **🟢 ACTIVE:** Operational mode responsible for executing the visual effects of triggered QuickCast spells (`RainbowBurst`, `LightningBlast`, `Lumina`) for their defined durations. |
| **RainbowBurst** | A QuickCast spell effect triggered by the CalmOffer gesture. Features a radial rainbow spectrum that accelerates outward, lasting for 7 seconds. |
| **ResolutionMode** | **⚫ DEPRECATED:** Originally planned as the spell effect execution mode following Invocation. Spell effects are now handled by `QuickCastSpellsMode`. |
| **ShieldPosition** | Hand to the side, palm out. Represented by pink color (RGB: 255, 105, 180). Detected via X-axis dominant negative. |
| **SpellEffects** | The visual animations triggered by QuickCast gestures. Current implementations include Rainbow Burst, Lightning Blast, and Lumina, each with unique animations, colors, and durations. |
| **ThresholdManager** | Component for centralizing management of detection thresholds. |
| **UBPD** | Ultra Basic Position Detection - The primary detection model for the device that converts raw accelerometer data to physical units (m/s²) and uses simplified dominant axis detection with explicit thresholds to identify hand positions. |

---

## 🔬 Development Glossary

### Core Terminology

#### System Components

- **Gauntlet**: The complete physical device, including all hardware and embedded software
- **UBPD**: Ultra Basic Position Detector, the primary position detection system
- **ThresholdManager**: System for managing sensor calibration thresholds
- **Config**: Header-only implementation containing system-wide constants and configuration values
- **LEDInterface**: Hardware abstraction layer for controlling the LED system
- **MPU9250Interface**: Hardware abstraction layer for the motion processing unit
- **GestureTransitionTracker**: Component that tracks transitions between two hand positions within a specified time window

#### Operational Modes

- **Idle Mode**: Default operation mode displaying current hand position with color indicators, detecting QuickCast gestures and Freecast trigger.
- **QuickCastSpells Mode**: Executes QuickCast spell effects.
- **Freecast Mode**: Mode for creating dynamic motion-based light patterns.
- **~~Invocation Mode~~**: Deprecated.
- **~~Resolution Mode~~**: Deprecated.

#### Hand Positions

- **Offer**: Palm up, fingers extended
- **Calm**: Palm down, fingers extended
- **Oath**: Palm vertical (thumb up), fingers extended
- **Dig**: Fingers pointed down
- **Shield**: Palm vertical (thumb up), facing outward
- **Null**: Palm vertical, facing inward

#### Gestures

- **CalmOffer**: Transition from Calm to Offer position
- **DigOath**: Transition from Dig to Oath position
- **NullShield**: Transition from Null to Shield position
- **LongShield**: Extended duration in Shield position
- **~~LongNull~~**: Extended duration in Null position (deprecated)
- **~~LongOffer~~**: Extended duration in Offer position (deprecated)

### Implementation Status Indicators

- **[PLANNED]**: Feature or component is planned but not yet started
- **[IN PROGRESS]**: Work has begun but is not complete
- **[IMPLEMENTED]**: Code exists and functions in test environments but not fully verified in system context
- **[VERIFIED]**: Implementation exists and has been fully tested in production context with all system integrations
- **[DEPRECATED]**: Component exists but has been superseded by a newer implementation

### Verification Standards

- **Test Environment Functionality**: Component works correctly in isolation
- **System Integration Verification**: Component functions in full system context
- **Production Verification**: Component meets all requirements in production environment

### Documentation Standards

- **Chronicle**: Primary running journal of development activity and historical record
- **Roadmap**: Mid-weight summary of project direction, priority features, and backlog
- **DirectoryIndex**: High-level index of codebase structure and key files
- **Glossary**: Shared terminology definitions and protocols

### Date Standards

- **Format**: All dates must use YYYY-MM-DD format (e.g., 2025-03-28)
- **Timezone**: All timestamps reference Eastern Standard Time (EST)
- **Document Alignment**: Related timestamp references must be synchronized across documents
- **Future Dating**: Only use future dates for explicit planned events with clear timelines

### Environment Scheme

The project uses a simplified environment scheme to maintain clarity and focus:

- **esp32dev**: Main production environment for the full application
- **calibration**: Environment for calibrating the device's position detection system
- **functionTest**: Flexible testing environment that replaces all other ad-hoc test environments

The functionTest environment is designed to be reconfigured for each new testing need. When a new test is required, the existing test code should be archived to archive/tests/ before the environment is updated with new test code.

### Development Protocols

#### Chronicle Transition Protocol

Process for transitioning from one chronicle version to the next when character limits are reached.

1. **Preparation**:
   - Create chronicle_vN+1.md file with standard header
   - Generate migration summary from chronicle_vN.md
   - Compile completed components, active work, critical decisions, and issues

2. **Execution**:
   - Create new entry in chronicle_vN+1.md with migration summary
   - Move chronicle_vN.md to archive/chronicles/
   - Update all references to chronicle files

3. **Verification**:
   - Ensure backwards navigability
   - Confirm chronological accuracy
   - Verify all critical information was preserved

#### Documentation Maintenance Protocol

Process for keeping documentation aligned with codebase changes.

1. **Code-First Updates**: Always update code before documentation
2. **Concurrent Documentation**: Update related documentation as part of the same commit
3. **Cross-Reference Verification**: Ensure consistency across all documentation
4. **Reference Path Validation**: Verify all file paths referenced in documentation exist

#### Full Git Commit

A comprehensive git operation that captures the complete state of the project at a specific point in time, ensuring the ability to restore the entire codebase to exactly this state if needed.

1. **Required Steps**:
   - Stage all modified files (`git add .` or targeted `git add` for all relevant files)
   - Include all new files that should be part of the project
   - Confirm removal of any deleted files
   - Create a commit with a descriptive message that summarizes the current state
   
2. **Key Characteristics**:
   - Captures the entire working directory state, not just specific files
   - Includes configuration files, documentation, and code changes
   - Creates a complete, restorable snapshot of the project
   - Preserves the relationships between components

3. **Usage Context**:
   - After completing significant work phases
   - Before major architectural changes
   - When reaching stable implementation milestones
   - At specified project checkpoints

4. **Verification**:
   - Review the git status to confirm no untracked or modified files remain
   - Ensure the commit message clearly identifies the project state
   - Confirm all build artifacts and environment-specific files are properly ignored

---

## 🏆 Quality Standards

| Standard | Description |
|----------|-------------|
| **Clean Code** | Code that is readable, maintainable, and follows project conventions. |
| **Initialization Verification** | All components must verify successful initialization before proceeding. |
| **Error Handling** | All errors must be explicitly handled with appropriate feedback mechanisms. |
| **Non-blocking Architecture** | All operations should use non-blocking patterns to maintain system responsiveness. |

---

## 🔧 Technical Terms

| Term | Definition |
|------|------------|
| **Activation Energy** | The amount of motion required to trigger a position change. |
| **Calibration Mode** | Special system mode for adjusting sensor thresholds. |
| **CalibrationThreshold** | The value that defines the transition between different hand positions. |
| **CalibrationValue** | The value that defines the transition between different hand positions. |
| **Physical Units** | Sensor readings converted to standard physical units (e.g., m/s², gauss) rather than raw values. |
| **Threshold Boundaries** | The values that define the transition between different hand positions. |

---

> Last updated: 2025-04-02

> Claude should suggest additions or edits when new terms, acronyms, or conventions arise during development.
