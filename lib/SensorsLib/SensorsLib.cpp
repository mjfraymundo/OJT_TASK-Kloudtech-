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
float lux = 0;

// Initialize and read light sensor
void setupLightSensor() {
    SerialMon.print("BH1750: ");
    bool light_status = lightMeter.begin();
    if (!light_status) {
        SerialMon.println(" Failed");
        light_str = "";
    } else {
        SerialMon.println(" OK");
        getLight();  // Read sensor values
        light_str = String(lux);
    }
}

void getLight() {
    lux = lightMeter.readLightLevel();
}

void Light() {
    getLight();
    SerialMon.print("Light: ");
    SerialMon.print(lux);
    SerialMon.println(" lx");
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
String uv_str;

// UV Variables
float sensorVoltage = 0;
float sensorValue = 0;
float UV_intensity = 0;

// UV Parameters
void getUV() {
  sensorValue = analogRead(32);  // Reading the analog value from pin 32
  sensorVoltage = sensorValue / 4095.0 * 3.3;  // Converting the sensor value to voltage
  UV_intensity = sensorVoltage * 1000;  // Calculating UV intensity
}

// UV
void UV() {
    getUV();  // Calls the function to get the UV intensity
    uv_str = String(UV_intensity);  // Converts the UV intensity to a string for display
    SerialMon.print("UV Intensity = ");
    SerialMon.print(uv_str);
    SerialMon.println(" mV");
}

void checkUVSensor() {
    int uvLevel = analogRead(32);
    if (uvLevel > 0) {
        SerialMon.println("UV Sensor OK");
    } else {
        SerialMon.println("UV Sensor Not Detected");
    }
}

//=============================================== INA219 ===============================================
Adafruit_INA219 ina219;
float busvoltage = 0;
String volt_str;

void setupINA219() {
    SerialMon.print("Voltage Meter: ");
    bool ina_status = ina219.begin();
    if (!ina_status) {
        SerialMon.println("Failed");
        volt_str = "";
    } else {
        SerialMon.println("OK");
        getINA();
        volt_str = String(busvoltage);
    }
}

void getINA() {
    busvoltage = ina219.getBusVoltage_V();
}

void INA219Sensor() {
    getINA();
    SerialMon.print("Bus Voltage: ");
    SerialMon.print(busvoltage);
    SerialMon.println(" V");
}

//============================================== General sensor check ==============================================
void checkSensors() {
    SerialMon.println("\n======================================== General sensor check ========================================");
    
    setupLightSensor();  // Initialize and read light sensor
    delay(10);

    setupBME280();       // Initialize and read BME280 sensors
    delay(10);

    checkUVSensor();    // Check UV sensor
    delay(10);

    setupWindDirection();  // Initialize and check wind direction sensor
    delay(10);

    setupINA219();  // Initialize and check INA219 sensor
    delay(10);
}
