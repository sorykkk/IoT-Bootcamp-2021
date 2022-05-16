#include "Sensor.h"
#include "FreeRTOS.h"
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(SOIL_TEMP_PIN);
DallasTemperature Stemp(&oneWire);

float temp, hum, soil_temp;
int moi, rain, is_raining, light;

DHT dht(DHT_PIN, DHT_TYPE);

void TaskSensorSetup()
{
  dht.begin();
  Stemp.begin();
  pinMode(LDR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  xTaskCreatePinnedToCore(
    TaskSensorLoop,
    "TaskSensor",   // A name just for humans
    1024,          // This stack size can be checked & adjusted by reading the Stack Highwater
    NULL,
    2,             // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    NULL, 
    ARDUINO_RUNNING_CORE
    );
}
void TaskSensorLoop(void *pvParameters)
{
  (void) pvParameters;

  for (;;)        // A Task shall never return or exit.
  {
    //geters
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    moi = analogRead(MOISTURE_PIN);
    moi = map(moi, 4096, 0, 0, 100);

    Stemp.requestTemperatures(); 
    soil_temp = Stemp.getTempCByIndex(0);

    is_raining= digitalRead(RAIN_PIN);
    light = digitalRead(LDR_PIN);

    vTaskDelay(1000);
  }
}

//all geters
float SensorGetTemperature(void) { return temp;        }
float SensorGetHumidity(void)    { return hum;         }
int   SensorGetMoisture(void)    { return moi;         }
int   SensorGetRain()            { return !is_raining; }
float SensorGetSoilTemperature() { return soil_temp;   }
int   SensorGetLight()           { return !light;      }
