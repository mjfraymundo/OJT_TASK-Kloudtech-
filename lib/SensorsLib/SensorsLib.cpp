#include "SensorsLib.h"

#define SerialMon Serial

//=============================================== WindDirection ===============================================
const byte SENSOR_ADDRESS = 0x36;
const byte RAW_ANGLE_REG_LOW = 0x0D;
const byte RAW_ANGLE_REG_HIGH = 0x0C;

// Variables
int lowbyte = 0;
word highbyte = 0;
int rawAngle = 0;
float degAngle = 0.0;
int correctedAngle = 0;
float startAngle = 0.0;
String winddir_str;

// Read and calculate the angle
void ReadRawAngle(byte sensorAddress) {
    Wire.beginTransmission(sensorAddress);
    Wire.write(RAW_ANGLE_REG_LOW);
    Wire.endTransmission();
    Wire.requestFrom(sensorAddress, (uint8_t)1);
    if (Wire.available()) {
        lowbyte = Wire.read();
    } else {
        SerialMon.println("Wind Direction Sensor Error: Low byte not available");
        return;
    }

    Wire.beginTransmission(sensorAddress);
    Wire.write(RAW_ANGLE_REG_HIGH);
    Wire.endTransmission();
    Wire.requestFrom(sensorAddress, (uint8_t)1);
    if (Wire.available()) {
        highbyte = Wire.read();
    } else {
        SerialMon.println("Wind Direction Sensor Error: High byte not available");
        return;
    }
    highbyte <<= 8;

    rawAngle = highbyte | lowbyte;
    degAngle = rawAngle * 0.087890625;

    // Correct the angle and update the winddir_str
    correctAngle();
    winddir_str = String(correctedAngle);
}

void correctAngle() {
    correctedAngle = degAngle - startAngle;
    if (correctedAngle < 0) {
        correctedAngle += 360;
    }
    SerialMon.println(correctedAngle);
}

// Setup and check wind direction sensor
void setupWindDirection() {
    Wire.begin();  // Initialize I2C communication
    SerialMon.print("AS5600: ");
    ReadRawAngle(SENSOR_ADDRESS);
    if (degAngle == 0) {
        SerialMon.println("Wind Direction Sensor Not Detected");
        return;
    }
    startAngle = degAngle;
    winddir_str = String(correctedAngle);
    SerialMon.println(" OK");
}

void WindDirection() {
    ReadRawAngle(SENSOR_ADDRESS);
    SerialMon.println("Wind Direction: " + winddir_str);
}

void checkWindDirectionSensor() {
    ReadRawAngle(SENSOR_ADDRESS);
    correctedAngle = degAngle - startAngle;
    if (correctedAngle < 0) {
        correctedAngle += 360;
    }
    if (degAngle != 0) {
        SerialMon.println("Wind Direction Sensor OK");
    } else {
        SerialMon.println("Wind Direction Sensor Not Detected");
    }
}

//=============================================== LightSensor ===============================================
BH1750 lightMeter;

// Variables to store light sensor values
String light_str;
String irradiance_str;
float lux = 0;
float irradiance = 0;

// Initialize and read light sensor
void setupLightSensor() {
    SerialMon.print("BH1750: ");
    bool light_status = lightMeter.begin();
    if (!light_status) {
        SerialMon.println(" Failed");
        light_str = "";
        irradiance_str = "";
    } else {
        SerialMon.println(" OK");
        getLight();  // Read sensor values
        light_str = String(lux);
        irradiance_str = String(irradiance);
    }
}

void getLight() {
    lux = lightMeter.readLightLevel();
    irradiance = lux / 683;  // Convert lux to irradiance
}

void Light() {
    getLight();
    SerialMon.print("Light: ");
    SerialMon.print(lux);
    SerialMon.print(" lx, Irradiance: ");
    SerialMon.print(irradiance);
    SerialMon.println(" W/m^2");
}

//=============================================== BME Multiplexer ===============================================
Adafruit_BME280 bme[3];
bool sensorDetected[3];

// Variables to store temperature, humidity, and pressure values
String t_str[3];  // Temperature strings
String h_str[3];  // Humidity strings
String p_str[3];  // Pressure strings

void Multiplexer(uint8_t bus) {
    Wire.beginTransmission(0x70);
    Wire.write(1 << bus);
    Wire.endTransmission();
    delay(5);
}

// Initialize and read BME280 sensors
void setupBME280() {
    for (int i = 0; i < 3; i++) {
        Multiplexer(i + 2);
        bool BME_status = bme[i].begin(0x76);
        sensorDetected[i] = BME_status;

        SerialMon.print("BME");
        SerialMon.print(i + 1);
        if (!BME_status) {
            SerialMon.println(" Failed");
            t_str[i] = "";
            h_str[i] = "";
            p_str[i] = "";
        } else {
            SerialMon.println(" OK");
            float temp, hum, pres;
            getBME(bme[i], i + 2, &temp, &hum, &pres);
            t_str[i] = String(temp);
            h_str[i] = String(hum);
            p_str[i] = String(pres);
        }
    }
}

void getBME(Adafruit_BME280& bme, uint8_t bus, float* temp, float* hum, float* pres) {
    Multiplexer(bus);
    *temp = bme.readTemperature();
    *hum = bme.readHumidity();
    *pres = bme.readPressure() / 100.0F;
}

void BME() {
    printValues(bme[0], "bme1", 2, sensorDetected[0]);
    printValues(bme[1], "bme2", 3, sensorDetected[1]);
    printValues(bme[2], "bme3", 4, sensorDetected[2]);
}

void printValues(Adafruit_BME280& bme, const char* name, int bus, bool detected) {
    Multiplexer(bus);
    SerialMon.print(name);
    if (detected) {
        SerialMon.print(": T=");
        SerialMon.print(bme.readTemperature());
        SerialMon.print(" *C, P=");
        SerialMon.print(bme.readPressure() / 100.0F);
        SerialMon.print(" mb, H=");
        SerialMon.print(bme.readHumidity());
        SerialMon.println(" %");
    } else {
        SerialMon.println(": T=N/A *C, P=N/A mb, H=N/A %");
    }
}

//=============================================== UV Sensor ===============================================
const int uvSensorPin = 34;

float mapVoltageToUVIndex(float voltage) {
    if (voltage < 0.99) return 0;
    else if (voltage < 1.38) return 1;
    else if (voltage < 1.58) return 2;
    else if (voltage < 1.79) return 3;
    else if (voltage < 2.00) return 4;
    else if (voltage < 2.20) return 5;
    else if (voltage < 2.40) return 6;
    else if (voltage < 2.60) return 7;
    else if (voltage < 2.80) return 8;
    else if (voltage < 3.00) return 9;
    else return 10;
}

// Read UV sensor value
void UV() {
    int uvLevel = analogRead(uvSensorPin);
    float voltage = uvLevel * (3.3 / 4095.0);
    float uvIndex = mapVoltageToUVIndex(voltage);
    SerialMon.print("UV Index = ");
    SerialMon.print(uvIndex);
    SerialMon.print(" (Raw: ");
    SerialMon.print(uvLevel);
    SerialMon.println(")");
}

void checkUVSensor() {
    int uvLevel = analogRead(uvSensorPin);
    if (uvLevel > 0) {
        SerialMon.println("UV Sensor OK");
    } else {
        SerialMon.println("UV Sensor Not Detected");
    }
}

//============================================== General sensor check ==============================================
void checkSensors() {
    setupLightSensor();  // Initialize and read light sensor
    delay(10);

    setupBME280();       // Initialize and read BME280 sensors
    delay(10);

    checkUVSensor();    // Check UV sensor
    delay(10);

    setupWindDirection();  // Initialize and check wind direction sensor
    delay(10);
}
