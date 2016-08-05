// FSWebServer.h

#ifndef _FSWEBSERVER_h
#define _FSWEBSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//#define HIDE_SECRET // Uncomment to avoid HTTP secret file downloading
#define HIDE_CONFIG // Uncomment to avoid HTTP config file downloading

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

extern AsyncWebServer server;

//holds the current upload
extern File fsUploadFile;
extern String browserMD5;

String formatBytes(size_t bytes);

/**
* Gets content type from filename extension
* @param[in] File name with extension
* @param[out] MIME content type
*/
String getContentType(String filename);

/**
* Gets a file from SPIFFS and streams it to web browser
* @param[in] File name
* @param[out] true if file exists
*/
bool handleFileRead(String path, AsyncWebServerRequest *request);

/**
* Manages file upload from web browser
*/
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

/**
* Manages file deletion of a SPIFFS file, triggered from web browser
*/
void handleFileDelete(AsyncWebServerRequest *request);

/**
* Manages file creation of a SPIFFS file, triggered from web browser
*/
void handleFileCreate(AsyncWebServerRequest *request);

void handleFileList(AsyncWebServerRequest *request);
void serverInit();
void updateFirmware(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
boolean checkAuth(AsyncWebServerRequest *request);
void setUpdateMD5(AsyncWebServerRequest *request);

#endif

