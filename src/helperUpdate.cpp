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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperUpdate.cpp 269 2025-07-01 19:25:14Z                $ #
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
	mqttTopicUpdateChanel = wpFZ.DeviceName + "/info/UpdateChanel";
	// commands
	mqttTopicUpdateFW = wpFZ.DeviceName + "/UpdateFW";
	mqttTopicSetUpdateChanel = wpFZ.DeviceName + "/settings/UpdateChanel";
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Update";
	
	twelveHours = 12 * 60 * 60 * 1000;
	lastUpdateCheck = 0;
	newVersion = false;
	updateChanel = 0;
	jsonsub = "firmware";
	file = F("firmware.bin");
	#if BUILDWITH == 99
		file = F("firmware.bin");
		updateChanel = 99;
		jsonsub = "firmware";
	#endif
	#if BUILDWITH == 1
		file = F("firmwarelight.bin");
		updateChanel = 1;
		jsonsub = "light";
	#endif
	#if BUILDWITH == 2
		file = F("firmwareio.bin");
		updateChanel = 2;
		jsonsub = "io";
	#endif
	#if BUILDWITH == 3
		file = F("firmwareheating.bin");
		updateChanel = 3;
		jsonsub = "heating";
	#endif
	#if BUILDWITH == 4
		file = F("firmwarerfid.bin");
		updateChanel = 4;
		jsonsub = "rfid";
	#endif
	wpMqtt.mqttClient.publish(mqttTopicNewVersion.c_str(), String(newVersion).c_str()); // hide Alarm until check is done
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

uint16_t helperUpdate::getVersion() {
	String SVN = "$Rev: 269 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperUpdate::changeDebug() {
	Debug = !Debug;
	wpEEPROM.WriteBoolToEEPROM("DebugUpdate", wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugUpdate, Debug);
	wpFZ.SendWSDebug("DebugUpdate", Debug);
	wpFZ.blink();
}

bool helperUpdate::setupOta() {
	bool returns = false;
	ArduinoOTA.onStart([]() {
		wpFZ.SetRestartReason(wpFZ.restartReasonUpdate);
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
	deserializeJson(doc, payload);
	payload.replace("\"", "'");
	wpFZ.DebugWS(wpFZ.strDEBUG, "UpdateCheck", "payload: " + payload);
	newVersion = !(doc["wpFreakaZone"][jsonsub]["VersionId"].as<String>() == installedVersion);
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "Selected Chanel: " + jsonsub);
	wpFZ.DebugWS(newVersion ? wpFZ.strWARN : wpFZ.strINFO, "UpdateCheck", "installed Version: " + installedVersion);
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "available Version: " + doc["wpFreakaZone"][jsonsub]["VersionId"].as<String>());
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "Date: " + doc["wpFreakaZone"][jsonsub]["date"].as<String>());
	wpFZ.DebugWS(wpFZ.strINFO, "UpdateCheck", "File: " + doc["wpFreakaZone"][jsonsub]["filename"].as<String>());
	serverVersion = doc["wpFreakaZone"][jsonsub]["VersionId"].as<String>();
	wpFZ.SendNewVersion(newVersion);
}
void helperUpdate::start() {
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
	wpFZ.DebugWS(wpFZ.strINFO, "Update::start", "Start Update with: '" + file + "'");
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
		wpMqtt.mqttClient.publish(mqttTopicUpdateChanel.c_str(), GetUpdateChanel().c_str());
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperUpdate::publishValues() {
	publishValues(false);
}
void helperUpdate::publishValues(bool force) {
	if(force) {
		publishNewVersionLast = 0;
		publishDebugLast = 0;
	}
	if(newVersionLast != newVersion || wpFZ.CheckQoS(publishNewVersionLast)) {
		newVersionLast = newVersion;
		wpMqtt.mqttClient.publish(mqttTopicNewVersion.c_str(), String(newVersion).c_str());
		wpFZ.SendNewVersion(newVersion);
		publishNewVersionLast = wpFZ.loopStartedAt;
	}
	if(DebugLast != Debug || wpFZ.CheckQoS(publishDebugLast)) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishDebugLast = wpFZ.loopStartedAt;
	}
}

void helperUpdate::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetUpdateChanel.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void helperUpdate::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetUpdateChanel.c_str()) == 0) {
		uint8_t readUpdateChanel = msg.toInt();
		if(updateChanel != readUpdateChanel) {
			SetUpdateChanel(updateChanel);
			wpFZ.DebugcheckSubscribes(mqttTopicSetUpdateChanel, GetUpdateChanel());
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			wpEEPROM.WriteBoolToEEPROM("DebugUpdate", wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugUpdate, Debug);
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
	if((lastUpdateCheck == 0 && m > 60000) || m > lastUpdateCheck + twelveHours) {
		check();
	}
}
String helperUpdate::GetUpdateChanel() {
	switch(updateChanel) {
		case 99:
			return F("cleaner");
			break;
		case 1:
			return F("light");
			break;
		case 2:
			return F("io");
			break;
		case 3:
			return F("heating");
			break;
		case 4:
			return F("rfid");
			break;
		default:
			return F("firmware");
			break;
	}
}
void helperUpdate::SetUpdateChanel(uint8_t uc) {
	switch(uc) {
		case 99:
			file = F("firmwarecleaner.bin");
			updateChanel = 99;
			break;
		case 1:
			file = F("firmwarelight.bin");
			updateChanel = 1;
			break;
		case 2:
			file = F("firmwareio.bin");
			updateChanel = 2;
			break;
		case 3:
			file = F("firmwareheating.bin");
			updateChanel = 3;
			break;
		case 4:
			file = F("firmwarerfid.bin");
			updateChanel = 3;
			break;
		default:
			file = F("firmware.bin");
			updateChanel = 0;
			break;
	}
	wpMqtt.mqttClient.publish(mqttTopicUpdateChanel.c_str(), GetUpdateChanel().c_str());
}
void helperUpdate::started() {
	wpFZ.SetRestartReason(wpFZ.restartReasonUpdate);
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
