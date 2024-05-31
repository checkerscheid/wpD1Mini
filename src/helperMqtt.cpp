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
	mqttTopicDebugMqtt = wpFZ.DeviceName + "/settings/Debug/MQTT";
	
	mqttClient.setServer(wpFZ.mqttServer, wpFZ.mqttServerPort);
	mqttClient.setCallback(callbackMqtt);
	connectMqtt();
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
	wpMqtt.DebugMqtt = !wpMqtt.DebugMqtt;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugMqtt, wpMqtt.DebugMqtt);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugMqtt\",\"value\":" + String(wpMqtt.DebugMqtt ? "true" : "false") + "}");
	wpFZ.blink();
}

void helperMqtt::publishSettings() {
	publishSettings(false);
}
void helperMqtt::publishSettings(bool force) {
	// values
	mqttClient.publish(mqttTopicMqttSince.c_str(), MqttSince.c_str());
	// settings
	mqttClient.publish(mqttTopicMqttServer.c_str(), (String(wpFZ.mqttServer) + ":" + String(wpFZ.mqttServerPort)).c_str(), true);
	if(force) {
		mqttClient.publish(mqttTopicDebugMqtt.c_str(), String(DebugMqtt).c_str());
		mqttClient.publish(mqttTopicForceMqttUpdate.c_str(), "0");
		mqttClient.publish(mqttTopicForceRenewValue.c_str(), "0");
	}
	wpFZ.publishSettings(force);
	wpWiFi.publishSettings(force);
}
void helperMqtt::publishValues() {
	if(DebugMqttLast != DebugMqtt || ++publishCountDebugMqtt > wpFZ.publishQoS) {
		DebugMqttLast = DebugMqtt;
		mqttClient.publish(mqttTopicDebugMqtt.c_str(), String(DebugMqtt).c_str());
		publishCountDebugMqtt = 0;
	}
	wpFZ.publishValues();
	wpWiFi.publishValues();
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
			String logmessage = "MQTT Connected";
			wpFZ.DebugWS(wpFZ.strINFO, "connectMqtt", logmessage);
		} else {
			String logmessage =  "failed, rc= " + String(mqttClient.state()) + ",  will try again in 5 seconds";
			wpFZ.DebugWS(wpFZ.strERRROR, "connectMqtt", logmessage);
			delay(5000);
		}
	}
}

void helperMqtt::callbackMqtt(char* topic, byte* payload, unsigned int length) {
	String msg = "";
	for (unsigned int i = 0; i < length; i++) {
		msg += (char)payload[i];
	}
	if(wpMqtt.DebugMqtt) {
		wpFZ.DebugWS(wpFZ.strDEBUG, "callbackMqtt", "Message arrived on topic: '" + String(topic) + "': " + msg);
	}
	if(msg == "") {
		wpFZ.DebugWS(wpFZ.strWARN, "callbackMqtt", "msg is empty, '" + String(topic) + "'");
	} else {
		if(strcmp(topic, wpMqtt.mqttTopicForceMqttUpdate.c_str()) == 0) {
			int readForceMqttUpdate = msg.toInt();
			if(readForceMqttUpdate != 0) {
				wpMqtt.publishSettings(true);
				wpMqtt.publishValues();
				//reset
				mqttClient.publish(wpMqtt.mqttTopicForceMqttUpdate.c_str(), String(false).c_str());
				wpFZ.DebugcheckSubscripes(wpMqtt.mqttTopicForceMqttUpdate, String(readForceMqttUpdate));
			}
		}
		if(strcmp(topic, wpMqtt.mqttTopicForceRenewValue.c_str()) == 0) {
			int readForceRenewValue = msg.toInt();
			if(readForceRenewValue != 0) {
				wpMqtt.publishValues();
				//reset
				mqttClient.publish(wpMqtt.mqttTopicForceRenewValue.c_str(), String(false).c_str());
				wpFZ.DebugcheckSubscripes(wpMqtt.mqttTopicForceRenewValue, String(readForceRenewValue));
			}
		}
		if(strcmp(topic, wpMqtt.mqttTopicDebugMqtt.c_str()) == 0) {
			bool readDebugMqtt = msg.toInt();
			if(wpMqtt.DebugMqtt != readDebugMqtt) {
				wpMqtt.DebugMqtt = readDebugMqtt;
				bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugMqtt, wpMqtt.DebugMqtt);
				EEPROM.write(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugWiFi);
				EEPROM.commit();
				wpFZ.SendWS("{\"id\":\"DebugMqtt\",\"value\":" + String(wpMqtt.DebugMqtt ? "true" : "false") + "}");
				wpFZ.DebugcheckSubscripes(wpMqtt.mqttTopicDebugMqtt, String(wpMqtt.DebugMqtt));
			}
		}
		wpFZ.checkSubscripes(topic, msg);
		wpWiFi.checkSubscripes(topic, msg);
	}
}
