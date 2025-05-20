#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "blink.h"

// Pin definitions for SPI and NRF24
#define VSPI_MOSI 23
#define VSPI_MISO 19
#define VSPI_CLK 18
#define NRF24_SS_PIN 21
#define NRF24_CE_PIN 22

// Button pin and debounce delay
#define BOOT_BUTTON 0      // Button pin
#define DEBOUNCE_DELAY 200 // Debounce delay in milliseconds

// Variables for channel hopping
int ptr_hop = 0;
unsigned long lastButtonPress = 0;               // Last button press time for debounce
unsigned long lastChannelChange = 0;             // Last channel change time
const unsigned long CHANNEL_CHANGE_INTERVAL = 2; // Channel change interval in milliseconds

// Channel arrays for different modes
byte wifi_channels[] = {
    6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,             // Channel 1
    22, 24, 26, 28,                                             // Mid band
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, // Channel 6
    46, 48, 50, 52,                                             // Mid band
    55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,     // Channel 11
}; // WiFi Channels
byte ble_channels[] = {1, 2, 3, 25, 26, 27, 79, 80, 81}; // BLE Advertising Channels: 2, 26, and 80
byte bluetooth_channels[] = {
    2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40,    // Even
    42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80 // Even
}; // Bluetooth Channels
byte usb_channels[] = {40, 50, 60};   // USB Wireless Channels
byte video_channels[] = {70, 75, 80}; // Video Streaming Channels
byte rc_channels[] = {1, 3, 5, 7};    // RC Toys/Drones Channels
byte full_channels[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
    85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100}; // All Channels

// Function declarations
void printMode();

RF24 NRFradio(NRF24_CE_PIN, NRF24_SS_PIN);
SPIClass NRFSPI;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting nRF24L01 Jammer on ESP32...");

    // Initialize button and LED pins
    pinMode(BOOT_BUTTON, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize SPI for NRF24
    NRFSPI.begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI);

    delay(50);

    // Initialize NRF24 module
    if (NRFradio.begin(&NRFSPI, NRF24_CE_PIN, NRF24_SS_PIN))
    {
        Serial.println("nRF24L01 detected and initialized successfully!");
    }
    else
    {
        Serial.println("Failed to detect nRF24L01! Check connections.");
        while (1)
            ;
    }

    // Configure NRF24 settings
    NRFradio.setPALevel(RF24_PA_MAX);
    NRFradio.startConstCarrier(RF24_PA_MAX, 45);
    NRFradio.setAddressWidth(3); // Optional
    NRFradio.setPayloadSize(2);  // Optional
    if (!NRFradio.setDataRate(RF24_2MBPS))
        Serial.println("Failed to set data rate");

    // Print starting mode
    Serial.print("Starting in mode: ");
    switch (nrfMode)
    {
    case 1:
        Serial.println("WiFi");
        break;
    case 2:
        Serial.println("BLE");
        break;
    case 3:
        Serial.println("Bluetooth");
        break;
    case 4:
        Serial.println("USB");
        break;
    case 5:
        Serial.println("Video");
        break;
    case 6:
        Serial.println("RC");
        break;
    case 7:
        Serial.println("Full");
        break;
    default:
        Serial.println("Unknown");
    }

    startBlinking(); // Start LED blinking
}

void loop()
{
    // Handle button press with debounce
    if (digitalRead(BOOT_BUTTON) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY))
    {
        nrfMode = (nrfMode % 7) + 1; // Cycle through modes
        ptr_hop = 0;                 // Reset channel hop pointer
        printMode();                 // Print current mode
        startBlinking();             // Restart LED blinking
        lastButtonPress = millis();
    }

    // Channel hopping logic
    ptr_hop++;
    if (nrfMode == 1)
    {
        if (ptr_hop >= sizeof(wifi_channels))
            ptr_hop = 0;
        NRFradio.setChannel(wifi_channels[ptr_hop]);
    }
    else if (nrfMode == 2)
    {
        if (ptr_hop >= sizeof(ble_channels))
            ptr_hop = 0;
        NRFradio.setChannel(ble_channels[ptr_hop]);
    }
    else if (nrfMode == 3)
    {
        if (ptr_hop >= sizeof(bluetooth_channels))
            ptr_hop = 0;
        NRFradio.setChannel(bluetooth_channels[ptr_hop]);
    }
    else if (nrfMode == 4)
    {
        if (ptr_hop >= sizeof(usb_channels))
            ptr_hop = 0;
        NRFradio.setChannel(usb_channels[ptr_hop]);
    }
    else if (nrfMode == 5)
    {
        if (ptr_hop >= sizeof(video_channels))
            ptr_hop = 0;
        NRFradio.setChannel(video_channels[ptr_hop]);
    }
    else if (nrfMode == 6)
    {
        if (ptr_hop >= sizeof(rc_channels))
            ptr_hop = 0;
        NRFradio.setChannel(rc_channels[ptr_hop]);
    }
    else if (nrfMode == 7)
    {
        if (ptr_hop >= sizeof(full_channels))
            ptr_hop = 0;
        NRFradio.setChannel(full_channels[ptr_hop]);
    }

    updateLED(); // Update LED state
}

void printMode()
{
    // Print the current NRF mode
    Serial.print("Current Mode: ");
    switch (nrfMode)
    {
    case 1:
        Serial.println("WiFi");
        break;
    case 2:
        Serial.println("BLE");
        break;
    case 3:
        Serial.println("Bluetooth");
        break;
    case 4:
        Serial.println("USB");
        break;
    case 5:
        Serial.println("Video");
        break;
    case 6:
        Serial.println("RC");
        break;
    case 7:
        Serial.println("Full");
        break;
    default:
        Serial.println("Unknown");
    }
}