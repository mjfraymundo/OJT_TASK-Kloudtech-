#include "SlaveLib.h"
#include <Wire.h>

#define SerialMon Serial
#define PI 3.1415926535897932384626433832795 

String rain_str;
String winds_str;
uint16_t rainCounter = 0;
float rain = 0;
float tipValue = 0;
uint16_t windCounter = 0;
float windspeed = 0;
int rev = 0;
int radius = 0;
int period = 10000;

void getSlave() {
  tipValue = 0.1099;
  Wire.beginTransmission(SLAVE_ADDR1);  
  Wire.endTransmission();              
  Wire.requestFrom(SLAVE_ADDR1, 4);    

  if (Wire.available() == 4) {
    rainCounter = Wire.read() << 8; 
    rainCounter |= Wire.read();       
    rain = rainCounter * tipValue;
  }
  if (Wire.available() == 2){
    windCounter = Wire.read() << 8; 
    windCounter |= Wire.read();       
    rev = windCounter;
    radius = 100;
    windspeed = (2 * PI * radius * rev * 3.6) / (period * 1000);
  }
}

void checkSlaveConnection() {
  Wire.beginTransmission(SLAVE_ADDR1); 
  Wire.endTransmission();             
  SerialMon.print("Slave: ");
  bool slave_status = Wire.endTransmission() == 0;
  if(!slave_status){
   SerialMon.println("Failed");
    rain_str = "";
    winds_str = "";
  }else{
   SerialMon.println("OK");
    getSlave();
    rain_str = String(rain);
    winds_str = String(windspeed);
  }

}

void printSlaveData() {
  SerialMon.print("Rain: ");Serial.print(rain);Serial.println("mm");
  SerialMon.print("Windspeed: ");Serial.print(windspeed); Serial.println("km/h");
  SerialMon.println("");
}
