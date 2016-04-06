/* 
  FSWebServer - Example WebServer with SPIFFS backend for esp8266
  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WebServer library for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
  or you can upload the contents of a folder if you CD in that folder and run the following command:
  for file in `ls -A1`; do curl -F "file=@$PWD/$file" esp8266fs.local/edit; done
  
  access the sample web page at http://esp8266fs.local
  edit the page by going to http://esp8266fs.local/edit
*/

#include <WebSocketsServer.h>
#include <WebSockets.h>
#include <Hash.h>
#include "global.h"
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <NtpClientLib.h>
#include "DynamicData.h"
#include "Config.h"
#include "FSWebServer.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Ticker.h>
#include <ArduinoOTA.h>

#define DBG_OUTPUT_PORT Serial





void setup(void){
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.print("\n");
#ifdef DEBUG
  DBG_OUTPUT_PORT.setDebugOutput(true);
#endif // DEBUG
  pinMode(CONNECTION_LED, OUTPUT); // CONNECTION_LED pin defined as output
  pinMode(AP_ENABLE_BUTTON, INPUT); // If this pin is HIGH during startup ESP will run in AP_ONLY mode. Backdoor to change WiFi settings when configured WiFi is not available.
  
  //analogWriteFreq(200);
  
  secondTk.attach( 1 , secondTick); // Task to run periodic things every second
  
  apConfig.APenable = digitalRead(AP_ENABLE_BUTTON); // Read AP button
#ifdef DEBUG
  DBG_OUTPUT_PORT.printf("AP Enable = %d\n", apConfig.APenable);
#endif // DEBUG
  
  digitalWrite(CONNECTION_LED, HIGH); // Turn LED off
  
  WiFi.onEvent(WiFiEvent); // Register wifi Event to control connection LED
  
						   //File System Init
  SPIFFS.begin();
#ifdef DEBUG
  { // List files
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();

	  DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
	}
	DBG_OUTPUT_PORT.printf("\n");
  }
#endif // DEBUG
  
  if (!load_config()) { // Try to load configuration from SPIFFS
	  defaultConfig(); // Load defaults if any error
  }
  loadHTTPAuth();

  //WIFI INIT
  WiFi.hostname(config.DeviceName.c_str());
  if (apConfig.APenable) {
	  ConfigureWifiAP(); // Set AP mode if AP button was pressed
  }
  else {
	  ConfigureWifi(); // Set WiFi config
  }
  delay(5000); // Wait for WiFi
#ifdef DEBUG
  DBG_OUTPUT_PORT.print("Open http://");
  DBG_OUTPUT_PORT.print(config.DeviceName);
  DBG_OUTPUT_PORT.println(".local/edit to see the file browser");
#endif
  // NTP client setup
  ntp = ntpClient::getInstance(config.ntpServerName, config.timezone/10 , config.daylight);
  if (config.Update_Time_Via_NTP_Every > 0) { // Enable NTP sync
	  ntp->setInterval(15, config.Update_Time_Via_NTP_Every*60);
	  ntp->begin();
  }
  serverInit(); // Configure and start Web server

  // Web socket server setup
  wsServer.begin();
  wsServer.onEvent(webSocketEvent);

  

  ConfigureOTA();

  //MDNS.begin(config.DeviceName.c_str()); // I've not got this to work. Need some investigation.
  MDNS.addService("http", "tcp", 80);
#ifdef DEBUG
  DBG_OUTPUT_PORT.println("END Setup");
#endif // DEBUG
}
 
void loop(void){
  server.handleClient(); // Handle Web server requests
  if (secondFlag) { // Run periodic tasks
	  secondFlag = false;
	  secondTask(); 
  }
  wsServer.loop(); // Handle WebSocket server requests
  ArduinoOTA.handle();
}
