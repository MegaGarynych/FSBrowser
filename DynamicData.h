// DynamicData.h

#ifndef _DYNAMICDATA_h
#define _DYNAMICDATA_h

#include "Config.h"
#include "global.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif



// ------------------ Web dynamic data methods -------------------------------- //

void send_general_configuration_html(AsyncWebServerRequest *request);

void send_general_configuration_values_html(AsyncWebServerRequest *request);

void send_connection_state_values_html(AsyncWebServerRequest *request);

void send_network_configuration_values_html(AsyncWebServerRequest *request);

void send_information_values_html(AsyncWebServerRequest *request);

void send_NTP_configuration_values_html(AsyncWebServerRequest *request);

void send_network_configuration_html(AsyncWebServerRequest *request);

void send_NTP_configuration_html(AsyncWebServerRequest *request);

void restart_esp(AsyncWebServerRequest *request);

void send_wwwauth_configuration_values_html(AsyncWebServerRequest *request);

void send_wwwauth_configuration_html(AsyncWebServerRequest *request);

void send_update_firmware_values_html(AsyncWebServerRequest *request);

String GetMacAddress();

void sendTimeData();

void clearScan(AsyncWebServerRequest *request);

void webSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *payload, size_t lenght);

#endif

