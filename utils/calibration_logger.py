#!/usr/bin/env python3
import serial
import time
import os
from datetime import datetime

def main():
    # Serial port configuration
    COM_PORT = 'COM7'  # Update this if your device uses a different port
    BAUD_RATE = 115200
    
    # Create logs directory if it doesn't exist
    log_dir = 'logs'
    os.makedirs(log_dir, exist_ok=True)
    
    # Generate log filename with timestamp
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    log_file = os.path.join(log_dir, f'calibration_data_{timestamp}.csv')
    
    print(f"Starting calibration data logger")
    print(f"Data will be saved to: {log_file}")
    print(f"Connecting to {COM_PORT} at {BAUD_RATE} baud...")
    
    try:
        # Open serial port
        with serial.Serial(COM_PORT, BAUD_RATE, timeout=1) as ser:
            print(f"Connected to {COM_PORT}")
            print("Waiting for calibration data...")
            print("Press Ctrl+C to stop logging")
            
            # Open log file in binary mode to handle any byte sequence
            with open(log_file, 'wb') as f:
                # Write header for the log file as bytes
                f.write(b"# Calibration Data Log\n")
                f.write(f"# Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n".encode('utf-8'))
                f.write(b"# Raw serial output follows:\n\n")
                
                # Log start time
                start_time = time.time()
                
                # Continue reading until interrupted
                while True:
                    if ser.in_waiting:
                        # Read raw bytes from serial - no decoding yet
                        raw_line = ser.readline()
                        
                        # Store raw bytes to file without decoding
                        f.write(raw_line)
                        f.flush()  # Ensure data is written immediately
                        
                        # Try to print to console with proper error handling
                        try:
                            # Try to decode for display purposes
                            decoded_line = raw_line.decode('utf-8', errors='replace').strip()
                            print(decoded_line)
                        except Exception as e:
                            # If we can't decode it, show hex representation
                            print(f"[Binary data, displaying as hex: {raw_line.hex()}]")
                    
                    # Small delay to reduce CPU usage
                    time.sleep(0.01)
                    
    except KeyboardInterrupt:
        # Handle clean exit on Ctrl+C
        elapsed = time.time() - start_time
        print(f"\nLogging stopped after {elapsed:.1f} seconds")
        print(f"Data saved to: {log_file}")
    except serial.SerialException as e:
        print(f"Error: Could not open port {COM_PORT}: {e}")
        print("Tips:")
        print("1. Make sure the device is connected")
        print("2. Check if the correct COM port is specified")
        print("3. Close any other programs that might be using the port")
    except Exception as e:
        print(f"Error: {e}")
        print(f"Error type: {type(e).__name__}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main() 