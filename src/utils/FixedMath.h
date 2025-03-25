#ifndef FIXED_MATH_H
#define FIXED_MATH_H

#include <stdint.h>

/**
 * @brief Fixed-point math utilities for efficient calculations
 * 
 * Uses 16.16 fixed point format (16-bit integer, 16-bit fraction)
 */
namespace FixedMath {
  // Fixed-point representation (16.16)
  typedef int32_t fixed32_t;
  
  // Constants
  constexpr fixed32_t FIXED_ONE = 0x00010000;  // 1.0 in fixed point
  constexpr fixed32_t FIXED_HALF = 0x00008000; // 0.5 in fixed point
  constexpr fixed32_t FIXED_PI = 0x0003243F;   // π in fixed point
  constexpr fixed32_t FIXED_TWO_PI = 0x0006487E; // 2π in fixed point
  
  // Conversion functions
  
  /**
   * @brief Convert integer to fixed point
   * @param i Integer value
   * @return Fixed point representation
   */
  inline fixed32_t intToFixed(int16_t i) {
    return static_cast<fixed32_t>(i) << 16;
  }
  
  /**
   * @brief Convert fixed point to integer
   * @param f Fixed point value
   * @return Integer part
   */
  inline int16_t fixedToInt(fixed32_t f) {
    return f >> 16;
  }
  
  /**
   * @brief Convert float to fixed point
   * @param f Float value
   * @return Fixed point representation
   */
  inline fixed32_t floatToFixed(float f) {
    return static_cast<fixed32_t>(f * FIXED_ONE);
  }
  
  /**
   * @brief Convert fixed point to float
   * @param f Fixed point value
   * @return Float representation
   */
  inline float fixedToFloat(fixed32_t f) {
    return static_cast<float>(f) / FIXED_ONE;
  }
  
  // Basic math operations
  
  /**
   * @brief Multiply two fixed point values
   * @param a First value
   * @param b Second value
   * @return Product in fixed point
   */
  inline fixed32_t multiply(fixed32_t a, fixed32_t b) {
    // Use 64-bit intermediate to prevent overflow
    int64_t temp = static_cast<int64_t>(a) * b;
    return static_cast<fixed32_t>(temp >> 16);
  }
  
  /**
   * @brief Divide two fixed point values
   * @param a Numerator
   * @param b Denominator
   * @return Quotient in fixed point
   */
  inline fixed32_t divide(fixed32_t a, fixed32_t b) {
    // Use 64-bit intermediate to prevent overflow
    int64_t temp = static_cast<int64_t>(a) << 16;
    return static_cast<fixed32_t>(temp / b);
  }
  
  /**
   * @brief Calculate square root in fixed point
   * @param a Input value
   * @return Square root in fixed point
   */
  fixed32_t sqrt(fixed32_t a);
  
  /**
   * @brief Calculate sine in fixed point
   * @param angle Angle in fixed point radians
   * @return Sine value in fixed point
   */
  fixed32_t sin(fixed32_t angle);
  
  /**
   * @brief Calculate cosine in fixed point
   * @param angle Angle in fixed point radians
   * @return Cosine value in fixed point
   */
  fixed32_t cos(fixed32_t angle);
  
  /**
   * @brief Calculate distance between two 3D points
   * @param x1 First point X
   * @param y1 First point Y
   * @param z1 First point Z
   * @param x2 Second point X
   * @param y2 Second point Y
   * @param z2 Second point Z
   * @return Distance in fixed point
   */
  fixed32_t distance3D(fixed32_t x1, fixed32_t y1, fixed32_t z1,
                       fixed32_t x2, fixed32_t y2, fixed32_t z2);
                       
  /**
   * @brief Calculate vector magnitude (length)
   * @param x X component
   * @param y Y component
   * @param z Z component
   * @return Magnitude in fixed point
   */
  fixed32_t magnitude(fixed32_t x, fixed32_t y, fixed32_t z);
  
  /**
   * @brief Linear interpolation between two values
   * @param a Start value
   * @param b End value
   * @param t Interpolation factor (0.0-1.0) in fixed point
   * @return Interpolated value in fixed point
   */
  inline fixed32_t lerp(fixed32_t a, fixed32_t b, fixed32_t t) {
    return a + multiply(b - a, t);
  }
};

#endif // FIXED_MATH_H 