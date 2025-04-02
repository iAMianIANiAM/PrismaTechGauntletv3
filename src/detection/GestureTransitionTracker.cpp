#include "GestureTransitionTracker.h"
#include "Arduino.h" // Required for millis() though passed in via timestamp

GestureTransitionTracker::GestureTransitionTracker(HandPosition startPosition, HandPosition endPosition, uint32_t maxDurationMs, SpellTransition transitionType)
    : startPosition_(startPosition),
      endPosition_(endPosition),
      maxDurationMs_(maxDurationMs),
      transitionType_(transitionType),
      startPositionActive_(false),
      startTimeMs_(0),
      transitionComplete_(false) {}

void GestureTransitionTracker::update(HandPosition currentPosition, uint32_t currentTimestamp) {
    // Ignore updates if transition already completed and waiting for reset
    if (transitionComplete_) {
        return;
    }

    // Check if the start position is currently active
    if (currentPosition == startPosition_) {
        if (!startPositionActive_) {
            // Start position just became active, record the time
            startPositionActive_ = true;
            startTimeMs_ = currentTimestamp;
            Serial.print("GestureTracker: Started "); Serial.println((int)transitionType_);
        } else {
            // Start position is still active, check for timeout
            if (currentTimestamp - startTimeMs_ > maxDurationMs_) {
                // Exceeded time limit, reset the timer
                Serial.print("GestureTracker: Timeout "); Serial.println((int)transitionType_);
                reset(); 
            }
        }
    } 
    // Check if the end position is achieved WHILE the start position timer was active
    else if (currentPosition == endPosition_) {
        if (startPositionActive_) {
            // End position achieved within the time window
            if (currentTimestamp - startTimeMs_ <= maxDurationMs_) {
                Serial.print("GestureTracker: Completed "); Serial.println((int)transitionType_);
                transitionComplete_ = true;
                startPositionActive_ = false; // Stop tracking start pos
            } else {
                // Reached end position, but too late. Reset.
                Serial.print("GestureTracker: Too Late "); Serial.println((int)transitionType_);
                reset();
            }
        } else {
            // Reached end position without the start position being active recently. Do nothing.
        }
    } 
    // Current position is neither start nor end
}

bool GestureTransitionTracker::isTransitionComplete() const {
    return transitionComplete_;
}

SpellTransition GestureTransitionTracker::getTransitionType() const {
    return transitionType_;
}

void GestureTransitionTracker::reset() {
    startPositionActive_ = false;
    startTimeMs_ = 0;
    transitionComplete_ = false;
    Serial.print("GestureTracker: Reset "); Serial.println((int)transitionType_);
} 