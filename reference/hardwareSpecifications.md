# PrismaTech Gauntlet 3.0 - Hardware Specifications

This document details the hardware components and technical specifications for the PrismaTech Gauntlet 3.0. It will be updated as development progresses to include wiring diagrams, pin assignments, power consumption details, and other pertinent hardware information.

## Core Components

### 1. Microcontroller: ESP32-S2 Mini

- **MCU:** ESP32-S2FN4R2 (single-core 32-bit LX7 processor)
- **Clock Speed:** Up to 240 MHz
- **Flash Memory:** 4 MB
- **RAM:** 2 MB PSRAM
- **Connectivity:** 2.4 GHz WiFi (no Bluetooth)
- **USB Connection:** Type-C with native USB support
- **Power Input:** 5V via USB-C or external 5V to VIN pin
- **Operating Voltage:** 3.3V (internal regulator)
- **Size:** Compact form factor

#### Key Features:
- Native USB OTG capabilities for programming and debugging
- Multiple GPIO pins with ADC, touch sensing, and PWM capabilities
- Sleep modes for power conservation
- OTA (Over-The-Air) update capability via WiFi
- Built-in temperature sensor (primarily for calibration)

### 2. Motion Sensor: GY-521 MPU6050

- **Sensor Type:** 6-axis inertial measurement unit (IMU)
- **Components:** 3-axis accelerometer and 3-axis gyroscope
- **Resolution:** 16-bit ADC for each channel
- **Interface:** I2C (I²C)
- **Operating Voltage:** 3.3V-5V (onboard regulator)
- **I2C Address:** 0x68 (default) or 0x69
- **Size:** Approximately 20×15 mm

#### Measurement Ranges:
- **Accelerometer:** Configurable to ±2g, ±4g, ±8g, or ±16g
- **Gyroscope:** Configurable to ±250, ±500, ±1000, or ±2000 degrees/second

#### Features:
- Embedded Digital Motion Processor (DMP) for motion processing
- Internal temperature sensor
- Low power consumption (~3-4mA during operation)

### 3. LED Output: WS2812 12-LED RGB Ring

- **LED Type:** WS2812 individually addressable RGB LEDs
- **Number of LEDs:** 12 in a ring configuration
- **Control Method:** Single data pin digital signal
- **Operating Voltage:** 5V
- **Color Order:** GRB (Green, Red, Blue)
- **Data Protocol:** Serial data at 800kHz

#### Power Consumption:
- **Full White (RGB=255):** ~60mA per LED (up to ~720mA total at max brightness)
- **Single Color (e.g., R=255):** ~20mA per LED
- **Total max consumption:** Calculate based on brightness and pattern usage

## Pin Assignments

*To be updated with specific pin connections during implementation.*

| Component | Pin on Component | Connected to ESP32-S2 Pin | Notes |
|-----------|------------------|---------------------------|-------|
| MPU6050 | VCC | 3.3V | |
| MPU6050 | GND | GND | |
| MPU6050 | SDA | GPIO? (TBD) | I2C Data Line |
| MPU6050 | SCL | GPIO? (TBD) | I2C Clock Line |
| MPU6050 | INT | GPIO? (TBD) | Optional interrupt pin |
| WS2812 | VDD | 5V | May require external power for full brightness |
| WS2812 | GND | GND | |
| WS2812 | DIN | GPIO? (TBD) | Data input pin |

## Power Requirements

- **Input Voltage:** 5V via USB-C connector
- **Estimated Current Draw:**
  - ESP32-S2: ~80-240mA with WiFi active
  - MPU6050: ~3-4mA
  - WS2812 LEDs: Variable depending on pattern and brightness (max ~720mA at full white)
  - **Total Estimated:** 300-800mA typical, up to ~1A peak

*Note: These are preliminary estimates. Actual measurements will be added during development.*

## Physical Layout

*To be updated with photos and diagrams of the physical construction.*

## Wiring Diagram

*A detailed wiring diagram will be added after specific pin assignments are determined.*

## Development Libraries

- **ESP32:** Arduino ESP32 core or PlatformIO ESP32 framework
- **MPU6050:** I2C communication using Wire library or specialized MPU6050 libraries
- **WS2812:** FastLED library for LED control and animation
- **OTA Updates:** ESP32 OTA library

## Notes and Considerations

- The ESP32-S2 has sufficient I/O for both the I2C sensor and LED data connection
- Power management will be important, especially during high-brightness LED patterns
- Consider adding a capacitor (100-1000μF) between 5V and GND near the LED ring to manage current spikes
- The system should implement calibration for the MPU6050 to ensure accurate position detection 