/**
 * CommandLineInterface.cpp
 * 
 * Implementation of the CommandLineInterface class for the LUTT toolkit.
 */

#include "CommandLineInterface.h"
#include "DiagnosticLogger.h"
#include "StateSnapshotCapture.h"
#include "VisualDebugIndicator.h"

// Initialize static variables
char CommandLineInterface::_cmdBuffer[MAX_CMD_LENGTH + 1] = {0};
uint8_t CommandLineInterface::_bufIndex = 0;
bool CommandLineInterface::_initialized = false;
const char* CommandLineInterface::_commandNames[10] = {0};
CommandLineInterface::CommandCallback CommandLineInterface::_commandCallbacks[10] = {0};
uint8_t CommandLineInterface::_numCommands = 0;

/**
 * Initialize the CLI
 */
void CommandLineInterface::init() {
  if (!CLI_ENABLED) return;
  
  _bufIndex = 0;
  _initialized = true;
  _numCommands = 0;
  
  // Register built-in commands
  registerCommand("help", cmdHelp);
  registerCommand("log", cmdLog);
  registerCommand("snapshot", cmdSnapshot);
  registerCommand("visual", cmdVisual);
  registerCommand("test", cmdTest);
  registerCommand("dump", cmdDump);
  
  // Print welcome message
  Serial.println("\n\n--- LUTT Command-line Interface ---");
  Serial.println("Type 'help' for available commands");
  printPrompt();
}

/**
 * Process any pending commands (call in loop)
 */
void CommandLineInterface::process() {
  if (!CLI_ENABLED || !_initialized) return;
  
  while (Serial.available()) {
    char c = Serial.read();
    
    // Handle backspace/delete
    if (c == 8 || c == 127) {
      if (_bufIndex > 0) {
        _bufIndex--;
        Serial.print("\b \b"); // Erase last character
      }
      continue;
    }
    
    // Echo character
    Serial.print(c);
    
    // Handle newline
    if (c == '\n' || c == '\r') {
      Serial.println();
      
      // Null-terminate
      _cmdBuffer[_bufIndex] = '\0';
      
      // Skip empty lines
      if (_bufIndex > 0) {
        executeCommand(_cmdBuffer);
      }
      
      // Reset buffer
      _bufIndex = 0;
      printPrompt();
      continue;
    }
    
    // Add to buffer if there's room
    if (_bufIndex < MAX_CMD_LENGTH) {
      _cmdBuffer[_bufIndex++] = c;
    }
  }
}

/**
 * Register a callback function for a specific command
 */
void CommandLineInterface::registerCommand(const char* command, CommandCallback callback) {
  if (!CLI_ENABLED || !_initialized) return;
  
  // Only add if we have space and the command doesn't already exist
  if (_numCommands < 10) {
    for (uint8_t i = 0; i < _numCommands; i++) {
      if (strcmp(_commandNames[i], command) == 0) {
        // Command already exists, update its callback
        _commandCallbacks[i] = callback;
        return;
      }
    }
    
    // Add new command
    _commandNames[_numCommands] = command;
    _commandCallbacks[_numCommands] = callback;
    _numCommands++;
  }
}

/**
 * Parse the command buffer into arguments
 */
void CommandLineInterface::parseCommand(int* argc, char* argv[]) {
  char* token;
  *argc = 0;
  
  // Get first token (command name)
  token = strtok(_cmdBuffer, " ");
  if (token) {
    argv[(*argc)++] = token;
    
    // Get remaining tokens (arguments)
    while (*argc < MAX_ARGS && (token = strtok(NULL, " "))) {
      argv[(*argc)++] = token;
    }
  }
}

/**
 * Process a complete command
 */
void CommandLineInterface::executeCommand(char* cmdBuffer) {
  int argc;
  char* argv[MAX_ARGS];
  
  // Parse command into arguments
  parseCommand(&argc, argv);
  
  if (argc == 0) return;
  
  // Look up command
  for (uint8_t i = 0; i < _numCommands; i++) {
    if (strcmp(_commandNames[i], argv[0]) == 0) {
      // Found command, call callback
      _commandCallbacks[i](argc, argv);
      return;
    }
  }
  
  // Command not found
  Serial.print("Unknown command: ");
  Serial.println(argv[0]);
  Serial.println("Type 'help' for available commands");
}

/**
 * Print CLI prompt
 */
void CommandLineInterface::printPrompt() {
  Serial.print("> ");
}

//
// Built-in command implementations
//

/**
 * Help command - list available commands
 */
void CommandLineInterface::cmdHelp(int argc, char* argv[]) {
  Serial.println("Available commands:");
  for (uint8_t i = 0; i < _numCommands; i++) {
    Serial.print("  ");
    Serial.println(_commandNames[i]);
  }
  
  Serial.println("\nBuilt-in command usage:");
  Serial.println("  help                  Show this help");
  Serial.println("  log <level> <enable>  Control diagnostic logging");
  Serial.println("  snapshot <trigger>    Capture state snapshot");
  Serial.println("  visual <type> <args>  Control visual indicators");
  Serial.println("  test <component>      Run component test");
  Serial.println("  dump <component>      Dump component state");
}

/**
 * Log command - control diagnostic logging
 */
void CommandLineInterface::cmdLog(int argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("Usage: log <level> <enable>");
    Serial.println("  level: 1=CRITICAL, 2=ERROR, 3=WARNING, 4=INFO, 5=DEBUG, 6=TRACE");
    Serial.println("  enable: 0=disabled, 1=enabled");
    Serial.println("Example: log 4 1  (enable INFO level logs)");
    return;
  }
  
  // Simple global control for now
  if (argc >= 3 && strcmp(argv[2], "0") == 0) {
    DiagnosticLogger::setEnabled(false);
    Serial.println("Diagnostic logging disabled");
  } else {
    DiagnosticLogger::setEnabled(true);
    Serial.println("Diagnostic logging enabled");
  }
}

/**
 * Snapshot command - capture state snapshot
 */
void CommandLineInterface::cmdSnapshot(int argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("Usage: snapshot <trigger>");
    Serial.println("  trigger: 1=GESTURE_START, 2=GESTURE_END, 4=MODE_CHANGE");
    Serial.println("           8=ANIMATION, 16=FREECAST, 32=ERROR, 255=ALL");
    Serial.println("Example: snapshot 1  (capture gesture start)");
    return;
  }
  
  int trigger = atoi(argv[1]);
  StateSnapshotCapture::capture(trigger, "CLI requested");
  StateSnapshotCapture::addField("cli_trigger", trigger);
  StateSnapshotCapture::printSnapshot();
}

/**
 * Visual command - control visual indicators
 */
void CommandLineInterface::cmdVisual(int argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("Usage: visual <type> <args>");
    Serial.println("  type: 0=GESTURE, 1=MODE, 2=ANIMATION, 3=MOTION");
    Serial.println("Example: visual 0 0.5 1  (gesture progress 50%, type 1)");
    return;
  }
  
  int type = atoi(argv[1]);
  switch(type) {
    case VISUAL_IND_GESTURE_PROGRESS:
      if (argc >= 4) {
        float progress = atof(argv[2]);
        int gestureType = atoi(argv[3]);
        VisualDebugIndicator::updateGestureProgress(progress, gestureType);
        Serial.printf("Updated gesture indicator: progress=%.2f, type=%d\n", progress, gestureType);
      } else {
        Serial.println("Usage: visual 0 <progress> <type>");
      }
      break;
      
    case VISUAL_IND_MODE_STATE:
      if (argc >= 4) {
        int mode = atoi(argv[2]);
        int subState = atoi(argv[3]);
        VisualDebugIndicator::updateModeState(mode, subState);
        Serial.printf("Updated mode indicator: mode=%d, subState=%d\n", mode, subState);
      } else {
        Serial.println("Usage: visual 1 <mode> <substate>");
      }
      break;
      
    case VISUAL_IND_ANIMATION_PHASE:
      if (argc >= 4) {
        int phase = atoi(argv[2]);
        float progress = atof(argv[3]);
        VisualDebugIndicator::updateAnimationPhase(phase, progress);
        Serial.printf("Updated animation indicator: phase=%d, progress=%.2f\n", phase, progress);
      } else {
        Serial.println("Usage: visual 2 <phase> <progress>");
      }
      break;
      
    case VISUAL_IND_MOTION_FEATURE:
      if (argc >= 4) {
        float intensity = atof(argv[2]);
        float complexity = atof(argv[3]);
        VisualDebugIndicator::updateMotionFeature(intensity, complexity);
        Serial.printf("Updated motion indicator: intensity=%.2f, complexity=%.2f\n", intensity, complexity);
      } else {
        Serial.println("Usage: visual 3 <intensity> <complexity>");
      }
      break;
      
    default:
      Serial.println("Unknown indicator type");
      break;
  }
}

/**
 * Test command - run component test
 */
void CommandLineInterface::cmdTest(int argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("Usage: test <component>");
    Serial.println("  component: logger, snapshot, visual");
    return;
  }
  
  if (strcmp(argv[1], "logger") == 0) {
    Serial.println("Testing DiagnosticLogger...");
    DiagnosticLogger::setEnabled(true);
    DIAG_LOG(DIAG_LEVEL_INFO, DIAG_TAG_GESTURE, "Test log message from CLI");
    DIAG_LOG(DIAG_LEVEL_WARNING, DIAG_TAG_MODE, "Test warning from CLI");
    DIAG_LOG(DIAG_LEVEL_ERROR, DIAG_TAG_ANIMATION, "Test error from CLI");
    Serial.println("Logger test complete");
  }
  else if (strcmp(argv[1], "snapshot") == 0) {
    Serial.println("Testing StateSnapshotCapture...");
    StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_GESTURE_START, "CLI test");
    StateSnapshotCapture::addField("test_int", 42);
    StateSnapshotCapture::addField("test_float", 3.14159f);
    StateSnapshotCapture::addField("test_bool", true);
    StateSnapshotCapture::addField("test_string", "Hello from CLI");
    StateSnapshotCapture::printSnapshot();
    Serial.println("Snapshot test complete");
  }
  else if (strcmp(argv[1], "visual") == 0) {
    Serial.println("Testing VisualDebugIndicator...");
    VisualDebugIndicator::updateGestureProgress(0.75f, 1);
    delay(1000);
    VisualDebugIndicator::updateModeState(2, 128);
    delay(1000);
    VisualDebugIndicator::updateAnimationPhase(3, 0.5f);
    delay(1000);
    VisualDebugIndicator::updateMotionFeature(0.8f, 0.6f);
    Serial.println("Visual test complete");
  }
  else {
    Serial.print("Unknown component: ");
    Serial.println(argv[1]);
  }
}

/**
 * Dump command - dump component state
 */
void CommandLineInterface::cmdDump(int argc, char* argv[]) {
  if (argc < 2) {
    Serial.println("Usage: dump <component>");
    Serial.println("  component: commands, memory, thresholds");
    return;
  }
  
  if (strcmp(argv[1], "commands") == 0) {
    Serial.println("Registered commands:");
    for (uint8_t i = 0; i < _numCommands; i++) {
      Serial.printf("  %d: %s [%p]\n", i, _commandNames[i], _commandCallbacks[i]);
    }
  }
  else if (strcmp(argv[1], "memory") == 0) {
    // Simple memory dump - could be expanded if memory monitoring is added
    Serial.println("Memory allocation not implemented yet");
  }
  else if (strcmp(argv[1], "thresholds") == 0) {
    // Would access the threshold manager to dump values
    Serial.println("Threshold dump not implemented yet");
  }
  else {
    Serial.print("Unknown component: ");
    Serial.println(argv[1]);
  }
} 