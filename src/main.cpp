#define SerialMon Serial

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

#include "SlaveLib.h"
#include "SensorsLib.h"
#include "GSM.h"

void setup() {
    SerialMon.begin(115200); // Start SerialMon communication
    Wire.begin();            // Start I2C communication

    SerialMon.println("");
    checkSensors(); // Initialize Light, Wind Direction, BME280, UV, & INA219 sensors
    checkSlaveConnection();
    setupGSM();      // Initialize GSM       
}

void loop() {
    BME();               // Read and print BME280 sensor data
    Light();             // Read and print light sensor data
    UV();                // Read and print UV sensor data
    WindDirection();     // Read and print wind direction data
    INA219Sensor();      // Read and print INA219 bus voltage
    printSlaveData();
    loopGSM();           // Execute GSM functions

    SerialMon.println("END");

    delay(5000);         // Delay before the next loop
}
