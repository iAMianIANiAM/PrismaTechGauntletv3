#ifndef GESTURE_TRANSITION_TRACKER_H
#define GESTURE_TRANSITION_TRACKER_H

#include "core/SystemTypes.h"
#include <stdint.h>

/**
 * @brief Tracks the transition between two specific hand positions within a time limit.
 * 
 * This class monitors the sequence StartPosition -> EndPosition.
 * It activates a timer when StartPosition is detected and checks if EndPosition
 * is detected before the timer expires (maxDurationMs).
 */
class GestureTransitionTracker {
public:
    /**
     * @brief Constructor
     * @param startPosition The initial hand position that starts the timer.
     * @param endPosition The target hand position that completes the transition.
     * @param maxDurationMs The maximum time allowed (in milliseconds) for the transition.
     * @param transitionType The type of transition this tracker represents (e.g., a QuickCast spell).
     */
    GestureTransitionTracker(HandPosition startPosition, HandPosition endPosition, uint32_t maxDurationMs, SpellTransition transitionType);

    /**
     * @brief Updates the tracker's state based on the current hand position.
     * @param currentPosition The currently detected hand position.
     * @param currentTimestamp The current system time (millis()).
     */
    void update(HandPosition currentPosition, uint32_t currentTimestamp);

    /**
     * @brief Checks if the specific gesture transition has been successfully completed.
     * @return True if the transition StartPosition -> EndPosition occurred within maxDurationMs, false otherwise.
     */
    bool isTransitionComplete() const;

    /**
     * @brief Gets the type of transition associated with this tracker.
     * @return The SpellTransition enum value.
     */
    SpellTransition getTransitionType() const;

    /**
     * @brief Resets the tracker's state to its initial condition.
     * Call this after handling a completed transition or when interrupting the sequence.
     */
    void reset();

private:
    HandPosition startPosition_;
    HandPosition endPosition_;
    uint32_t maxDurationMs_;
    SpellTransition transitionType_;

    bool startPositionActive_; // True if the start position is currently held
    uint32_t startTimeMs_;      // Timestamp when the start position was first detected
    bool transitionComplete_;   // True if the full sequence completed successfully
};

#endif // GESTURE_TRANSITION_TRACKER_H 