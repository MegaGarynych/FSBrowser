// 
// 
// 
#define DBG_OUTPUT_PORT Serial

#define DEBUG_WEBSERVER

#include "FSWebServer.h"
#include "DynamicData.h"
#include "Config.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
File fsUploadFile;
String browserMD5="";
static uint32_t updateSize = 0;

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

String getContentType(String filename, AsyncWebServerRequest *request) {
	if (request->hasArg("download")) return "application/octet-stream";
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

bool handleFileRead(String path, AsyncWebServerRequest *request) {
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println("handleFileRead: " + path);
#endif // DEBUG_WEBSERVER
	if (CONNECTION_LED >= 0) {
		flashLED(CONNECTION_LED, 1, 25); // Show activity on LED
	}
	if (path.endsWith("/"))
		path += "index.htm";
	String contentType = getContentType(path,request);
	String pathWithGz = path + ".gz";
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if (SPIFFS.exists(pathWithGz))
			path += ".gz";
		//File file = SPIFFS.open(path, "r");
		request->send(SPIFFS, path, contentType);
		//size_t sent = request->streamFile(file, contentType);
		//DBG_OUTPUT_PORT.printf("File %s exist\n", file.name());
		//file.close();
		return true;
	}
#ifdef DEBUG_WEBSERVER
	else
		DBG_OUTPUT_PORT.printf("Cannot find %s\n", path.c_str());
#endif // DEBUG_WEBSERVER
	return false;
}

void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
	//if (request->url() != "/edit") return;
	if (!index) { // Start
#ifdef DEBUG_WEBSERVER
		DBG_OUTPUT_PORT.printf("handleFileUpload Name: %s\n", filename.c_str());
#endif // DEBUG_WEBSERVER
		if (!filename.startsWith("/")) filename = "/" + filename;
		fsUploadFile = SPIFFS.open(filename, "w");

	}
	// Continue
	if (fsUploadFile) {
		if (fsUploadFile.write(data, len) != len) {
			DBG_OUTPUT_PORT.println("Write error during upload");
		}
	}
	/*for (size_t i = 0; i < len; i++) {
		if (fsUploadFile)
			fsUploadFile.write(data[i]);
	}*/
	if (final) { // End
		if (fsUploadFile)
			fsUploadFile.close();
#ifdef DEBUG_WEBSERVER
		DBG_OUTPUT_PORT.printf("handleFileUpload Size: %u\n", len);
#endif // DEBUG_WEBSERVER

	}

	/*HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		//String filename = upload.filename;
		if (!filename.startsWith("/")) filename = "/" + filename;
#ifdef DEBUG_WEBSERVER
		DBG_OUTPUT_PORT.printf("handleFileUpload Name: %s\n", filename.c_str());
#endif // DEBUG_WEBSERVER
		fsUploadFile = SPIFFS.open(filename, "w");
		//filename = String();
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
	}*/
}

void handleFileDelete(AsyncWebServerRequest *request) {
	if (request->args() == 0) return request->send(500, "text/plain", "BAD ARGS");
	String path = request->arg(0);
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
#endif // DEBUG_WEBSERVER
	if (path == "/")
		return request->send(500, "text/plain", "BAD PATH");
	if (!SPIFFS.exists(path))
		return request->send(404, "text/plain", "FileNotFound");
	SPIFFS.remove(path);
	request->send(200, "text/plain", "");
	path = String(); // Remove? Useless statement?
}

void handleFileCreate(AsyncWebServerRequest *request) {
	if (request->args() == 0)
		return request->send(500, "text/plain", "BAD ARGS");
	String path = request->arg(0);
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
#endif // DEBUG_WEBSERVER
	if (path == "/")
		return request->send(500, "text/plain", "BAD PATH");
	if (SPIFFS.exists(path))
		return request->send(500, "text/plain", "FILE EXISTS");
	File file = SPIFFS.open(path, "w");
	if (file)
		file.close();
	else
		return request->send(500, "text/plain", "CREATE FAILED");
	request->send(200, "text/plain", "");
	path = String(); // Remove? Useless statement?
}

void handleFileList(AsyncWebServerRequest *request) {
	if (!request->hasArg("dir")) { request->send(500, "text/plain", "BAD ARGS"); return; }

	String path = request->arg("dir");
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
	request->send(200, "text/json", output);
}

void setUpdateMD5(AsyncWebServerRequest *request) {
	browserMD5 = "";
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.printf("Arg number: %d\n", request->args());
#endif // DEBUG_WEBSERVER
	if (request->args() > 0)  // Read hash
	{
		//String temp = "";
		for (uint8_t i = 0; i < request->args(); i++) {
#ifdef DEBUG_WEBSERVER
			DBG_OUTPUT_PORT.printf("Arg %s: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
#endif // DEBUG_WEBSERVER
			if (request->argName(i) == "md5") {
				browserMD5 = urldecode(request->arg(i));
				Update.setMD5(browserMD5.c_str());
				continue;
			}if (request->argName(i) == "size") {
				updateSize = request->arg(i).toInt();
#ifdef DEBUG_WEBSERVER
				DBG_OUTPUT_PORT.printf("Update size: %u\n", request->arg(i).toInt());
#endif // DEBUG_WEBSERVER
				continue;
			}
		}
		request->send(200, "text/html", "OK --> MD5: "+browserMD5);
	}

}

void updateFirmware(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
	// handler for the file upload, get's the sketch bytes, and writes
	// them through the Update object
	static long totalSize = 0;
	if (!index) { //UPLOAD_FILE_START
		SPIFFS.end();
		Update.runAsync(true);
		DBG_OUTPUT_PORT.printf("Update start: %s\n", filename.c_str());
		uint32_t maxSketchSpace = ESP.getSketchSize();
		DBG_OUTPUT_PORT.printf("Max free scketch space: %u\n", maxSketchSpace);
		DBG_OUTPUT_PORT.printf("New scketch size: %u\n", updateSize);
		if (browserMD5 != NULL && browserMD5 != "") {
			Update.setMD5(browserMD5.c_str());
			DBG_OUTPUT_PORT.printf("Hash from client: %s\n", browserMD5.c_str());
		}
		if (!Update.begin(updateSize)) {//start with max available size
			Update.printError(DBG_OUTPUT_PORT);
		}

	}

	// Get upload file, continue if not start
	totalSize += len;
	DBG_OUTPUT_PORT.print(".");
	size_t written = Update.write(data, len);
	if (written != len) {
		DBG_OUTPUT_PORT.printf("len = %d, written = %d, totalSize = %d\r\n",len,written,totalSize);
		//Update.printError(DBG_OUTPUT_PORT);
		//return;
	}
	if (final) {  // UPLOAD_FILE_END
		String updateHash;
		DBG_OUTPUT_PORT.println("Applying update...");
		if (Update.end(true)) { //true to set the size to the current progress
			updateHash = Update.md5String();
			DBG_OUTPUT_PORT.printf("Upload finished. Calculated MD5: %s\n", updateHash.c_str());
			DBG_OUTPUT_PORT.printf("Update Success: %u\nRebooting...\n", request->contentLength());
		}
		else {
			updateHash = Update.md5String();
			DBG_OUTPUT_PORT.printf("Upload failed. Calculated MD5: %s\n", updateHash.c_str());
			Update.printError(DBG_OUTPUT_PORT);
		}
	}
	
	//delay(2);
}

void serverInit() {
	//SERVER INIT
	//list directory
	server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		handleFileList(request);
	});
	//load editor
	server.on("/edit", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		if (!handleFileRead("/edit.html", request)) 
			request->send(404, "text/plain", "FileNotFound");
	});
	//create file
	server.on("/edit", HTTP_PUT, [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		handleFileCreate(request);
	});
	//delete file
	server.on("/edit", HTTP_DELETE, [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		handleFileDelete(request);
	});
	//first callback is called after the request has ended with all parsed arguments
	//second callback handles file uploads at that location
	server.on("/edit", HTTP_POST, [](AsyncWebServerRequest *request) { request->send(200, "text/plain", ""); }, handleFileUpload);
	server.on("/admin/generalvalues", HTTP_GET, [](AsyncWebServerRequest *request) { send_general_configuration_values_html(request); });
	server.on("/admin/values", [](AsyncWebServerRequest *request) { send_network_configuration_values_html(request); });
	server.on("/admin/connectionstate", [](AsyncWebServerRequest *request) { send_connection_state_values_html(request); });
	server.on("/admin/infovalues", [](AsyncWebServerRequest *request) { send_information_values_html(request); });
	server.on("/admin/ntpvalues", [](AsyncWebServerRequest *request) { send_NTP_configuration_values_html(request); });
	server.on("/config.html", [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		send_network_configuration_html(request);
	});
	server.on("/general.html", [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		send_general_configuration_html(request);
	});
	server.on("/ntp.html", [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		send_NTP_configuration_html(request);
	});
	//server.on("/admin/devicename", send_devicename_value_html);
	server.on("/admin/restart", [](AsyncWebServerRequest *request) {
		DBG_OUTPUT_PORT.println(request->url());
		if (!checkAuth(request))
			return request->requestAuthentication();
		restart_esp(request);
	});
	server.on("/admin/wwwauth", [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		send_wwwauth_configuration_values_html(request);
	});
	server.on("/admin", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		if (!handleFileRead("/admin.html", request))
			request->send(404, "text/plain", "FileNotFound");
	});
	server.on("/system.html", [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		send_wwwauth_configuration_html(request);
	});
	server.on("/update/updatepossible", [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		send_update_firmware_values_html(request);
	});
	server.on("/setmd5", [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		//DBG_OUTPUT_PORT.println("md5?");
		setUpdateMD5(request);
	});
	server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		if (!handleFileRead("/update.html", request))
			request->send(404, "text/plain", "FileNotFound");
	});
	server.on("/update", HTTP_POST,[](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		AsyncWebServerResponse *response = request->beginResponse(200, "text/html", (Update.hasError()) ? "FAIL" : "<META http-equiv=\"refresh\" content=\"15;URL=/update\">Update correct. Restarting...");
		response->addHeader("Connection", "close");
		response->addHeader("Access-Control-Allow-Origin", "*");
		request->send(response);
		ESP.restart();
	}, updateFirmware);

	ws.onEvent(webSocketEvent);
	server.addHandler(&ws);

	//called when the url is not defined here
	//use it to load content from SPIFFS
	server.onNotFound([](AsyncWebServerRequest *request) {
		DBG_OUTPUT_PORT.printf("Not found: %s\r\n", request->url().c_str());
		if (!checkAuth(request))
			return request->requestAuthentication();
		AsyncWebServerResponse *response = request->beginResponse(200);
		response->addHeader("Connection", "close");
		response->addHeader("Access-Control-Allow-Origin", "*");
		if (!handleFileRead(request->url(), request))
			request->send(404, "text/plain", "FileNotFound");
	});

#ifdef HIDE_SECRET
	server.on(SECRET_FILE, HTTP_GET, [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		AsyncWebServerResponse *response = request->beginResponse(403, "text/plain", "Forbidden");
		response->addHeader("Connection", "close");
		response->addHeader("Access-Control-Allow-Origin", "*");
		request->send(response);
	});
#endif // HIDE_SECRET

#ifdef HIDE_CONFIG
	server.on(CONFIG_FILE, HTTP_GET, [](AsyncWebServerRequest *request) {
		if (!checkAuth(request))
			return request->requestAuthentication();
		AsyncWebServerResponse *response = request->beginResponse(403, "text/plain", "Forbidden");
		response->addHeader("Connection", "close");
		response->addHeader("Access-Control-Allow-Origin", "*");
		request->send(response);
	});
#endif // HIDE_CONFIG

	//get heap status, analog input value and all GPIO statuses in one json call
	server.on("/all", HTTP_GET, [](AsyncWebServerRequest *request) {
		String json = "{";
		json += "\"heap\":" + String(ESP.getFreeHeap());
		json += ", \"analog\":" + String(analogRead(A0));
		json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
		json += "}";
		request->send(200, "text/json", json);
		json = String();
	});
	server.begin();
	//httpUpdater.setup(&server,httpAuth.wwwUsername,httpAuth.wwwPassword);
#ifdef DEBUG_WEBSERVER
	DBG_OUTPUT_PORT.println("HTTP server started");
#endif // DEBUG_WEBSERVER
}

boolean checkAuth(AsyncWebServerRequest *request) {
	if (!httpAuth.auth) {
		return true;
	}
	else
	{
		return request->authenticate(httpAuth.wwwUsername.c_str(), httpAuth.wwwPassword.c_str());
	}
	
}
