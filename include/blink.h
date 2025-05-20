#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>

// LED pin and timing constants
#define LED_PIN 2           // Pin for the LED
#define BLINK_INTERVAL 200  // Blink interval in milliseconds
#define PAUSE_INTERVAL 3000 // Pause interval in milliseconds

// Variables for LED blinking pattern
extern unsigned long lastLEDEvent; // Time of the last LED event
extern bool ledBlinking;           // State of LED blinking
extern bool ledState;              // Current state of the LED (on/off)
extern bool inPause;               // Pause state for LED blinking
extern int blinkCount;             // Current number of blinks
extern int totalBlinks;            // Total number of blinks for the current mode
extern int nrfMode;                // NRF mode (used to determine blink pattern)

// Function declarations
void startBlinking();
void updateLED();

#endif