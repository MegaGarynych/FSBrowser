// 
// 
// 

#define DBG_OUTPUT_PORT Serial

#define DEBUG_DYNAMICDATA

#include "DynamicData.h"
#include "Config.h"
#include "FSWebServer.h"
#include <NtpClientLib.h>
#include <StreamString.h>

extern ntpClient* ntp;
static int numNetwoks = 0;
static bool scanTriggered = false;

#ifdef DEBUG_DYNAMICDATA
int wsNumber = 0;
#endif // DEBUG_DYNAMICDATA

const char Page_WaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="10; URL=/config.html">
Please Wait....Configuring and Restarting.
)=====";

const char Page_Restart[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="10; URL=/general.html">
Please Wait....Configuring and Restarting.
)=====";

void send_general_configuration_values_html( AsyncWebServerRequest *request )
{
	String values = "";
	values += "devicename|" + (String)config.DeviceName + "|input\n";
	request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void onCompleteScan(int n) {
	os_printf("Complete scan %d\r\n", n);
	delay(3000);
	numNetwoks = n;
}

void clearScan(AsyncWebServerRequest *request) {
	WiFi.scanDelete();
	request->send(200, "text/plain", "");
	scanTriggered = false;
	numNetwoks = 0;
}

//
//   FILL THE PAGE WITH NETWORKSTATE & NETWORKS
//

void send_connection_state_values_html(AsyncWebServerRequest *request)
{
	
	//static bool networksReady = false;

	String state = "N/A";
	String Networks = "";
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECT FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";


	//int n = 0;
	if (!scanTriggered) {
		WiFi.scanNetworksAsync(onCompleteScan);
		scanTriggered = true;
	}
	os_printf("Scan result = %d Triggered = %s\r\n", numNetwoks, scanTriggered?"true":"false");
	if (numNetwoks == 0)
	{
		Networks = "<font color='#FF0000'>No networks found!</font>";
		scanTriggered = false;
	}
	else if (numNetwoks == WIFI_SCAN_FAILED) {
		Networks = "<font color='#FF0000'>Error scanning networks!</font>";
		scanTriggered = false;
	}
	else if (numNetwoks == WIFI_SCAN_RUNNING) {
		//Networks = "<font color='#0000FF'>Scanning networks...</font>";
	}
	else
	{
		/*if (!networksReady) {
			networksReady = true;
		} else {*/
			Networks = "Found " + String(numNetwoks) + " Networks<br>";
			Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
			Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
			for (int i = 0; i < numNetwoks; ++i)
			{
				os_printf("Network %d, %s\r\n", i, WiFi.SSID(i).c_str());
				int quality = 0;
				if (WiFi.RSSI(i) <= -100)
				{
					quality = 0;
				}
				else if (WiFi.RSSI(i) >= -50)
				{
					quality = 100;
				}
				else
				{
					quality = 2 * (WiFi.RSSI(i) + 100);
				}


				Networks += "<tr><td><a href='javascript:selssid(\"" + String(WiFi.SSID(i)) + "\")'>" + String(WiFi.SSID(i)) + "</a></td><td>" + String(quality) + "%</td><td>" + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*") + "</td></tr>";
			}
			Networks += "</table>";
			//scanTriggered = false;
			//networksReady = false;
		//}
	}

	//if ((numNetwoks != WIFI_SCAN_RUNNING) ) {
		String values = "";
		values += "connectionstate|" + state + "|div\n";
		values += "networks|" + Networks + "|div\n";
		request->send(200, "text/plain", values);
	//}
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

//
//   FILL THE PAGE WITH VALUES
//

void send_network_configuration_values_html(AsyncWebServerRequest *request)
{

	String values = "";

	values += "ssid|" + (String)config.ssid + "|input\n";
	values += "password|" + (String)config.password + "|input\n";
	values += "ip_0|" + (String)config.IP[0] + "|input\n";
	values += "ip_1|" + (String)config.IP[1] + "|input\n";
	values += "ip_2|" + (String)config.IP[2] + "|input\n";
	values += "ip_3|" + (String)config.IP[3] + "|input\n";
	values += "nm_0|" + (String)config.Netmask[0] + "|input\n";
	values += "nm_1|" + (String)config.Netmask[1] + "|input\n";
	values += "nm_2|" + (String)config.Netmask[2] + "|input\n";
	values += "nm_3|" + (String)config.Netmask[3] + "|input\n";
	values += "gw_0|" + (String)config.Gateway[0] + "|input\n";
	values += "gw_1|" + (String)config.Gateway[1] + "|input\n";
	values += "gw_2|" + (String)config.Gateway[2] + "|input\n";
	values += "gw_3|" + (String)config.Gateway[3] + "|input\n";
	values += "dns_0|" + (String)config.DNS[0] + "|input\n";
	values += "dns_1|" + (String)config.DNS[1] + "|input\n";
	values += "dns_2|" + (String)config.DNS[2] + "|input\n";
	values += "dns_3|" + (String)config.DNS[3] + "|input\n";
	values += "dhcp|" + (String)(config.dhcp ? "checked" : "") + "|chk\n";
	
	request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

//
// FILL WITH INFOMATION
// 

void send_information_values_html(AsyncWebServerRequest *request)
{

	String values = "";

	values += "x_ssid|" + (String)WiFi.SSID() + "|div\n";
	values += "x_ip|" + (String)WiFi.localIP()[0] + "." + (String)WiFi.localIP()[1] + "." + (String)WiFi.localIP()[2] + "." + (String)WiFi.localIP()[3] + "|div\n";
	values += "x_gateway|" + (String)WiFi.gatewayIP()[0] + "." + (String)WiFi.gatewayIP()[1] + "." + (String)WiFi.gatewayIP()[2] + "." + (String)WiFi.gatewayIP()[3] + "|div\n";
	values += "x_netmask|" + (String)WiFi.subnetMask()[0] + "." + (String)WiFi.subnetMask()[1] + "." + (String)WiFi.subnetMask()[2] + "." + (String)WiFi.subnetMask()[3] + "|div\n";
	values += "x_mac|" + GetMacAddress() + "|div\n";
	values += "x_dns|" + (String)WiFi.dnsIP()[0] + "." + (String)WiFi.dnsIP()[1] + "." + (String)WiFi.dnsIP()[2] + "." + (String)WiFi.dnsIP()[3] + "|div\n";
	values += "x_ntp_sync|" + ntp->getTimeString(ntp->getLastNTPSync()) + "|div\n";
	values += "x_ntp_time|" + ntp->getTimeStr() + "|div\n";
	values += "x_ntp_date|" + ntp->getDateStr() + "|div\n";
	
	request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

String GetMacAddress()
{
	uint8_t mac[6];
	char macStr[18] = { 0 };
	WiFi.macAddress(mac);
	sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return  String(macStr);
}

void send_NTP_configuration_values_html(AsyncWebServerRequest *request)
{

	String values = "";
	values += "ntpserver|" + (String)config.ntpServerName + "|input\n";
	values += "update|" + (String)config.Update_Time_Via_NTP_Every + "|input\n";
	values += "tz|" + (String)config.timezone + "|input\n";
	values += "dst|" + (String)(config.daylight ? "checked" : "") + "|chk\n";
	request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

void send_network_configuration_html(AsyncWebServerRequest *request)
{
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
	if (request->args() > 0)  // Save Settings
	{
		//String temp = "";
		bool oldDHCP = config.dhcp; // Save status to avoid general.html cleares it
		config.dhcp = false;
		for (uint8_t i = 0; i < request->args(); i++) {
#ifdef DEBUG_DYNAMICDATA
			DBG_OUTPUT_PORT.printf("Arg %d: %s\n", i, request->arg(i).c_str());
#endif // DEBUG_DYNAMICDATA
			if (request->argName(i) == "devicename") {
				config.DeviceName = urldecode(request->arg(i));
				config.dhcp = oldDHCP;
				continue; }
			if (request->argName(i) == "ssid") { config.ssid = urldecode(request->arg(i));	continue; }
			if (request->argName(i) == "password") {	config.password = urldecode(request->arg(i)); continue; }
			if (request->argName(i) == "ip_0") {	if (checkRange(request->arg(i))) 	config.IP[0] = request->arg(i).toInt(); continue;	}
			if (request->argName(i) == "ip_1") {  if (checkRange(request->arg(i))) 	config.IP[1] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "ip_2") {  if (checkRange(request->arg(i))) 	config.IP[2] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "ip_3") {  if (checkRange(request->arg(i))) 	config.IP[3] = request->arg(i).toInt(); continue;	}
			if (request->argName(i) == "nm_0") {  if (checkRange(request->arg(i))) 	config.Netmask[0] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "nm_1") {  if (checkRange(request->arg(i))) 	config.Netmask[1] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "nm_2") {  if (checkRange(request->arg(i))) 	config.Netmask[2] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "nm_3") {  if (checkRange(request->arg(i))) 	config.Netmask[3] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "gw_0") {  if (checkRange(request->arg(i))) 	config.Gateway[0] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "gw_1") {  if (checkRange(request->arg(i))) 	config.Gateway[1] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "gw_2") {  if (checkRange(request->arg(i))) 	config.Gateway[2] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "gw_3") {  if (checkRange(request->arg(i))) 	config.Gateway[3] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "dns_0") { if (checkRange(request->arg(i))) 	config.DNS[0] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "dns_1") { if (checkRange(request->arg(i))) 	config.DNS[1] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "dns_2") { if (checkRange(request->arg(i))) 	config.DNS[2] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "dns_3") { if (checkRange(request->arg(i))) 	config.DNS[3] = request->arg(i).toInt(); continue; }
			if (request->argName(i) == "dhcp") { config.dhcp = true; continue; }
		}
		request->send(200, "text/html", Page_WaitAndReload);
		save_config();
		yield();
		delay(1000);
		ESP.restart();
		//ConfigureWifi();
		//AdminTimeOutCounter = 0;
	}
	else
	{
		DBG_OUTPUT_PORT.println(request->url());
		handleFileRead(request->url(),request);
	}
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void send_general_configuration_html(AsyncWebServerRequest *request)
{
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
	if (request->args() > 0)  // Save Settings
	{
		for (uint8_t i = 0; i < request->args(); i++) {
#ifdef DEBUG_DYNAMICDATA
			DBG_OUTPUT_PORT.printf("Arg %d: %s\n", i, request->arg(i).c_str());
#endif // DEBUG_DYNAMICDATA
			if (request->argName(i) == "devicename") {
				config.DeviceName = urldecode(request->arg(i));
				continue;
			}
		}
		request->send(200, "text/html", Page_Restart);
		save_config();
		ESP.restart();
		//ConfigureWifi();
		//AdminTimeOutCounter = 0;
	}
	else
	{
		handleFileRead(request->url(), request);
	}
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void send_NTP_configuration_html(AsyncWebServerRequest *request)
{
	if (request->args() > 0)  // Save Settings
	{
		config.daylight = false;
		//String temp = "";
		for (uint8_t i = 0; i < request->args(); i++) {
			if (request->argName(i) == "ntpserver") {
				config.ntpServerName = urldecode(request->arg(i));
				ntp->setNtpServerName(config.ntpServerName);
				continue;
			}
			if (request->argName(i) == "update") {
				config.Update_Time_Via_NTP_Every = request->arg(i).toInt();
				ntp->setInterval(config.Update_Time_Via_NTP_Every * 60);
				continue;
			}
			if (request->argName(i) == "tz") {
				config.timezone = request->arg(i).toInt();
				ntp->setTimeZone(config.timezone / 10);
				continue;
			}
			if (request->argName(i) == "dst") {
				config.daylight = true;
#ifdef DEBUG_DYNAMICDATA
				DBG_OUTPUT_PORT.printf("Daylight Saving: %d\n", config.daylight);
#endif // DEBUG_DYNAMICDATA
				continue;
			}
		}
		
		ntp->setDayLight(config.daylight);
		save_config();
		//firstStart = true;
		
		setTime(ntp->getTime()); //set time
	}
	handleFileRead("/ntp.html", request);
	//server.send(200, "text/html", PAGE_NTPConfiguration);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

void restart_esp(AsyncWebServerRequest *request) {
	//server.send(200, "text/html", Page_Restart);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
	SPIFFS.end();
	delay(1000);
	ESP.restart();
}

void send_wwwauth_configuration_values_html(AsyncWebServerRequest *request) {
	String values = "";

	values += "wwwauth|" + (String)(httpAuth.auth ? "checked" : "") + "|chk\n";
	values += "wwwuser|" + (String)httpAuth.wwwUsername + "|input\n";
	values += "wwwpass|" + (String)httpAuth.wwwPassword + "|input\n";
	
	request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void send_wwwauth_configuration_html(AsyncWebServerRequest *request)
{
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.printf("%s %d\n",__FUNCTION__, request->args());
#endif // DEBUG_DYNAMICDATA
	if (request->args() > 0)  // Save Settings
	{
		httpAuth.auth = false;
		//String temp = "";
		for (uint8_t i = 0; i < request->args(); i++) {
			if (request->argName(i) == "wwwuser") {
				httpAuth.wwwUsername = urldecode(request->arg(i));
#ifdef DEBUG_DYNAMICDATA
				DBG_OUTPUT_PORT.printf("User: %s\n", httpAuth.wwwUsername.c_str());
#endif // DEBUG_DYNAMICDATA
				continue;
			}
			if (request->argName(i) == "wwwpass") {
				httpAuth.wwwPassword = urldecode(request->arg(i));
#ifdef DEBUG_DYNAMICDATA
				DBG_OUTPUT_PORT.printf("Pass: %s\n", httpAuth.wwwPassword.c_str());
#endif // DEBUG_DYNAMICDATA
				continue;
			}
			if (request->argName(i) == "wwwauth") {
				httpAuth.auth = true;
#ifdef DEBUG_DYNAMICDATA
				DBG_OUTPUT_PORT.printf("HTTP Auth enabled\n");
#endif // DEBUG_DYNAMICDATA
				continue;
			}
		}

		saveHTTPAuth();
	}
	handleFileRead("/system.html", request);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

void send_update_firmware_values_html(AsyncWebServerRequest *request) {
	String values = "";
	uint32_t maxSketchSpace = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
	//bool updateOK = Update.begin(maxSketchSpace);
	bool updateOK = maxSketchSpace < ESP.getFreeSketchSpace();
	StreamString result;
	Update.printError(result);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.printf("--MaxSketchSpace: %d\n", maxSketchSpace);
	DBG_OUTPUT_PORT.printf("--Update error = %s\n", result.c_str());
#endif // DEBUG_DYNAMICDATA
	values += "remupd|" + (String)((updateOK) ? "OK" : "ERROR") + "|div\n";
	
	if (Update.hasError()) {
		result.trim();
		values += "remupdResult|" + result + "|div\n";
	}
	else {
		values += "remupdResult||div\n";
	}
	
	request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void sendTimeData() {
	String time = "T" + ntp->getTimeStr();
	ws.textAll(time);
	String date = "D" + ntp->getDateStr();
	ws.textAll(date);
	String sync = "S" + ntp->getTimeString(ntp->getLastNTPSync());
	ws.textAll(sync);
#ifdef DEBUG_DYNAMICDATA
	//DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void webSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *payload, size_t length) {

	if (type == WS_EVT_DISCONNECT) {
#ifdef DEBUG_DYNAMICDATA
		os_printf("[%u] Disconnected!\n", client->id());
#endif // DEBUG_DYNAMICDATA
	} else if (type== WS_EVT_CONNECT) {
#ifdef DEBUG_DYNAMICDATA
		wsNumber = client->id();
		IPAddress ip = client->remoteIP();
		os_printf("[%u] Connected from %d.%d.%d.%d url: %s\n", client->id(), ip[0], ip[1], ip[2], ip[3], payload);
#endif // DEBUG_DYNAMICDATA

		// send message to client
		//wsServer.sendTXT(num, "Connected");
	}
	else if (type == WS_EVT_DATA) {
		AwsFrameInfo * info = (AwsFrameInfo*)arg;
		String msg = "";
		if (info->final && info->index == 0 && info->len == length) { 	//the whole message is in a single frame and we got all of it's data

			if (info->opcode == WS_TEXT) {
				for (size_t i = 0; i < info->len; i++) {
					msg += (char)payload[i];
				}
			}
			else { // Binary
				char buff[3];
				for (size_t i = 0; i < info->len; i++) {
					sprintf(buff, "%02x ", (uint8_t)payload[i]);
					msg += buff;
				}
			}
#ifdef DEBUG_DYNAMICDATA
			os_printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
			os_printf("%s\r\n", msg.c_str());
#endif // DEBUG_DYNAMICDATA
		}
		else {	//message is comprised of multiple frames or the frame is split into multiple packets
			if (info->index == 0) {
				if (info->num == 0)
					os_printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
				os_printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
			}
			os_printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + length);

			if (info->opcode == WS_TEXT) { // Text
				for (size_t i = 0; i < info->len; i++) {
					msg += (char)payload[i];
				}
			}
			else { // Binary
				char buff[3];
				for (size_t i = 0; i < info->len; i++) {
					sprintf(buff, "%02x ", (uint8_t)payload[i]);
					msg += buff;
				}
			}
			os_printf("%s\r\n", msg.c_str());

			if ((info->index + length) == info->len) {
				os_printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
				if (info->final) {
					os_printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
					if (info->message_opcode == WS_TEXT)
						client->text("I got your text message");
					else
						client->binary("I got your binary message");
				}
			}
		}
		// send message to client
		//client->text("message here");

		// send data to all connected clients
		//client->message("message here");
		// webSocket.broadcastTXT("message here");
	}

}
