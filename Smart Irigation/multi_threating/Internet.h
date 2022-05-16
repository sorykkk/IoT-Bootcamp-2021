#include <WiFi.h>
#ifndef _INTERNET_H_
#define _INTERNET_H_

// WiFi access point
#define WIFI_AP_NAME        "Orange-Tekwill"
// WiFi password
#define WIFI_PASSWORD       ""

void InitWiFi();
void reconnect_if_needed();
void reconnect();

#endif
