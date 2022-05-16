#include "config.h"//configurations for freertos
#ifndef _SENSOR_H_
#define _SENSOR_H_
//pins
#define DHT_PIN 4 //digital
#define MOISTURE_PIN 32 //analog 
#define SOIL_TEMP_PIN 5//digital
#define RAIN_PIN 15//digital
#define LDR_PIN 35//digital
#define RELAY_PIN 16//digital

//defines
#define DHT_TYPE DHT11  

//functions

//freertos
void TaskSensorSetup();
void TaskSensorLoop(void *pvParameters);

//dht11
float SensorGetTemperature(void);
float SensorGetHumidity(void);

//rain sensor
int SensorGetRain();

//moisture sensor
int SensorGetMoisture();

//DS18B20
float SensorGetSoilTemperature();
//LDR LM393
int SensorGetLight();


#endif
