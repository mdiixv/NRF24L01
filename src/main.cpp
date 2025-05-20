#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

#define VSPI_MOSI 23
#define VSPI_MISO 19
#define VSPI_CLK 18
// #define NRF24_SS_PIN 5
// #define NRF24_CE_PIN 15
#define NRF24_SS_PIN 21
#define NRF24_CE_PIN 22

#define BOOT_BUTTON 0 //button pin
#define LED_PIN 2   // led pin

#define debounceDelay  200
#define blinkInterval  200
#define pauseInterval  3000

// متغیرهای مربوط به الگوی چشمک‌زدن LED
unsigned long lastLEDEvent = 0; // زمان آخرین تغییر وضعیت LED

bool ledBlinking = false;
bool ledState = false;
bool inPause = false;

int blinkCount = 0;  
int totalBlinks = 0; 
int NRF_MODE_N = 1;

int ptr_hop = 0;

unsigned long lastButtonPress = 0; //Debounce
unsigned long lastChannelChange = 0; // برای زمان‌بندی تغییر کانال
const unsigned long channelChangeInterval = 2; // فاصله تغییر کانال (2 میلی‌ثانیه)


byte wifi_channels[] = {
    6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,             // Channel 1
    22, 24, 26, 28,                                             // mid band
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, // Channel 6
    46, 48, 50, 52,                                             // mid band
    55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,     // Channel 11
}; // WiFi Channels
byte ble_channels[] = {1, 2, 3, 25, 26, 27, 79, 80, 81}; // BLE Advertising Channels: 2, 26 and 80
byte bluetooth_channels[] = {
    // 1,  3,  5,  7,  9,  11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, // odds
    // 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, // odds
    2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40,    // even
    42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80 // even
}; // Bluetooth Channels
byte usb_channels[] = {40, 50, 60};   // USB Wireless Channels
byte video_channels[] = {70, 75, 80}; // Video Streaming Channels
byte rc_channels[] = {1, 3, 5, 7};    // RC Toys/ Drones Channels
byte full_channels[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
    85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
    //, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
    // 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124
}; // All Channels


void printMode();
void startBlinking();
void updateLED();

RF24 NRFradio(NRF24_CE_PIN, NRF24_SS_PIN);
SPIClass NRFSPI;


void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting nRF24L01 Jammer on ESP32...");

    pinMode(0, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW); 

    NRFSPI.begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI);

    delay(50);

    if (NRFradio.begin(&NRFSPI, NRF24_CE_PIN, NRF24_SS_PIN))
    {
        Serial.println("nRF24L01 detected and initialized successfully!");
    }
    else
    {
        Serial.println("Failed to detect nRF24L01! Check connections.");
        while (1);
    }

    // int NRF_MODE_N_X = 0;
    // String NRF_MODE = "WiFi";

    // String NRF_STATUS = "OFF";
    // int NRF_STATUS_X = 0;

    NRFradio.setPALevel(RF24_PA_MAX);
    NRFradio.startConstCarrier(RF24_PA_MAX, 45);
    NRFradio.setAddressWidth(3); // optional
    NRFradio.setPayloadSize(2);  // optional
    if (!NRFradio.setDataRate(RF24_2MBPS))
        Serial.println("Fail setting data Rate");

    Serial.print("Starting in mode: ");

    switch (NRF_MODE_N)
    {
    case 1:  Serial.println("WiFi");     break;
    case 2:  Serial.println("BLE");      break;
    case 3:  Serial.println("Bluetooth");break;
    case 4:  Serial.println("USB");      break;
    case 5:  Serial.println("Video");    break;
    case 6:  Serial.println("RC");       break;
    case 7:  Serial.println("Full");     break;
    default: Serial.println("Unknown");
    }

    startBlinking();
}

void loop()
{
    if (digitalRead(0) == LOW && (millis() - lastButtonPress > debounceDelay))
    {
        NRF_MODE_N = (NRF_MODE_N % 7) + 1; 
        ptr_hop = 0; 
        printMode(); 
        startBlinking();
        lastButtonPress = millis(); 
    }

    ptr_hop++;
    if (NRF_MODE_N == 1)
    {

        if (ptr_hop >= sizeof(wifi_channels))
            ptr_hop = 0;
        NRFradio.setChannel(wifi_channels[ptr_hop]);
    }
    else if (NRF_MODE_N == 2)
    {
        if (ptr_hop >= sizeof(ble_channels))
            ptr_hop = 0;
        NRFradio.setChannel(ble_channels[ptr_hop]);
    }
    else if (NRF_MODE_N == 3)
    {
        if (ptr_hop >= sizeof(bluetooth_channels))
            ptr_hop = 0;
        NRFradio.setChannel(bluetooth_channels[ptr_hop]);
    }
    else if (NRF_MODE_N == 4)
    {
        if (ptr_hop >= sizeof(usb_channels))
            ptr_hop = 0;
        NRFradio.setChannel(usb_channels[ptr_hop]);
    }
    else if (NRF_MODE_N == 5)
    {
        if (ptr_hop >= sizeof(video_channels))
            ptr_hop = 0;
        NRFradio.setChannel(video_channels[ptr_hop]);
    }
    else if (NRF_MODE_N == 6)
    {
        if (ptr_hop >= sizeof(rc_channels))
            ptr_hop = 0;
        NRFradio.setChannel(rc_channels[ptr_hop]);
    }
    else if (NRF_MODE_N == 7)
    {
        if (ptr_hop >= sizeof(full_channels))
            ptr_hop = 0;
        NRFradio.setChannel(full_channels[ptr_hop]);
    }

    updateLED();
   // Serial.println(ptr_hop);
   // delay(2);
}

void printMode()
{
    Serial.print("Current Mode: ");
    switch (NRF_MODE_N)
    {
    case 1: Serial.println("WiFi"); break;
    case 2: Serial.println("BLE"); break;
    case 3: Serial.println("Bluetooth"); break;
    case 4: Serial.println("USB"); break;
    case 5: Serial.println("Video"); break;
    case 6: Serial.println("RC"); break;
    case 7: Serial.println("Full"); break;
    default: Serial.println("Unknown");
    }
}

void startBlinking()
{
    // تنظیم تعداد چشمک‌ها بر اساس حالت
    ledBlinking = true;
    inPause = false;
    ledState = false;
    digitalWrite(LED_PIN, LOW); // خاموش کردن LED در شروع
    lastLEDEvent = millis();
    blinkCount = 0;
    switch (NRF_MODE_N)
    {
    case 1: totalBlinks = 1; break; // WiFi: 1 چشمک
    case 3: totalBlinks = 2; break; // Bluetooth: 2 چشمک
    case 6: totalBlinks = 3; break; // RC: 3 چشمک
    case 7: totalBlinks = 4; break; // Full: 4 چشمک
    default: ledBlinking = false; totalBlinks = 0; inPause = false; return; // برای حالت‌های دیگر LED خاموش
    }
}

void updateLED()
{
    if (!ledBlinking) return; // left if Blink is off 

    unsigned long currentTime = millis();
    if (inPause)
    {
        if (currentTime - lastLEDEvent >= pauseInterval)
        {  
            startBlinking();
        }
    }
    else
    {
        if (currentTime - lastLEDEvent >= blinkInterval)
        {
            if (ledState) // if LED is ON
            {
                digitalWrite(LED_PIN, LOW);
                ledState = false;
                lastLEDEvent = currentTime;
                blinkCount++;
                if (blinkCount >= totalBlinks) // ON & OFF
                {
                    inPause = true; // Pause
                    lastLEDEvent = currentTime;
                }
            }
            else
            {
                if (blinkCount < totalBlinks)
                {
                    digitalWrite(LED_PIN, HIGH);
                    ledState = true;
                    lastLEDEvent = currentTime;
                }
            }
        }
    }
}