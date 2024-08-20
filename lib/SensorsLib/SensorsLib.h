#ifndef SENSORSLIB_H
#define SENSORSLIB_H

#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//=============================================== WindDirection ===============================================
extern const byte SENSOR_ADDRESS;
extern const byte RAW_ANGLE_REG_LOW;
extern const byte RAW_ANGLE_REG_HIGH;

extern int lowbyte;
extern word highbyte;
extern int rawAngle;
extern float degAngle;
extern int correctedAngle;
extern float startAngle;

// Variable to store the wind direction string
extern String winddir_str;  

void ReadRawAngle(byte sensorAddress);  // Function now accepts sensorAddress as a parameter
void correctAngle();
void setupWindDirection();
void WindDirection();
void checkWindDirectionSensor();

//=============================================== LightSensor ===============================================
extern BH1750 lightMeter;
extern String light_str;
extern String irradiance_str;

void setupLightSensor();
void Light();
void getLight();

//=============================================== BME Multiplexer ===============================================
extern Adafruit_BME280 bme[3];
extern bool sensorDetected[3];

// String variables to store temperature, humidity, and pressure values
extern String t_str[3];
extern String h_str[3];
extern String p_str[3];

void Multiplexer(uint8_t bus);
void setupBME280();
void getBME(Adafruit_BME280& bme, uint8_t bus, float* temp, float* hum, float* pres);
void printValues(Adafruit_BME280& bme, const char* name, int bus, bool detected);
void BME();

//=============================================== UV Sensor ===============================================
extern const int uvSensorPin;

float mapVoltageToUVIndex(float voltage);
void UV();
void checkUVSensor();

//============================================== General sensor check ==============================================
void checkSensors();

#endif
