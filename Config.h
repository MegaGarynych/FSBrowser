// Config.h

#ifndef _CONFIG_h
#define _CONFIG_h

#define DEBUG

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
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
	String APssid = "ESP";
	String APpassword = "12345678";
	boolean APEnable = true;
	uint APtimeout = 5;
} strConfig;

/*typedef struct {
	byte hour=10;
	byte minute=15;
	byte second=30;
	int year=2016;
	byte month=02;
	byte day=28;
	byte wday=6;

} strDateTime;*/

/*static const uint8_t monthDays[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
#define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )*/

extern strConfig config;
//extern strDateTime DateTime;

//boolean summertime(int year, byte month, byte day, byte hour, byte tzHours);
unsigned char h2int(char c);
//void ConvertUnixTimeStamp(unsigned long TimeStamp, strDateTime* DateTime);
void defaultConfig();
boolean load_config();
boolean save_config();


#endif

