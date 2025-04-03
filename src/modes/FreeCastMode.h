#ifndef FREECAST_MODE_H
#define FREECAST_MODE_H

#include <Arduino.h>
#include <FastLED.h>
#include "../hardware/HardwareManager.h"
#include "../detection/UltraBasicPositionDetector.h"
#include "../core/SystemTypes.h"
#include "../core/Config.h"

// Pattern types
enum class PatternType {
    SHOOTING_STARS,  // For quick flicks
    WAVES,           // For circular motions
    SPARKLES,        // For shaking motions
    COLOR_TRAILS,    // For directional movements
    PULSES           // For sharp direction changes
};

/**
 * @brief FreeCast mode implementation for motion-to-pattern translation
 * 
 * Implements the FreeCast mode as described in the TrueFunctionGuide.
 * Alternates between data collection and pattern display phases,
 * translating motion data into visual LED patterns.
 */
class FreeCastMode {
private:
    // Dependencies
    HardwareManager* hardwareManager;
    UltraBasicPositionDetector* positionDetector;
    
    // Operation state
    enum class FreeCastState {
        INITIALIZING,  // Initial setup phase
        RECORDING,     // Data collection phase
        DISPLAYING     // Pattern display phase
    };
    FreeCastState currentState;
    
    // Motion data storage
    static const uint16_t MOTION_BUFFER_SIZE = 100;  // 50Hz for 2 seconds
    ProcessedData motionBuffer[MOTION_BUFFER_SIZE];
    uint16_t motionBufferIndex;
    uint16_t motionBufferCount;
    
    // Motion analysis results
    float motionIntensity;        // Overall motion intensity (0.0-1.0)
    float motionDirectionality;   // How directional vs. chaotic (0.0-1.0)
    float rotationIntensity;      // Rotation speed (0.0-1.0)
    uint8_t dominantAxis;         // Primary motion axis (0-2)
    
    // Pattern generation
    PatternType currentPatternType;  // Determined by motion characteristics
    CRGB patternColors[5];           // Color palette for current pattern
    
    // Timing
    unsigned long phaseStartTime;       // When current phase began
    
    // NULL position tracking for exit gesture (deprecated)
    unsigned long nullPositionStartTime; 
    bool inNullCountdown;               
    
    // SHIELD position tracking for exit gesture
    unsigned long shieldPositionStartTime;
    bool inShieldCountdown;
    
    // Position tracking
    PositionReading currentPosition;
    
    // Internal methods
    void collectMotionData();
    void analyzeMotionData();
    void generatePattern();
    void renderBackgroundAnimation();
    void renderCurrentPattern(unsigned long elapsedTime);
    
    // Gesture detection methods
    bool detectLongNullGesture(); // Deprecated
    bool detectLongShieldGesture(); // New exit gesture
    
    // Pattern rendering methods
    void renderShootingStars(unsigned long elapsedTime);
    void renderWaves(unsigned long elapsedTime);
    void renderSparkles(unsigned long elapsedTime);
    void renderColorTrails(unsigned long elapsedTime);
    void renderPulses(unsigned long elapsedTime);
    
    // Helper methods
    void generateColorPalette();
    float calculateMotionIntensity();
    float calculateDirectionality();
    uint8_t determinePatternType();
    
public:
    FreeCastMode();
    bool init(HardwareManager* hardware, UltraBasicPositionDetector* detector);
    void initialize();
    ModeTransition update();
    void renderLEDs();
    
    /**
     * @brief Reset FreeCast mode state
     * 
     * Resets state for clean exit when cancelled via shake gesture
     */
    void reset();
    
    #ifdef DEBUG_MODE
    void printStatus() const;
    #endif
};

#endif // FREECAST_MODE_H 