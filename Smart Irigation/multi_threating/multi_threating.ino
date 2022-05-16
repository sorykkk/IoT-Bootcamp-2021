#include <ThingsBoard.h>
#include "Sensor.h"
#include "FreeRTOS.h"
#include "Internet.h"

// Baudrate for serial monitor
#define SERIAL_DEBUG_BAUD    115200
// ThingsBoard server instance.
#define THINGSBOARD_SERVER  "172.31.124.124"
#define TOKEN               "GM-3"

//Used for another purpose
#define WATER_PLANTS_RELAY_PIN  2//LED_BUILTIN //Not used
#define WATER_PLANTS_TIME_MS    1000
#define TELEMETRY_PERIOD_MS     2000

const char* mqtt_server = "broker.mqttdashboard.com";

// Set to true if application is subscribed for the RPC messages.
static bool subscribed = false;
// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
PubSubClient client(espClient);
// Protects the tb instance
SemaphoreHandle_t tb_mutex;

//used for another purposes
RPC_Response processWaterPlants(const RPC_Data &data) 
{
  Serial.println("Received the waterPlants RPC method");
  digitalWrite(RELAY_PIN, HIGH);
  delay(WATER_PLANTS_TIME_MS);
  digitalWrite(RELAY_PIN, LOW);

  return RPC_Response(NULL, true);
}

// RPC handlers
RPC_Callback callbacks[] = {
  { "setValue",         processWaterPlants }
};

// Setup an application
void setup() {
  TaskSensorSetup();
  
  // Initialize serial for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  InitWiFi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  tb_mutex = xSemaphoreCreateMutex();

  xTaskCreate(telemetryTask, "TelemetryTask", 8196, NULL, tskIDLE_PRIORITY + 2, NULL);
}

// Main application loop
// Handle internet connection and processes messages from thingsboard
void loop() {

  delay(100);
  client.loop();
  // Reconnect to WiFi, if needed
  reconnect_if_needed();

  // Reconnect to ThingsBoard, if needed
  if (!tb.connected()) {
    subscribed = false;
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  // Subscribe for RPC, if needed
  if (!subscribed) {
    Serial.println("Subscribing for RPC...");

    // Perform a subscription. All consequent data processing will happen in
    // callbacks as denoted by callbacks[] array.
    if (!tb.RPC_Subscribe(callbacks, 1)) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }

    Serial.println("Subscribe done");
    subscribed = true;
  }

  // Process messages
  xSemaphoreTake(tb_mutex, portMAX_DELAY);
  tb.loop();
  xSemaphoreGive(tb_mutex);
}



static void telemetryTask(void* pvParameters) {
  while (true) {
    // For simplicity, we will ignore concurrent access to tb.connected() internal state
    if (tb.connected()) {
      xSemaphoreTake(tb_mutex, portMAX_DELAY);
      Serial.println("Sending telemetry");
      tb.sendTelemetryFloat("Temperature", SensorGetTemperature());  
      tb.sendTelemetryFloat("Humidity", SensorGetHumidity());
      tb.sendTelemetryFloat("Rain", SensorGetRain());
      tb.sendTelemetryInt  ("Moisture", SensorGetMoisture());
      tb.sendTelemetryFloat("Soil temperature", SensorGetSoilTemperature());
      tb.sendTelemetryInt("Light", SensorGetLight());
      xSemaphoreGive(tb_mutex);
    }
    vTaskDelay(TELEMETRY_PERIOD_MS);
  }
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if((char)payload[length-3]=='s') digitalWrite(RELAY_PIN, LOW); //false
  else if ((char)payload[length-3]=='u')digitalWrite(RELAY_PIN, HIGH); //true
  else digitalWrite(RELAY_PIN, LOW);
}
