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
#include <NtpClientLib.h>
#include <WebSocketsServer.h>
#include <WebSockets.h>
#include "Config.h"

#ifdef DEBUG
#define DEBUG_GLOBALH
#endif // DEBUG

typedef enum {
	FIRST_RUN = 0,
	WIFI_STA_CONNECTED,
	WIFI_STA_DISCONNECTED,
	AP_ONLY
} wifiStatus;


extern Ticker secondTk; // Second - Timer to do periodic tasks
extern boolean secondFlag;
extern ntpClient* ntp;

extern long wifiDisconnectedSince;

extern wifiStatus currentWifiStatus;

extern WebSocketsServer wsServer;

extern boolean ledOn;
extern boolean ledOff;


void ConfigureWifi();
void ConfigureWifiAP();
void secondTick();
void secondTask();
String urldecode(String input);
boolean checkRange(String Value);
void WiFiEvent(WiFiEvent_t event);
void flashLED(int pin, int times, int delayTime);
void dimLEDon(int pin, int range);
void ConfigureOTA();

#endif

