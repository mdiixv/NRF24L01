#include <RF24.h>
//#include <globals.h>

// Define the Macros case it hasn't been declared
// #ifndef NRF24_CE_PIN
#define NRF24_CE_PIN 15
// #endif
// #ifndef NRF24_SS_PIN
#define NRF24_SS_PIN 5
// #endif

#define VSPI_MOSI 23
#define VSPI_MISO 19
#define VSPI_CLK 18

extern RF24 NRFradio;

bool nrf_start();

// void nrf_info();
