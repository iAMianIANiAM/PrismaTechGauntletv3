# Lightweight Universal Troubleshooting Toolkit (LUTT) Guide

## Executive Summary

The Lightweight Universal Troubleshooting Toolkit (LUTT) is a minimal, zero-overhead diagnostic system for the PrismaTech Gauntlet 3.0 project. It provides critical debugging capabilities with minimal code impact, focusing on the four most likely problem areas: gesture detection, animation states, mode transitions, and motion analysis.

LUTT consists of four independent components:

1. **DiagnosticLogger** - Standardized diagnostic output with tag filtering and log levels
2. **StateSnapshotCapture** - JSON-based state serialization at critical points
3. **VisualDebugIndicator** - LED-based visual feedback without disrupting normal operation  
4. **CommandLineInterface** - Serial command processor for runtime diagnostic control

The toolkit implementation follows these guiding principles:
- **Extreme Simplicity**: The simplest possible implementation that solves the problem
- **Minimal Intrusiveness**: Minimal impact on existing code; easy to add or remove
- **Zero Runtime Impact**: No performance penalty when diagnostics aren't active  
- **Universal Applicability**: Tools that work across multiple subsystems
- **Incremental Value**: Each component is independently useful without requiring others

## Implementation Approach

LUTT adopts a dual implementation approach:

1. **Main build (esp32dev)**: Diagnostic tools are included but disabled by default. This allows you to quickly enable them (by changing flags) without switching builds.

2. **lutt_debug environment**: All diagnostic features are enabled by default, offering a comprehensive diagnostic setup for intensive debugging sessions.

This dual approach ensures diagnostics are available when you need them but have zero overhead in production builds.

## AI Assistant Guide: Using LUTT for Diagnostics

This section provides guidance for AI assistants on when and how to use LUTT effectively.

### When to Use LUTT Diagnostics

#### Quick Diagnostics (Modify Main Build)
Use this approach when:
- You need to add temporary diagnostics to a specific area
- You want to preserve the normal behavior of the system
- You need to understand an isolated issue

#### Intensive Diagnostics (Use lutt_debug Environment)
Use this approach when:
- You need comprehensive diagnostics across multiple subsystems
- You're investigating a complex issue with unclear root cause
- You want to monitor system behavior over time
- You need to instrument multiple components at once

### Integration Patterns

#### DiagnosticLogger

Add logging at:
- Entry/exit points of critical functions
- State transitions
- Error cases 
- Timing-sensitive operations

```cpp
// Log at entry points
void GestureTransitionTracker::evaluateGesture(HandPosition pos) {
  DIAG_INFO(DIAG_TAG_GESTURE, "Evaluating gesture for position %d with confidence %.2f", 
            pos.position, pos.confidence);
            
  // Function implementation...
}

// Log state transitions 
if (prevPos != currPos) {
  DIAG_INFO(DIAG_TAG_GESTURE, "Position changed from %d to %d", prevPos, currPos);
}

// Log errors
if (confidence < CONFIDENCE_THRESHOLD) {
  DIAG_WARNING(DIAG_TAG_GESTURE, "Low confidence detection: %.2f", confidence);
}
```

#### StateSnapshotCapture

Use snapshots to capture complex state at critical points:

```cpp
void GestureDetector::beginGestureSequence() {
  // Capture state at start of gesture
  StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_GESTURE_START, "GestureDetector::beginGestureSequence");
  StateSnapshotCapture::addField("gestureType", _currentGestureType);
  StateSnapshotCapture::addField("timestamp", millis());
  StateSnapshotCapture::addField("position", _currentPosition);
  StateSnapshotCapture::addField("confidence", _currentConfidence);
  
  // Rest of implementation...
}
```

#### VisualDebugIndicator

Use for visualizing state without requiring serial connection:

```cpp
// Visualize gesture progress 
void GestureTransitionTracker::update() {
  // Calculate progress (0.0 to 1.0)
  float progress = (float)(millis() - _gestureStartTime) / (float)_gestureDuration;
  
  // Update the visual indicator (will only have effect in diagnostic builds)
  VisualDebugIndicator::updateGestureProgress(progress, _gestureType);
  
  // Normal implementation continues...
}
```

#### CommandLineInterface

Register custom commands to control component behavior:

```cpp
void setupModeController() {
  // Register mode-specific debug command
  CommandLineInterface::registerCommand("mode", handleModeCommand);
  
  // Implementation continues...
}

// Command handler
void handleModeCommand(int argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("Usage: mode [info|switch|reset]");
    return;
  }
  
  if (strcmp(argv[1], "info") == 0) {
    // Print current mode info
    Serial.printf("Current mode: %s\n", getCurrentModeName());
  } else if (strcmp(argv[1], "switch") == 0 && argc > 2) {
    // Switch to specified mode
    int modeId = atoi(argv[2]);
    switchToMode(modeId);
    Serial.printf("Switched to mode: %d\n", modeId);
  }
}
```

### Best Practices

1. **Add Diagnostic Points Strategically**
   - Focus on state transitions and complex logic
   - Instrument entry/exit points of critical functions
   - Capture key variable values at decision points

2. **Use Appropriate Log Levels**
   - CRITICAL: System cannot continue functioning
   - ERROR: Operation failed but system can continue
   - WARNING: Unexpected state that might cause problems
   - INFO: Important state changes and operations
   - DEBUG: Detailed information for troubleshooting
   - TRACE: Very detailed step-by-step execution

3. **Prefer Conditional Compilation**
   - Use `#if` directives to avoid runtime overhead
   - Ensure diagnostic code is completely removed in production builds

4. **Minimal Memory Usage**
   - Use static allocation for diagnostic data
   - Clear/reset diagnostic state regularly
   - Consider the stack impact of diagnostic function calls

## User Guide: Using LUTT in Practice

This section provides guidance for human users on how to utilize the LUTT diagnostics effectively.

### Build Options

LUTT gives you two main ways to use diagnostics:

1. **Main Build** - For quick diagnostics of specific issues:
   ```
   pio run -e esp32dev
   ```
   The diagnostic flags are disabled by default in main builds. To enable specific diagnostics, modify the build flags in `platformio.ini` temporarily.

2. **lutt_debug Environment** - For comprehensive diagnostics:
   ```
   pio run -e lutt_debug
   ```
   All diagnostic features are enabled by default in this build.

### Serial Console Commands

Once you have a build with LUTT enabled (either main build with flags or lutt_debug), you can use these commands in the serial console:

#### Basic Commands

- `help` - Show available commands
- `log [level] [enable]` - Control diagnostic logging level and output
- `snapshot [trigger]` - Request a state snapshot with specified trigger
- `visual [type] [params]` - Control visual indicators
- `test [component]` - Run component test
- `dump` - Show current system state

#### Example Usage

1. **Enable INFO level logs for gesture system**:
   ```
   log 4 1
   ```

2. **Capture a snapshot at next gesture start**:
   ```
   snapshot 1
   ```

3. **Display gesture progress visually on LEDs**:
   ```
   visual 0 0.5 1
   ```
   This shows 50% progress of gesture type 1

### Interpreting Diagnostic Output

#### Log Messages

Diagnostic logs follow this format:
```
[timestamp] [TAG] message
```

Example:
```
12345 [GESTURE] Position changed to POS_OATH with confidence 0.95
```

The timestamp is milliseconds since system startup, helping identify timing issues.

#### State Snapshots

Snapshots provide a JSON representation of system state at important points:

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

Use snapshots to understand the exact system state during specific events.

### Troubleshooting Common Issues

#### Gesture Recognition Problems

1. **Enable gesture diagnostics**:
   ```
   log 5 1
   ```

2. **Watch for position changes and gesture progress**:
   ```
   visual 0 0 0
   ```

3. **Look for confidence issues in logs**:
   - Low confidence values (< 0.6) may indicate sensor issues
   - Fluctuating positions may indicate threshold issues

#### Mode Transition Issues

1. **Enable mode diagnostics**:
   ```
   log 4 3
   ```

2. **Capture snapshots at mode changes**:
   ```
   snapshot 4
   ```

3. **Check for incomplete state cleanup** in snapshots when modes change

#### Animation Sync Issues

1. **Enable animation diagnostics**:
   ```
   log 4 2
   ```

2. **Visualize animation phases**:
   ```
   visual 2 0 0
   ```

3. **Look for timing inconsistencies** in the logs

#### Motion Analysis (FreeCast) Issues

1. **Enable motion diagnostics**:
   ```
   log 5 4
   ```

2. **Visualize motion features**:
   ```
   visual 3 0.5 0.7
   ```

3. **Capture snapshots during motion**:
   ```
   snapshot 16
   ```

## Implementation Reference

### Header Files

```
src/diagnostics/DiagnosticLogger.h
src/diagnostics/StateSnapshotCapture.h
src/diagnostics/VisualDebugIndicator.h
src/diagnostics/CommandLineInterface.h
```

### Compilation Flags

| Flag | Description | Default |
|------|-------------|---------|
| `DIAG_LEVEL` | Log level (0-6, 0=disabled) | 0 |
| `DIAG_TAG_FILTER` | Tag filter (subsystem bitmask) | 0xFF |
| `SNAPSHOT_TRIGGER_FILTER` | Snapshot trigger filter (bitmask) | 0xFF |
| `VISUAL_DEBUG_ENABLED` | Enable visual indicators | 0 |
| `CLI_ENABLED` | Enable command-line interface | 0 |

### Memory Usage

| Component | Static RAM | Flash Usage |
|-----------|------------|-------------|
| DiagnosticLogger | ~20 bytes | ~250 bytes |
| StateSnapshotCapture | ~520 bytes | ~600 bytes |
| VisualDebugIndicator | ~60 bytes | ~500 bytes |
| CommandLineInterface | ~80 bytes | ~800 bytes |
| **Total Impact** | **~680 bytes** | **~2150 bytes** |

Note: When disabled via build flags, memory impact is near-zero as the preprocessor removes diagnostic code.

## Conclusion

The Lightweight Universal Troubleshooting Toolkit (LUTT) provides a powerful yet minimally invasive diagnostic system for the PrismaTech Gauntlet 3.0 project. By following the guiding principles of KISS, DRY, and YAGNI, LUTT delivers essential diagnostic capabilities without compromising system performance or increasing maintenance burden.

Whether you're dealing with gesture recognition issues, mode transitions, animation states, or motion analysis, LUTT provides the tools you need to understand and resolve complex problems efficiently. 