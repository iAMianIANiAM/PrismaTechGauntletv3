#ifndef GESTURE_RECOGNIZER_H
#define GESTURE_RECOGNIZER_H

#include "../core/SystemTypes.h"
#include "../core/Config.h"

// Position history buffer size
#define POSITION_HISTORY_SIZE 20

/**
 * @brief Recognizes hand gestures from position history
 */
class GestureRecognizer {
public:
  /**
   * @brief Initialize the gesture recognizer
   * @return True if initialization was successful
   */
  bool init();
  
  /**
   * @brief Update with a new position reading
   * @param position Current position reading
   */
  void updatePosition(const PositionReading& position);
  
  /**
   * @brief Detect CalmOffer gesture (Calm→Offer transition)
   * @return True if gesture detected
   */
  bool detectCalmOffer();
  
  /**
   * @brief Detect LongNull gesture (Null position held)
   * @return True if gesture detected
   */
  bool detectLongNull();
  
  /**
   * @brief Get the progress toward completing a LongNull gesture
   * @return Value 0.0-1.0 representing progress (1.0 = complete)
   */
  float getLongNullProgress() const;
  
  /**
   * @brief Reset the gesture detection state
   */
  void reset();

private:
  // Position history circular buffer
  PositionReading positionHistory[POSITION_HISTORY_SIZE];
  uint8_t historyIndex = 0;
  uint8_t historyCount = 0;
  
  // Gesture state tracking
  bool calmOfferInProgress = false;
  unsigned long calmExitTime = 0;
  
  // LongNull tracking
  unsigned long nullStartTime = 0;
  bool nullActive = false;
  bool longNullDetected = false;
  
  // Internal helper methods
  bool wasRecentPosition(uint8_t position, unsigned long maxAgeMs) const;
  bool transitionedBetween(uint8_t fromPosition, uint8_t toPosition, unsigned long windowMs) const;
  unsigned long getPositionDuration(uint8_t position) const;
  PositionReading getHistoryEntry(uint8_t index) const;
};

#endif // GESTURE_RECOGNIZER_H 