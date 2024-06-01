//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 08.03.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperMqtt.cpp 120 2024-05-31 03:32:41Z                  $ #
//#                                                                                 #
//###################################################################################
#include <helperMqtt.h>

helperMqtt wpMqtt;

WiFiClient helperMqtt::wifiClient;
PubSubClient helperMqtt::mqttClient(helperMqtt::wifiClient);

helperMqtt::helperMqtt() {}
void helperMqtt::init() {
	// values
	mqttTopicMqttSince = wpFZ.DeviceName + "/info/MQTT/Since";
	// settings
	mqttTopicMqttServer = wpFZ.DeviceName + "/info/MQTT/Server";
	// commands
	mqttTopicForceMqttUpdate = wpFZ.DeviceName + "/ForceMqttUpdate";
	mqttTopicForceRenewValue = wpFZ.DeviceName + "/ForceRenewValue";
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/MQTT";
	
	mqttClient.setServer(wpFZ.mqttServer, wpFZ.mqttServerPort);
	mqttClient.setCallback(callbackMqtt);
}

//###################################################################################
// public
//###################################################################################
void helperMqtt::cycle() {
	if(!mqttClient.connected()) {
		connectMqtt();
	}
	mqttClient.loop();
	publishValues();
}

uint16_t helperMqtt::getVersion() {
	String SVN = "$Rev: 120 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperMqtt::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugMqtt, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugMqtt\",\"value\":" + String(Debug ? "true" : "false") + "}");
	wpFZ.blink();
}

void helperMqtt::publishSettings() {
	publishSettings(false);
}
void helperMqtt::publishSettings(bool force) {
	// values
	mqttClient.publish(mqttTopicMqttSince.c_str(), MqttSince.c_str());
	// settings
	mqttClient.publish(mqttTopicMqttServer.c_str(), (String(wpFZ.mqttServer) + ":" + String(wpFZ.mqttServerPort)).c_str());
	if(force) {
		mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		mqttClient.publish(mqttTopicForceMqttUpdate.c_str(), "0");
		mqttClient.publish(mqttTopicForceRenewValue.c_str(), "0");
	}
	wpFZ.publishSettings(force);
	wpEEPROM.publishSettings(force);
	wpWiFi.publishSettings(force);
	wpOnlineToggler.publishSettings(force);
	wpFinder.publishSettings(force);
	wpWebServer.publishSettings(force);
	wpRest.publishSettings(force);
}
void helperMqtt::publishValues() {
	publishValues(false);
}
void helperMqtt::publishValues(bool force) {
	if(force) publishCountDebug = wpFZ.publishQoS;
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
	wpFZ.publishValues(force);
	wpEEPROM.publishValues(force);
	wpWiFi.publishValues(force);
	wpOnlineToggler.publishValues(force);
	wpFinder.publishValues(force);
	wpWebServer.publishValues(force);
	wpRest.publishValues(force);
}

//###################################################################################
// private
//###################################################################################
void helperMqtt::connectMqtt() {
	String logmessage = "Connecting MQTT Server: " + String(wpFZ.mqttServer) + ":" + String(wpFZ.mqttServerPort) + " as " + wpFZ.DeviceName;
	wpFZ.DebugWS(wpFZ.strINFO, "connectMqtt", logmessage);
	while(!mqttClient.connected()) {
		if(mqttClient.connect(wpFZ.DeviceName.c_str())) {
			MqttSince = wpFZ.getDateTime();
			mqttClient.publish(wpOnlineToggler.mqttTopicOnlineToggler.c_str(), String(1).c_str());
			setSubscribes();
			String logmessage = "MQTT Connected";
			wpFZ.DebugWS(wpFZ.strINFO, "connectMqtt", logmessage);
		} else {
			String logmessage =  "failed, rc= " + String(mqttClient.state()) + ",  will try again in 5 seconds";
			wpFZ.DebugWS(wpFZ.strERRROR, "connectMqtt", logmessage);
			delay(5000);
		}
	}
}

void helperMqtt::setSubscribes() {
	mqttClient.subscribe(mqttTopicForceMqttUpdate.c_str());
	mqttClient.subscribe(mqttTopicForceRenewValue.c_str());
	mqttClient.subscribe(mqttTopicDebug.c_str());
	wpFZ.setSubscribes();
	wpWiFi.setSubscribes();
	wpEEPROM.setSubscribes();
	wpOnlineToggler.setSubscribes();
	wpFinder.setSubscribes();
	wpWebServer.setSubscribes();
	wpRest.setSubscribes();
}
void helperMqtt::callbackMqtt(char* topic, byte* payload, unsigned int length) {
	String msg = "";
	for (unsigned int i = 0; i < length; i++) {
		msg += (char)payload[i];
	}
	if(wpMqtt.Debug) {
		wpFZ.DebugWS(wpFZ.strDEBUG, "callbackMqtt", "Message arrived on topic: '" + String(topic) + "': " + msg);
	}
	if(msg == "") {
		wpFZ.DebugWS(wpFZ.strWARN, "callbackMqtt", "msg is empty, '" + String(topic) + "'");
	} else {
		if(strcmp(topic, wpMqtt.mqttTopicForceMqttUpdate.c_str()) == 0) {
			int readForceMqttUpdate = msg.toInt();
			if(readForceMqttUpdate != 0) {
				wpMqtt.publishSettings(true);
				wpMqtt.publishValues(true);
				//reset
				wpMqtt.mqttClient.publish(wpMqtt.mqttTopicForceMqttUpdate.c_str(), String(false).c_str());
				wpFZ.DebugcheckSubscribes(wpMqtt.mqttTopicForceMqttUpdate, String(readForceMqttUpdate));
			}
		}
		if(strcmp(topic, wpMqtt.mqttTopicForceRenewValue.c_str()) == 0) {
			int readForceRenewValue = msg.toInt();
			if(readForceRenewValue != 0) {
				wpMqtt.publishValues(true);
				//reset
				wpMqtt.mqttClient.publish(wpMqtt.mqttTopicForceRenewValue.c_str(), String(false).c_str());
				wpFZ.DebugcheckSubscribes(wpMqtt.mqttTopicForceRenewValue, String(readForceRenewValue));
			}
		}
		if(strcmp(topic, wpMqtt.mqttTopicDebug.c_str()) == 0) {
			bool readDebug = msg.toInt();
			if(wpMqtt.Debug != readDebug) {
				wpMqtt.Debug = readDebug;
				bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugMqtt, wpMqtt.Debug);
				EEPROM.write(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugWiFi);
				EEPROM.commit();
				wpFZ.SendWS("{\"id\":\"DebugMqtt\",\"value\":" + String(wpMqtt.Debug ? "true" : "false") + "}");
				wpFZ.DebugcheckSubscribes(wpMqtt.mqttTopicDebug, String(wpMqtt.Debug));
			}
		}
		wpFZ.checkSubscribes(topic, msg);
		wpWiFi.checkSubscribes(topic, msg);
		wpEEPROM.checkSubscribes(topic, msg);
		wpOnlineToggler.checkSubscribes(topic, msg);
		wpFinder.checkSubscribes(topic, msg);
		wpWebServer.checkSubscribes(topic, msg);
		wpRest.checkSubscribes(topic, msg);
	}
}
