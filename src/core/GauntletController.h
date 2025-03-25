#ifndef GAUNTLET_CONTROLLER_H
#define GAUNTLET_CONTROLLER_H

#include "SystemTypes.h"
#include "../hardware/HardwareManager.h"
#include "../detection/PositionDetector.h"
#include "../detection/GestureRecognizer.h"
#include "../animation/AnimationSystem.h"

/**
 * @brief Main controller for the PrismaTech Gauntlet system
 * 
 * Coordinates all subsystems and manages the main execution loop
 */
class GauntletController {
public:
  /**
   * @brief Initializes the controller and all subsystems
   * @return True if initialization was successful
   */
  bool init();
  
  /**
   * @brief Main execution loop, should be called continuously from Arduino loop()
   */
  void update();
  
  /**
   * @brief Get the current system mode
   * @return Current SystemMode
   */
  SystemMode getCurrentMode() const;

private:
  // System state
  SystemMode currentMode = MODE_IDLE;
  unsigned long lastUpdateTime = 0;
  unsigned long modeStartTime = 0;

  // Subsystem instances
  HardwareManager hardware;
  PositionDetector positionDetector;
  GestureRecognizer gestureRecognizer;
  AnimationSystem animationSystem;
  
  // Invocation state
  InvocationSlots invocationSlots = {};
  
  // Mode handlers
  void handleIdleMode();
  void handleInvocationMode();
  void handleResolutionMode();
  void handleFreecastMode();
  
  // Mode transitions
  void transitionToMode(SystemMode newMode);
  
  // Timing utilities
  unsigned long getElapsedModeTime() const;
  bool hasElapsed(unsigned long startTime, unsigned long duration) const;
  void maintainLoopTiming();
};

#endif // GAUNTLET_CONTROLLER_H 