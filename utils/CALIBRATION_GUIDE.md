# Calibration Guide for PrismaTech Gauntlet 3.0

This guide provides simple step-by-step instructions for calibrating the Gauntlet's position detection system, particularly when making adjustments to sensor placement or orientation.

## Prerequisites

- Python 3.6+ with PySerial, Pandas, and NumPy libraries installed
- PrismaTech Gauntlet 3.0 hardware
- USB connection to your computer

## Calibration Workflow

### Step 1: Upload the Calibration Sketch

1. Connect your Gauntlet to your computer via USB
2. Upload the calibration sketch to your device:
   ```
   pio run -t upload -e calibration
   ```
   Or use your IDE to upload the `examples/CalibrationProtocol.cpp` sketch

### Step 2: Prepare for Data Collection

1. Open a terminal and navigate to your project directory
2. Run the calibration data logger:
   ```
   python utils/calibration_logger.py
   ```
3. When prompted, make a note of:
   - Sensor placement (back-of-hand, forearm, etc.)
   - Sensor orientation (face-down, face-up, etc.)
   - Any other relevant hardware details

### Step 3: Perform the Calibration Sequence

1. The gauntlet will start with a 30-second warmup (white pulsing light)
2. Get ready to place the gauntlet in the correct position on your hand
3. The last 5 seconds of warmup will have rapid white flashing
4. For each position (30 seconds each), hold your hand in the indicated position:
   - **Offer (Purple)**: Z-axis dominant positive (palm up)
   - **Calm (Yellow)**: Z-axis dominant negative (palm down)
   - **Oath (Red)**: Y-axis dominant negative (pointing up)
   - **Dig (Green)**: Y-axis dominant positive (pointing down)
   - **Shield (Blue)**: X-axis dominant negative (palm out to side)
   - **Null (Orange)**: X-axis dominant positive (palm in to side)
5. A flashing white light indicates transitions between positions
6. When all positions are complete, the gauntlet will display a white pulsing light

### Step 4: Analyze the Calibration Data

1. After the calibration is complete, stop the data logger (Ctrl+C)
2. Note the filename of the saved data (e.g., `logs/calibration_data_20250401_120000.csv`)
3. Run the analysis script:
   ```
   python utils/analyze_calibration.py logs/calibration_data_YYYYMMDD_HHMMSS.csv
   ```
4. The script will output suggested threshold values and save them to `logs/suggested_thresholds.txt`

### Step 5: Update Your Code

1. Open the generated `logs/suggested_thresholds.txt` file
2. Copy the threshold constants to your position detection code
3. Update the appropriate variables in `src/detection/PositionDetector.cpp` or wherever the thresholds are defined

## Tips for Successful Calibration

1. **Consistent Positioning**: Hold each position as steadily as possible during the 30-second data collection period
2. **Documentation**: Record the exact sensor placement and orientation with each calibration run
3. **Multiple Runs**: Consider performing multiple calibration runs to ensure consistent results
4. **Test After Implementation**: Always test the position detection after applying new thresholds
5. **Save Raw Data**: Keep the raw calibration data for future reference and comparison

## Troubleshooting

- **Serial Port Issues**: If the logger can't connect, check that COM7 is correct in `calibration_logger.py`
- **Invalid Data**: If analysis shows strange values, ensure the sensor is properly connected
- **Poor Detection**: If positions are difficult to detect after calibration, try adjusting the mounting or orientation

## File Organization

- **Calibration Sketches**: `examples/CalibrationProtocol.cpp`
- **Data Collection Tools**: `utils/calibration_logger.py`
- **Analysis Tools**: `utils/analyze_calibration.py`
- **Logs and Results**: `logs/` directory 