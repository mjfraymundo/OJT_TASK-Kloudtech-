#ifndef SLAVELIB_H
#define SLAVELIB_H

#include <Arduino.h> 
#include <stdint.h>  

#define SLAVE_ADDR1 0x30

extern uint16_t rainCounter;
extern float rain;
extern float tipValue;
extern uint16_t windCounter;
extern float windspeed;
extern int rev;
extern int radius;
extern int period;

extern String rain_str;
extern String winds_str;

void getSlave();
void checkSlaveConnection();
void printSlaveData();

#endif
