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
      currentPosition({POS_UNKNOWN, 0, 0})
{
    // Initialize color palette with default values
    patternColors[0] = CRGB(255, 0, 0);   // Red
    patternColors[1] = CRGB(0, 255, 0);   // Green
    patternColors[2] = CRGB(0, 0, 255);   // Blue
    patternColors[3] = CRGB(255, 255, 0); // Yellow
    patternColors[4] = CRGB(0, 255, 255); // Cyan
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
    // Reset state variables
    currentState = FreeCastState::INITIALIZING;
    motionBufferIndex = 0;
    motionBufferCount = 0;
    motionIntensity = 0.0f;
    motionDirectionality = 0.0f;
    rotationIntensity = 0.0f;
    dominantAxis = 0;
    currentPatternType = PatternType::SPARKLES;
    phaseStartTime = millis();
    nullPositionStartTime = 0;
    inNullCountdown = false;
    
    // Initial position reading
    currentPosition = positionDetector->getCurrentPosition();
    
    // Clear motion buffer
    for (uint16_t i = 0; i < MOTION_BUFFER_SIZE; i++) {
        motionBuffer[i] = {0.0f, 0.0f, 0.0f};
    }
    
    // Initialize with a sweeping orange pattern as described in TrueFunctionGuide
    Color orange = {255, 165, 0};
    for (int i = 0; i < Config::NUM_LEDS; i++) {
        hardwareManager->setLED(i, orange);
    }
    hardwareManager->updateLEDs();
    
    // After 500ms of initialization, transition to recording state
    currentState = FreeCastState::RECORDING;
    
    #ifdef DEBUG_MODE
    Serial.println(F("FreeCast Mode initialized"));
    #endif
}

// Main update function - called repeatedly
void FreeCastMode::update() {
    // Get current time
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - phaseStartTime;
    
    // Update position for exit gesture detection
    currentPosition = positionDetector->update();
    
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
    
    // NULL position tracking for exit gesture
    if (currentPosition.position == POS_NULLPOS) {
        if (nullPositionStartTime == 0) {
            // Just entered NULL position
            nullPositionStartTime = currentTime;
            inNullCountdown = false;
        }
        
        // Check for NULL countdown trigger (after 3 seconds)
        if (!inNullCountdown && currentTime - nullPositionStartTime >= Config::LONGNULL_WARNING_MS) {
            inNullCountdown = true;
        }
    } else {
        // Reset NULL tracking when position changes
        nullPositionStartTime = 0;
        inNullCountdown = false;
    }
    
    // Update LEDs through renderLEDs() which will be called separately
}

// Check for transition to another mode
ModeTransition FreeCastMode::checkForTransition() {
    // Check for LongNull gesture to exit FreeCast mode
    if (detectLongNullGesture()) {
        return ModeTransition::TO_IDLE;
    }
    
    return ModeTransition::NONE;
}

// Detect LongNull gesture (hold NULL position for 5 seconds)
bool FreeCastMode::detectLongNullGesture() {
    // Check if we're in NULL position and have been for the required time
    if (inNullCountdown && currentPosition.position == POS_NULLPOS) {
        unsigned long currentTime = millis();
        unsigned long nullDuration = currentTime - nullPositionStartTime;
        
        // Check if we've held NULL long enough (5 seconds)
        if (nullDuration >= Config::LONGNULL_TIME_MS) {
            inNullCountdown = false; // Reset to prevent repeated triggers
            return true;
        }
    }
    
    return false;
}

// Render LEDs based on current state
void FreeCastMode::renderLEDs() {
    // Clear all LEDs first
    hardwareManager->setAllLEDs({0, 0, 0});
    
    // Handle rendering based on state
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - phaseStartTime;
    
    switch (currentState) {
        case FreeCastState::INITIALIZING:
            // Orange sweep pattern for initialization
            {
                Color orange = {255, 165, 0};
                int pos = (elapsedTime / 50) % Config::NUM_LEDS;
                hardwareManager->setLED(pos, orange);
                hardwareManager->setLED((pos + 1) % Config::NUM_LEDS, orange);
                hardwareManager->setLED((pos + 2) % Config::NUM_LEDS, orange);
            }
            break;
            
        case FreeCastState::RECORDING:
            // Subtle background animation during recording
            renderBackgroundAnimation();
            break;
            
        case FreeCastState::DISPLAYING:
            // Show generated pattern
            renderCurrentPattern(elapsedTime);
            break;
    }
    
    // Handle LongNull exit countdown (like in IdleMode)
    if (inNullCountdown) {
        unsigned long nullDuration = currentTime - nullPositionStartTime;
        if (nullDuration >= Config::LONGNULL_WARNING_MS && nullDuration < Config::LONGNULL_TIME_MS) {
            // Flash at 2Hz (250ms on, 250ms off)
            if ((currentTime / 250) % 2 == 0) {
                // On phase - show orange
                Color orange = {255, 165, 0};
                for (int i = 0; i < 4; i++) {
                    hardwareManager->setLED(i * 3, orange);
                }
            }
            // Off phase is handled by the initial clear
        }
    }
    
    // Update the LED display
    hardwareManager->updateLEDs();
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
    
    // Normalize to 0.0-1.0 range (capped at 15.0f which is ~1.5g)
    return constrain(maxMagnitude / 15.0f, 0.1f, 1.0f);
}

// Calculate motion directionality (consistency of direction)
float FreeCastMode::calculateDirectionality() {
    // Simple implementation - would be more sophisticated with gyro data
    
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
    variance /= motionBufferCount;
    
    // Convert variance to directionality (inverse relationship)
    // Higher variance = lower directionality
    return constrain(1.0f - (variance / 50.0f), 0.1f, 0.9f);
}

// Determine pattern type based on motion characteristics
uint8_t FreeCastMode::determinePatternType() {
    // Quick flicks: high intensity, high directionality
    if (motionIntensity > 0.7f && motionDirectionality > 0.6f) {
        return static_cast<uint8_t>(PatternType::SHOOTING_STARS);
    }
    
    // Circular motions: medium intensity, medium directionality
    if (motionIntensity > 0.3f && motionIntensity < 0.7f && 
        motionDirectionality > 0.4f && motionDirectionality < 0.7f) {
        return static_cast<uint8_t>(PatternType::WAVES);
    }
    
    // Shaking: high intensity, low directionality
    if (motionIntensity > 0.6f && motionDirectionality < 0.4f) {
        return static_cast<uint8_t>(PatternType::SPARKLES);
    }
    
    // Directional movements: medium intensity, high directionality
    if (motionIntensity > 0.3f && motionIntensity < 0.8f && 
        motionDirectionality > 0.7f) {
        return static_cast<uint8_t>(PatternType::COLOR_TRAILS);
    }
    
    // Sharp direction changes: variable intensity, medium directionality
    if (motionDirectionality > 0.3f && motionDirectionality < 0.6f) {
        return static_cast<uint8_t>(PatternType::PULSES);
    }
    
    // Default to sparkles
    return static_cast<uint8_t>(PatternType::SPARKLES);
}

// Generate a color palette based on motion characteristics
void FreeCastMode::generateColorPalette() {
    // Create vibrant rainbow colors regardless of motion type
    // Brightness will still be affected by motion intensity
    
    // Generate more vibrant rainbow palette
    patternColors[0] = CRGB(255, 0, 0);     // Red
    patternColors[1] = CRGB(0, 255, 0);     // Green
    patternColors[2] = CRGB(0, 0, 255);     // Blue
    patternColors[3] = CRGB(255, 0, 255);   // Magenta
    patternColors[4] = CRGB(255, 255, 0);   // Yellow
    
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
    uint8_t tailLength = 3 + (motionIntensity * 6); // 3-9 LEDs
    
    // Clear all LEDs first
    hardwareManager->setAllLEDs({0, 0, 0});
    
    // Create stars at different positions
    for (uint8_t star = 0; star < numStars; star++) {
        // Calculate position based on time and star index
        uint8_t pos = ((elapsedTime / (200 - starSpeed)) + (star * 7)) % Config::NUM_LEDS;
        
        // Create tail
        for (uint8_t tail = 0; tail < tailLength; tail++) {
            // Calculate tail position (wrapping around the ring)
            uint8_t tailPos = (pos + Config::NUM_LEDS - tail) % Config::NUM_LEDS;
            
            // Calculate tail brightness (fading out)
            uint8_t brightness = 255 - ((255 * tail) / tailLength);
            
            // Get color from palette
            CRGB color = patternColors[star % 5];
            
            // Scale by brightness and intensity
            uint8_t finalBrightness = ((brightness * motionIntensity) / 255) * 255;
            color.nscale8(finalBrightness);
            
            // Set LED
            hardwareManager->setLED(tailPos, {color.r, color.g, color.b});
        }
    }
}

// Render waves pattern (for circular motions)
void FreeCastMode::renderWaves(unsigned long elapsedTime) {
    // Parameters based on motion
    float speed = 0.05f + (motionIntensity * 0.2f); // Wave speed
    float intensity = 0.5f + (motionIntensity * 0.5f); // Increased minimum intensity
    
    // Create wave effect around the ring
    for (int i = 0; i < Config::NUM_LEDS; i++) {
        // Calculate wave phase for this LED
        float phase = (float)i / Config::NUM_LEDS * TWO_PI + elapsedTime * speed;
        
        // Generate sinusoidal wave
        float wave = (sinf(phase) + 1.0f) / 2.0f; // 0.0-1.0
        
        // Full rainbow color selection - HSV color wheel
        CRGB color;
        uint8_t hue = (wave * 255) + (elapsedTime / 20) % 255; // Moving through the color wheel
        color.setHSV(hue, 240, 255 * intensity); // Full saturation, brightness based on intensity
        
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
    
    // Create random sparkles
    for (uint8_t i = 0; i < numSparkles; i++) {
        // Random position
        uint8_t pos = random(Config::NUM_LEDS);
        
        // Random color using HSV for more vibrant colors
        CRGB color;
        uint8_t hue = random(255); // Full color wheel
        uint8_t brightness = 128 + random(128); // 128-255
        color.setHSV(hue, 255, brightness); // Full saturation, random brightness
        
        // Set LED
        hardwareManager->setLED(pos, {color.r, color.g, color.b});
    }
}

// Render color trails pattern (for directional movements)
void FreeCastMode::renderColorTrails(unsigned long elapsedTime) {
    // Parameters based on motion
    uint8_t trailLength = Config::NUM_LEDS; // Full ring gradient
    uint8_t speed = 50 + (motionIntensity * 150); // Rotation speed
    
    // Calculate position
    uint8_t pos = (elapsedTime / (200 - speed)) % Config::NUM_LEDS;
    
    // Create gradient around the ring
    for (int i = 0; i < Config::NUM_LEDS; i++) {
        // Calculate distance from position (0 to NUM_LEDS/2)
        uint8_t distance = min(
            (i + Config::NUM_LEDS - pos) % Config::NUM_LEDS,
            (pos + Config::NUM_LEDS - i) % Config::NUM_LEDS
        );
        
        // Normalize distance to 0-255
        uint8_t normalizedDistance = (distance * 255) / (Config::NUM_LEDS / 2);
        
        // Select color based on distance
        CRGB color;
        if (normalizedDistance < 85) {
            color = blend(patternColors[0], patternColors[1], normalizedDistance * 3);
        } else if (normalizedDistance < 170) {
            color = blend(patternColors[1], patternColors[2], (normalizedDistance - 85) * 3);
        } else {
            color = blend(patternColors[2], patternColors[0], (normalizedDistance - 170) * 3);
        }
        
        // Apply intensity based on directionality
        uint8_t brightness = 64 + (motionDirectionality * 191);
        color.nscale8(brightness);
        
        // Set LED
        hardwareManager->setLED(i, {color.r, color.g, color.b});
    }
}

// Render pulses pattern (for sharp direction changes)
void FreeCastMode::renderPulses(unsigned long elapsedTime) {
    // Parameters based on motion
    uint8_t numPulses = 1 + (motionIntensity * 2); // 1-3 pulses
    uint8_t pulseSpeed = 100 + (motionIntensity * 400); // Expansion speed
    
    // Clear all LEDs first
    hardwareManager->setAllLEDs({0, 0, 0});
    
    // Create pulses
    for (uint8_t pulse = 0; pulse < numPulses; pulse++) {
        // Calculate pulse center and radius
        uint8_t center = (pulse * Config::NUM_LEDS / numPulses); // Evenly spaced
        float radius = ((elapsedTime / pulseSpeed) + (pulse * 0.33f)) * Config::NUM_LEDS;
        radius = fmodf(radius, Config::NUM_LEDS * 1.5f); // Wrap around with gap
        
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
                
                // Select color
                CRGB color = patternColors[pulse % 5];
                
                // Scale by intensity
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