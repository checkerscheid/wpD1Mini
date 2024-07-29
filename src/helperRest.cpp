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
//# Revision     : $Rev:: 183                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperRest.cpp 183 2024-07-29 03:32:26Z                  $ #
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
	if(trySend) {
		errorIsSet = error;
	}
	publishValues();
}

uint16 helperRest::getVersion() {
	String SVN = "$Rev: 183 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperRest::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugRest, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugRest", Debug);
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
bool helperRest::sendRestRGB(uint8 r, uint8 g, uint8 b, uint8 br) {
	String rgb = String(r) + "&g=" + String(g) + "&b=" + String(b) + "&br=" + String(br);
	return sendRest("r", rgb);
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
		publishErrorLast = 0;
		publishDebugLast = 0;
	}
	if(errorLast != errorIsSet || wpFZ.CheckQoS(publishErrorLast)) {
		errorLast = errorIsSet;
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(errorIsSet).c_str());
		publishErrorLast = wpFZ.loopStartedAt;
	}
	if(DebugLast != Debug || wpFZ.CheckQoS(publishDebugLast)) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishDebugLast = wpFZ.loopStartedAt;
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
			bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugRest, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
			EEPROM.commit();
			wpFZ.SendWSDebug("DebugRest", Debug);
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}

//###################################################################################
// private
//###################################################################################

