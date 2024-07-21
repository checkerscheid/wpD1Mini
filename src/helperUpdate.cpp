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
//# Revision     : $Rev:: 170                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperUpdate.cpp 170 2024-07-16 16:50:17Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperUpdate.h>

helperUpdate wpUpdate;

helperUpdate::helperUpdate() {}
void helperUpdate::init() {
	// values
	mqttTopicMode = wpFZ.DeviceName + "/UpdateMode";
	mqttTopicNewVersion = wpFZ.DeviceName + "/ERROR/NewVersion";
	// settings
	mqttTopicServer = wpFZ.DeviceName + "/info/Update/Server";
	// commands
	mqttTopicUpdateFW = wpFZ.DeviceName + "/UpdateFW";
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Update";
	
	twelveHours = 12 * 60 * 60 * 1000;
	lastUpdateCheck = twelveHours - (12 * 58 * 60 * 1000);
	serverVersion = "";
	installedVersion = "v" + String(wpFZ.MajorVersion) + "." + String(wpFZ.MinorVersion) + "-build" + String(wpFZ.Build);
}

//###################################################################################
// public
//###################################################################################

void helperUpdate::cycle() {
	if(UpdateFW) ArduinoOTA.handle();
	publishValues();
	doCheckUpdate();
}

uint16 helperUpdate::getVersion() {
	String SVN = "$Rev: 170 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperUpdate::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugUpdate, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugUpdate", Debug);
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
		//wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage, false);
		wpFZ.updateProgress((progress / (total / 100)));
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
	lastUpdateCheck = millis();
	WiFiClient wifi;
	HTTPClient http;
	JsonDocument doc;
	const String url = "http://" + String(wpFZ.updateServer);
	http.begin(wifi, url);
	int httpCode = http.GET();
	wpFZ.DebugWS(wpFZ.strDEBUG, "UpdateCheck", "http Code: " + String(httpCode));
	String payload = http.getString();
	wpFZ.DebugWS(wpFZ.strDEBUG, "UpdateCheck", "payload: " + payload);
	deserializeJson(doc, payload);
	serverVersion = String(doc["wpFreakaZone"]["Version"]);
	newVersion = !(serverVersion == installedVersion);
	wpFZ.DebugWS(serverVersion == installedVersion ? wpFZ.strINFO : wpFZ.strWARN, "UpdateCheck", "installed Version: " + installedVersion);
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "available Version: " + serverVersion);
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "Date: " + String(doc["wpFreakaZone"]["date"]));
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "File: " + String(doc["wpFreakaZone"]["filename"]));
	wpFZ.SendNewVersion(newVersion);
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
	// 
	// t_httpUpdate_return ret = ESPhttpUpdate.update(client, server, 80, file);
	
	//const String urlfile = "http://" + String(wpFZ.updateServer) + "/" + file;
	//t_httpUpdate_return ret = ESPhttpUpdate.update(client, urlfile);
	
	t_httpUpdate_return ret = ESPhttpUpdate.update(client, String(wpFZ.updateServer), 80, "/" + file);

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
	if(force) {
		publishCountDebug = wpFZ.publishQoS;
		publishCountNewVersion = wpFZ.publishQoS;
	}
	if(newVersionLast != newVersion || ++publishCountNewVersion > wpFZ.sekunde10) {
		newVersionLast = newVersion;
		wpMqtt.mqttClient.publish(mqttTopicNewVersion.c_str(), String(newVersion).c_str());
		wpFZ.SendNewVersion(newVersion);
		publishCountNewVersion = 0;
	}
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
			bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugUpdate, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
			EEPROM.commit();
			wpFZ.SendWSDebug("DebugUpdate", Debug);
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}

//###################################################################################
// private
//###################################################################################
void helperUpdate::doCheckUpdate() {
	unsigned long m = millis();
	if(m > lastUpdateCheck + twelveHours) {
		check();
	}
}
void helperUpdate::started() {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::started", "HTTP update started");
}

void helperUpdate::finished() {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::finished", "HTTP update finished");
}

void helperUpdate::progress(int cur, int total) {
	String logmessage = "HTTP update: " + String(cur) + " of " + String(total) + " bytes";
	//wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::progress", logmessage, false);
	wpFZ.updateProgress((cur / (total / 100)));
}

void helperUpdate::error(int err) {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::error", "HTTP update fatal error, code: " + String(err));
}
