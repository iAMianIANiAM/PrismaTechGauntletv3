# PrismaTech Gauntlet 3.0 - Hardware Analysis

This document assesses the chosen hardware components against the functionality outlined in the TrueFunctionGuide.md document, determining their suitability for implementing the described features.

## Hardware Assessment

### Core Requirements from TrueFunctionGuide

The PrismaTech Gauntlet requires hardware capable of:
1. Detecting hand orientation and movement through an IMU sensor
2. Providing visual feedback via an LED ring
3. Processing input for four distinct operational modes (Idle, Invocation, Resolution, and Freecast)
4. Recognizing specific hand positions and gestures
5. Running animations and visual effects on the LED ring
6. Operating in a wearable, standalone form factor

### Component Suitability Analysis

#### ESP32-S2 Mini Microcontroller

**Advantages:**
- Processing power (240 MHz single-core) is sufficient for the required motion processing and LED control
- Wi-Fi capability enables OTA updates as specified in the development roadmap
- Native USB support simplifies programming and debugging
- 4MB Flash and 2MB PSRAM provide ample memory for complex animations and gesture recognition algorithms
- Multiple GPIO pins available for sensor interfacing and LED control
- I2C support for MPU6050 communication
- Low power modes available for battery efficiency when implemented

**Potential Limitations:**
- Single-core processor may face challenges with simultaneous intensive tasks (e.g., motion processing while rendering complex LED animations)
- Power management will be critical when driving the LED ring at high brightness

**Verdict:** The ESP32-S2 Mini is SUITABLE for this application. Its processing power, memory, and connectivity features provide a solid foundation for implementing all required modes and functionality.

#### MPU6050 6-Axis IMU

**Advantages:**
- Combined 3-axis accelerometer and 3-axis gyroscope provides all motion data needed for position detection
- 16-bit resolution enables precise detection of hand orientation and movements
- DMP (Digital Motion Processor) can offload some sensor fusion tasks from the ESP32
- I2C interface simplifies connection to the ESP32
- Low power consumption (~3-4mA) is reasonable for a wearable device
- Configurable sensitivity ranges accommodate various motion intensities

**Alignment with Requirements:**
- The TrueFunctionGuide specifies a "Dominant Axis Detection Model" for hand positions, which is well-suited to the MPU6050's capabilities
- The sensor can easily detect the six defined hand positions (Offer, Calm, Oath, Dig, Shield, and Null)
- The combined accelerometer and gyroscope data provides sufficient input for both position detection and the more complex motion analysis required for Freecast Mode

**Verdict:** The MPU6050 is SUITABLE for detecting all required hand positions and gestures. The accelerometer can reliably determine orientation for the static positions, while the additional gyroscope data enables the motion tracking needed for Freecast Mode.

#### WS2812 12-LED RGB Ring

**Advantages:**
- Individually addressable RGB LEDs allow for complex patterns and animations
- 12 LEDs provide sufficient resolution for the described visual effects
- Single-pin control simplifies wiring
- Compatible with the FastLED library for efficient animation implementation

**Alignment with Requirements:**
- The TrueFunctionGuide describes various LED patterns using specific segments of the ring (e.g., four cardinal LEDs for Idle Mode)
- 12 LEDs allow for three 4-LED segments as described for the Invocation slots
- The specified animations (fire patterns, scanning beams, rainbow effects) are all implementable with this LED ring

**Power Considerations:**
- Full white brightness for all 12 LEDs (~720mA) plus the ESP32 (~200mA) could approach 1A
- USB-C can typically provide 500mA-3A depending on the power source
- Brightness management will be necessary to avoid power issues

**Verdict:** The WS2812 12-LED RGB Ring is SUITABLE for implementing all the visual effects described. Power management through software (limiting maximum brightness) will be necessary to ensure stability when powered via USB-C.

### Overall Hardware Compatibility

The selected hardware components are WELL-SUITED to implement the functionality described in the TrueFunctionGuide. The ESP32-S2 Mini provides sufficient processing power and memory, the MPU6050 offers appropriate motion sensing capabilities, and the WS2812 LED ring enables all the required visual feedback.

## Build Guide

### Component List

- 1 × ESP32-S2 Mini development board
- 1 × GY-521 MPU6050 breakout board
- 1 × WS2812 12-LED RGB ring
- Hookup wire (preferably in multiple colors)
- 1 × 470Ω resistor (for LED data line)
- 1 × 100-1000μF capacitor (for power stabilization)
- USB-C cable (for power/programming)
- Optional: Small breadboard or perfboard for prototyping

### Wiring Instructions

#### Pin Assignments

| Component | Pin on Component | Connected to ESP32-S2 Pin | Notes |
|-----------|------------------|---------------------------|-------|
| MPU6050 | VCC | 3.3V | |
| MPU6050 | GND | GND | |
| MPU6050 | SDA | GPIO7 | I2C Data Line |
| MPU6050 | SCL | GPIO8 | I2C Clock Line |
| MPU6050 | INT | GPIO5 | For motion interrupts (optional) |
| WS2812 | VDD | 5V | From USB power |
| WS2812 | GND | GND | |
| WS2812 | DIN | GPIO6 | 470Ω resistor in series |

#### Wiring Diagram Notes

1. **Power Management:**
   - The ESP32-S2 Mini will be powered directly via its USB-C port
   - The 5V pin on the ESP32 (connected to USB power) will power the WS2812 LED ring
   - The MPU6050 will use the regulated 3.3V from the ESP32

2. **Data Connections:**
   - Place a 470Ω resistor between GPIO6 and the WS2812 DIN pin to protect the LED ring
   - Connect a 100-1000μF capacitor between 5V and GND near the LED ring to stabilize power
   - Keep the data wire to the LED ring as short as possible to maintain signal integrity

3. **I2C Configuration:**
   - The I2C pins (GPIO7 and GPIO8) need pull-up resistors to 3.3V
   - The MPU6050 board typically has built-in pull-ups, so additional resistors may not be needed
   - If communication issues arise, add 4.7kΩ pull-up resistors to SDA and SCL lines

### Physical Assembly Considerations

1. **Placement:**
   - Mount the MPU6050 sensor on the back of the hand or top of the wrist where it can accurately detect hand movements
   - Position the LED ring on the top/front of the hand for optimal visibility
   - Place the ESP32-S2 Mini in a location where it's protected but accessible for USB connection

2. **Orientation:**
   - Mount the MPU6050 with its axes aligned with the hand:
     - X-axis pointing across the width of the hand (thumb to pinky)
     - Y-axis pointing along the length of the hand (wrist to fingertips)
     - Z-axis pointing perpendicular to the palm
   - This alignment simplifies the implementation of the Dominant Axis Detection Model

3. **Wiring Protection:**
   - Use flexible, stranded wire for connections to allow for hand movement
   - Secure all wires to prevent tension on solder joints during movement
   - Consider cable sleeving or braiding for durability and aesthetics

4. **Power Considerations:**
   - The USB-C connection will be the primary power source
   - Depending on the final installation, routing the USB cable along the arm may be necessary
   - Consider using a USB power bank for complete mobility

### Testing and Verification

After assembly, perform these basic tests to verify the hardware:

1. **Power Test:**
   - Connect USB power and verify the ESP32 powers on
   - Check for any excessive heating or other anomalies

2. **LED Ring Test:**
   - Upload a simple WS2812 test sketch to verify all LEDs function properly
   - Test with different brightness levels to assess power stability

3. **MPU6050 Test:**
   - Upload a basic MPU6050 reading sketch to verify communication
   - Test basic orientation changes to ensure the sensor responds correctly

4. **Integration Test:**
   - Implement a simple version of Idle Mode to verify the system works as a whole
   - Confirm that sensor readings translate to appropriate LED responses

### Power Management Implementation

1. **Software Brightness Limiting:**
   - Implement a maximum brightness limit of around 70% as a default
   - Consider implementing dynamic brightness based on activity level

2. **Power Monitoring:**
   - Add code to monitor power stability and automatically reduce brightness if needed
   - Include undervoltage detection to prevent erratic behavior

3. **Efficient Animation:**
   - Design LED animations to minimize the number of LEDs at full brightness simultaneously
   - Utilize patterns that alternate LEDs for visual effects while reducing power draw

This build approach ensures that the hardware will be capable of implementing all the functionality described in the TrueFunctionGuide while maintaining stability and reliability when powered via USB-C. 