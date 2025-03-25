# PrismaTech Gauntlet 3.0 - Hardware Specifications

This document details the hardware components and technical specifications for the PrismaTech Gauntlet 3.0. It will be updated as development progresses to include wiring diagrams, power consumption details, and other pertinent hardware information.

## Core Components

### 1. Microcontroller: ESP32-WROOM Development Board

- **MCU:** ESP32 (Dual-Core Tensilica LX6 processor)
- **Clock Speed:** Up to 240 MHz
- **Flash Memory:** 4 MB
- **RAM:** Approximately 520 KB SRAM
- **Connectivity:** 2.4 GHz WiFi and Bluetooth (both Classic and BLE)
- **USB Connection:** USB connector with CP2102 USB-to-serial chip for programming
- **Power Input:** 5V via USB or external 5V to VIN pin
- **Operating Voltage:** 3.3V (internal regulator)
- **Size:** Standard ESP32 development board form factor

#### Key Features:
- Dual-core processor for efficient multitasking (motion processing and LED control)
- Built-in WiFi for OTA updates and connectivity
- Bluetooth capabilities for possible future enhancements
- Multiple GPIO pins with support for digital I/O, ADC, and communication protocols
- I2C support for sensor communication
- Sufficient processing power for complex animations and gesture recognition

### 2. Motion Sensor: MPU9250/6500/9255 (GY-521 Breakout)

- **Sensor Type:** 9-axis inertial measurement unit (IMU)
- **Components:** 
  - 3-axis accelerometer
  - 3-axis gyroscope
  - 3-axis magnetometer (AK8963)
- **Resolution:** 16-bit ADC for each channel
- **Interface:** I2C (I²C) with standard 0x68 address
- **Operating Voltage:** 3.3V-5V (onboard regulator)
- **Size:** Standard GY-521 breakout board dimensions

#### Measurement Ranges:
- **Accelerometer:** Configurable to ±2g, ±4g, ±8g, or ±16g
- **Gyroscope:** Configurable to ±250, ±500, ±1000, or ±2000 degrees/second
- **Magnetometer:** ±4800μT

#### Features:
- Digital Motion Processor (DMP) for on-chip motion processing
- 9-axis sensor fusion capability
- Internal temperature sensor
- Low power consumption
- Compatible with existing MPU6050 libraries with minor modifications

### 3. LED Output: WS2812 12-LED RGB Ring

- **LED Type:** WS2812 individually addressable RGB LEDs
- **Number of LEDs:** 12 in a ring configuration
- **Control Method:** Single data pin digital signal
- **Operating Voltage:** 5V
- **Color Order:** GRB (Green, Red, Blue)
- **Data Protocol:** Serial data at 800kHz
- **Library:** FastLED

#### Power Consumption:
- **Full White (RGB=255):** ~60mA per LED (up to ~720mA total at max brightness)
- **Single Color (e.g., R=255):** ~20mA per LED
- **Total max consumption:** Calculate based on brightness and pattern usage

## Pin Assignments and Configuration

### LED Configuration
- **LED_PIN:** GPIO12 - Data pin for controlling the LED strip
- **NUM_LEDS:** 12 - Total number of LEDs in the strip
- **COLOR_ORDER:** GRB - The color order used by the LED strip
- **LED_BRIGHTNESS:** 100 - The brightness level for the LEDs (out of 255)

### I2C Configuration
- **SDA_PIN:** GPIO21 - Data line for I2C communication
- **SCL_PIN:** GPIO22 - Clock line for I2C communication 
- **I2C_CLOCK:** 100000 (100kHz) - Clock speed for I2C communication
- **MPU_ADDR:** 0x68 - The I2C address of the MPU sensor

### Connection Table

| Component | Pin on Component | Connected to ESP32 Pin | Notes |
|-----------|------------------|--------------------------|-------|
| MPU9250 | VCC | 3.3V | |
| MPU9250 | GND | GND | |
| MPU9250 | SDA | GPIO21 | I2C Data Line (100kHz) |
| MPU9250 | SCL | GPIO22 | I2C Clock Line (100kHz) |
| MPU9250 | INT | GPIO19 | Optional interrupt pin |
| WS2812 | VDD | 5V | From USB power |
| WS2812 | GND | GND | |
| WS2812 | DIN | GPIO12 | Data input pin (not GPIO23 as originally thought) |

## Power Requirements

- **Input Voltage:** 5V via USB connector
- **Estimated Current Draw:**
  - ESP32-WROOM: ~80-240mA during WiFi operation
  - MPU9250: ~3-4mA
  - WS2812 LEDs: Variable depending on pattern and brightness (max ~720mA at full white with brightness=255)
  - **Total Estimated:** 300-800mA typical, up to ~1A peak
- **LED Brightness Management:** Default brightness level set to 100/255 (~39%) to conserve power

*Note: These are preliminary estimates. Actual measurements will be added during development.*

## Physical Layout

*The recovered prototype already has a physical layout. Details and photos will be added during documentation.*

## Software Configuration

### Connectivity
- **WiFi:** Enabled for OTA updates and telnet debugging
- **WiFi Credentials:** Stored in a separate Credentials.h file (not in main source code)

### Libraries
- **ESP32:** Arduino ESP32 core or PlatformIO ESP32 framework
- **MPU9250:** I2C communication using Wire library and specialized MPU libraries
- **WS2812:** FastLED library for LED control and animation
- **OTA Updates:** ESP32 OTA library for wireless updates

### Gesture Detection
- The device specifically detects palm-down and palm-up gestures (CalmOffer) using accelerometer data
- The LED strip displays different visual effects and indicators based on detected gestures
- Rainbow Burst effect is triggered by the palm-down-up motion

## Notes and Considerations

- The ESP32-WROOM offers dual-core processing which helps with simultaneous intensive tasks
- The I2C bus is configured for a standard 100kHz clock, which is sufficient for motion sensing
- The WS2812 data pin is GPIO12, not GPIO23 as originally documented
- LED brightness is maintained at a moderate 100/255 for power efficiency
- WiFi connectivity enables remote debugging and updates without physical access
- The magnetometer in the MPU9250 is not currently utilized but may enable future enhancements
- The system implements gesture detection focused primarily on palm orientation changes 