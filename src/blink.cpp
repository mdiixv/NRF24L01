#include "blink.h"

// Variable definitions
unsigned long lastLEDEvent = 0; // Time of the last LED event (in milliseconds)
bool ledBlinking = false;       // Indicates if the LED is in blinking mode
bool ledState = false;          // Current state of the LED (true = on, false = off)
bool inPause = false;           // Indicates if the LED is in pause state
int blinkCount = 0;             // Current number of completed blinks
int totalBlinks = 0;            // Total number of blinks required for the current mode
int nrfMode = 1;                // NRF mode to determine the blink pattern

void startBlinking()
{
    // Initialize the blinking pattern based on the NRF mode
    ledBlinking = true;
    inPause = false;
    ledState = false;
    digitalWrite(LED_PIN, LOW); // Turn off the LED at the start
    lastLEDEvent = millis();    // Record the current time
    blinkCount = 0;             // Reset blink counter
    switch (nrfMode)
    {
    case 1:
        totalBlinks = 1;
        break; // WiFi mode: 1 blink
    case 3:
        totalBlinks = 2;
        break; // Bluetooth mode: 2 blinks
    case 6:
        totalBlinks = 3;
        break; // RC mode: 3 blinks
    case 7:
        totalBlinks = 4;
        break; // Full mode: 4 blinks
    default:
        ledBlinking = false;
        totalBlinks = 0;
        inPause = false;
        return; // Disable blinking for undefined modes
    }
}

void updateLED()
{
    if (!ledBlinking)
        return; // Exit if blinking is disabled

    unsigned long currentTime = millis(); // Get current time
    if (inPause)
    {
        // Check if pause interval has elapsed
        if (currentTime - lastLEDEvent >= PAUSE_INTERVAL)
        {
            startBlinking(); // Restart blinking after pause
        }
    }
    else
    {
        // Check if blink interval has elapsed
        if (currentTime - lastLEDEvent >= BLINK_INTERVAL)
        {
            if (ledState)
            {                               // If LED is on
                digitalWrite(LED_PIN, LOW); // Turn off the LED
                ledState = false;
                lastLEDEvent = currentTime; // Update last event time
                blinkCount++;               // Increment blink counter
                if (blinkCount >= totalBlinks)
                {                               // Check if all blinks are done
                    inPause = true;             // Enter pause state
                    lastLEDEvent = currentTime; // Update last event time
                }
            }
            else
            {
                if (blinkCount < totalBlinks)
                {                                // If more blinks are needed
                    digitalWrite(LED_PIN, HIGH); // Turn on the LED
                    ledState = true;
                    lastLEDEvent = currentTime; // Update last event time
                }
            }
        }
    }
}