#include "GestureRecognizer.h"
#include <Arduino.h>

bool GestureRecognizer::init() {
  // Initialize state variables
  historyIndex = 0;
  historyCount = 0;
  calmOfferInProgress = false;
  calmExitTime = 0;
  nullStartTime = 0;
  nullActive = false;
  longNullDetected = false;
  
  // Clear position history
  for (uint8_t i = 0; i < POSITION_HISTORY_SIZE; i++) {
    positionHistory[i] = {POS_UNKNOWN, 0, 0};
  }
  
  return true;
}

void GestureRecognizer::updatePosition(const PositionReading& position) {
  // Store current time for timestamp calculations
  unsigned long currentTime = millis();
  
  // Add to position history
  positionHistory[historyIndex] = position;
  
  // Update circular buffer index
  historyIndex = (historyIndex + 1) % POSITION_HISTORY_SIZE;
  
  // Update history count for initialization period
  if (historyCount < POSITION_HISTORY_SIZE) {
    historyCount++;
  }
  
  // CalmOffer gesture tracking
  if (position.position == POS_CALM) {
    // In Calm position, prepare for potential Calm->Offer transition
    calmOfferInProgress = true;
  } 
  else if (calmOfferInProgress && position.position != POS_CALM) {
    // Just exited Calm position
    calmExitTime = currentTime;
    calmOfferInProgress = false;
  }
  
  // LongNull gesture tracking
  if (position.position == POS_NULLPOS) {
    if (!nullActive) {
      // Just entered Null position
      nullStartTime = currentTime;
      nullActive = true;
    }
    // Else: still in Null position, time keeps accumulating
  } 
  else {
    // Not in Null position
    nullActive = false;
    longNullDetected = false;
  }
  
  // Check for LongNull completion (5000ms)
  if (nullActive && (currentTime - nullStartTime >= 5000)) {
    longNullDetected = true;
  }
}

bool GestureRecognizer::detectCalmOffer() {
  // Early exit if we haven't filled the history buffer yet
  if (historyCount < 2) {
    return false;
  }
  
  // Check if current position is Offer
  unsigned long currentTime = millis();
  uint8_t latestIndex = (historyIndex == 0) ? POSITION_HISTORY_SIZE - 1 : historyIndex - 1;
  
  if (positionHistory[latestIndex].position == POS_OFFER) {
    // Check if we recently left Calm position (within 1000ms window)
    if (currentTime - calmExitTime <= 1000) {
      return true;
    }
  }
  
  return false;
}

bool GestureRecognizer::detectLongNull() {
  return longNullDetected;
}

float GestureRecognizer::getLongNullProgress() const {
  if (!nullActive) {
    return 0.0f;
  }
  
  unsigned long currentTime = millis();
  unsigned long nullDuration = currentTime - nullStartTime;
  
  // Progress starts at 3000ms (3s) and completes at 5000ms (5s)
  if (nullDuration < 3000) {
    return 0.0f;
  } else if (nullDuration >= 5000) {
    return 1.0f;
  } else {
    // Map 3000-5000ms range to 0.0-1.0 progress
    return (float)(nullDuration - 3000) / 2000.0f;
  }
}

void GestureRecognizer::reset() {
  // Reset state variables
  calmOfferInProgress = false;
  calmExitTime = 0;
  nullStartTime = 0;
  nullActive = false;
  longNullDetected = false;
  
  // Don't clear history - just reset the state tracking
}

bool GestureRecognizer::wasRecentPosition(uint8_t position, unsigned long maxAgeMs) const {
  if (historyCount == 0) {
    return false;
  }
  
  unsigned long currentTime = millis();
  
  // Search history for the position within the time window
  for (uint8_t i = 0; i < historyCount; i++) {
    uint8_t index = (historyIndex - 1 - i + POSITION_HISTORY_SIZE) % POSITION_HISTORY_SIZE;
    const PositionReading& reading = positionHistory[index];
    
    // Check if this reading has the position we're looking for
    if (reading.position == position) {
      // Check if it's within the time window
      if (currentTime - reading.timestamp <= maxAgeMs) {
        return true;
      }
    }
  }
  
  return false;
}

bool GestureRecognizer::transitionedBetween(uint8_t fromPosition, uint8_t toPosition, unsigned long windowMs) const {
  if (historyCount < 2) {
    return false;
  }
  
  // Check if current position is the target position
  uint8_t latestIndex = (historyIndex == 0) ? POSITION_HISTORY_SIZE - 1 : historyIndex - 1;
  if (positionHistory[latestIndex].position != toPosition) {
    return false;
  }
  
  // Find when we were last in the source position
  unsigned long currentTime = millis();
  unsigned long lastFromTime = 0;
  
  for (uint8_t i = 1; i < historyCount; i++) {
    uint8_t index = (historyIndex - 1 - i + POSITION_HISTORY_SIZE) % POSITION_HISTORY_SIZE;
    const PositionReading& reading = positionHistory[index];
    
    if (reading.position == fromPosition) {
      lastFromTime = reading.timestamp;
      break;
    }
  }
  
  // Check if the transition happened within the time window
  return (lastFromTime > 0 && (currentTime - lastFromTime <= windowMs));
}

unsigned long GestureRecognizer::getPositionDuration(uint8_t position) const {
  if (historyCount == 0) {
    return 0;
  }
  
  unsigned long currentTime = millis();
  uint8_t latestIndex = (historyIndex == 0) ? POSITION_HISTORY_SIZE - 1 : historyIndex - 1;
  
  // Check if we're currently in the position
  if (positionHistory[latestIndex].position != position) {
    return 0;
  }
  
  // Search backwards to find when we entered this position
  unsigned long entryTime = positionHistory[latestIndex].timestamp;
  
  for (uint8_t i = 1; i < historyCount; i++) {
    uint8_t index = (historyIndex - 1 - i + POSITION_HISTORY_SIZE) % POSITION_HISTORY_SIZE;
    const PositionReading& reading = positionHistory[index];
    
    if (reading.position != position) {
      // Found the transition point
      break;
    }
    
    // Still in the same position, update entry time
    entryTime = reading.timestamp;
  }
  
  return currentTime - entryTime;
}

PositionReading GestureRecognizer::getHistoryEntry(uint8_t index) const {
  if (index >= historyCount) {
    // Invalid index, return default
    return {POS_UNKNOWN, 0, 0};
  }
  
  uint8_t actualIndex = (historyIndex - 1 - index + POSITION_HISTORY_SIZE) % POSITION_HISTORY_SIZE;
  return positionHistory[actualIndex];
} 