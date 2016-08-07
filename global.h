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
//#include <WebSocketsServer.h>
//#include <WebSockets.h>
#include "Config.h"

//#ifdef DEBUG
#define DEBUG_GLOBALH
//#endif // DEBUG

#define CONFIG_FILE "/config.json"
#define SECRET_FILE "/secret.json"

typedef enum {
	FIRST_RUN = 0,
	WIFI_STA_CONNECTED,
	WIFI_STA_DISCONNECTED,
	AP_ONLY
} wifiStatus;


extern Ticker secondTk; // Second - Timer to do periodic tasks
extern boolean secondFlag; // Flag to activate periodic task
extern ntpClient* ntp; // NtpClient instance

extern long wifiDisconnectedSince; // When disconnected, time since this situation has happened

extern wifiStatus currentWifiStatus;

//extern WebSocketsServer wsServer; // Socket server to send dynamic data to web browser

/**
* Sets WiFi configuration as config struct
*/
void ConfigureWifi();

/**
* Sets WiFi AP
*/
void ConfigureWifiAP();

/**
* Periodically sets seconfFlag to run secondTask
*/
void secondTick();

/**
* Periodic task to send time data to Web Socket
*/
void secondTask();

/**
* Gets arguments from an URL string
* @param[in] String with URL to decode.
* @param[out] Pointer to ntpClient instance
*/
String urldecode(String input);

/**
* Checks number is in 0-255 range
*/
boolean checkRange(String Value);

/**
* Runs on every WiFi event (connection, disconnection)
*/
void WiFiEvent(WiFiEvent_t event);

/**
* Makes a LED flash n-times
*/
void flashLED(int pin, int times, int delayTime);

/**
* Slowly turn a LED on
*/
void dimLEDon(int pin, int range);

/**
* Sets and start OTA configuration
* @param[in] Password for OTA
*/
void ConfigureOTA(String password="");

#endif

