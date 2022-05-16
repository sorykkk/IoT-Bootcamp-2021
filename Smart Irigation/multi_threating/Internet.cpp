#include "Internet.h"

// the Wifi radio's status
int status = WL_IDLE_STATUS;

void InitWiFi() 
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

// Reconnect to WIFI
void reconnect() 
{
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}

void reconnect_if_needed()
{
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
    return;
  }
}
