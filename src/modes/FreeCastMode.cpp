#include "FreeCastMode.h"
#include "../core/Config.h"
#include "../core/SystemTypes.h"

// Constructor - initialize all member variables to default values
FreeCastMode::FreeCastMode() 
    : hardwareManager(nullptr),
      positionDetector(nullptr),
      currentState(FreeCastState::INITIALIZING),
      motionBufferIndex(0),
      motionBufferCount(0),
      motionIntensity(0.0f),
      motionDirectionality(0.0f),
      rotationIntensity(0.0f),
      dominantAxis(0),
      currentPatternType(PatternType::SPARKLES),
      phaseStartTime(0),
      nullPositionStartTime(0),
      inNullCountdown(false),
      shieldPositionStartTime(0),
      inShieldCountdown(false),
      currentPosition({POS_UNKNOWN, 0, 0})
{
    // Initialize color palette with position colors
    patternColors[0] = CRGB(255, 0, 0);       // Red (Null position)
    patternColors[1] = CRGB(0, 255, 0);       // Green (Dig position)
    patternColors[2] = CRGB(255, 105, 180);   // Pink (Shield position)
    patternColors[3] = CRGB(128, 0, 255);     // Purple (Offer position)
    patternColors[4] = CRGB(255, 255, 0);     // Yellow (Oath position)
}

// Initialize with dependencies
bool FreeCastMode::init(HardwareManager* hardware, UltraBasicPositionDetector* detector) {
    // Store dependencies
    hardwareManager = hardware;
    positionDetector = detector;
    
    // Verify dependencies
    if (!hardwareManager || !positionDetector) {
        return false;
    }
    
    return true;
}

// Initialize the mode when first activated
void FreeCastMode::initialize() {
    // Reset state tracking
    currentState = FreeCastState::INITIALIZING;
    motionBufferIndex = 0;
    motionBufferCount = 0;
    phaseStartTime = millis();
    
    // Reset NULL position tracking
    nullPositionStartTime = 0;
    inNullCountdown = false;
    
    // Reset SHIELD position tracking
    shieldPositionStartTime = 0;
    inShieldCountdown = false;
    
    // Set initial position
    currentPosition = positionDetector->update();
    
    // Initialize pattern values
    motionIntensity = 0.5f;
    motionDirectionality = 0.5f;
    rotationIntensity = 0.5f;
    dominantAxis = 0;
    currentPatternType = PatternType::SPARKLES;
    
    // Generate initial color palette
    generateColorPalette();
    
    #ifdef DEBUG_MODE
    Serial.println(F("FreeCast Mode initialized"));
    #endif
}

// Main update function - called repeatedly
ModeTransition FreeCastMode::update() {
    // Get current time
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - phaseStartTime;
    
    // Update position for exit gesture detection
    currentPosition = positionDetector->update();
    
    // --- Exit Gesture Detection --- 
    // SHIELD position tracking for exit gesture
    if (currentPosition.position == POS_SHIELD) {
        if (shieldPositionStartTime == 0) {
            // Just entered SHIELD position
            shieldPositionStartTime = currentTime;
            inShieldCountdown = false;
        }
        
        // Check for SHIELD countdown trigger (after 3 seconds)
        if (!inShieldCountdown && currentTime - shieldPositionStartTime >= Config::LONGSHIELD_WARNING_MS) {
            inShieldCountdown = true;
            #ifdef DEBUG_MODE
            Serial.println(F("FreeCast Mode: Shield countdown started"));
            #endif
        }
        
        // Check if LongShield gesture is complete to exit FreeCast mode
        if (inShieldCountdown && currentTime - shieldPositionStartTime >= Config::LONGSHIELD_TIME_MS) {
            #ifdef DEBUG_MODE
            Serial.println(F("FreeCast Mode: LongShield detected! Exiting."));
            #endif
            initialize(); // Reset state before exiting
            return ModeTransition::TO_IDLE;
        }
        
    } else {
        // Reset SHIELD tracking when position changes
        if (shieldPositionStartTime != 0) { // Only print/reset if it was active
            #ifdef DEBUG_MODE
            if (inShieldCountdown) Serial.println(F("FreeCast Mode: Shield countdown aborted"));
            #endif
            shieldPositionStartTime = 0;
            inShieldCountdown = false;
        }
    }
    // --- End Exit Gesture Detection ---
    
    // Check which state we're in and handle accordingly
    switch (currentState) {
        case FreeCastState::INITIALIZING:
            // Should only be in this state briefly during initialize()
            if (elapsedTime >= 500) {
                // Transition to recording state
                currentState = FreeCastState::RECORDING;
                phaseStartTime = currentTime;
                #ifdef DEBUG_MODE
                Serial.println(F("FreeCast Mode: Transition to Recording phase"));
                #endif
            }
            break;
            
        case FreeCastState::RECORDING:
            // Collect motion data
            collectMotionData();
            
            // Check if recording phase is complete (2 seconds as per TrueFunctionGuide)
            if (elapsedTime >= Config::FREECAST_COLLECTION_MS) {
                // Analyze motion data and generate pattern
                analyzeMotionData();
                generatePattern();
                
                // Transition to display state
                currentState = FreeCastState::DISPLAYING;
                phaseStartTime = currentTime;
                #ifdef DEBUG_MODE
                Serial.println(F("FreeCast Mode: Transition to Display phase"));
                Serial.print(F("Motion intensity: "));
                Serial.println(motionIntensity);
                Serial.print(F("Pattern type: "));
                Serial.println((int)currentPatternType);
                #endif
            }
            break;
            
        case FreeCastState::DISPLAYING:
            // Display current pattern
            renderCurrentPattern(elapsedTime);
            
            // Check if display phase is complete (2 seconds as per TrueFunctionGuide)
            if (elapsedTime >= Config::FREECAST_DISPLAY_MS) {
                // Transition back to recording state
                currentState = FreeCastState::RECORDING;
                phaseStartTime = currentTime;
                motionBufferIndex = 0;
                motionBufferCount = 0;
                #ifdef DEBUG_MODE
                Serial.println(F("FreeCast Mode: Transition to Recording phase"));
                #endif
            }
            break;
    }
    
    // NULL position tracking for exit gesture (legacy support) - disabled
    // if (currentPosition.position == POS_NULLPOS) {
    //     if (nullPositionStartTime == 0) {
    //         // Just entered NULL position
    //         nullPositionStartTime = currentTime;
    //         inNullCountdown = false;
    //     }
    //     
    //     // Check for NULL countdown trigger (after 3 seconds)
    //     if (!inNullCountdown && currentTime - nullPositionStartTime >= Config::LONGNULL_WARNING_MS) {
    //         inNullCountdown = true;
    //     }
    // } else {
    //     // Reset NULL tracking when position changes
    //     nullPositionStartTime = 0;
    //     inNullCountdown = false;
    // }
    
    // SHIELD position tracking moved to the top for exit check
    
    // Update LEDs through renderLEDs() which will be called separately
    
    return ModeTransition::NONE; // Stay in FreeCast mode if no transition detected
}

// Add the missing renderLEDs function definition
void FreeCastMode::renderLEDs() {
    unsigned long currentTime = millis();
    
    // Prioritize exit countdown flashing
    if (inShieldCountdown) {
        // Flash blue LEDs during the countdown
        bool flashState = (currentTime / 250) % 2; // Simple on/off flash every 250ms
        // Use the project's Color struct
        hardwareManager->setAllLEDs(flashState ? Color{0, 0, 255} : Color{0, 0, 0}); 
    } else {
        // Otherwise, render based on the current state
        switch (currentState) {
            case FreeCastState::INITIALIZING:
                // Use the project's Color struct
                hardwareManager->setAllLEDs(Color{0, 0, 0}); // Clear LEDs during init
                break;
            case FreeCastState::RECORDING:
                renderBackgroundAnimation();
                break;
            case FreeCastState::DISPLAYING:
                // Calculate elapsed time *within the display phase* for pattern rendering
                renderCurrentPattern(currentTime - phaseStartTime); 
                break;
        }
    }
    
    // Always update the LEDs after setting them
    hardwareManager->updateLEDs();
}

/**
 * @brief Reset FreeCast mode state
 * 
 * Resets state for clean exit when cancelled via shake gesture
 */
void FreeCastMode::reset() {
    // Reset core state
    currentState = FreeCastState::INITIALIZING;
    motionBufferIndex = 0;
    motionBufferCount = 0;
    
    // Reset timing
    phaseStartTime = millis();
    
    // Reset gesture tracking
    nullPositionStartTime = 0;
    inNullCountdown = false;
    shieldPositionStartTime = 0;
    inShieldCountdown = false;
    
    // Clear LEDs
    hardwareManager->setAllLEDs(Color{0, 0, 0});
    hardwareManager->updateLEDs();
    
    #ifdef DEBUG_MODE
    Serial.println(F("FreeCast Mode: Reset by ShakeCancel"));
    #endif
    
    #if DIAG_LOGGING_ENABLED
    DIAG_INFO(DIAG_TAG_MODE, "FreeCastMode reset by ShakeCancel");
    #endif
}

// Collect motion data during recording phase
void FreeCastMode::collectMotionData() {
    // Get current processed sensor data
    ProcessedData currentData = positionDetector->getProcessedData();
    
    // Store in circular buffer
    motionBuffer[motionBufferIndex] = currentData;
    motionBufferIndex = (motionBufferIndex + 1) % MOTION_BUFFER_SIZE;
    
    // Track buffer fullness
    if (motionBufferCount < MOTION_BUFFER_SIZE) {
        motionBufferCount++;
    }
}

// Analyze collected motion data at the end of recording phase
void FreeCastMode::analyzeMotionData() {
    // Skip analysis if we don't have enough data
    if (motionBufferCount < 10) {
        motionIntensity = 0.1f;
        motionDirectionality = 0.5f;
        rotationIntensity = 0.1f;
        dominantAxis = 0;
        currentPatternType = PatternType::SPARKLES;
        return;
    }
    
    // Calculate motion intensity (overall acceleration magnitude)
    motionIntensity = calculateMotionIntensity();
    
    // Calculate directionality (how consistent the motion direction is)
    motionDirectionality = calculateDirectionality();
    
    // Determine dominant axis (X=0, Y=1, Z=2)
    float sumX = 0.0f, sumY = 0.0f, sumZ = 0.0f;
    for (uint16_t i = 0; i < motionBufferCount; i++) {
        sumX += fabsf(motionBuffer[i].accelX);
        sumY += fabsf(motionBuffer[i].accelY);
        sumZ += fabsf(motionBuffer[i].accelZ);
    }
    
    if (sumX > sumY && sumX > sumZ) dominantAxis = 0;
    else if (sumY > sumX && sumY > sumZ) dominantAxis = 1;
    else dominantAxis = 2;
    
    // Rotation intensity is simulated since we don't have gyroscope data
    // Use variance in axes orthogonal to dominant axis as proxy
    rotationIntensity = 0.5f; // Default value
    
    // Determine pattern type based on motion characteristics
    currentPatternType = static_cast<PatternType>(determinePatternType());
    
    // Generate color palette based on motion
    generateColorPalette();
}

// Calculate motion intensity from acceleration data
float FreeCastMode::calculateMotionIntensity() {
    // Guard against division by zero or insufficient data
    if (motionBufferCount == 0) {
        return 0.1f; // Default low intensity
    }
    
    float totalMagnitude = 0.0f;
    float maxMagnitude = 0.0f;
    
    for (uint16_t i = 0; i < motionBufferCount; i++) {
        float mag = sqrtf(
            sq(motionBuffer[i].accelX) + 
            sq(motionBuffer[i].accelY) + 
            sq(motionBuffer[i].accelZ)
        );
        
        totalMagnitude += mag;
        if (mag > maxMagnitude) maxMagnitude = mag;
    }
    
    // Normalize to 0.0-1.0 range (capped at 12.0f which is ~1.2g)
    // Lowered from 15.0f to make it more sensitive to motion
    // Guard against division by zero (if maxMagnitude is near-zero)
    if (maxMagnitude < 0.0001f) {
        return 0.1f; // Default low intensity for minimal motion
    }
    
    float intensity = constrain(maxMagnitude / 12.0f, 0.1f, 1.0f);
    
    // Apply a curve to make small movements more noticeable
    // (x^0.7 gives more response to lower values while preserving range)
    return powf(intensity, 0.7f);
}

// Calculate motion directionality (consistency of direction)
float FreeCastMode::calculateDirectionality() {
    // Simple implementation - would be more sophisticated with gyro data
    
    // Guard against division by zero
    if (motionBufferCount == 0) {
        return 0.5f; // Default medium directionality
    }
    
    // Calculate mean direction vector
    float meanX = 0.0f, meanY = 0.0f, meanZ = 0.0f;
    for (uint16_t i = 0; i < motionBufferCount; i++) {
        meanX += motionBuffer[i].accelX;
        meanY += motionBuffer[i].accelY;
        meanZ += motionBuffer[i].accelZ;
    }
    meanX /= motionBufferCount;
    meanY /= motionBufferCount;
    meanZ /= motionBufferCount;
    
    // Calculate variance from mean direction
    float variance = 0.0f;
    for (uint16_t i = 0; i < motionBufferCount; i++) {
        variance += sq(motionBuffer[i].accelX - meanX) +
                    sq(motionBuffer[i].accelY - meanY) +
                    sq(motionBuffer[i].accelZ - meanZ);
    }
    
    // Guard against division by zero before dividing variance by count
    if (motionBufferCount == 0) {
        return 0.5f; // Default medium directionality
    }
    
    variance /= motionBufferCount;
    
    // Convert variance to directionality (inverse relationship)
    // Higher variance = lower directionality
    // Guard against division by zero (if variance is near-zero)
    if (variance < 0.0001f) {
        return 0.9f; // High directionality when variance is extremely low
    }
    
    return constrain(1.0f - (variance / 50.0f), 0.1f, 0.9f);
}

// Determine pattern type based on motion characteristics
uint8_t FreeCastMode::determinePatternType() {
    // Guard against division by zero
    if (motionBufferCount == 0) {
        return static_cast<uint8_t>(PatternType::SPARKLES); // Default pattern
    }
    
    // Use the dominant axis to influence pattern selection
    float xMag = 0, yMag = 0, zMag = 0;
    
    // Calculate average magnitude for each axis
    for (uint16_t i = 0; i < motionBufferCount; i++) {
        xMag += abs(motionBuffer[i].accelX);
        yMag += abs(motionBuffer[i].accelY);
        zMag += abs(motionBuffer[i].accelZ);
    }
    
    // Guard against division by zero before calculating averages
    if (motionBufferCount == 0) {
        return static_cast<uint8_t>(PatternType::SPARKLES); // Default pattern
    }
    
    xMag /= motionBufferCount;
    yMag /= motionBufferCount;
    zMag /= motionBufferCount;
    
    // Determine dominant axis
    if (xMag > yMag && xMag > zMag) {
        dominantAxis = 0; // X-axis dominant (side-to-side motion)
    } else if (yMag > xMag && yMag > zMag) {
        dominantAxis = 1; // Y-axis dominant (up-down motion)
    } else {
        dominantAxis = 2; // Z-axis dominant (forward-backward motion)
    }
    
    // X-axis dominant (side to side) -> COLOR_TRAILS
    if (dominantAxis == 0 && motionIntensity > 0.3f) {
        return static_cast<uint8_t>(PatternType::COLOR_TRAILS);
    }
    
    // Y-axis dominant (up-down) -> WAVES
    if (dominantAxis == 1 && motionIntensity > 0.3f) {
        return static_cast<uint8_t>(PatternType::WAVES);
    }
    
    // Z-axis dominant (forward-backward) -> SHOOTING_STARS
    if (dominantAxis == 2 && motionIntensity > 0.3f) {
        return static_cast<uint8_t>(PatternType::SHOOTING_STARS);
    }
    
    // Shaking: high intensity, low directionality
    if (motionIntensity > 0.6f && motionDirectionality < 0.4f) {
        return static_cast<uint8_t>(PatternType::SPARKLES);
    }
    
    // Sharp direction changes: variable intensity, medium directionality
    if (motionDirectionality > 0.3f && motionDirectionality < 0.6f && motionIntensity > 0.4f) {
        return static_cast<uint8_t>(PatternType::PULSES);
    }
    
    // Default to sparkles for low intensity motion
    return static_cast<uint8_t>(PatternType::SPARKLES);
}

// Generate a color palette based on motion characteristics
void FreeCastMode::generateColorPalette() {
    // Create vibrant rainbow colors regardless of motion type
    // Brightness will still be affected by motion intensity
    
    // Generate more vibrant rainbow palette
    patternColors[0] = CRGB(255, 0, 0);       // Red (Null position)
    patternColors[1] = CRGB(0, 255, 0);       // Green (Dig position)
    patternColors[2] = CRGB(255, 105, 180);   // Pink (Shield position)
    patternColors[3] = CRGB(128, 0, 255);     // Purple (Offer position)
    patternColors[4] = CRGB(255, 255, 0);     // Yellow (Oath position)
    
    // Scale brightness based on motion intensity
    float brightnessScale = 0.5f + (motionIntensity * 0.5f); // 0.5-1.0 brightness scale
    
    // Apply brightness scaling based on motion intensity
    for (int i = 0; i < 5; i++) {
        patternColors[i].r = patternColors[i].r * brightnessScale;
        patternColors[i].g = patternColors[i].g * brightnessScale;
        patternColors[i].b = patternColors[i].b * brightnessScale;
    }
}

// Generate final pattern after analysis
void FreeCastMode::generatePattern() {
    // Pattern generation is handled in the renderCurrentPattern method
    // Nothing specific to do here as we've already determined pattern type
}

// Render a subtle background animation during recording phase
void FreeCastMode::renderBackgroundAnimation() {
    // Create a subtle pulsing effect
    unsigned long currentTime = millis();
    uint8_t pulse = (sin8(currentTime / 10) * 64) / 255; // 0-64 brightness pulse
    
    // Dim white pulsing around the ring
    for (int i = 0; i < Config::NUM_LEDS; i++) {
        uint8_t brightness = pulse + (i % 3 == 0 ? 16 : 0); // Slightly brighter every 3rd LED
        hardwareManager->setLED(i, {brightness, brightness, brightness});
    }
}

// Render the current pattern based on pattern type
void FreeCastMode::renderCurrentPattern(unsigned long elapsedTime) {
    // Ensure we have at least some motion data before trying to render patterns
    // This provides an extra layer of protection against division by zero in the renderers
    if (motionBufferCount == 0) {
        // If no motion data, just show a simple default pattern
        // Create a simple pulsing effect on all LEDs
        uint8_t pulse = (sin8(elapsedTime / 20) * 128) / 255 + 40; // 40-168 brightness pulse
        CRGB color = CRGB(pulse, pulse, pulse); // White pulsing
        
        for (int i = 0; i < Config::NUM_LEDS; i++) {
            hardwareManager->setLED(i, {color.r, color.g, color.b});
        }
        return;
    }
    
    // Regular rendering with pattern selection
    switch (currentPatternType) {
        case PatternType::SHOOTING_STARS:
            renderShootingStars(elapsedTime);
            break;
        case PatternType::WAVES:
            renderWaves(elapsedTime);
            break;
        case PatternType::SPARKLES:
            renderSparkles(elapsedTime);
            break;
        case PatternType::COLOR_TRAILS:
            renderColorTrails(elapsedTime);
            break;
        case PatternType::PULSES:
            renderPulses(elapsedTime);
            break;
        default:
            renderSparkles(elapsedTime); // Default to sparkles
            break;
    }
}

// Render shooting stars pattern (for quick flicks)
void FreeCastMode::renderShootingStars(unsigned long elapsedTime) {
    // Parameters based on motion
    uint8_t numStars = 1 + (motionIntensity * 3); // 1-4 shooting stars
    uint8_t starSpeed = 50 + (motionIntensity * 200); // Speed factor
    // Ensure tailLength is never zero by using max()
    uint8_t tailLength = max(1, 3 + (int)(motionIntensity * 6)); // 3-9 LEDs, guaranteed at least 1
    
    // Clear all LEDs first
    hardwareManager->setAllLEDs({0, 0, 0});
    
    // Create stars at different positions
    for (uint8_t star = 0; star < numStars; star++) {
        // Calculate position based on time and star index
        uint8_t pos = ((elapsedTime / (200 - starSpeed)) + (star * 7)) % Config::NUM_LEDS;
        
        // Each star has its own color based on position and time
        // This ensures variety between stars and runs
        CRGB headColor;
        uint8_t starHue = (star * 40 + (elapsedTime / 50)) % 255; // Spaced hues, slowly shifting
        headColor.setHSV(starHue, 255, 255); // Full saturation and brightness for head
        
        // Create tail
        for (uint8_t tail = 0; tail < tailLength; tail++) {
            // Calculate tail position (wrapping around the ring)
            uint8_t tailPos = (pos + Config::NUM_LEDS - tail) % Config::NUM_LEDS;
            
            // Calculate tail brightness (fading out)
            // tailLength is now guaranteed to be at least 1
            uint8_t brightness = 255 - ((255 * tail) / tailLength);
            
            // For tail colors, fade to white or yellow at the tips
            CRGB tailColor;
            
            if (tail < tailLength / 3) {
                // Head section - keep the star's color
                tailColor = headColor;
            } else {
                // Tail section - blend toward white/yellow for a fiery effect
                // Guard against division by zero or very small divisor
                float divisor = max(1.0f, (float)(tailLength - tailLength/3));
                float blendFactor = (float)(tail - tailLength/3) / divisor;
                CRGB fadeColor = CRGB(255, 180, 0); // Yellowish/orange
                tailColor = blend(headColor, fadeColor, blendFactor * 255);
            }
            
            // Scale by brightness and intensity
            float intensityFactor = 0.4f + (motionIntensity * 0.6f); // 0.4-1.0 scaling
            uint8_t finalBrightness = brightness * intensityFactor;
            tailColor.nscale8(finalBrightness);
            
            // Set LED
            hardwareManager->setLED(tailPos, {tailColor.r, tailColor.g, tailColor.b});
        }
    }
}

// Render waves pattern (for circular motions)
void FreeCastMode::renderWaves(unsigned long elapsedTime) {
    // Parameters based on motion
    float speed = 0.05f + (motionIntensity * 0.3f); // Increased wave speed responsiveness
    float intensity = 0.5f + (motionIntensity * 0.5f); // Brightness intensity
    
    // Use directionality to control wave frequency
    float waveFrequency = 1.0f + (motionDirectionality * 3.0f); // 1-4 waves based on directionality
    
    // Create wave effect around the ring
    int numLeds = (Config::NUM_LEDS > 0) ? Config::NUM_LEDS : 1; // Ensure NUM_LEDS is at least 1
    
    for (int i = 0; i < numLeds; i++) {
        // Calculate wave phase for this LED with frequency modulation
        // Guard against division by zero by ensuring denominator is at least 1
        float phase = waveFrequency * ((float)i / numLeds * TWO_PI) + elapsedTime * speed;
        
        // Generate sinusoidal wave
        float wave = (sinf(phase) + 1.0f) / 2.0f; // 0.0-1.0
        
        // Full rainbow color selection - HSV color wheel
        CRGB color;
        
        // Use the full color wheel based on position and time
        // Scale up the time division to make the colors shift more slowly
        uint8_t hue = ((i * 21) + (elapsedTime / 30)) % 255;
        
        // Use the wave to vary the specific hue around the base
        // Convert wave to an integer offset before using modulo
        uint8_t waveOffset = (uint8_t)(wave * 30.0f);
        hue = (hue + waveOffset) % 255;
        
        // Saturation based on directionality - more directional = more saturated
        uint8_t sat = 200 + (motionDirectionality * 55); // 200-255
        
        // Brightness based on wave value and motion intensity
        uint8_t val = 128 + (wave * 127 * intensity); // 128-255 with intensity scaling
        
        // Set HSV color
        color.setHSV(hue, sat, val);
        
        // Set LED
        hardwareManager->setLED(i, {color.r, color.g, color.b});
    }
}

// Render sparkles pattern (for shaking motions)
void FreeCastMode::renderSparkles(unsigned long elapsedTime) {
    // Parameters based on motion
    uint8_t numSparkles = 3 + (motionIntensity * 9); // 3-12 sparkles
    uint8_t changeRate = 100 - (motionIntensity * 70); // How often sparkles change
    
    // Seed random number generator with elapsedTime
    uint32_t seed = elapsedTime / changeRate;
    randomSeed(seed);
    
    // Clear all LEDs first
    hardwareManager->setAllLEDs({0, 0, 0});
    
    // Define a more varied color palette using preset colors
    CRGB sparkleColors[] = {
        CRGB(255, 0, 0),     // Red
        CRGB(255, 255, 0),   // Yellow
        CRGB(0, 255, 0),     // Green
        CRGB(0, 255, 255),   // Cyan
        CRGB(0, 0, 255),     // Blue
        CRGB(255, 0, 255),   // Magenta
        CRGB(255, 255, 255)  // White
    };
    
    // Create random sparkles
    for (uint8_t i = 0; i < numSparkles; i++) {
        // Random position
        uint8_t pos = random(Config::NUM_LEDS);
        
        // Get a random color from our defined palette
        CRGB color = sparkleColors[random(7)];
        
        // Random brightness with intensity scaling
        uint8_t brightness = 128 + random(128); 
        float scaling = 0.5f + (motionIntensity * 0.5f);  // Scale 50-100% based on motion
        
        color.r = color.r * scaling * brightness / 255;
        color.g = color.g * scaling * brightness / 255;
        color.b = color.b * scaling * brightness / 255;
        
        // Set LED
        hardwareManager->setLED(pos, {color.r, color.g, color.b});
    }
}

// Render color trails pattern (for directional movements)
void FreeCastMode::renderColorTrails(unsigned long elapsedTime) {
    // Parameters based on motion
    uint8_t speed = 50 + (motionIntensity * 150); // Rotation speed
    
    // Ensure we don't divide by zero in position calculation
    uint8_t speedForCalculation = (speed < 199) ? speed : 199; // Prevent 200-speed from being zero
    
    // Calculate position
    uint8_t pos = (elapsedTime / (200 - speedForCalculation)) % Config::NUM_LEDS;
    
    // Create gradient around the ring
    for (int i = 0; i < Config::NUM_LEDS; i++) {
        // Calculate distance from position (0 to NUM_LEDS/2)
        uint8_t distance = min(
            (i + Config::NUM_LEDS - pos) % Config::NUM_LEDS,
            (pos + Config::NUM_LEDS - i) % Config::NUM_LEDS
        );
        
        // Normalize distance to 0-255
        // Guard against division by zero by ensuring denominator is at least 1
        uint8_t normalizedDistance;
        if (Config::NUM_LEDS > 1) {
            normalizedDistance = (distance * 255) / (Config::NUM_LEDS / 2);
        } else {
            normalizedDistance = 0; // Default if NUM_LEDS is invalid
        }
        
        // Use HSV for more vibrant colors with a full rainbow spectrum
        CRGB color;
        
        // Base hue depends on time, making the color pattern rotate over time
        uint8_t baseHue = (elapsedTime / 20) % 255;
        
        // Add distance-based hue shift to create rainbow gradient
        uint8_t hue = (baseHue + normalizedDistance) % 255;
        
        // Saturation based on directionality - more directional = more saturated
        uint8_t sat = 200 + (motionDirectionality * 55); // 200-255
        
        // Brightness based on motion intensity and distance from center point
        float falloff = 1.0f - (normalizedDistance / 255.0f) * 0.4f; // 0.6-1.0 range
        uint8_t val = (200 + (motionIntensity * 55)) * falloff; // 120-255 with falloff
        
        // Set HSV color
        color.setHSV(hue, sat, val);
        
        // Set LED
        hardwareManager->setLED(i, {color.r, color.g, color.b});
    }
}

// Render pulses pattern (for sharp direction changes)
void FreeCastMode::renderPulses(unsigned long elapsedTime) {
    // Parameters based on motion
    // Ensure numPulses is never zero by using max()
    uint8_t numPulses = max(1, 1 + (int)(motionIntensity * 2)); // 1-3 pulses, guaranteed at least 1
    uint8_t pulseSpeed = 100 + (motionIntensity * 400); // Expansion speed
    
    // Clear all LEDs first
    hardwareManager->setAllLEDs({0, 0, 0});
    
    // Create pulses
    for (uint8_t pulse = 0; pulse < numPulses; pulse++) {
        // Calculate pulse center and radius - numPulses is now guaranteed to be at least 1
        uint8_t center = (pulse * Config::NUM_LEDS / numPulses); // Evenly spaced
        float radius = ((elapsedTime / pulseSpeed) + (pulse * 0.33f)) * Config::NUM_LEDS;
        radius = fmodf(radius, Config::NUM_LEDS * 1.5f); // Wrap around with gap
        
        // Each pulse has its own color based on motion intensity
        CRGB pulseColor;
        uint8_t hue = (pulse * 85 + (elapsedTime / 30)) % 255; // Spaced evenly around color wheel
        uint8_t sat = 200 + (motionDirectionality * 55); // 200-255 based on directionality
        uint8_t val = 200 + (motionIntensity * 55); // 200-255 based on intensity
        pulseColor.setHSV(hue, sat, val);
        
        // Draw pulse
        for (int i = 0; i < Config::NUM_LEDS; i++) {
            // Calculate distance from pulse center (shortest path around ring)
            uint8_t distance = min(
                (i + Config::NUM_LEDS - center) % Config::NUM_LEDS,
                (center + Config::NUM_LEDS - i) % Config::NUM_LEDS
            );
            
            // Check if this LED is within pulse radius
            if (distance <= radius && distance >= radius - 3) {
                // Calculate intensity based on distance from radius edge
                float intensity = 1.0f - fabsf((radius - distance) / 3.0f);
                
                // Scale by intensity
                CRGB color = pulseColor;
                color.nscale8(intensity * 255);
                
                // Set LED
                hardwareManager->setLED(i, {color.r, color.g, color.b});
            }
        }
    }
}

#ifdef DEBUG_MODE
void FreeCastMode::printStatus() const {
    Serial.println(F("=== FreeCast Mode Status ==="));
    
    // Print current state
    Serial.print(F("Current State: "));
    switch (currentState) {
        case FreeCastState::INITIALIZING:
            Serial.println(F("INITIALIZING"));
            break;
        case FreeCastState::RECORDING:
            Serial.println(F("RECORDING"));
            break;
        case FreeCastState::DISPLAYING:
            Serial.println(F("DISPLAYING"));
            break;
    }
    
    // Print motion data statistics
    Serial.print(F("Motion buffer samples: "));
    Serial.println(motionBufferCount);
    
    Serial.print(F("Motion intensity: "));
    Serial.println(motionIntensity);
    
    Serial.print(F("Motion directionality: "));
    Serial.println(motionDirectionality);
    
    Serial.print(F("Dominant axis: "));
    switch (dominantAxis) {
        case 0: Serial.println(F("X")); break;
        case 1: Serial.println(F("Y")); break;
        case 2: Serial.println(F("Z")); break;
    }
    
    // Print pattern information
    Serial.print(F("Current pattern: "));
    switch (currentPatternType) {
        case PatternType::SHOOTING_STARS: Serial.println(F("SHOOTING_STARS")); break;
        case PatternType::WAVES: Serial.println(F("WAVES")); break;
        case PatternType::SPARKLES: Serial.println(F("SPARKLES")); break;
        case PatternType::COLOR_TRAILS: Serial.println(F("COLOR_TRAILS")); break;
        case PatternType::PULSES: Serial.println(F("PULSES")); break;
    }
    
    // Print exit gesture status
    Serial.print(F("In Null Countdown: "));
    Serial.println(inNullCountdown ? F("YES") : F("NO"));
    
    if (nullPositionStartTime > 0) {
        unsigned long nullDuration = millis() - nullPositionStartTime;
        Serial.print(F("Null Position Duration: "));
        Serial.print(nullDuration);
        Serial.println(F("ms"));
    }
}
#endif 