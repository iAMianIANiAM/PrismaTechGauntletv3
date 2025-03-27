# Current Development Plan - PrismaTech Gauntlet 3.0 (v2)

## Current Project Status Assessment

The project has experienced a significant setback due to version control issues, resulting in a discrepancy between the documented implementations and the actual codebase. This document represents our current understanding of the project status and the plan to recover and move forward.

### Current Issues

1. **Missing Primary Detection Model**: The Ultra Basic Position Detection (UBPD) system, which is the primary detection model for the entire device, is missing from the codebase despite being described in detail in working documents.

2. **Version Control Inconsistency**: There appears to be a significant disconnect between the git repository's state and the documented progress, suggesting issues with commit procedures or backup protocols.

3. **Documentation vs. Reality Mismatch**: The working documents (roadmap.md, directoryIndex.md, and previous currentplan.md) describe functionality and files that do not exist in the current codebase.

### What We Currently Have

Based on our analysis of the existing codebase:

1. **Hardware Interface Layer**: The MPU9250 interface, LED interface, and Hardware Manager implementations appear to be present and functional.

2. **Position Detection Core**: The basic Position Detector using the Point Detection Model (dominant axis approach) is implemented and functional. However, this is not the final optimized UBPD approach that was intended to be used.

3. **Calibration Protocol**: The calibration protocol for collecting position data and generating thresholds is present.

4. **Test Environments**: Several test environments for hardware and position detection testing exist in the platformio.ini file.

### What's Missing

1. **Ultra Basic Position Detection (UBPD)**: The primary detection model for the entire device is missing from the codebase. This model was designed to:
   - Convert sensor data into meaningful real-world units (m/s²)
   - Implement an optimized detection algorithm with absolute thresholds
   - Provide a simplified yet robust calibration protocol
   - Offer superior reliability through clear, direct data processing
   - The actual implementation files (`UltraBasicPositionDetector.h/cpp` and `/examples/component_tests/UltraBasicPositionTest.cpp`) must be recreated.

2. **ECHO Document**: Referenced ECHO documentation that would provide insights from previous implementations is not present.

> **IMPORTANT NOTE**: The other detection models documented (Hand-Cube Orientation Model and Hybrid Position Detection) were experimental approaches that proved overly complex and less reliable in testing. These do NOT need to be recovered. Our focus is solely on recovering and implementing the UBPD model, which is the intended final detection approach for the device.

## Recovery and Forward Plan

### Phase 1: Stabilization and Assessment ✓

1. **Harmonize Working Documents and Codebase**: Update all working documents (roadmap.md, directoryIndex.md, and this currentplan_v2.md) to accurately reflect the current state of the codebase.
   - [x] Create UBPD.md to preserve knowledge about the Ultra Basic Position Detection approach
   - [x] Update directoryIndex.md to accurately reflect existing files and their purpose
   - [x] Update roadmap.md with revised timelines and completion status

2. **Repository Stabilization**: Ensure local and remote repositories are in sync and properly backed up.
   - [x] Verify current state of local git repository
     - Local repository exists with commit history
     - Local branch is ahead of origin/main by 4 commits
     - Multiple uncommitted changes present in the working directory
   - [x] Check remote repository status and synchronization
     - Remote repository is set up at https://github.com/iAMianIANiAM/PrismaTechGauntletv3.git
     - Local changes need to be pushed to synchronize
   - [x] Establish proper backup procedures to prevent future data loss
     - Created git_backup_protocol_draft.md with detailed backup procedures
     - Protocol includes commit, push, and branch management guidelines
     - Includes verification steps to ensure repository integrity
   - [x] Create a recovery branch to track our rebuilding efforts
     - Branch 'recovery-phase1' successfully created and active
     - Recovery branch will be used for all Phase 1 rebuilding work

3. **Code Inventory and Verification**: Thoroughly catalog all existing code to confirm what's actually present and functional.
   - [x] List all implementation files and their current status
     - Hardware layer implemented: HardwareManager, MPU9250Interface, LEDInterface, PowerManager
     - Position detection: PositionDetector (basic implementation) present without UBPD
     - Calibration protocol present and functional
     - Gesture recognition interface defined but not fully implemented
   - [x] Verify build status of all environments in platformio.ini
     - Active production environment: esp32dev
     - Active test environments: idletest, simplepostest, postest, calibration, hwmtest
     - Several archived test environments commented out for reference
   - [x] Document any compilation errors or runtime issues
     - Created code_verification_plan.md with specific build verification steps
     - Listed commands to test runtime functionality of key components
   - [x] Create a priority list focusing on UBPD implementation
     - Defined implementation order in code_verification_plan.md
     - Established file requirements, implementation sequence, and validation strategy

### Phase 2: Process Improvement

1. **Workflow and Rules Review**: Assess our development workflow to prevent similar issues in the future.
   - [x] Review working document usage patterns
   - [x] Analyze our version control practices
   - [x] Identify points of failure in our backup procedures
   - [x] Develop recommendations for workflow improvements
   - [x] Create detailed implementation plan for priority improvements in working/workflow_improvement_plan.md

2. **Git Backup Protocol Implementation**: Create and implement a formal git backup protocol.
   - [ ] Draft backup protocol rules with clear steps and timing
     - Initial draft created: working/git_backup_protocol_draft.md
     - Needs review and finalization in Phase 2
   - [ ] Define triggers for backups (code milestones, time intervals)
   - [ ] Establish verification procedures for backup success
   - [ ] Create automation scripts if appropriate

3. **Documentation Improvements**: Enhance our documentation practices to better track implementation status.
   - [ ] Add implementation status tracking to directoryIndex.md
   - [ ] Create file existence verification in our review process
   - [ ] Improve cross-referencing between documentation and code

### Phase 3: Implementation Recovery

1. **ECHO Document Integration**: Import missing ECHO document to provide insights from previous implementation.
   - [x] Locate and integrate the ECHO document
   - [x] Extract key lessons and implementation details
   - [x] Update references in working documents

2. **Ultra Basic Position Detection Implementation**: Rebuild the UBPD implementation based on our documentation.
   - [x] Create detailed implementation plan for UBPD system
     - Approved implementation plan includes:
       - Data processing pipeline using physical units (m/s²)
       - Simple 3-sample averaging for noise reduction
       - Direct threshold comparison for position detection
       - Interactive calibration with 80% threshold scaling
       - Consistent -10 to +10 scale matching ECHO approach
   - [x] Create UltraBasicPositionDetector.h/.cpp files with:
     - [x] Physical unit conversion (raw data to m/s²)
     - [x] Simplified detection logic with absolute thresholds
     - [x] Position-specific threshold management
     - [x] Streamlined calibration process
   - [x] Implement UltraBasicPositionTest.cpp
   - [x] Add ultrabasic environment to platformio.ini
   - [ ] Test and validate the implementation on physical hardware

## Key Priorities

1. **Stabilize/harmonize working documents, codebase, and git repo** (both local and remote). This is our starting point, as rough as it is, and we're going to make that ground as solid as we can.

2. **Perform a workflow/rules review**. We'll assess our workflow, including working document usage and general protocols. We're not going to undertake any major changes, but want to consider a careful analysis. We will, however, be drafting and implementing a rule specifying exactly what the git backup protocol is going to be moving forward, to avoid a similar catastrophe.

3. **Bring in the missing ECHO document** and, combined with our new UBPD.md, rebuild the Ultra Basic Position Detection system as the primary detection model for the device.

## Next Steps

**Phase 1 Complete**: All stabilization and assessment tasks have been completed successfully. The current state of the project is now properly documented, and a recovery branch has been established.

**Phase 2 Implementation Progress**: 
- The Ultra Basic Position Detection (UBPD) system has been implemented with the following components:
  - `UltraBasicPositionDetector.h` - Interface with physical unit conversion and threshold detection
  - `UltraBasicPositionDetector.cpp` - Implementation with 3-sample averaging and calibration
  - `UltraBasicPositionTest.cpp` - Test application with calibration mode and visualization
  - `platformio.ini` updated with `env:ultrabasic` environment
  
- Next steps in Phase 2:
  - Build the implementation with `pio run -e ultrabasic`
  - Upload to physical hardware with `pio run -e ultrabasic -t upload`
  - Test position detection accuracy
  - Compare with original Dominant Axis detection approach
  - Document findings and adjust if needed

3. Prepare for Phase 3: Implementation Recovery
   - Locate and integrate the missing ECHO document
   - Begin planning for UBPD implementation based on documentation

This plan represents our path forward to recover from the current setback and rebuild the project on a more stable foundation. We will proceed methodically, ensuring that our documentation accurately reflects reality at each step, and that our version control practices prevent similar issues in the future.

## Approved Hybrid Plan: Process Improvement + UBPD Implementation

Based on our project principles of Simplicity, Prudence, and Diligence, we have developed the following hybrid plan to implement both critical process improvements and the Ultra Basic Position Detection system.

### Phase 1: Immediate Process Stabilization (Days 1-2)

#### 1. Git Backup Protocol Implementation
- Finalize the draft protocol document with clear, actionable steps
- Implement the simplest effective pre-commit hook that verifies file existence
- Create a standardized commit message template
- Test the protocol with a sample commit cycle

#### 2. Working Document Alignment
- Update directoryIndex.md with accurate [PLANNED] status for missing files
- Ensure clear status indications for all components
- Verify that roadmap.md and currentplan_v2.md are consistent with each other
- Document the current state of all test environments

### Phase 2: UBPD Implementation (Days 3-5)

#### 1. Preparation and Planning
- [x] Review existing PositionDetector implementation
- [x] Document the exact changes needed for the UBPD model
- [x] Outline class structure and method requirements
- [ ] Add env:ultrabasic to platformio.ini

#### 2. Core Implementation
- [x] Create UltraBasicPositionDetector.h with clear interface definition
- [x] Implement UltraBasicPositionDetector.cpp with:
  - Physical unit conversion (raw to m/s²)
  - 3-sample averaging for noise reduction
  - Absolute threshold implementation
  - Position-specific threshold management
- [x] Thoroughly document code with meaningful comments

#### 3. Test Application Implementation
- [x] Develop UltraBasicPositionTest.cpp test application
- [x] Include debug output of physical units
- [x] Implement LED feedback for detected positions
- [x] Add simple calibration capability for testing

#### 4. Verification Process
- [x] Add env:ultrabasic to platformio.ini for building the test application
- [x] Fix UltraBasicPositionTest.cpp to define BUTTON_PIN locally instead of using Config namespace
- [x] Update calibration method to use serial commands instead of physical button
- [ ] Test implementation on physical hardware
- [ ] Verify position detection accuracy
- [ ] Compare with original Dominant Axis detection approach
- [ ] Document findings and adjust if needed

### Phase 3: Documentation and Integration (Days 6-7)

#### 1. Working Document Updates
- Update all working documents to reflect new implementation
- Mark appropriate items as [IMPLEMENTED]
- Document any deviations from original design
- Update thresholds and calibration parameters

#### 2. Repository Maintenance
- Commit implementation with proper documentation references
- Push changes to remote repository
- Verify branch synchronization
- Update project status in all documents

#### 3. Integration Planning
- Outline steps to integrate UBPD with main application
- Document configuration changes needed
- Prepare for hardware testing and refinement

## Implementation Details

### UltraBasicPositionDetector Design

```cpp
// UltraBasicPositionDetector.h (key elements)

class UltraBasicPositionDetector {
public:
  bool init(HardwareManager* hardware);
  PositionReading detectPosition(const SensorData& sensorData);
  PositionReading getCurrentPosition() const;
  
#ifdef TEST_MODE
  bool calibrate(uint8_t position, uint16_t samples = 50);
  void setThreshold(uint8_t position, float threshold);
#endif
  
private:
  // Hardware reference
  HardwareManager* _hardware = nullptr;
  
  // Position reading
  PositionReading _currentPosition;
  
  // Averaging buffer
  static const uint8_t POSITION_AVERAGE_SAMPLES = 3;
  SensorData _sampleBuffer[POSITION_AVERAGE_SAMPLES];
  uint8_t _currentSampleIndex = 0;
  
  // Thresholds in m/s² (physical units)
  float _positiveXThreshold = 7.0f;
  float _negativeXThreshold = -7.0f;
  float _positiveYThreshold = 7.0f;
  float _negativeYThreshold = -7.0f;
  float _positiveZThreshold = 7.0f;
  float _negativeZThreshold = -7.0f;
  
  // Internal methods
  void processRawData(const SensorData& raw, ProcessedData& processed);
  uint8_t detectPosition(const ProcessedData& data);
  SensorData calculateAveragedData();
};
```

### UltraBasicPositionTest Structure

```cpp
// Key elements of test application

void setup() {
  Serial.begin(115200);
  Serial.println("Ultra Basic Position Detection Test");
  
  if (!hardware.init()) {
    Serial.println("Hardware initialization failed!");
    while (1) { delay(1000); }
  }
  
  if (!positionDetector.init(&hardware)) {
    Serial.println("Position detector initialization failed!");
    while (1) { delay(1000); }
  }
  
  // Initialize LED with position colors
  hardware.getLED()->init();
  hardware.getLED()->setPositionColors(positionColors);
  
  Serial.println("Setup complete. Starting position detection...");
}

void loop() {
  // Read sensor data
  SensorData sensorData = hardware.getSensorData();
  
  // Detect position
  PositionReading reading = positionDetector.detectPosition(sensorData);
  
  // Update LED based on position
  hardware.getLED()->setPositionColor(reading.position);
  
  // Display position information at regular intervals
  static uint32_t lastPrintTime = 0;
  if (millis() - lastPrintTime > 500) {
    printPositionInfo(reading, sensorData);
    lastPrintTime = millis();
  }
  
  // Handle serial commands
  handleSerialCommands();
  
  delay(20);
}
```

## Success Criteria

1. Git backup protocol is actively followed for all implementation work
2. Working documents accurately reflect implementation status at all times
3. UBPD implementation correctly converts raw values to m/s²
4. Test application demonstrates position detection capabilities
5. All code is documented and verification steps are clear
6. Changes are committed and pushed to remote repository
7. Repository and working documents remain in sync

## Current Implementation Status

We are now beginning Phase 1 of the hybrid plan: Immediate Process Stabilization. 

### Implementation Status

- [x] Define UltraBasicPositionDetector interface
- [x] Implement core detection algorithm based on physical units
- [x] Create test application (UltraBasicPositionTest.cpp)
- [x] Implement calibration routine
- [x] Document threshold values and calibration process
- [x] Integrate UBPD into main application

### Next Steps

- [ ] Create comprehensive documentation for position detection system
- [ ] Consolidate test environments to reduce code duplication
- [ ] Implement preference saving for calibrated thresholds
- [ ] Develop improved visual feedback during position detection

We are now beginning Phase 1 of the hybrid plan: Immediate Process Stabilization. 