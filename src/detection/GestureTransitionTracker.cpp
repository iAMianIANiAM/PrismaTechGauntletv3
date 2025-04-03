#include "GestureTransitionTracker.h"
#include "Arduino.h" // Required for millis() though passed in via timestamp

// LUTT Diagnostic includes (conditionally compiled)
#if DIAG_LOGGING_ENABLED
#include "../diagnostics/DiagnosticLogger.h"
#include "../diagnostics/StateSnapshotCapture.h"
#endif

GestureTransitionTracker::GestureTransitionTracker(HandPosition startPosition, HandPosition endPosition, uint32_t maxDurationMs, SpellTransition transitionType)
    : startPosition_(startPosition),
      endPosition_(endPosition),
      maxDurationMs_(maxDurationMs),
      transitionType_(transitionType),
      startPositionActive_(false),
      startTimeMs_(0),
      transitionComplete_(false) {
          
    #if DIAG_LOGGING_ENABLED
    DIAG_DEBUG(DIAG_TAG_GESTURE, "GestureTracker created: %d->%d, window: %lu ms, type: %d", 
              (int)startPosition, (int)endPosition, maxDurationMs, (int)transitionType);
    #endif
}

void GestureTransitionTracker::update(HandPosition currentPosition, uint32_t currentTimestamp) {
    // Ignore updates if transition already completed and waiting for reset
    if (transitionComplete_) {
        return;
    }

    #if DIAG_LOGGING_ENABLED
    static HandPosition lastLoggedPosition = {255}; // Invalid position to force first log
    if (currentPosition != lastLoggedPosition) {
        DIAG_DEBUG(DIAG_TAG_GESTURE, "Position update: %d, active: %d, elapsed: %lu ms", 
                  (int)currentPosition, startPositionActive_, 
                  startPositionActive_ ? (currentTimestamp - startTimeMs_) : 0);
        lastLoggedPosition = currentPosition;
    }
    #endif

    // Check if the start position is currently active
    if (currentPosition == startPosition_) {
        if (!startPositionActive_) {
            // Start position just became active, record the time
            startPositionActive_ = true;
            startTimeMs_ = currentTimestamp;
            Serial.print("GestureTracker: Started "); Serial.println((int)transitionType_);
            
            #if DIAG_LOGGING_ENABLED
            DIAG_INFO(DIAG_TAG_GESTURE, "Gesture started: type %d", (int)transitionType_);
            
            // Capture snapshot for gesture start
            StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_GESTURE_START, "GestureTransitionTracker::update");
            StateSnapshotCapture::addField("startPosition", (int)startPosition_);
            StateSnapshotCapture::addField("endPosition", (int)endPosition_);
            StateSnapshotCapture::addField("transitionType", (int)transitionType_);
            StateSnapshotCapture::addField("maxDuration", (int)maxDurationMs_);
            #endif
        } else {
            // Start position is still active, check for timeout
            if (currentTimestamp - startTimeMs_ > maxDurationMs_) {
                // Exceeded time limit, reset the timer
                Serial.print("GestureTracker: Timeout "); Serial.println((int)transitionType_);
                
                #if DIAG_LOGGING_ENABLED
                DIAG_INFO(DIAG_TAG_GESTURE, "Gesture timeout: type %d after %lu ms", 
                         (int)transitionType_, currentTimestamp - startTimeMs_);
                #endif
                
                reset(); 
            }
        }
    } 
    // Check if the end position is achieved WHILE the start position timer was active
    else if (currentPosition == endPosition_) {
        if (startPositionActive_) {
            unsigned long elapsed = currentTimestamp - startTimeMs_;
            
            #if DIAG_LOGGING_ENABLED
            DIAG_DEBUG(DIAG_TAG_GESTURE, "End position detected, elapsed: %lu ms, max: %lu ms", 
                      elapsed, maxDurationMs_);
            #endif
            
            // End position achieved within the time window
            if (elapsed <= maxDurationMs_) {
                Serial.print("GestureTracker: Completed "); Serial.println((int)transitionType_);
                transitionComplete_ = true;
                startPositionActive_ = false; // Stop tracking start pos
                
                #if DIAG_LOGGING_ENABLED
                DIAG_INFO(DIAG_TAG_GESTURE, "Gesture completed: type %d in %lu ms", 
                         (int)transitionType_, elapsed);
                         
                // Capture snapshot for gesture completion
                StateSnapshotCapture::capture(SNAPSHOT_TRIGGER_GESTURE_END, "GestureTransitionTracker::update");
                StateSnapshotCapture::addField("startPosition", (int)startPosition_);
                StateSnapshotCapture::addField("endPosition", (int)endPosition_);
                StateSnapshotCapture::addField("transitionType", (int)transitionType_);
                StateSnapshotCapture::addField("duration", (int)elapsed);
                StateSnapshotCapture::addField("success", true);
                #endif
            } else {
                // Reached end position, but too late. Reset.
                Serial.print("GestureTracker: Too Late "); Serial.println((int)transitionType_);
                
                #if DIAG_LOGGING_ENABLED
                DIAG_INFO(DIAG_TAG_GESTURE, "Gesture too late: type %d in %lu ms (max %lu ms)", 
                         (int)transitionType_, elapsed, maxDurationMs_);
                #endif
                
                reset();
            }
        } else {
            // Reached end position without the start position being active recently. Do nothing.
            #if DIAG_LOGGING_ENABLED
            DIAG_DEBUG(DIAG_TAG_GESTURE, "End position without start: %d", (int)endPosition_);
            #endif
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
    
    #if DIAG_LOGGING_ENABLED
    DIAG_DEBUG(DIAG_TAG_GESTURE, "Gesture tracker reset: type %d", (int)transitionType_);
    #endif
} 