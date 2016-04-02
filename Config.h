// Config.h

#ifndef _CONFIG_h
#define _CONFIG_h

#define DEBUG

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define CONNECTION_LED 0 // Connection LED pin (External)
//#define CONNECTION_LED 2 // Connection LED pin (Built in)
#define AP_ENABLE_BUTTON 4 // Button pin to enable AP during startup for configuration

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

typedef struct {
	String ssid;
	String password;
	byte  IP[4];
	byte  Netmask[4];
	byte  Gateway[4];
	byte  DNS[4];
	boolean dhcp;
	String ntpServerName;
	long Update_Time_Via_NTP_Every;
	long timezone;
	boolean daylight;
	String DeviceName;
} strConfig;

typedef struct {
	String APssid = "ESP"; // ChipID is appended to this name
	String APpassword = "12345678";
	boolean APenable = false; // AP disabled by default
} strApConfig;

extern strConfig config; // General and WiFi configuration
extern strApConfig apConfig; // Static AP config settings

unsigned char h2int(char c);

void defaultConfig();

boolean load_config();

boolean save_config();


#endif

