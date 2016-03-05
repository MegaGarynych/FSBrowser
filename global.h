// global.h

#ifndef _GLOBAL_h
#define _GLOBAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <Ticker.h>
#include <ESP8266WiFi.h>

extern boolean firstStart; // On firststart = true, NTP will try to get a valid time
extern int AdminTimeOutCounter; // Counter for Disabling the AdminMode
extern unsigned long UnixTimestamp; // GLOBALTIME  ( Will be set by NTP)
extern Ticker tkSecond; // Second - Timer for Updating Datetime Structure
extern boolean AdminEnabled; // Enable Admin Mode for a given Time
extern int cNTP_Update; // Counter for Updating the time via NTP
extern boolean Refresh; // For Main Loop, to refresh things like GPIO / WS2812
extern boolean wifiIsConnected;
extern long wifiDisconnectedSince;
extern boolean APStarted;
extern boolean APMode;

void ConfigureWifi();
void ConfigureWifiAP();
//void Second_Tick();
String urldecode(String input);
boolean checkRange(String Value);
void WiFiEvent(WiFiEvent_t event);

#endif

