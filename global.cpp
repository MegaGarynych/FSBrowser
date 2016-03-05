// 
// 
// 

#define DBG_OUTPUT_PORT Serial

#include "global.h"
#include "Config.h"
#include <ESP8266WiFi.h>

boolean firstStart = true;
int AdminTimeOutCounter = 0;
unsigned long UnixTimestamp = 0;
Ticker tkSecond;
boolean AdminEnabled = true;
int cNTP_Update = 0;
boolean Refresh = false;
boolean wifiIsConnected = false;
long wifiDisconnectedSince = 0;
boolean APStarted = false;
boolean APMode = false;
wifiStatus currentWifiStatus = FIRST_RUN;

void ConfigureWifi()
{
	if (config.APEnable) {
		WiFi.mode(WIFI_AP_STA);
	}
	else {
		WiFi.mode(WIFI_STA);
	}
	WiFi.enableAP(false);
	DBG_OUTPUT_PORT.printf("Connecting to %s\n", config.ssid.c_str());
	WiFi.begin(config.ssid.c_str(), config.password.c_str());
	if (!config.dhcp)
	{
		DBG_OUTPUT_PORT.println("NO DHCP");
		WiFi.config(
			IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3]), 
			IPAddress(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3]), 
			IPAddress(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3]),
			IPAddress(config.DNS[0], config.DNS[1], config.DNS[2], config.DNS[3])
		);
	}
	delay(2000);
	while (!WL_CONNECTED) {
		delay(1000);
		Serial.print(".");
	}
	/*if (WL_CONNECTED) {
		currentWifiStatus = WIFI_STA_CONNECTED;
	}*/
	//DBG_OUTPUT_PORT.println("");
	//DBG_OUTPUT_PORT.print("Connected! IP address: ");
	DBG_OUTPUT_PORT.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
	DBG_OUTPUT_PORT.printf("Gateway:    %s\n", WiFi.gatewayIP().toString().c_str());
	DBG_OUTPUT_PORT.printf("DNS:        %s\n", WiFi.dnsIP().toString().c_str());
	Serial.println(__PRETTY_FUNCTION__);
}

void ConfigureWifiAP() {
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
	WiFi.disconnect();
	WiFi.mode(WIFI_AP);
	WiFi.softAP(config.APssid.c_str(), config.APpassword.c_str());
	
}

/*void Second_Tick()
{
	strDateTime tempDateTime;
	AdminTimeOutCounter++;
	cNTP_Update++;
	UnixTimestamp++;
	ConvertUnixTimeStamp(UnixTimestamp + (config.timezone * 360), &tempDateTime);
	if (config.daylight) // Sommerzeit beachten
		if (summertime(tempDateTime.year, tempDateTime.month, tempDateTime.day, tempDateTime.hour, 0))
		{
			ConvertUnixTimeStamp(UnixTimestamp + (config.timezone * 360) + 3600, &DateTime);
		}
		else
		{
			DateTime = tempDateTime;
		}
	else
	{
		DateTime = tempDateTime;
	}
	Refresh = true;
}*/

String urldecode(String input) // (based on https://code.google.com/p/avr-netino/)
{
	char c;
	String ret = "";

	for (byte t = 0; t<input.length(); t++)
	{
		c = input[t];
		if (c == '+') c = ' ';
		if (c == '%') {


			t++;
			c = input[t];
			t++;
			c = (h2int(c) << 4) | h2int(input[t]);
		}

		ret.concat(c);
	}
	return ret;

}

//
// Check the Values is between 0-255
//
boolean checkRange(String Value)
{
	if (Value.toInt() < 0 || Value.toInt() > 255)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void WiFiEvent(WiFiEvent_t event) {
	DBG_OUTPUT_PORT.printf("%s: %d\n",__PRETTY_FUNCTION__,event);
	switch (event) {
		case WIFI_EVENT_STAMODE_GOT_IP:
			//DBG_OUTPUT_PORT.println(event);
			digitalWrite(0, LOW);
			//wifiIsConnected = true;
			wifiDisconnectedSince = 0;
			APMode = false;
			currentWifiStatus = WIFI_STA_CONNECTED;
			break;
		case WIFI_EVENT_STAMODE_DISCONNECTED:
			//DBG_OUTPUT_PORT.println(event);
			digitalWrite(0, HIGH);
			if (currentWifiStatus == WIFI_STA_CONNECTED) {
				currentWifiStatus == WIFI_STA_DISCONNECTED;
				//wifiIsConnected = false;
				wifiDisconnectedSince = millis();
			}
			DBG_OUTPUT_PORT.println((millis() - wifiDisconnectedSince));
			if (!config.APEnable) {
				DBG_OUTPUT_PORT.println("Disabling STA. Starting AP");
				if ((millis() - wifiDisconnectedSince) > 10000) {
					DBG_OUTPUT_PORT.println("AP mode started");
					ConfigureWifiAP();
					APMode = true;
				}
			}
	}
}