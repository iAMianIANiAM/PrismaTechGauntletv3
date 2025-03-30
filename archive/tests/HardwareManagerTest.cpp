/**
 * @file HardwareManagerTest.cpp
 * @brief Test application for HardwareManager
 *
 * This example demonstrates the core functionality of the HardwareManager,
 * including hardware initialization, sensor data reading, LED control,
 * and power management.
 */

#include <Arduino.h>
#include "../src/hardware/HardwareManager.h"
#include "../src/core/Config.h"
#include "../src/utils/DebugTools.h"

// Hardware Manager instance
HardwareManager* hardwareManager = nullptr;

// Timer variables for non-blocking operation
unsigned long lastActionTime = 0;
unsigned long lastDisplayTime = 0;

// Test state variables
int testPhase = 0;
int testStep = 0;
unsigned long phaseStartTime = 0;

// Test phases
enum TestPhase {
  PHASE_STARTUP,
  PHASE_SENSOR_TEST,
  PHASE_LED_TEST,
  PHASE_ERROR_RECOVERY,
  PHASE_POWER_MANAGEMENT,
  PHASE_COMPLETE
};

// Function prototypes
void runStartupPhase();
void runSensorTestPhase();
void runLEDTestPhase();
void runErrorRecoveryPhase();
void runPowerManagementPhase();
void displaySensorData(const SensorData& data);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);  // Give time for serial connection to establish
  
  Serial.println("\n=================================================");
  Serial.println("PrismaTech Gauntlet 3.0 - Hardware Manager Test");
  Serial.println("=================================================");
  
  // Initialize debug tools
  DEBUG_INIT(115200);
  
  // Get Hardware Manager instance
  hardwareManager = HardwareManager::getInstance();
  
  // Initialize Hardware Manager
  Serial.println("\nInitializing Hardware Manager...");
  if (!hardwareManager->init()) {
    Serial.println("ERROR: Hardware Manager initialization failed!");
    while (1) { delay(1000); }  // Halt execution
  }
  
  Serial.println("Hardware Manager initialized successfully.");
  Serial.println("\nBeginning test sequence...");
  
  // Start with initial phase
  testPhase = PHASE_STARTUP;
  testStep = 0;
  phaseStartTime = millis();
}

void loop() {
  // Update the Hardware Manager (read sensors, etc.)
  hardwareManager->update();
  
  // Run the current test phase
  switch (testPhase) {
    case PHASE_STARTUP:
      runStartupPhase();
      break;
    case PHASE_SENSOR_TEST:
      runSensorTestPhase();
      break;
    case PHASE_LED_TEST:
      runLEDTestPhase();
      break;
    case PHASE_ERROR_RECOVERY:
      runErrorRecoveryPhase();
      break;
    case PHASE_POWER_MANAGEMENT:
      runPowerManagementPhase();
      break;
    case PHASE_COMPLETE:
      // Test complete - just display sensor data
      if (millis() - lastDisplayTime >= 500) {
        lastDisplayTime = millis();
        
        // Get the latest sensor data
        SensorData data = hardwareManager->getSensorData();
        displaySensorData(data);
      }
      break;
  }
}

/**
 * @brief Initial startup test phase
 */
void runStartupPhase() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - phaseStartTime;
  
  if (elapsedTime > 3000) {
    // Move to next phase after 3 seconds
    Serial.println("\n--- Phase 1: Startup Complete ---");
    Serial.println("Starting Sensor Test Phase...");
    
    testPhase = PHASE_SENSOR_TEST;
    testStep = 0;
    phaseStartTime = currentTime;
  } else if (currentTime - lastDisplayTime >= 500) {
    lastDisplayTime = currentTime;
    
    // Display countdown
    Serial.printf("Starting sensor test in %d seconds...\n", 
                  3 - (elapsedTime / 1000));
  }
}

/**
 * @brief Sensor test phase
 */
void runSensorTestPhase() {
  unsigned long currentTime = millis();
  
  // Display sensor data periodically
  if (currentTime - lastDisplayTime >= 200) {
    lastDisplayTime = currentTime;
    
    // Get the latest sensor data
    SensorData data = hardwareManager->getSensorData();
    displaySensorData(data);
  }
  
  // Move to next phase after 5 seconds
  if (currentTime - phaseStartTime >= 5000) {
    Serial.println("\n--- Phase 2: Sensor Test Complete ---");
    Serial.println("Starting LED Test Phase...");
    
    testPhase = PHASE_LED_TEST;
    testStep = 0;
    phaseStartTime = currentTime;
  }
}

/**
 * @brief LED test phase
 */
void runLEDTestPhase() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - phaseStartTime;
  
  // Run through a sequence of LED tests
  switch (testStep) {
    case 0:  // Individual LED test
      if (elapsedTime < 3000) {
        // Light each LED in sequence
        int ledIndex = (elapsedTime / 250) % Config::NUM_LEDS;
        
        // Clear all LEDs
        hardwareManager->setAllLEDs({0, 0, 0});
        
        // Set the current LED to red
        hardwareManager->setLED(ledIndex, {255, 0, 0});
        hardwareManager->updateLEDs();
        
        if (currentTime - lastDisplayTime >= 250) {
          lastDisplayTime = currentTime;
          Serial.printf("Testing LED %d\n", ledIndex);
        }
      } else {
        testStep++;
        Serial.println("Individual LED test complete.");
      }
      break;
      
    case 1:  // Color test
      if (elapsedTime < 6000) {
        if (currentTime - lastActionTime >= 1000) {
          lastActionTime = currentTime;
          
          // Cycle through colors: Red, Green, Blue
          Color colors[] = {
            {255, 0, 0},    // Red
            {0, 255, 0},    // Green
            {0, 0, 255}     // Blue
          };
          
          int colorIndex = ((elapsedTime - 3000) / 1000) % 3;
          
          hardwareManager->setAllLEDs(colors[colorIndex]);
          hardwareManager->updateLEDs();
          
          Serial.printf("Testing color: %s\n", 
                      colorIndex == 0 ? "Red" : 
                      colorIndex == 1 ? "Green" : "Blue");
        }
      } else {
        testStep++;
        Serial.println("Color test complete.");
      }
      break;
      
    case 2:  // Brightness test
      if (elapsedTime < 9000) {
        // Modulate brightness from 20 to 255
        unsigned long cycleTime = (elapsedTime - 6000) % 1000;
        uint8_t brightness;
        
        if (cycleTime < 500) {
          // Ascending brightness
          brightness = 20 + (cycleTime * 235 / 500);
        } else {
          // Descending brightness
          brightness = 255 - ((cycleTime - 500) * 235 / 500);
        }
        
        hardwareManager->setAllLEDs({255, 255, 255});  // White
        hardwareManager->setBrightness(brightness);
        hardwareManager->updateLEDs();
        
        if (currentTime - lastDisplayTime >= 100) {
          lastDisplayTime = currentTime;
          Serial.printf("Testing brightness: %d\n", brightness);
        }
      } else {
        // Complete the phase
        Serial.println("Brightness test complete.");
        Serial.println("\n--- Phase 3: LED Test Complete ---");
        Serial.println("Starting Error Recovery Test Phase...");
        
        testPhase = PHASE_ERROR_RECOVERY;
        testStep = 0;
        phaseStartTime = currentTime;
      }
      break;
  }
}

/**
 * @brief Error recovery test phase
 */
void runErrorRecoveryPhase() {
  unsigned long currentTime = millis();
  
  switch (testStep) {
    case 0:  // Setup
      Serial.println("Testing error recovery capabilities...");
      Serial.println("1. Testing component reset functionality");
      
      // Test component reset (MPU)
      if (hardwareManager->resetComponent(HW_COMPONENT_MPU)) {
        Serial.println("MPU reset successful!");
      } else {
        Serial.println("MPU reset failed!");
      }
      
      // Test component reset (LED)
      if (hardwareManager->resetComponent(HW_COMPONENT_LED)) {
        Serial.println("LED reset successful!");
      } else {
        Serial.println("LED reset failed!");
      }
      
      testStep++;
      lastActionTime = currentTime;
      break;
      
    case 1:  // Self-test
      if (currentTime - lastActionTime >= 1000) {
        Serial.println("\n2. Running self-tests on components");
        
        // Run self-test on MPU
        if (hardwareManager->runSelfTest(HW_COMPONENT_MPU)) {
          Serial.println("MPU self-test passed!");
        } else {
          Serial.println("MPU self-test failed!");
        }
        
        // Run self-test on LEDs
        if (hardwareManager->runSelfTest(HW_COMPONENT_LED)) {
          Serial.println("LED self-test passed!");
        } else {
          Serial.println("LED self-test failed!");
        }
        
        testStep++;
        lastActionTime = currentTime;
      }
      break;
      
    case 2:  // Complete recovery test
      if (currentTime - lastActionTime >= 2000) {
        Serial.println("\n--- Phase 4: Error Recovery Test Complete ---");
        Serial.println("Starting Power Management Test Phase...");
        
        testPhase = PHASE_POWER_MANAGEMENT;
        testStep = 0;
        phaseStartTime = currentTime;
      }
      break;
  }
}

/**
 * @brief Power management test phase
 */
void runPowerManagementPhase() {
  unsigned long currentTime = millis();
  
  switch (testStep) {
    case 0:  // Normal power mode
      if (currentTime - phaseStartTime < 2000) {
        if (currentTime - lastDisplayTime >= 500) {
          lastDisplayTime = currentTime;
          Serial.println("Testing power modes: Active Mode");
          
          // Ensure active power mode
          hardwareManager->setPowerState(true);
          
          // Show active state with bright blue light
          hardwareManager->setAllLEDs({0, 0, 255});
          hardwareManager->setBrightness(200);
          hardwareManager->updateLEDs();
        }
      } else {
        testStep++;
      }
      break;
      
    case 1:  // Low power mode
      if (currentTime - phaseStartTime < 4000) {
        if (currentTime - lastActionTime >= 500) {
          lastActionTime = currentTime;
          Serial.println("Testing power modes: Low Power Mode");
          
          // Set low power mode
          hardwareManager->setPowerState(false);
          
          // Show low power state with dim blue light
          hardwareManager->setAllLEDs({0, 0, 255});
          hardwareManager->updateLEDs();
        }
      } else {
        testStep++;
      }
      break;
      
    case 2:  // Back to normal power
      if (currentTime - phaseStartTime < 6000) {
        if (currentTime - lastActionTime >= 500) {
          lastActionTime = currentTime;
          Serial.println("Testing power modes: Returning to Active Mode");
          
          // Return to active power mode
          hardwareManager->setPowerState(true);
          
          // Show active state with white light
          hardwareManager->setAllLEDs({255, 255, 255});
          hardwareManager->updateLEDs();
        }
      } else {
        // Complete all tests
        Serial.println("\n--- Phase 5: Power Management Test Complete ---");
        Serial.println("\n=================================================");
        Serial.println("All Hardware Manager tests completed successfully!");
        Serial.println("=================================================");
        Serial.println("\nContinuing to display sensor data...");
        
        // Turn LEDs green to indicate success
        hardwareManager->setAllLEDs({0, 255, 0});
        hardwareManager->updateLEDs();
        
        testPhase = PHASE_COMPLETE;
        testStep = 0;
      }
      break;
  }
}

/**
 * @brief Display sensor data in a readable format
 * @param data Sensor data to display
 */
void displaySensorData(const SensorData& data) {
  Serial.printf("Accel: X=%6d Y=%6d Z=%6d | ", 
               data.accelX, data.accelY, data.accelZ);
  Serial.printf("Gyro: X=%6d Y=%6d Z=%6d\n", 
               data.gyroX, data.gyroY, data.gyroZ);
} 