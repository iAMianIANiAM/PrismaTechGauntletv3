# Position Detection Threshold Values

## UBPD Calibrated Thresholds

These are the current calibrated thresholds from the Ultra Basic Position Detector implementation. These values are in physical units (m/s²) and are used directly in the UBPD implementation.

| Position | Axis    | Threshold Value | Direction  |
|----------|---------|----------------|------------|
| OFFER    | Z-axis  | 7.0 m/s²       | Positive   |
| CALM     | Z-axis  | -7.0 m/s²      | Negative   |
| OATH     | Y-axis  | 7.0 m/s²       | Positive   |
| DIG      | Y-axis  | -7.0 m/s²      | Negative   |
| SHIELD   | X-axis  | -7.0 m/s²      | Negative   |
| NULL     | X-axis  | 7.0 m/s²       | Positive   |

## Axis Mapping

Here is the mapping between accelerometer axes and arm/hand positions:

- **X-axis:** Points along the length of the arm
  - Positive: Toward elbow (NULL position)
  - Negative: Toward hand (SHIELD position)
  
- **Y-axis:** Points across the width of the arm
  - Positive: Outward from arm (DIG position)
  - Negative: Inward toward body (OATH position)
  
- **Z-axis:** Points perpendicular to arm surface
  - Positive: Away from arm surface (OFFER position)
  - Negative: Into arm surface (CALM position)

## Calibration Process

The threshold values were determined through the following process:

1. For each position, the user held the arm/hand in the correct position
2. 50 samples were collected and averaged
3. The final threshold was set to 80% of the measured average (THRESHOLD_SCALE = 0.8f)
4. This provides a margin to prevent false positives

## Implementation Details

- These thresholds are used with a dominant axis detection model
- The UBPD uses a 3-sample moving average to reduce noise
- The sensor data is converted from raw values to physical units (m/s²) using:
  ```cpp
  float SCALING_FACTOR = 4.0f * 9.81f / 32768.0f; // For ±4g range to m/s²
  ```

## Verification Date

These thresholds were last verified on: [DATE] 