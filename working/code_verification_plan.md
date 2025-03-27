# Code Verification Plan and UBPD Implementation Priorities

## Build Verification Plan

To completely verify the compilation status of all environments, the following commands will need to be run manually:

```bash
# Main application environment
pio run -e esp32dev

# Test environments
pio run -e idletest
pio run -e simplepostest
pio run -e postest
pio run -e calibration
pio run -e hwmtest
```

Results should be documented to:
1. Verify each environment builds successfully
2. Note any compilation errors requiring fixes
3. Identify any deprecated or outdated dependencies
4. Document warnings that may impact stability

## Runtime Verification Plan

After successful compilation, runtime verification should include:

1. Upload and test the basic position detection:
   ```bash
   pio run -e postest -t upload
   pio device monitor -p COM7 -b 115200
   ```

2. Test the hardware manager to ensure all components are functional:
   ```bash
   pio run -e hwmtest -t upload
   pio device monitor -p COM7 -b 115200
   ```

3. Test the calibration protocol:
   ```bash
   pio run -e calibration -t upload
   pio device monitor -p COM7 -b 115200
   ```

## UBPD Implementation Priorities

Based on our analysis, the UBPD implementation should follow these priorities:

1. **Create Base Implementation Files**:
   - `/src/detection/UltraBasicPositionDetector.h`
   - `/src/detection/UltraBasicPositionDetector.cpp`
   - `/examples/component_tests/UltraBasicPositionTest.cpp`

2. **Implementation Requirements (in order)**:
   - Physical unit conversion (raw data to m/s²)
   - Simplified detection logic with absolute thresholds
   - Position-specific threshold management
   - Streamlined calibration process integration

3. **Integration Path**:
   1. Create standalone test first (UltraBasicPositionTest)
   2. Verify functionality in isolation
   3. Integrate with GauntletController
   4. Update main application environment to use UBPD implementation

4. **Validation Strategy**:
   - Create quantifiable metrics for detection accuracy
   - Compare performance against existing PositionDetector
   - Measure response time and consistency in different positions
   - Verify calibration recovery and persistence 