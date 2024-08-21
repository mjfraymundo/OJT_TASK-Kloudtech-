#include "GSM.h"
#include "SensorsLib.h"
#include "SlaveLib.h"
#include <TinyGsmClient.h>
#include <SSLClient.h>
#include <Wire.h>
#include <RTClib.h>  // Include RTC library

// Sleep Factors
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 60         /* Time ESP32 will go to sleep (in seconds) */

#define TINY_GSM_MODEM_SIM7600
#define SerialMon Serial
TinyGsm modem(SerialAT);

// GSM Pins - GSM Initiation
#define GSM_PIN "0000"
#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 26
#define PIN_RX 27
#define PWR_PIN 4
#define PIN_RI 33
#define RESET 5
#define BAT_ADC 35
#define BAT_EN 12

// GSM Serial Monitor
HardwareSerial SerialAT(1);

// Website Credentials
const char apn[] = "smartlte";
const char gprsUser[] = "";
const char gprsPass[] = "";
const char server[] = "v1server.kloudtechsea.com";
const char resource[] = "https://v1server.kloudtechsea.com/insert-weather?serial=816a2a9a-5f29-4d47-a545-d0ab0e97ffdd";
const int port = 443;
unsigned long timeout;

const char *root_ca = 
"-----BEGIN CERTIFICATE-----\n"
    "MIIGJTCCBQ2gAwIBAgISBNKBw5RrEabvEQ8qXuXIicRZMA0GCSqGSIb3DQEBCwUA\n"
    "MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n"
    "EwJSMzAeFw0yNDA1MDMwNTAwMTlaFw0yNDA4MDEwNTAwMThaMCYxJDAiBgNVBAMT\n"
    "G2tsb3VkdHJhY2sua2xvdWR0ZWNoc2VhLmNvbTCCAiIwDQYJKoZIhvcNAQEBBQAD\n"
    "ggIPADCCAgoCggIBAJ2jaT1JSGu/EB3ZasUd1iqGWoUypAxCuSD5unPIWrMePVj4\n"
    "lJ/DTyrusWzbnkSfN/IiObhZR0hfdUMJVDXJ5kVw21iGFTiG6Q88G1mnICH7Dmju\n"
    "fAXVrTdPtPRDnM17nGKVv/eNtxA9tqrvnfAA+aaRiWe2zI+8r/FBPSMr1/sEzLCb\n"
    "cpbNSSA56EFQ6KN7nJNFLCZyiJq+3DyIWvhxTW5nmTsj4GTz9LNA1KxQTIxPdS7P\n"
    "4DYfPoXbwGOQJd6Qp+rBNp0q8P2zazFwCUctJMCzUvTPkXiLOQa10ZmgXMS+m+qd\n"
    "xcnV+OBBhm5A4ePkWxj3gG/qMs9xL46u3oYO5a8XFevtQtFefZ/XOh3kWFd1HcMe\n"
    "aLeaZSizZhqZLKMl5jjhnytdbJLqr3XMqHTwYBrqzDjIIQrz05QtwiO2rgmAueTQ\n"
    "I5PkCFoM/BCMYncXWr2ut2dpaUmjnc+m+0Tj9UhC8EywjomDHuxoz88jLCeXcucT\n"
    "OqAn4CnNGPqnCpjZsi8eDc/UScwX9/wvKK3SjVoxkQ79JYEMJrt9T8QpCdz7Jnlz\n"
    "o7mLkjIg1myrNPaXT9e43+2BBsB0Z45dzCwSCdfQa6Yiq9hhL35jH/9gewwG0Cts\n"
    "2ZeAOgOLBfdDIZ0iMmgJrjqJMquGJo9jhjitZ2n12kR9kqXXMH+whut2FJ9tAgMB\n"
    "AAGjggI/MIICOzAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYwFAYIKwYBBQUHAwEG\n"
    "CCsGAQUFBwMCMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFA+bIIZGnU749LOPr1Zm\n"
    "JsTDDFMbMB8GA1UdIwQYMBaAFBQusxe3WFbLrlAJQOYfr52LFMLGMFUGCCsGAQUF\n"
    "BwEBBEkwRzAhBggrBgEFBQcwAYYVaHR0cDovL3IzLm8ubGVuY3Iub3JnMCIGCCsG\n"
    "AQUFBzAChhZodHRwOi8vcjMuaS5sZW5jci5vcmcvMEcGA1UdEQRAMD6CG2tsb3Vk\n"
    "dHJhY2sua2xvdWR0ZWNoc2VhLmNvbYIfd3d3Lmtsb3VkdHJhY2sua2xvdWR0ZWNo\n"
    "c2VhLmNvbTATBgNVHSAEDDAKMAgGBmeBDAECATCCAQUGCisGAQQB1nkCBAIEgfYE\n"
    "gfMA8QB3AEiw42vapkc0D+VqAvqdMOscUgHLVt0sgdm7v6s52IRzAAABjz0KDG4A\n"
    "AAQDAEgwRgIhAOcRiEp+2Ci1hTER/7D53V/ffcWqViZjqRlpzCA2ob1eAiEAqt7B\n"
    "+w5YGGcbninZX4zhX+KDjpHZroSuBr/6i9L+V/IAdgA/F0tP1yJHWJQdZRyEvg0S\n"
    "7ZA3fx+FauvBvyiF7PhkbgAAAY89Cgx7AAAEAwBHMEUCIEeNLdSPgsped5lnQvbD\n"
    "K3KC5HVQo5qaFk71RB7XbF+dAiEAoG42c29IPxepsHcno1hedijlr6As/FSTeAYa\n"
    "nh5lcSQwDQYJKoZIhvcNAQELBQADggEBAGkNCgI4kxfP5LfpqHYBJXsqfIXBLEca\n"
    "4nfp4j894K56du9age2XbzTg/c5nlQJAUEoMPZyN0yimqpdPwDR89Eq+m9EdVcNl\n"
    "5imJzeM766eWM+K78d2bE+1reqij63Y34lurZUIH0CAP5cH9q85CYok53ZFTQque\n"
    "mkfsYT+furu3I88RGfAgpEpWczZTJO+TK0Yudm8EipwUsdlKCOkreFMUwOpBCPdk\n"
    "Th2/SDDI9OHFSFYbQZVKabOkQaW4UbVgjydehXnWnzOuzRkQD64rdPBk9Xw3DRS9\n"
    "oeges2GJDrMBrUvFL4uetweyPEeDJktOAsQKEqeD3O3t4Qqi2uKpGns=\n"
    "-----END CERTIFICATE-----\n";

// HTTPS Transport
TinyGsmClient base_client(modem, 0);
SSLClient secure_layer(&base_client);
HttpClient client = HttpClient(secure_layer, server, port);

RTC_DS3231 rtc;  // Declare the RTC object
String Time;  // For storing the time string

void GSMinit() {
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, LOW);
    delay(100);
    digitalWrite(RESET, HIGH);
    delay(3000);
    digitalWrite(RESET, LOW);

    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(100);
    digitalWrite(PWR_PIN, HIGH);
    delay(1000);
    digitalWrite(PWR_PIN, LOW);
}

void setupGSM() {
    SerialMon.println("\n========================================GSM Initializing========================================");
    SerialMon.println("Starting GSM...");
    GSMinit();
    SerialMon.print("Waiting for 3s...");
    delay(3000);
    SerialMon.println(" >OK");

    SerialMon.print("Starting Serial Communications...");
    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
    SerialMon.println(" >OK");

    SerialMon.print("Initializing modem...");
    if (!modem.init()) {
        SerialMon.println(" >Failed (Restarting in 10s)");
        return;
    }
    SerialMon.println(" >OK");

    SerialMon.print("Selecting Network Mode...");
    bool result = modem.setNetworkMode(2);  // Store in a bool
    if (modem.waitResponse(10000L) == 1)
    {
    SerialMon.println(" >OK");
    return;
    }
    SerialMon.println(" >Failed");

    //secure_layer.setCACert(root_ca);
}


void loopGSM() {
    SerialMon.println("\n========================================Connecting to APN========================================");
    SerialMon.print("Connecting to ");
    SerialMon.print(apn);

    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" >Failed");
    } else {
        SerialMon.println(" >OK");
        SerialMon.println("\n========================================Connecting to Server========================================");
        SerialMon.print("Connecting to ");
        SerialMon.print(server);

        if (!base_client.connect(server, port)) {
            SerialMon.println(" >Failed");
        } else {
            SerialMon.println(" >OK");

            if (modem.isNetworkConnected()) {
                SerialMon.println("Success to connect to network.");
            }

            if (modem.isGprsConnected()) {
                SerialMon.println("Success to connect to GPRS.");
            }

            SerialMon.println("====================================== Print results =========================================================");
            SerialMon.println("T1 = " + t_str[0]);
            SerialMon.println("T2 = " + t_str[1]);
            SerialMon.println("T3 = " + t_str[2]);
            SerialMon.println("H1 = " + h_str[0]);
            SerialMon.println("H2 = " + h_str[1]);
            SerialMon.println("H3 = " + h_str[2]);
            SerialMon.println("P1 = " + p_str[0]);
            SerialMon.println("P2 = " + p_str[1]);
            SerialMon.println("P3 = " + p_str[2]);
            SerialMon.println("Light Intensity = " + light_str);
            SerialMon.println("Wind Direction = " + winddir_str);
            SerialMon.println("Battery Voltage = " + volt_str);
            SerialMon.println("Precipitation = " + rain_str);
            SerialMon.println("Wind Speed = " + winds_str);

            SerialMon.println("\n========================================RTC Initializing========================================");
            SerialMon.print("Connecting to RTC...");
            if (!rtc.begin()) {
                Serial.println("Couldn't connect to RTC");
            } else {
                if (rtc.lostPower()) {
                    Serial.println("RTC lost power, let's set the time!...");
                    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
                }

                SerialMon.print("Time is ");
                Time = rtc.now().timestamp();
                Serial.println(Time);
                delay(10);
            }

            SerialMon.println("\n========================================HTTP Post Request========================================");
            SerialMon.println("Performing HTTP POST request...");
            client.connectionKeepAlive();
            SerialMon.print("Connecting to ");
            SerialMon.println(server);

            SerialMon.println("Making POST request securely");
            String contentType = "Content-Type: application/json";
            String postData = "{\"recordedAt\":\"" + Time + "\", \"light\" :\"" + light_str + "\", \"uvIntensity\" :\"" + uv_str + "\", \"windDirection\" :\"" + winddir_str + "\", \"windSpeed\" :\""+winds_str+"\", \"precipitation\" :\""+rain_str+"\", \"T1\":\"" + t_str[0] + "\", \"T2\":\"" + t_str[1] + "\", \"T3\":\"" + t_str[2] + "\", \"H1\":\"" + h_str[0] + "\", \"H2\":\"" + h_str[1] + "\", \"H3\":\"" + h_str[2] + "\", \"P1\":\"" + p_str[0] + "\", \"P2\":\"" + p_str[1] + "\", \"P3\":\"" + p_str[2] + "\",\"batteryVoltage\" :\"" + volt_str + "\"}";



            SerialMon.println("\n=========================================POST Data ============================================");
            SerialMon.println(postData);

            int postDataLength = postData.length();
            client.sendHeader("Content-Length", postDataLength);
            client.sendHeader("Connection", "Close");
            int posting = client.post(resource, contentType, postData);
            SerialMon.print("Reply:");
            SerialMon.println(posting);
            int status_code = client.responseStatusCode();
            String response = client.responseBody();

            SerialMon.print("Status code: ");
            SerialMon.println(status_code);
            SerialMon.print("Response: ");
            SerialMon.println(response);

            SerialMon.println("\n========================================Closing Client========================================");
            client.stop();
            SerialMon.println(F("Server disconnected"));
            modem.gprsDisconnect();
            SerialMon.println(F("GPRS disconnected"));

            // Set Timer and Sleep
            esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
            SerialMon.println("=====================================Entering deep sleep...=================================================");
            esp_deep_sleep_start(); 
        }
    }
}
