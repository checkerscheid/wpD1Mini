//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 29.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 125                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperUpdate.cpp 125 2024-06-03 03:11:11Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperUpdate.h>

helperUpdate wpUpdate;

helperUpdate::helperUpdate() {}
void helperUpdate::init() {
	// values
	mqttTopicMode = wpFZ.DeviceName + "/UpdateMode";
	// settings
	mqttTopicServer = wpFZ.DeviceName + "/info/Update/Server";
	// commands
	mqttTopicUpdateFW = wpFZ.DeviceName + "/UpdateFW";
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Update";
}

//###################################################################################
// public
//###################################################################################

void helperUpdate::cycle() {
	if(UpdateFW) ArduinoOTA.handle();
	publishValues();
}

uint16_t helperUpdate::getVersion() {
	String SVN = "$Rev: 125 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperUpdate::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugUpdate, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugUpdate\",\"value\":" + String(Debug ? "true" : "false") + "}");
	wpFZ.blink();
}

bool helperUpdate::setupOta() {
	bool returns = false;
	ArduinoOTA.onStart([]() {
		wpOnlineToggler.setMqttOffline();
		String logmessage = "OTA Start";
		wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage);
	});
	ArduinoOTA.onEnd([]() {
		String logmessage = "OTA End";
		wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage);
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		String logmessage = "OTA Progress: " + String((progress / (total / 100))) + " %";
		wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage, false);
	});
	ArduinoOTA.onError([](ota_error_t error) {
		String logmessage = "Error (" + String(error) + ") ";
		if (error == OTA_AUTH_ERROR) logmessage += "Auth Failed";
		else if (error == OTA_BEGIN_ERROR) logmessage += "Begin Failed";
		else if (error == OTA_CONNECT_ERROR) logmessage += "Connect Failed";
		else if (error == OTA_RECEIVE_ERROR) logmessage += "Receive Failed";
		else if (error == OTA_END_ERROR) logmessage += "End Failed";
		wpFZ.DebugWS(wpFZ.strERRROR, "setupOta", logmessage);
	});
	ArduinoOTA.begin();
	returns = true;
	String logmessage = "OTA Ready, IP address: " + WiFi.localIP().toString();
	wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage);
	return returns;
}

void helperUpdate::check() {
	WiFiClient wifi;
	HTTPClient http;
	JsonDocument doc;
	http.begin(wifi, String(wpFZ.updateServer));
	int httpCode = http.GET();
	wpFZ.DebugWS(wpFZ.strDEBUG, "UpdateCheck", "http Code: " + String(httpCode));
	String payload = http.getString();
	wpFZ.DebugWS(wpFZ.strDEBUG, "UpdateCheck", "payload: " + payload);
	deserializeJson(doc, payload);
	//v3.0-build121
	String v = "v" + String(wpFZ.MajorVersion) + "." + String(wpFZ.MinorVersion) + "-build" + String(wpFZ.Build);
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "installed Version: " + v);
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "available Version: " + String(doc["wpFreakaZone"]["Version"]));
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "Date: " + String(doc["wpFreakaZone"]["date"]));
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "File: " + String(doc["wpFreakaZone"]["filename"]));
}
void helperUpdate::start() {
	start("firmware.bin");
}
void helperUpdate::start(String file) {
	WiFiClient client;
	ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

	// Add optional callback notifiers
	ESPhttpUpdate.onStart(started);
	ESPhttpUpdate.onEnd(finished);
	ESPhttpUpdate.onProgress(progress);
	ESPhttpUpdate.onError(error);

	// t_httpUpdate_return ret = ESPhttpUpdate.update(client, wpFZ.updateServer);
	// Or:
	// t_httpUpdate_return ret = ESPhttpUpdate.update(client, server, 80, file);
	t_httpUpdate_return ret = ESPhttpUpdate.update(client, String(wpFZ.updateServer) + "/" + file);

	switch (ret) {
		case HTTP_UPDATE_FAILED:
			wpFZ.DebugWS(wpFZ.strERRROR, "wpUpdate::start", "HTTP_UPDATE_FAILD Error (" +
				String(ESPhttpUpdate.getLastError()) + "): " +
				ESPhttpUpdate.getLastErrorString());
			break;

		case HTTP_UPDATE_NO_UPDATES:
			wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::start", "HTTP_UPDATE_NO_UPDATES");
			break;

		case HTTP_UPDATE_OK:
			wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::start", "HTTP_UPDATE_OK");
			break;
	}
}

void helperUpdate::publishSettings() {
	publishSettings(false);
}
void helperUpdate::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperUpdate::publishValues() {
	publishValues(false);
}
void helperUpdate::publishValues(bool force) {
	if(force) publishCountDebug = wpFZ.publishQoS;
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void helperUpdate::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void helperUpdate::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugUpdate, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
			EEPROM.commit();
			wpFZ.SendWS("{\"id\":\"DebugUpdate\",\"value\":" + String(Debug ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}

//###################################################################################
// private
//###################################################################################
void helperUpdate::started() {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::started", "HTTP update started");
}

void helperUpdate::finished() {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::finished", "HTTP update finished");
}

void helperUpdate::progress(int cur, int total) {
	String logmessage = "HTTP update: " + String(cur) + " of " + String(total) + " bytes";
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::progress", logmessage, false);
}

void helperUpdate::error(int err) {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::error", "HTTP update fatal error, code: " + String(err));
}
