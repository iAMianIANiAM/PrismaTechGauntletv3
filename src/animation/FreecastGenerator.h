#ifndef FREECAST_GENERATOR_H
#define FREECAST_GENERATOR_H

#include "../core/SystemTypes.h"

/**
 * @brief Generates LED patterns based on motion data in Freecast mode
 */
class FreecastGenerator {
public:
  /**
   * @brief Initialize the freecast generator
   * @return True if initialization successful
   */
  bool init();
  
  /**
   * @brief Generate a pattern from motion data
   * @param motionData Array of motion data samples
   * @param sampleCount Number of samples in array
   * @param ledColors Output buffer for LED colors
   * @param progress Progress value 0.0-1.0 for animation timing
   */
  void generatePattern(const SensorData* motionData, uint8_t sampleCount, 
                      Color* ledColors, float progress);
                      
  /**
   * @brief Generate a simple collection phase animation
   * @param ledColors Output buffer for LED colors
   * @param progress Progress value 0.0-1.0
   */
  void generateCollectionAnimation(Color* ledColors, float progress);

private:
  // Pattern generation variables
  uint8_t patternType = 0;  // Current pattern type
  float patternIntensity = 0.0f;  // Pattern intensity based on motion
  float patternSpeed = 1.0f;  // Pattern speed multiplier
  
  // Internal helper methods
  void analyzeMotionData(const SensorData* motionData, uint8_t sampleCount);
  
  // Pattern generators
  void generateShootingStarPattern(Color* ledColors, float progress);
  void generateWavePattern(Color* ledColors, float progress);
  void generateSparklePattern(Color* ledColors, float progress);
  void generateBlendPattern(Color* ledColors, float progress);
  void generateRandomPattern(Color* ledColors, float progress);
  
  // Pattern analysis helpers
  float calculateAccelerationMagnitude(const SensorData& data);
  float calculateRotationMagnitude(const SensorData& data);
  float detectCircularMotion(const SensorData* motionData, uint8_t sampleCount);
  float detectSharpChanges(const SensorData* motionData, uint8_t sampleCount);
  
  // Utility functions
  Color blendColors(const Color& a, const Color& b, float ratio);
  Color generateRandomColor(float intensity);
};

#endif // FREECAST_GENERATOR_H 