/**
 * CommandLineInterface.h
 * 
 * A minimal serial command processor for controlling diagnostic features at runtime.
 * Part of the Lightweight Universal Troubleshooting Toolkit (LUTT)
 */

#ifndef COMMAND_LINE_INTERFACE_H
#define COMMAND_LINE_INTERFACE_H

#include <Arduino.h>

// Maximum command length
#define MAX_CMD_LENGTH 32
#define MAX_ARGS 4

// Control whether CLI is enabled through build flag
#ifndef CLI_ENABLED
#define CLI_ENABLED 0
#endif

class CommandLineInterface {
public:
  /**
   * Initialize the CLI
   */
  static void init();
  
  /**
   * Process any pending commands (call in loop)
   */
  static void process();
  
  /**
   * Register a callback function for a specific command
   * @param command The command string (without parameters)
   * @param callback Function to call when command is received
   */
  typedef void (*CommandCallback)(int argc, char* argv[]);
  static void registerCommand(const char* command, CommandCallback callback);
  
  /**
   * Built-in diagnostic commands
   */
  static void cmdHelp(int argc, char* argv[]);
  static void cmdLog(int argc, char* argv[]);
  static void cmdSnapshot(int argc, char* argv[]);
  static void cmdVisual(int argc, char* argv[]);
  static void cmdTest(int argc, char* argv[]);
  static void cmdDump(int argc, char* argv[]);

private:
  /**
   * Parse the command buffer into arguments
   * @param argc Pointer to store argument count
   * @param argv Array to store argument pointers
   */
  static void parseCommand(int* argc, char* argv[]);
  
  /**
   * Process a complete command
   * @param cmdBuffer Complete command string
   */
  static void executeCommand(char* cmdBuffer);
  
  /**
   * Print CLI prompt
   */
  static void printPrompt();
  
  // Command buffer
  static char _cmdBuffer[MAX_CMD_LENGTH + 1];
  static uint8_t _bufIndex;
  static bool _initialized;
  
  // Simple command registry (limited to 10 commands)
  static const char* _commandNames[10];
  static CommandCallback _commandCallbacks[10];
  static uint8_t _numCommands;
};

#endif // COMMAND_LINE_INTERFACE_H 