#!/usr/bin/env python3
import os
import sys
import csv
import pandas as pd
import numpy as np
from pathlib import Path

def analyze_calibration_data(file_path):
    """Analyze calibration data and suggest threshold values"""
    print(f"Analyzing calibration data from: {file_path}")
    
    # Read the CSV data, skipping comment lines
    data = []
    try:
        # First try reading as text file (backwards compatibility)
        with open(file_path, 'r', errors='replace') as f:
            for line in f:
                if not line.startswith('#') and ',' in line:
                    data.append(line.strip())
    except UnicodeDecodeError:
        # If that fails, try reading as binary file
        print("Detected binary format file - processing bytes directly...")
        with open(file_path, 'rb') as f:
            for line_bytes in f:
                try:
                    # Try to decode the line
                    line = line_bytes.decode('utf-8', errors='replace').strip()
                    if not line.startswith('#') and ',' in line:
                        data.append(line)
                except Exception as e:
                    # Skip lines that can't be processed
                    print(f"Skipping invalid line: {e}")
                    continue
    
    if not data:
        print("No valid calibration data found in the file.")
        return False
        
    print(f"Successfully loaded {len(data)} data points")
    
    # Parse CSV data
    csv_data = []
    for row in data:
        try:
            values = row.split(',')
            if len(values) >= 8:  # Ensure row has all expected columns
                csv_data.append(values)
        except Exception as e:
            print(f"Error parsing row: {e}")
            continue
    
    if not csv_data:
        print("Failed to parse any valid data rows")
        return False
        
    # Convert to DataFrame for easier analysis
    try:
        df = pd.DataFrame(csv_data, columns=['timestamp', 'position', 
                                           'accelX', 'accelY', 'accelZ', 
                                           'gyroX', 'gyroY', 'gyroZ'])
        
        # Convert columns to numeric
        numeric_cols = ['position', 'accelX', 'accelY', 'accelZ', 'gyroX', 'gyroY', 'gyroZ']
        for col in numeric_cols:
            df[col] = pd.to_numeric(df[col], errors='coerce')
        
        # Filter out any rows with NaN values (from header or invalid data)
        original_len = len(df)
        df = df.dropna()
        if len(df) < original_len:
            print(f"Filtered out {original_len - len(df)} rows with invalid data")
        
        if df.empty:
            print("No valid data points remain after filtering")
            return False
        
        # Position names mapping
        position_names = {
            0: "OFFER",
            1: "CALM",
            2: "OATH",
            3: "DIG",
            4: "SHIELD",
            5: "NULL",
            99: "UNKNOWN"
        }
        
        # Convert position numbers to names for display
        df['position_name'] = df['position'].map(lambda x: position_names.get(int(x), f"Unknown ({int(x)})"))
        
        # Print raw data with positions for debugging
        print("\n=== Raw Position Data (First few rows per position) ===\n")
        for pos_num, pos_name in position_names.items():
            if pos_num == 99:  # Skip unknown
                continue
                
            pos_data = df[df['position'] == pos_num].head(3)
            if not pos_data.empty:
                print(f"\nPosition {pos_num} ({pos_name}):")
                for _, row in pos_data.iterrows():
                    print(f"  X:{row['accelX']:8.1f}, Y:{row['accelY']:8.1f}, Z:{row['accelZ']:8.1f}")
        
        # Group by position
        grouped = df.groupby('position_name')
        
        print("\n=== Calibration Data Summary ===\n")
        print(f"Total samples: {len(df)}")
        print(f"Positions detected: {len(grouped)}\n")
        
        # Calculate statistics for each position
        accel_cols = ['accelX', 'accelY', 'accelZ']
        
        # Create results table
        print("Position Statistics (Accelerometer Readings):")
        print("-" * 100)
        print(f"{'Position':<10} | {'Axis':<6} | {'Mean':>10} | {'Min':>10} | {'Max':>10} | {'Std Dev':>10} | {'Samples':>7}")
        print("-" * 100)
        
        # Store dominant axis values for threshold calculation
        dominant_values = {}
        
        for name, group in grouped:
            # Determine sample count for this position
            sample_count = len(group)
            if sample_count < 5:
                print(f"Warning: Position {name} has only {sample_count} samples (may be unreliable)")
                
            dominant_values[name] = {}
            
            for axis in accel_cols:
                # Basic statistics
                mean = group[axis].mean()
                min_val = group[axis].min()
                max_val = group[axis].max()
                std = group[axis].std()
                
                # Save for threshold calculation
                dominant_values[name][axis] = {
                    'mean': mean,
                    'min': min_val,
                    'max': max_val,
                    'std': std
                }
                
                # Print formatted stats
                print(f"{name:<10} | {axis:<6} | {mean:10.2f} | {min_val:10.2f} | {max_val:10.2f} | {std:10.2f} | {sample_count:7d}")
        
        print("-" * 100)
        
        # Calculate suggested thresholds
        print("\n=== Suggested Position Detection Thresholds ===\n")
        
        position_thresholds = {}
        
        # Expected dominant axis for each position based on design
        dominant_axis_map = {
            "OFFER": "accelZ",  # Z-axis dominant positive
            "CALM": "accelZ",   # Z-axis dominant negative
            "OATH": "accelY",   # Y-axis dominant negative
            "DIG": "accelY",    # Y-axis dominant positive
            "SHIELD": "accelX", # X-axis dominant negative
            "NULL": "accelX"    # X-axis dominant positive
        }
        
        for position, dom_axis in dominant_axis_map.items():
            if position in dominant_values:
                # Get statistics for the dominant axis
                axis_stats = dominant_values[position][dom_axis]
                
                # Simple threshold at 80% between mean and extreme value
                # Adjust the sign based on what direction we expect
                if position in ["CALM", "OATH", "SHIELD"]:
                    # Negative direction
                    threshold = axis_stats['mean'] - 0.8 * abs(axis_stats['mean'] - axis_stats['min'])
                    comparator = "<"
                else:
                    # Positive direction
                    threshold = axis_stats['mean'] + 0.8 * abs(axis_stats['max'] - axis_stats['mean'])
                    comparator = ">"
                
                position_thresholds[position] = {
                    'axis': dom_axis,
                    'threshold': threshold,
                    'comparator': comparator
                }
                
                print(f"{position}: {dom_axis} {comparator} {threshold:.2f}")
        
        print("\nThese are suggested starting thresholds based on calibration data.")
        print("You may need to adjust them for optimal detection.")
        
        # Save thresholds to a file
        output_file = Path(file_path).parent / "suggested_thresholds.txt"
        with open(output_file, 'w') as f:
            f.write("// Suggested thresholds from calibration data analysis\n")
            f.write(f"// Generated from {Path(file_path).name}\n\n")
            
            for position, data in position_thresholds.items():
                f.write(f"// {position}: {data['axis']} {data['comparator']} {data['threshold']:.2f}\n")
                
                # Convert to C++ variable name format
                var_name = f"THRESHOLD_{position}"
                f.write(f"const float {var_name} = {data['threshold']:.2f}f;\n\n")
        
        print(f"\nThresholds saved to: {output_file}")
        
    except Exception as e:
        print(f"Error analyzing data: {e}")
        import traceback
        traceback.print_exc()
        return False
        
    return True

def main():
    if len(sys.argv) != 2:
        print("Usage: python analyze_calibration.py <calibration_data_file.csv>")
        print("Example: python analyze_calibration.py logs/calibration_data_20230501_120000.csv")
        return False
    
    file_path = sys.argv[1]
    if not os.path.exists(file_path):
        print(f"Error: File not found: {file_path}")
        return False
        
    analyze_calibration_data(file_path)
    return True

if __name__ == "__main__":
    main() 