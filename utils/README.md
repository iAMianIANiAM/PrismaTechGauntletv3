# Calibration Data Logger

A simple utility to capture calibration data from the PrismaTech Gauntlet via serial port.

## Requirements

- Python 3.6+ 
- PySerial library (for logging)
- Pandas and NumPy (for data analysis)

## Installation

1. Install Python 3 if not already installed
2. Install the required libraries:

```
pip install pyserial pandas numpy
```

## Usage

### Data Collection

1. Connect the PrismaTech Gauntlet to your computer via USB
2. Make sure the device is running the calibration protocol
3. Run the logger:

```
python calibration_logger.py
```

4. The script will:
   - Connect to COM7 (default port - edit the script if different)
   - Log all serial output to a timestamped file in the `logs` directory
   - Display the data in the console as it's being received
   
5. Press Ctrl+C to stop logging when the calibration is complete

### Data Analysis

After collecting the calibration data, analyze it to determine optimal threshold values:

```
python analyze_calibration.py logs/calibration_data_YYYYMMDD_HHMMSS.csv
```

The analysis script will:
1. Read the calibration data
2. Calculate statistics for each hand position
3. Suggest threshold values for each position
4. Generate a C++ compatible thresholds file in the logs directory

## Output Files

- **Raw Data**: `logs/calibration_data_YYYYMMDD_HHMMSS.csv`
- **Threshold Values**: `logs/suggested_thresholds.txt`

## Manual Analysis

You can also manually analyze the data:
1. Open the CSV file in any spreadsheet software (Excel, Google Sheets, etc.)
2. Use the timestamp and position columns to filter data for each position
3. Calculate averages, ranges, and other statistics for each axis in each position

This will help establish appropriate thresholds for the position detection system.

## Troubleshooting

### Encoding Issues
The logger is designed to handle binary data and encoding errors from the serial port. It will:
- Store all raw bytes to the log file without modification
- Display readable text in the console when possible
- Show hex representation for binary/non-text data
- Continue logging even when it encounters invalid characters

### Analysis Issues
The analysis script has robust error handling for:
- Binary format log files
- Corrupted data lines
- Missing or incomplete data
- Invalid numeric values

If the analysis fails, check the error messages for hints about what went wrong. The script will try to provide detailed error information. 