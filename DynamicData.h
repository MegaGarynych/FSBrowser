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

void send_general_configuration_html();

void send_general_configuration_values_html();

void send_connection_state_values_html();

void send_network_configuration_values_html();

void send_information_values_html();

void send_NTP_configuration_values_html();

void send_network_configuration_html();

void send_NTP_configuration_html();

void restart_esp();

void send_wwwauth_configuration_values_html();

void send_wwwauth_configuration_html();

void send_update_firmware_values_html();

String GetMacAddress();

void sendTimeData();

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);

#endif

