#ifndef GSM_H
#define GSM_H

// GSM Library Configuration
#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024
#define TINY_GSM_YIELD() { delay(2); }

// Include necessary libraries after defining modem type
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include "SSLClient.h"

// GSM Serial Monitor
extern HardwareSerial SerialAT;

// GSM Pins Configuration
#define GSM_PIN "0000"

// Website Credentials
extern const char apn[];
extern const char gprsUser[];
extern const char gprsPass[];
extern const char server[];
extern const char resource[];
extern const int port;
extern unsigned long timeout;
extern const char *root_ca;

// GSM Objects
extern TinyGsm modem;
extern TinyGsmClient base_client;
extern SSLClient secure_layer;
extern HttpClient client;

// Function Declarations
void GSMinit();
void setupGSM();
void loopGSM();

#endif // GSM_H
