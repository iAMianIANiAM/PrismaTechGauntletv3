 # FreeCast Mode: Motion-to-Pattern Mapping

This chart explains how different hand movements with the gauntlet translate to specific visual patterns in FreeCast Mode.

## Pattern Types and Corresponding Movements

| Pattern Type    | Main Movement Type       | Visual Effect                         | Movement Intensity Effect            | Directionality Effect                      |
|-----------------|--------------------------|---------------------------------------|-------------------------------------|-------------------------------------------|
| **Shooting Stars** | Forward-Backward Motion (Z-axis) | Bright points moving along ring with fading tails | • More stars (1-4)<br>• Longer tails (3-9 LEDs)<br>• Faster movement<br>• Brighter colors | N/A - Pattern selected by axis dominance |
| **Waves**          | Up-Down Motion (Y-axis) | Smooth, undulating rainbow waves flowing around ring | • Faster wave speed<br>• Brighter colors | • More wave cycles around the ring (1-4 waves) |
| **Color Trails**   | Side-to-Side Motion (X-axis) | Gradient color trails that rotate around the ring | • Faster rotation speed<br>• Brighter, more vivid colors | • More saturated colors with higher directionality |
| **Sparkles**       | Shaking/Random Motion | Random sparkle points appearing around the ring | • More sparkle points (3-12)<br>• Brighter colors<br>• Faster changes | • Low directionality triggers this pattern |
| **Pulses**         | Sharp Direction Changes | Expanding rings of light | • Brighter, more intense pulses<br>• More pulses (1-3) | • Medium directionality triggers this pattern |

## How Motion Parameters Affect Visuals

### 1. Motion Intensity (How energetic/fast the movement is)
- **Calculation**: Square root of maximum acceleration magnitude over the recording period
- **Scaled**: Motion intensity is more responsive to smaller movements (using power curve)
- **Visual Effects**:
  - Higher brightness in all patterns (0.5-1.0 scaling)
  - Faster animation speeds
  - More visual elements (more stars, sparkles, pulses, etc.)
  - More saturated colors

### 2. Motion Directionality (How consistent the movement direction is)
- **Calculation**: Inverse of motion variance - low variance means consistent direction
- **Visual Effects**:
  - Controls wave frequency in the Waves pattern
  - Affects color saturation in some patterns
  - Helps determine which pattern is selected

### 3. Dominant Axis (Which direction has the most movement)
- **Calculation**: Axis with highest average acceleration magnitude
- **X-axis dominant** → Color Trails pattern
- **Y-axis dominant** → Waves pattern
- **Z-axis dominant** → Shooting Stars pattern

## Movement Examples

1. **Gentle Side-to-Side Sweep** (X-axis)
   - Pattern: Color Trails
   - Visual: Smooth color gradient rotating around ring
   - Speed based on how fast you sweep

2. **Up and Down Motion** (Y-axis)
   - Pattern: Waves
   - Visual: Rainbow waves flowing around the ring
   - More distinct waves with controlled movement

3. **Forward and Backward Thrust** (Z-axis)
   - Pattern: Shooting Stars
   - Visual: Bright points with tails moving around ring
   - Faster thrusts = more and brighter stars

4. **Shaking/Vibrating**
   - Pattern: Sparkles
   - Visual: Random bright points appearing around ring
   - More vigorous shaking = more sparkle points

5. **Quick Directional Changes**
   - Pattern: Pulses
   - Visual: Expanding rings of light
   - Sharper changes = more visible pulses

## Tips for Best Effects

- For more vibrant patterns, use more energetic movements
- For more consistent/predictable patterns, focus on single-axis movements
- Experiment with different speeds and intensities within the same motion type
- The smoothness of your movement affects directionality and pattern selection 