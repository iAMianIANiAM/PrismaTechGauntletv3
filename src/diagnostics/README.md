# Lightweight Universal Troubleshooting Toolkit (LUTT)

This directory contains the implementation of the Lightweight Universal Troubleshooting Toolkit (LUTT) for the PrismaTech Gauntlet 3.0 project. LUTT provides essential diagnostic capabilities with minimal code complexity and performance impact.

## Overview

LUTT consists of four lightweight components that can be used independently or together:

1. **DiagnosticLogger** - Standardized diagnostic output with tag filtering and log levels
2. **StateSnapshotCapture** - JSON-based state serialization at critical points
3. **VisualDebugIndicator** - LED-based visual feedback without disrupting normal operation
4. **CommandLineInterface** - Serial command processor for runtime diagnostic control

Each component is designed to have zero overhead when disabled, and minimal impact when enabled.

## Build Configuration

LUTT is controlled through build flags in `platformio.ini`. To enable diagnostic features, add the appropriate flags to your environment:

```ini
; Development build with full diagnostics
[env:esp32dev_debug]
build_flags = 
  -D DIAG_LEVEL=5                       ; Set diagnostic log level
  -D DIAG_TAG_FILTER=DIAG_TAG_ALL       ; Show logs from all subsystems
  -D SNAPSHOT_TRIGGER_FILTER=SNAPSHOT_TRIGGER_ALL  ; Enable all snapshot triggers
  -D VISUAL_DEBUG_ENABLED=1             ; Enable visual indicators
  -D CLI_ENABLED=1                      ; Enable command-line interface
```

### Common Configurations

#### Production Build (No Diagnostics)
```ini
build_flags = 
  -D DIAG_LEVEL=0
```

#### Minimal Diagnostics (Critical Issues Only)
```ini
build_flags = 
  -D DIAG_LEVEL=1
  -D DIAG_TAG_FILTER=DIAG_TAG_ALL
```

#### Gesture Debugging Focus
```ini
build_flags = 
  -D DIAG_LEVEL=5
  -D DIAG_TAG_FILTER=DIAG_TAG_GESTURE
  -D SNAPSHOT_TRIGGER_FILTER=SNAPSHOT_TRIGGER_GESTURE_START|SNAPSHOT_TRIGGER_GESTURE_END
  -D VISUAL_DEBUG_ENABLED=1
```

## Usage Guide

### Integrating LUTT Into Your Code

#### 1. DiagnosticLogger

First, include the header:
```cpp
#include "diagnostics/DiagnosticLogger.h"
```

Then, add logging statements where needed:
```cpp
// In GestureTransitionTracker.cpp
void GestureTransitionTracker::update(HandPosition position) {
  // Log position changes
  DIAG_LOG(DIAG_INFO, DIAG_TAG_GESTURE, "Position changed to %d with confidence %f", 
           (int)position.position, position.confidence);
           
  // Log errors with higher priority
  if (position.confidence < 0.3f) {
    DIAG_LOG(DIAG_WARNING, DIAG_TAG_GESTURE, "Low confidence detection: %f", position.confidence);
  }
}
```

#### 2. StateSnapshotCapture

First, include the header:
```cpp
#include "diagnostics/StateSnapshotCapture.h"
```

Then, capture state at critical points:
```cpp
// In IdleMode.cpp
void IdleMode::checkForTransition() {
  // Capture state at start of transition check
  StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_GESTURE_START, "IdleMode::checkForTransition");
  StateSnapshotCapture::addField("currentPosition", (int)currentPosition.position);
  StateSnapshotCapture::addField("confidence", currentPosition.confidence);
  StateSnapshotCapture::addField("nullHeldTime", nullHeldTime);
  StateSnapshotCapture::printSnapshot();
  
  // Existing code continues...
}
```

#### 3. VisualDebugIndicator

First, include the header:
```cpp
#include "diagnostics/VisualDebugIndicator.h"
```

Initialize in setup:
```cpp
// In main.cpp or GauntletController.cpp
void setup() {
  // After hardware manager is initialized
  VisualDebugIndicator::init(&hardwareManager);
}
```

Add visualization calls:
```cpp
// In GestureTransitionTracker.cpp
void GestureTransitionTracker::update(HandPosition currentPosition) {
  // Existing code...
  
  // Update visual indicator for gesture progress
  if (isGestureActive) {
    float progress = (float)(millis() - gestureStartTime) / (float)gestureTimeWindow;
    VisualDebugIndicator::updateGestureProgress(progress, gestureType);
  }
}
```

Process indicators in the main loop:
```cpp
// In main.cpp or GauntletController.cpp
void loop() {
  // Normal processing
  
  // Allow debug indicators to process if enabled
  VisualDebugIndicator::process();
}
```

#### 4. CommandLineInterface

First, include the header:
```cpp
#include "diagnostics/CommandLineInterface.h"
```

Initialize in setup:
```cpp
// In main.cpp
void setup() {
  // After Serial is initialized
  CommandLineInterface::init();
  
  // Register custom commands
  CommandLineInterface::registerCommand("mycommand", handleMyCommand);
}

// Custom command handler
void handleMyCommand(int argc, char* argv[]) {
  if (argc > 1) {
    Serial.print("Argument: ");
    Serial.println(argv[1]);
  }
}
```

Process commands in the main loop:
```cpp
// In main.cpp
void loop() {
  // Process any pending commands
  CommandLineInterface::process();
  
  // Continue with normal operation
}
```

### Using LUTT During Debugging

#### Serial Diagnostic Output

When `DIAG_LEVEL` is set to a value greater than 0, diagnostic logs will be output over the serial port. Example:

```
12345 [GESTURE] Position changed to 2 with confidence 0.95
12450 [GESTURE] Position changed to 3 with confidence 0.87
12900 [WARNING] Low confidence detection: 0.25
```

#### State Snapshots

When `SNAPSHOT_TRIGGER_FILTER` includes the relevant triggers, state snapshots will be output as JSON data:

```
--- SNAPSHOT [13245] ---
{
  "time": 13245,
  "trigger": 1,
  "context": "IdleMode::checkForTransition",
  "currentPosition": 2,
  "confidence": 0.87,
  "nullHeldTime": 0
}
--- END SNAPSHOT ---
```

#### Visual Indicators

When `VISUAL_DEBUG_ENABLED` is set to 1, visual indicators will show diagnostic information on the LEDs without disrupting normal operation. For example:

- Gesture progress: LEDs fill up as gesture progresses (different colors for different gestures)
- Mode state: Different LEDs light up for different modes
- Animation phase: Color changes indicate progress through animation phases
- Motion features: Brightness and color changes indicate motion intensity and complexity

#### Command-Line Interface

When `CLI_ENABLED` is set to 1, a command-line interface is available over the serial port. Type `help` for available commands:

```
--- LUTT Command-line Interface ---
Type 'help' for available commands
> help
Available commands:
  help
  log
  snapshot
  visual
  test
  dump
  mycommand

Built-in command usage:
  help                  Show this help
  log <level> <enable>  Control diagnostic logging
  snapshot <trigger>    Capture state snapshot
  visual <type> <args>  Control visual indicators
  test <component>      Run component test
  dump <component>      Dump component state
```

## Extending LUTT

### Adding New Log Tags

To add a new diagnostic tag, add it to `DiagnosticLogger.h`:

```cpp
#define DIAG_TAG_MYNEW_TAG  0x10  // Use next available bit
```

Then add it to `getTagName()` in `DiagnosticLogger.cpp`:

```cpp
const char* DiagnosticLogger::getTagName(uint8_t tag) {
  switch(tag) {
    // Existing cases...
    case DIAG_TAG_MYNEW_TAG: return "MYNEW_TAG";
    default:                return "UNKNOWN";
  }
}
```

### Adding New Snapshot Triggers

To add a new snapshot trigger, add it to `StateSnapshotCapture.h`:

```cpp
#define SNAPSHOT_TRIGGER_MY_TRIGGER  0x40  // Use next available bit
```

### Adding New Visual Indicators

To add a new indicator type, add it to `VisualDebugIndicator.h`:

```cpp
#define VISUAL_IND_MY_INDICATOR    4  // Use next available number
```

Then implement a method in `VisualDebugIndicator.cpp` and update the `applyIndicator()` method.

### Adding New CLI Commands

To add custom commands, register them after initializing:

```cpp
CommandLineInterface::registerCommand("mycommand", handleMyCommand);
```

Then implement the handler function:

```cpp
void handleMyCommand(int argc, char* argv[]) {
  // Handle the command
}
```

## Troubleshooting

### Diagnostics Not Working

1. Check that the appropriate build flags are set in your `platformio.ini`
2. Verify that `Serial` is initialized before using diagnostic features
3. Check that the `process()` methods are being called in the main loop
4. Verify that hardware manager reference is passed to `VisualDebugIndicator::init()`

### Component-Specific Issues

#### DiagnosticLogger
- Logs not appearing: Check `DIAG_LEVEL` and `DIAG_TAG_FILTER` settings
- Make sure the log level used is <= the configured `DIAG_LEVEL`

#### StateSnapshotCapture
- Snapshots not appearing: Check `SNAPSHOT_TRIGGER_FILTER` settings
- Make sure to call `printSnapshot()` after adding fields

#### VisualDebugIndicator
- Indicators not showing: Check `VISUAL_DEBUG_ENABLED` setting
- Verify hardware manager is properly initialized and passed to init()
- Ensure `process()` is called in the main loop

#### CommandLineInterface
- Not responding: Check `CLI_ENABLED` setting
- Verify Serial is initialized at the appropriate baud rate
- Ensure `process()` is called in the main loop
``` 