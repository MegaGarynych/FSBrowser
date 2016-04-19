// 
// 
// 
#define DBG_OUTPUT_PORT Serial

#define DEBUG_WEBSERVER

#include "FSWebServer.h"
#include "DynamicData.h"
#include "Config.h"

ESP8266WebServer server(80);
File fsUploadFile;
String browserMD5="";

//format bytes
String formatBytes(size_t bytes) {
	if (bytes < 1024) {
		return String(bytes) + "B";
	}
	else if (bytes < (1024 * 1024)) {
		return String(bytes / 1024.0) + "KB";
	}
	else if (bytes < (1024 * 1024 * 1024)) {
		return String(bytes / 1024.0 / 1024.0) + "MB";
	}
	else {
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
	}
}

String getContentType(String filename) {
	if (server.hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".json")) return "application/json";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

bool handleFileRead(String path) {
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println("handleFileRead: " + path);
#endif // DEBUG_WEBSERVER
	if (CONNECTION_LED >= 0) {
		flashLED(CONNECTION_LED, 1, 25); // Show activity on LED
	}
	if (path.endsWith("/"))
		path += "index.htm";
	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if (SPIFFS.exists(pathWithGz))
			path += ".gz";
		File file = SPIFFS.open(path, "r");
		size_t sent = server.streamFile(file, contentType);
		//DBG_OUTPUT_PORT.printf("File %s exist\n", file.name());
		file.close();
		return true;
	}
#ifdef DEBUG_WEBSERVER
	else
		DBG_OUTPUT_PORT.printf("Cannot find %s\n", path.c_str());
#endif // DEBUG_WEBSERVER
	return false;
}

void handleFileUpload() {
	if (server.uri() != "/edit") return;
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/")) filename = "/" + filename;
#ifdef DEBUG_WEBSERVER
		DBG_OUTPUT_PORT.printf("handleFileUpload Name: %s\n", filename.c_str());
#endif // DEBUG_WEBSERVER
		fsUploadFile = SPIFFS.open(filename, "w");
		filename = String();
	}
	else if (upload.status == UPLOAD_FILE_WRITE) {
		//DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
		if (fsUploadFile)
			fsUploadFile.write(upload.buf, upload.currentSize);
	}
	else if (upload.status == UPLOAD_FILE_END) {
		if (fsUploadFile)
			fsUploadFile.close();
#ifdef DEBUG_WEBSERVER
		DBG_OUTPUT_PORT.printf("handleFileUpload Size: %u\n", upload.totalSize);
#endif // DEBUG_WEBSERVER
	}
}

void handleFileDelete() {
	if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
#endif // DEBUG_WEBSERVER
	if (path == "/")
		return server.send(500, "text/plain", "BAD PATH");
	if (!SPIFFS.exists(path))
		return server.send(404, "text/plain", "FileNotFound");
	SPIFFS.remove(path);
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileCreate() {
	if (server.args() == 0)
		return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
#endif // DEBUG_WEBSERVER
	if (path == "/")
		return server.send(500, "text/plain", "BAD PATH");
	if (SPIFFS.exists(path))
		return server.send(500, "text/plain", "FILE EXISTS");
	File file = SPIFFS.open(path, "w");
	if (file)
		file.close();
	else
		return server.send(500, "text/plain", "CREATE FAILED");
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileList() {
	if (!server.hasArg("dir")) { server.send(500, "text/plain", "BAD ARGS"); return; }

	String path = server.arg("dir");
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println("handleFileList: " + path);
#endif // DEBUG_WEBSERVER
	Dir dir = SPIFFS.openDir(path);
	path = String();

	String output = "[";
	while (dir.next()) {
		File entry = dir.openFile("r");
		if (true)//entry.name()!="secret.json") // Do not show secrets
		{
			if (output != "[")
				output += ',';
			bool isDir = false;
			output += "{\"type\":\"";
			output += (isDir) ? "dir" : "file";
			output += "\",\"name\":\"";
			output += String(entry.name()).substring(1);
			output += "\"}";
		}
		entry.close();
	}

	output += "]";
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println(output);
#endif // DEBUG_WEBSERVER
	server.send(200, "text/json", output);
}

void setUpdateMD5() {
	browserMD5 = "";
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.printf("Arg number: %d\n", server.args());
#endif // DEBUG_WEBSERVER
	if (server.args() > 0)  // Read hash
	{
		//String temp = "";
		for (uint8_t i = 0; i < server.args(); i++) {
#ifdef DEBUG_WEBSERVER
			DBG_OUTPUT_PORT.printf("Arg %s: %s\n", server.argName(i).c_str(), server.arg(i).c_str());
#endif // DEBUG_WEBSERVER
			if (server.argName(i) == "md5") { 
				browserMD5 = urldecode(server.arg(i));
				Update.setMD5(browserMD5.c_str());
				continue;
			}
		}
		server.send(200, "text/html", "OK --> MD5: "+browserMD5);
	}

}

void updateFirmware () {
	// handler for the file upload, get's the sketch bytes, and writes
	// them through the Update object
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		WiFiUDP::stopAll();
#ifdef DEBUG_WEBSERVER
		DBG_OUTPUT_PORT.printf("Update: %s\n", upload.filename.c_str());
#endif // DEBUG_WEBSERVER
		//uint32_t maxSketchSpace = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
		uint32_t maxSketchSpace = ESP.getSketchSize();
#ifdef DEBUG_WEBSERVER
		uint32_t oldValue = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
		DBG_OUTPUT_PORT.printf("Max free scketch space: %u\n", maxSketchSpace);
		DBG_OUTPUT_PORT.printf("Old value: %u\n", oldValue);
#endif // DEBUG_WEBSERVER
		if (browserMD5!= NULL && browserMD5 != "") {
			Update.setMD5(browserMD5.c_str());
#ifdef DEBUG_WEBSERVER
			DBG_OUTPUT_PORT.printf("Hash from client: %s\n", browserMD5.c_str());
#endif // DEBUG_WEBSERVER
		}
		if (!Update.begin(maxSketchSpace)) {//start with max available size
#ifdef DEBUG_WEBSERVER
			Update.printError(DBG_OUTPUT_PORT);
#endif // DEBUG_WEBSERVER
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE) {
#ifdef DEBUG_WEBSERVER
		DBG_OUTPUT_PORT.print(".");
#endif // DEBUG_WEBSERVER
		if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
#ifdef DEBUG_WEBSERVER
			Update.printError(DBG_OUTPUT_PORT);
#endif // DEBUG_WEBSERVER
		}
	}
	else if (upload.status == UPLOAD_FILE_END) {
		String updateHash;
		if (Update.end(true)) { //true to set the size to the current progress
#ifdef DEBUG_WEBSERVER
			updateHash = Update.md5String();
			DBG_OUTPUT_PORT.printf("Upload finished. Calculated MD5: %s\n", updateHash.c_str());
			DBG_OUTPUT_PORT.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
#endif // DEBUG_WEBSERVER
		}
		else {
#ifdef DEBUG_WEBSERVER
			updateHash = Update.md5String();
			DBG_OUTPUT_PORT.printf("Upload failed. Calculated MD5: %s\n", updateHash.c_str());
			Update.printError(DBG_OUTPUT_PORT);
#endif // DEBUG_WEBSERVER
		}
	}
	else if (upload.status == UPLOAD_FILE_ABORTED) {
		Update.end();
#ifdef DEBUG_WEBSERVER
		DBG_OUTPUT_PORT.println("Update was aborted");
#endif // DEBUG_WEBSERVER
	}
	delay(2);
}

void serverInit() {
	//SERVER INIT
	//list directory
	server.on("/list", HTTP_GET, []() {
		if (!checkAuth())
			return server.requestAuthentication();
		handleFileList(); 
	});
	//load editor
	server.on("/edit", HTTP_GET, []() {
		if (!checkAuth())
			return server.requestAuthentication();
		if (!handleFileRead("/edit.html")) 
			server.send(404, "text/plain", "FileNotFound");
	});
	//create file
	server.on("/edit", HTTP_PUT, []() {
		if (!checkAuth())
			return server.requestAuthentication();
		handleFileCreate();
	});
	//delete file
	server.on("/edit", HTTP_DELETE, []() {
		if (!checkAuth())
			return server.requestAuthentication();
		handleFileDelete();
	});
	//first callback is called after the request has ended with all parsed arguments
	//second callback handles file uploads at that location
	server.on("/edit", HTTP_POST, []() { server.send(200, "text/plain", ""); }, handleFileUpload);

	server.on("/admin/generalvalues", send_general_configuration_values_html);
	server.on("/admin/values", send_network_configuration_values_html);
	server.on("/admin/connectionstate", send_connection_state_values_html);
	server.on("/admin/infovalues", send_information_values_html);
	server.on("/admin/ntpvalues", send_NTP_configuration_values_html);
	server.on("/config.html", []() {
		if (!checkAuth())
			return server.requestAuthentication();
		send_network_configuration_html();
	});
	server.on("/general.html", []() {
		if (!checkAuth())
			return server.requestAuthentication();
		send_network_configuration_html();
	});
	server.on("/ntp.html", []() {
		if (!checkAuth())
			return server.requestAuthentication();
		send_NTP_configuration_html();
	});
	//server.on("/admin/devicename", send_devicename_value_html);
	server.on("/admin", HTTP_GET, []() {
		if (!checkAuth())
			return server.requestAuthentication();
		if (!handleFileRead("/admin.html")) 
			server.send(404, "text/plain", "FileNotFound");
	});
	server.on("/admin/restart", []() {
		if (!checkAuth())
			return server.requestAuthentication();
		restart_esp();
	});
	server.on("/system.html", []() {
		if (!checkAuth())
			return server.requestAuthentication();
		send_wwwauth_configuration_html();
	});
	server.on("/admin/wwwauth", []() {
		if (!checkAuth())
			return server.requestAuthentication();
		send_wwwauth_configuration_values_html();
	});
	server.on("/update/updatepossible", []() {
		if (!checkAuth())
			return server.requestAuthentication();
		send_update_firmware_values_html();
	});
	server.on("/setmd5", [&]() {
		if (!checkAuth())
			return server.requestAuthentication();
		DBG_OUTPUT_PORT.println("md5?");
		setUpdateMD5();
	});
	server.on("/update", HTTP_GET, []() {
		if (!checkAuth())
			return server.requestAuthentication();
		if (!handleFileRead("/update.html"))
			server.send(404, "text/plain", "FileNotFound");
	});
	server.on("/update", HTTP_POST, [&]() {
		if (!checkAuth())
			return server.requestAuthentication();
		server.sendHeader("Connection", "close");
		server.sendHeader("Access-Control-Allow-Origin", "*");
		server.send(200, "text/html", (Update.hasError()) ? "FAIL" : "<META http-equiv=\"refresh\" content=\"15;URL=/update\">OK");
		ESP.restart();
	}, updateFirmware);

	//called when the url is not defined here
	//use it to load content from SPIFFS
	server.onNotFound( []() {
		if (!checkAuth())
			return server.requestAuthentication();
		server.sendHeader("Connection", "close");
		server.sendHeader("Access-Control-Allow-Origin", "*");
		if (!handleFileRead(server.uri()))
			server.send(404, "text/plain", "FileNotFound");
	});

#ifndef HIDE_SECRET
	server.on(SECRET_FILE, HTTP_GET, []() {
		if (!checkAuth())
			return server.requestAuthentication();
		server.sendHeader("Connection", "close");
		server.sendHeader("Access-Control-Allow-Origin", "*");
		server.send(403, "text/plain", "Forbidden");
	});
#endif // HIDE_SECRET

#ifndef HIDE_CONFIG
	server.on(CONFIG_FILE, HTTP_GET, []() {
		if (!checkAuth())
			return server.requestAuthentication();
		server.sendHeader("Connection", "close");
		server.sendHeader("Access-Control-Allow-Origin", "*");
		server.send(403, "text/plain", "Forbidden");
	});
#endif // HIDE_CONFIG

	//get heap status, analog input value and all GPIO statuses in one json call
	server.on("/all", HTTP_GET, []() {
		String json = "{";
		json += "\"heap\":" + String(ESP.getFreeHeap());
		json += ", \"analog\":" + String(analogRead(A0));
		json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
		json += "}";
		server.send(200, "text/json", json);
		json = String();
	});
	server.begin();
	//httpUpdater.setup(&server,httpAuth.wwwUsername,httpAuth.wwwPassword);
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println("HTTP server started");
#endif // DEBUG_WEBSERVER
}

boolean checkAuth() {
	if (!httpAuth.auth) {
		return true;
	}
	else
	{
		return server.authenticate(httpAuth.wwwUsername.c_str(), httpAuth.wwwPassword.c_str());
	}
	
}
