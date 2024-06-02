//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 30.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 123                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperRest.cpp 123 2024-06-02 04:37:07Z                  $ #
//#                                                                                 #
//###################################################################################
#include <helperRest.h>

helperRest wpRest;

helperRest::helperRest() {}
void helperRest::init() {
	// settings
	mqttTopicRestServer = wpFZ.DeviceName + "/info/Rest/Server";
	// commands
	mqttTopicError = wpFZ.DeviceName + "/ERROR/Rest";
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Rest";

	macId = WiFi.macAddress();
	macId.replace(":", "");
	macId.toLowerCase();
}

//###################################################################################
// public
//###################################################################################
void helperRest::cycle() {
	publishValues();
}

uint16_t helperRest::getVersion() {
	String SVN = "$Rev: 123 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperRest::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugRest, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugRest\",\"value\":" + String(Debug ? "true" : "false") + "}");
	wpFZ.blink();
}

// return true on success
bool helperRest::sendRest(String name, String value) {
	bool returns = false;
	WiFiClient client;
	HTTPClient http;
	String targetwithident = "http://" + String(wpFZ.restServer) + ":" + String(wpFZ.restServerPort) + "/?m=" + macId;
	String target = targetwithident + "&" + name + "=" + value;
	if(Debug) {
		String logmessage = "HTTP: '" + target + "'";
		wpFZ.DebugWS(wpFZ.strINFO, "sendRest", logmessage);
	}
	http.begin(client, target.c_str());
	int httpResponseCode = http.GET();
	if (httpResponseCode > 0) {
		if(Debug) {
			String payload = http.getString();
			payload.replace("\"", "'");
			String logmessage = "HTTP Response (" + String(httpResponseCode) + "): " + payload;
			wpFZ.DebugWS(wpFZ.strINFO, "sendRest", logmessage);
		}
		returns = true;
	} else {
		String logmessage = "HTTP Response (" + String(httpResponseCode) + ")";
		wpFZ.DebugWS(wpFZ.strERRROR, "sendRest", logmessage);
	}
	http.end();
	return returns;
}

// return true on success
bool helperRest::sendRawRest(String target) {
	bool returns = false;
	WiFiClient client;
	HTTPClient http;
	if(Debug) {
		String logmessage = "HTTP: '" + target + "'";
		wpFZ.DebugWS(wpFZ.strINFO, "sendRawRest", logmessage);
	}
	http.begin(client, target.c_str());
	int httpResponseCode = http.GET();
	if (httpResponseCode > 0) {
		if(Debug) {
			String payload = http.getString();
			payload.replace("\"", "'");
			String logmessage = "HTTP Response (" + String(httpResponseCode) + "): " + payload;
			wpFZ.DebugWS(wpFZ.strINFO, "sendRawRest", logmessage);
		}
		returns = true;
	} else {
		String logmessage = "HTTP Response (" + String(httpResponseCode) + ")";
		wpFZ.DebugWS(wpFZ.strERRROR, "sendRawRest", logmessage);
	}
	http.end();
	return returns;
}

void helperRest::publishSettings() {
	publishSettings(false);
}
void helperRest::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperRest::publishValues() {
	publishValues(false);
}
void helperRest::publishValues(bool force) {
	if(force) {
		publishCountError = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
	}
	if(errorLast != error || ++publishCountError > wpFZ.publishQoS) {
		errorLast = error;
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
		publishCountError = 0;
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void helperRest::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void helperRest::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugFinder, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
			EEPROM.commit();
			wpFZ.SendWS("{\"id\":\"DebugRest\",\"value\":" + String(Debug ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}

//###################################################################################
// private
//###################################################################################

