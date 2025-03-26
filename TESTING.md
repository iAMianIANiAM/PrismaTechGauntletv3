# PrismaTech Gauntlet 3.0 Testing Guide

This document provides guidelines for testing the PrismaTech Gauntlet 3.0 project, including information about test environments, how to run tests, and how to manage test code.

## Test Environment Structure

The project uses a structured approach to testing with clearly separated environments:

```
[environments]
├── production/    <- Production-ready environments
├── testing/       <- Active test environments
└── archive/       <- Archived test environments
```

### Directory Organization

```
/examples
  /production           <- Examples showing main application usage
  /component_tests      <- Active component test applications
  /archived_tests       <- Historical test applications

/test
  /utils               <- Shared test utilities
  /fixtures            <- Test data and fixtures
  /archived            <- Archived test code
```

## Available Test Environments

### Active Test Environments

| Environment | Purpose | Command |
|-------------|---------|---------|
| `postest` | Test position detection system | `pio run -e postest -t upload` |
| `calibration` | Run calibration protocol | `pio run -e calibration -t upload` |
| `hwmtest` | Test hardware manager integration | `pio run -e hwmtest -t upload` |

### Archived Test Environments

These environments are maintained for reference but commented out in platformio.ini. To use them, uncomment the relevant section in platformio.ini first.

| Environment | Purpose | Command (after uncommenting) |
|-------------|---------|---------|
| `ledtest` | Test LED subsystem | `pio run -e ledtest -t upload` |
| `mpudiag` | MPU diagnostic tests | `pio run -e mpudiag -t upload` |
| `mputest` | MPU unit tests | `pio run -e mputest -t upload` |
| `mpufilter` | Test MPU filtering | `pio run -e mpufilter -t upload` |

## Running Tests

### Building and Uploading

To build and upload a specific test environment:

```bash
pio run -e [environment_name] -t upload
```

Example:
```bash
pio run -e postest -t upload
```

### Monitoring Test Output

To view serial output from tests:

```bash
pio device monitor -p COM7 -b 115200
```

Or with direct upload and monitoring:

```bash
pio run -e postest -t upload && pio device monitor -p COM7 -b 115200
```

### Important Warning

**NEVER** run just `pio run` without specifying an environment, as this will attempt to build ALL environments including archived ones, which may fail or cause confusion.

## Creating New Tests

1. **Decide Test Scope**: Determine if you need a component test or integration test
2. **Choose Location**: Place new test code in appropriate directory
   - Component tests go in `/examples/component_tests/`
   - Integration tests may go in the main source directory
3. **Create Environment**: Add a new environment to platformio.ini following the pattern of existing test environments
4. **Isolate Dependencies**: Include only the necessary source files in `build_src_filter`
5. **Set Test Flags**: Always include `-D TEST_MODE=1` in build flags

### Example New Test Environment

```ini
[env:newtest]
platform = espressif32
board = esp32dev
framework = arduino
upload_port = COM7
monitor_port = COM7
monitor_speed = 115200
build_flags = 
    -D SERIAL_DEBUG=1
    -D TEST_MODE=1
build_src_filter = -<*> +<../examples/component_tests/NewTest.cpp> +<required/source/files.cpp>
lib_deps = 
    # required libraries
```

## Test-Only Code in Header Files

To maintain clean separation between test and production code, use preprocessor guards for test-only methods:

```cpp
// Regular production methods...

#ifdef TEST_MODE
/**
 * @brief TESTING ONLY: Additional method for testing
 * @note This method is only for testing, not for production use
 */
void testOnlyMethod();
#endif
```

## Archiving Test Code

When a test is no longer actively used but might be valuable for reference:

1. Move the test implementation to `/examples/archived_tests/` or `/test/archived/`
2. Comment out the environment in platformio.ini and move it to the ARCHIVED section
3. Add a note explaining why it was archived and when it might be useful

## Troubleshooting Tests

### Common Issues

1. **Build Failures**: Check if you're including all necessary source files and dependencies
2. **Configuration Issues**: Verify pin assignments and hardware setup match your test expectations
3. **Serial Communication**: Ensure monitor port and baud rate match your device

### Getting Help

If you encounter issues with the testing system:

1. Check this documentation first
2. Review the environment setup in platformio.ini
3. Look for similar tests as examples
4. Make sure you're using the correct environment for your test

## Best Practices

1. **Isolation**: Test environments should include ONLY what they need
2. **Documentation**: Document what your test does and how to interpret results
3. **Clean Separation**: Use preprocessor guards to separate test and production code
4. **Serial Output**: Include helpful debug output in test applications
5. **Hardware Validation**: Always check hardware connections before testing
6. **Failure Handling**: Add clear failure indicators (like red LEDs) for hardware test failures 