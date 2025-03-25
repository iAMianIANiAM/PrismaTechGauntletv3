#include "GauntletController.h"
#include "Config.h"
#include <Arduino.h>

// Main initialization
bool GauntletController::init() {
  // Initialize hardware
  if (!hardware.init()) {
    return false;
  }
  
  // Initialize position detector with hardware reference
  positionDetector.init(&hardware);
  
  // Initialize gesture recognizer
  gestureRecognizer.init();
  
  // Initialize animation system
  animationSystem.init(&hardware);
  
  // Set initial mode
  currentMode = MODE_IDLE;
  modeStartTime = millis();
  lastUpdateTime = millis();
  
  return true;
}

// Main execution loop
void GauntletController::update() {
  // Update hardware (read sensors)
  hardware.update();
  
  // Process sensor data to detect position
  PositionReading position = positionDetector.detectPosition(hardware.getSensorData());
  
  // Update gesture recognizer with latest position
  gestureRecognizer.updatePosition(position);
  
  // Handle current mode
  switch (currentMode) {
    case MODE_IDLE:
      handleIdleMode();
      break;
    case MODE_INVOCATION:
      handleInvocationMode();
      break;
    case MODE_RESOLUTION:
      handleResolutionMode();
      break;
    case MODE_FREECAST:
      handleFreecastMode();
      break;
  }
  
  // Update animation system
  animationSystem.update();
  
  // Maintain consistent loop timing
  maintainLoopTiming();
}

// Get current mode
SystemMode GauntletController::getCurrentMode() const {
  return currentMode;
}

// Handle Idle Mode
void GauntletController::handleIdleMode() {
  // Display current hand position on the LEDs
  animationSystem.showIdlePosition(positionDetector.getCurrentPosition());
  
  // Check for CalmOffer gesture
  if (gestureRecognizer.detectCalmOffer()) {
    transitionToMode(MODE_INVOCATION);
    return;
  }
  
  // Check for LongNull gesture
  if (gestureRecognizer.detectLongNull()) {
    transitionToMode(MODE_FREECAST);
    return;
  }
  
  // If approaching LongNull threshold, show warning animation
  if (gestureRecognizer.getLongNullProgress() > 0.6f && 
      gestureRecognizer.getLongNullProgress() < 1.0f) {
    animationSystem.showLongNullWarning(gestureRecognizer.getLongNullProgress());
  }
}

// Handle Invocation Mode
void GauntletController::handleInvocationMode() {
  unsigned long elapsedTime = getElapsedModeTime();
  
  // Manage invocation sequence timing
  for (int slot = 0; slot < 3; slot++) {
    // Calculate timing for this slot
    unsigned long slotStartTime = slot * (Config::INDICATOR_PHASE_MS + 
                                         Config::COUNTDOWN_PHASE_MS + 
                                         Config::CHECKPOINT_PHASE_MS + 
                                         Config::CONFIRMATION_PHASE_MS);
    
    // Only process the current slot
    if (elapsedTime < slotStartTime) {
      break;
    }
    
    unsigned long slotElapsedTime = elapsedTime - slotStartTime;
    
    // Indicator phase
    if (slotElapsedTime < Config::INDICATOR_PHASE_MS) {
      animationSystem.showSlotIndicator(slot);
      invocationSlots.currentSlot = slot;
      continue;
    }
    
    // Countdown phase
    if (slotElapsedTime < Config::INDICATOR_PHASE_MS + Config::COUNTDOWN_PHASE_MS) {
      unsigned long countdownElapsed = slotElapsedTime - Config::INDICATOR_PHASE_MS;
      float progress = (float)countdownElapsed / Config::COUNTDOWN_PHASE_MS;
      animationSystem.showCountdownAnimation(progress);
      continue;
    }
    
    // Checkpoint phase
    if (slotElapsedTime < Config::INDICATOR_PHASE_MS + 
                          Config::COUNTDOWN_PHASE_MS + 
                          Config::CHECKPOINT_PHASE_MS) {
      // Flash effect
      animationSystem.showCheckpointFlash();
      
      // Only record the position once
      if (!invocationSlots.filled[slot]) {
        // Capture the position for this slot
        invocationSlots.positions[slot] = positionDetector.getCurrentPosition().position;
        invocationSlots.filled[slot] = true;
      }
      
      continue;
    }
    
    // Confirmation phase
    if (slotElapsedTime < Config::INDICATOR_PHASE_MS + 
                          Config::COUNTDOWN_PHASE_MS + 
                          Config::CHECKPOINT_PHASE_MS + 
                          Config::CONFIRMATION_PHASE_MS) {
      // Show the imprinted slot
      animationSystem.showSlotConfirmation(slot, invocationSlots.positions[slot]);
      continue;
    }
  }
  
  // Check if all slots are filled
  if (invocationSlots.filled[0] && invocationSlots.filled[1] && invocationSlots.filled[2]) {
    // Allow a brief period to show all slots, then transition to Resolution mode
    unsigned long fullSequenceTime = 3 * (Config::INDICATOR_PHASE_MS + 
                                         Config::COUNTDOWN_PHASE_MS + 
                                         Config::CHECKPOINT_PHASE_MS + 
                                         Config::CONFIRMATION_PHASE_MS);
    
    if (elapsedTime > fullSequenceTime) {
      transitionToMode(MODE_RESOLUTION);
    }
  }
}

// Handle Resolution Mode
void GauntletController::handleResolutionMode() {
  unsigned long elapsedTime = getElapsedModeTime();
  
  // Resolution preparation (0-500ms)
  if (elapsedTime < 500) {
    animationSystem.showResolutionPrepare(invocationSlots);
    return;
  }
  
  // Spell selection (500-1000ms)
  if (elapsedTime < 1000) {
    animationSystem.showResolutionSelect(invocationSlots);
    return;
  }
  
  // Spell core indication (1000-1500ms)
  if (elapsedTime < 1500) {
    animationSystem.showResolutionCore(invocationSlots);
    return;
  }
  
  // Determine spell effect duration
  uint8_t spellType = animationSystem.determineSpellType(invocationSlots);
  unsigned long spellDuration = (spellType == 3) ? // Detect Magic is spell 3
                                Config::EXTENDED_SPELL_MS : 
                                Config::DEFINITE_SPELL_MS;
  
  // Spell effect animation
  if (elapsedTime < 1500 + spellDuration) {
    animationSystem.showSpellEffect(spellType, (elapsedTime - 1500) / (float)spellDuration);
    return;
  }
  
  // Blackout period
  if (elapsedTime < 1500 + spellDuration + Config::BLACKOUT_PERIOD_MS) {
    animationSystem.showBlackout();
    return;
  }
  
  // Return to idle animation
  if (elapsedTime < 1500 + spellDuration + Config::BLACKOUT_PERIOD_MS + Config::IDLE_RETURN_MS) {
    float progress = (float)(elapsedTime - (1500 + spellDuration + Config::BLACKOUT_PERIOD_MS)) / 
                    Config::IDLE_RETURN_MS;
    animationSystem.showIdleReturn(progress, positionDetector.getCurrentPosition());
    return;
  }
  
  // Transition back to Idle mode
  transitionToMode(MODE_IDLE);
}

// Handle Freecast Mode
void GauntletController::handleFreecastMode() {
  unsigned long elapsedTime = getElapsedModeTime();
  
  // Freecast Initialization (0-500ms)
  if (elapsedTime < 500) {
    animationSystem.showFreecastInit();
    return;
  }
  
  // Determine the cycle phase - alternating between collection and display
  unsigned long cycleDuration = Config::FREECAST_COLLECTION_MS + Config::FREECAST_DISPLAY_MS;
  unsigned long cycleTime = (elapsedTime - 500) % cycleDuration;
  
  // Check for LongNull gesture to exit Freecast mode
  if (gestureRecognizer.detectLongNull()) {
    transitionToMode(MODE_IDLE);
    return;
  }
  
  // Data Collection phase
  if (cycleTime < Config::FREECAST_COLLECTION_MS) {
    hardware.recordMotionData();
    animationSystem.showFreecastCollecting(cycleTime / (float)Config::FREECAST_COLLECTION_MS);
    
    // If approaching LongNull, show warning
    if (gestureRecognizer.getLongNullProgress() > 0.6f && 
        gestureRecognizer.getLongNullProgress() < 1.0f) {
      animationSystem.showLongNullWarning(gestureRecognizer.getLongNullProgress());
    }
    return;
  }
  
  // Pattern Display phase
  SensorData* motionData = hardware.getMotionData();
  unsigned int motionDataSize = hardware.getMotionDataSize();
  float progress = (cycleTime - Config::FREECAST_COLLECTION_MS) / (float)Config::FREECAST_DISPLAY_MS;
  
  animationSystem.showFreecastPattern(motionData, motionDataSize, progress);
}

// Mode transition
void GauntletController::transitionToMode(SystemMode newMode) {
  // Exit animations for current mode
  switch (currentMode) {
    case MODE_IDLE:
      if (newMode == MODE_INVOCATION) {
        animationSystem.showTransitionToInvocation();
      } else if (newMode == MODE_FREECAST) {
        animationSystem.showTransitionToFreecast();
      }
      break;
      
    case MODE_INVOCATION:
      // Transition handled in Resolution mode
      break;
      
    case MODE_RESOLUTION:
      // Transition handled in resolution handler
      break;
      
    case MODE_FREECAST:
      animationSystem.showBlackout();
      break;
  }
  
  // Reset mode-specific state
  if (newMode == MODE_INVOCATION) {
    // Clear invocation slots
    for (int i = 0; i < 3; i++) {
      invocationSlots.positions[i] = POS_UNKNOWN;
      invocationSlots.filled[i] = false;
    }
    invocationSlots.currentSlot = 0;
  }
  else if (newMode == MODE_FREECAST) {
    // Clear motion data buffer
    hardware.clearMotionData();
  }
  
  // Update mode state
  currentMode = newMode;
  modeStartTime = millis();
}

// Get time elapsed in current mode
unsigned long GauntletController::getElapsedModeTime() const {
  return millis() - modeStartTime;
}

// Check if a duration has elapsed since a start time
bool GauntletController::hasElapsed(unsigned long startTime, unsigned long duration) const {
  return (millis() - startTime) >= duration;
}

// Maintain consistent timing for the main loop
void GauntletController::maintainLoopTiming() {
  // Target 100Hz (10ms per iteration)
  const unsigned long targetLoopTime = 10;
  
  // Calculate elapsed time since last update
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastUpdateTime;
  
  // If we completed faster than target, delay the difference
  if (elapsedTime < targetLoopTime) {
    delay(targetLoopTime - elapsedTime);
  }
  
  // Update last update time
  lastUpdateTime = millis();
} 