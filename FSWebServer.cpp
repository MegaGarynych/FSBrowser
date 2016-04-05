// 
// 
// 
#define DBG_OUTPUT_PORT Serial

#include "FSWebServer.h"
#include "DynamicData.h"
#include "Config.h"

ESP8266WebServer server(80);
File fsUploadFile;

const char* www_username = "admin";
const char* www_password = "esp8266";

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
	DBG_OUTPUT_PORT.println("handleFileRead: " + path);
	if (path.endsWith("/")) path += "index.htm";
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
	else
		DBG_OUTPUT_PORT.printf("Cannot find %s\n", path.c_str());
	return false;
}

void handleFileUpload() {
	if (server.uri() != "/edit") return;
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/")) filename = "/" + filename;
		DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
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
		DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
	}
}

void handleFileDelete() {
	if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
	DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
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
	DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
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
	DBG_OUTPUT_PORT.println("handleFileList: " + path);
	Dir dir = SPIFFS.openDir(path);
	path = String();

	String output = "[";
	while (dir.next()) {
		File entry = dir.openFile("r");
		if (output != "[") output += ',';
		bool isDir = false;
		output += "{\"type\":\"";
		output += (isDir) ? "dir" : "file";
		output += "\",\"name\":\"";
		output += String(entry.name()).substring(1);
		output += "\"}";
		entry.close();
	}

	output += "]";
	DBG_OUTPUT_PORT.println(output);
	server.send(200, "text/json", output);
}

void serverInit() {
	//SERVER INIT
	//list directory
	server.on("/list", HTTP_GET, []() {
		if (!server.authenticate(www_username, www_password))
			return server.requestAuthentication();
		handleFileList(); 
	});
	//load editor
	server.on("/edit", HTTP_GET, []() {
		if (!server.authenticate(www_username, www_password))
			return server.requestAuthentication();
		if (!handleFileRead("/edit.html")) 
			server.send(404, "text/plain", "FileNotFound");
	});
	//create file
	server.on("/edit", HTTP_PUT, []() {
		if (!server.authenticate(www_username, www_password))
			return server.requestAuthentication();
		handleFileCreate();
	});
	//delete file
	server.on("/edit", HTTP_DELETE, []() {
		if (!server.authenticate(www_username, www_password))
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
		if (!server.authenticate(www_username, www_password))
			return server.requestAuthentication();
		send_network_configuration_html();
	});
	server.on("/ntp.html", []() {
		if (!server.authenticate(www_username, www_password))
			return server.requestAuthentication();
		send_NTP_configuration_html();
	});
	//server.on("/admin/devicename", send_devicename_value_html);
	server.on("/admin", HTTP_GET, []() {
		if (!server.authenticate(www_username, www_password))
			return server.requestAuthentication();
		if (!handleFileRead("/admin.html")) 
			server.send(404, "text/plain", "FileNotFound");
	});
	server.on("/admin/restart", []() {
		if (!server.authenticate(www_username, www_password))
			return server.requestAuthentication();
		restart_esp();
	});

	//called when the url is not defined here
	//use it to load content from SPIFFS
	server.onNotFound([]() {
		if (!server.authenticate(www_username, www_password))
			return server.requestAuthentication();
		if (!handleFileRead(server.uri()))
			server.send(404, "text/plain", "FileNotFound");
	});

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
	DBG_OUTPUT_PORT.println("HTTP server started");
}
