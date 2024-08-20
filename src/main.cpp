#define SerialMon Serial

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

#include "SensorsLib.h"
#include "GSM.h"

void setup() {
    SerialMon.begin(115200); // Start SerialMon communication
    Wire.begin();            // Start I2C communication

    checkSensors(); // Initialize Light, WindD, BME, & UV
    setupGSM();      // Initialize GSM       

    SerialMon.println("START");
}

void loop() {
    BME();               // Read and print BME280 sensor data
    Light();             // Read and print light sensor data
    UV();                // Read and print UV sensor data
    WindDirection();     // Read and print wind direction data
    loopGSM();

    SerialMon.println("END");

    delay(5000);           // Delay before the next loop
}
