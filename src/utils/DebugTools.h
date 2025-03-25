#ifndef DEBUG_TOOLS_H
#define DEBUG_TOOLS_H

#include <Arduino.h>
#include "../core/SystemTypes.h"

// Comment this line to disable debug output
#define DEBUG_ENABLED

/**
 * @brief Debug output utilities
 */
namespace DebugTools {
  /**
   * @brief Initialize debug tools
   * @param baudRate Serial baud rate
   */
  void init(unsigned long baudRate = 115200);
  
  /**
   * @brief Print debug message
   * @param message Message to print
   */
  void print(const char* message);
  
  /**
   * @brief Print debug message with newline
   * @param message Message to print
   */
  void println(const char* message);
  
  /**
   * @brief Print formatted debug message
   * @param format Format string
   * @param ... Format arguments
   */
  void printf(const char* format, ...);
  
  /**
   * @brief Print sensor data for debugging
   * @param data Sensor data to print
   */
  void printSensorData(const SensorData& data);
  
  /**
   * @brief Print position reading for debugging
   * @param position Position reading to print
   */
  void printPositionReading(const PositionReading& position);
  
  /**
   * @brief Print the current mode
   * @param mode Current system mode
   */
  void printMode(uint8_t mode);
  
  /**
   * @brief Enable or disable timing measurements
   * @param enabled Whether timing is enabled
   */
  void setTimingEnabled(bool enabled);
  
  /**
   * @brief Start timing a section of code
   * @param sectionName Section name for identification
   */
  void startTiming(const char* sectionName);
  
  /**
   * @brief End timing and print elapsed time
   */
  void endTiming();
  
  /**
   * @brief Log memory usage statistics
   */
  void logMemoryUsage();
  
  /**
   * @brief Convert a hand position to string
   * @param position Hand position value
   * @return String representation
   */
  const char* positionToString(uint8_t position);
};

// Macros for conditional debugging
#ifdef DEBUG_ENABLED
  #define DEBUG_INIT(baud) DebugTools::init(baud)
  #define DEBUG_PRINT(msg) DebugTools::print(msg)
  #define DEBUG_PRINTLN(msg) DebugTools::println(msg)
  #define DEBUG_PRINTF(...) DebugTools::printf(__VA_ARGS__)
  #define DEBUG_SENSOR(data) DebugTools::printSensorData(data)
  #define DEBUG_POSITION(pos) DebugTools::printPositionReading(pos)
  #define DEBUG_MODE(mode) DebugTools::printMode(mode)
  #define DEBUG_START_TIMING(name) DebugTools::startTiming(name)
  #define DEBUG_END_TIMING() DebugTools::endTiming()
  #define DEBUG_MEMORY() DebugTools::logMemoryUsage()
#else
  #define DEBUG_INIT(baud)
  #define DEBUG_PRINT(msg)
  #define DEBUG_PRINTLN(msg)
  #define DEBUG_PRINTF(...)
  #define DEBUG_SENSOR(data)
  #define DEBUG_POSITION(pos)
  #define DEBUG_MODE(mode)
  #define DEBUG_START_TIMING(name)
  #define DEBUG_END_TIMING()
  #define DEBUG_MEMORY()
#endif

#endif // DEBUG_TOOLS_H 