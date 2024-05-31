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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperRest.cpp 120 2024-05-31 03:32:41Z                  $ #
//#                                                                                 #
//###################################################################################
#include <helperRest.h>

helperRest wpRest;

helperRest::helperRest() {}
void helperRest::init() {
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Init");
#endif	
	// settings
	mqttTopicRestServer = wpFZ.DeviceName + "/info/Rest/Server";
	// commands
	mqttTopicErrorRest = wpFZ.DeviceName + "/ERROR/Rest";
	mqttTopicDebugRest = wpFZ.DeviceName + "/settings/Debug/Rest";

	setupRest();
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Inited");
#endif
}

//###################################################################################
// public
//###################################################################################
void helperRest::cycle() {
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycle");
#endif
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycled");
#endif
}

uint16_t helperRest::getVersion() {
	String SVN = "$Rev: 120 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperRest::changeDebug() {
	DebugRest = !DebugRest;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugRest, DebugRest);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugRest\",\"value\":" + String(DebugRest ? "true" : "false") + "}");
	wpFZ.blink();
}

void helperRest::setupRest() {
}

// return true on success
bool helperRest::sendRest(String name, String value) {
	bool returns = false;
	WiFiClient client;
	HTTPClient http;
	String targetmac = WiFi.macAddress();
	targetmac.replace(":", "");
	targetmac.toLowerCase();
	String targetwithident = "http://" + String(wpFZ.restServer) + ":" + String(wpFZ.restServerPort) + "/?m=" + targetmac;
	String target = targetwithident + "&" + name + "=" + value;
	if(DebugRest) {
		String logmessage = "HTTP: '" + target + "'";
		wpFZ.DebugWS(wpFZ.strINFO, "sendRest", logmessage);
	}
	http.begin(client, target.c_str());
	int httpResponseCode = http.GET();
	if (httpResponseCode > 0) {
		if(DebugRest) {
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
	if(DebugRest) {
		String logmessage = "HTTP: '" + target + "'";
		wpFZ.DebugWS(wpFZ.strINFO, "sendRawRest", logmessage);
	}
	http.begin(client, target.c_str());
	int httpResponseCode = http.GET();
	if (httpResponseCode > 0) {
		if(DebugRest) {
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

void helperRest::publishErrorRest() {
	wpMqtt.mqttClient.publish(mqttTopicErrorRest.c_str(), String(errorRest).c_str());
	errorRestLast = errorRest;
	trySendRest = false;
	if(DebugRest)
		publishInfoDebug("ErrorRest", String(errorRest), String(publishCountErrorRest));
	publishCountErrorRest = 0;
}

//###################################################################################
// private
//###################################################################################

void helperRest::publishInfoDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
