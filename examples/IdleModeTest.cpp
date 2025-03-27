#ifdef ENV_IDLE_TEST

#include <Arduino.h>
#include "src/core/GauntletController.h"

GauntletController controller;
bool interpolationEnabled = true;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    Serial.println(F("================================"));
    Serial.println(F("Idle Mode Test Environment"));
    Serial.println(F("================================"));
    
    Serial.println(F("Initializing controller..."));
    controller.initialize();
    
    Serial.println(F("Ready. Available commands:"));
    Serial.println(F("- 'i': Toggle color interpolation"));
    Serial.println(F("- 'h': Show this help message"));
}

void loop() {
    // Check for serial commands
    if (Serial.available() > 0) {
        char command = Serial.read();
        
        switch (command) {
            case 'i':
                interpolationEnabled = !interpolationEnabled;
                controller.setInterpolationEnabled(interpolationEnabled);
                Serial.print(F("Color interpolation: "));
                Serial.println(interpolationEnabled ? F("ENABLED") : F("DISABLED"));
                break;
                
            case 'h':
                Serial.println(F("Available commands:"));
                Serial.println(F("- 'i': Toggle color interpolation"));
                Serial.println(F("- 'h': Show this help message"));
                break;
        }
        
        // Consume any other characters in the buffer
        while (Serial.available() > 0) {
            Serial.read();
        }
    }
    
    // Update the controller
    controller.update();
}

#endif // ENV_IDLE_TEST 