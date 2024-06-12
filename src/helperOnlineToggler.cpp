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
//# Revision     : $Rev:: 141                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperOnlineToggler.cpp 141 2024-06-12 06:33:28Z         $ #
//#                                                                                 #
//###################################################################################
#include <helperOnlineToggler.h>

helperOnlineToggler wpOnlineToggler;

helperOnlineToggler::helperOnlineToggler() {}
void helperOnlineToggler::init() {
	lastContact = millis();
	// settings
	mqttTopicErrorOnline = wpFZ.DeviceName + "/ERROR/Online";
	// commands
	mqttTopicOnlineToggler = wpFZ.DeviceName + "/info/Online";
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/OnlineToggler";

}

//###################################################################################
// public
//###################################################################################
void helperOnlineToggler::cycle() {
	publishValues();
	if(millis() > lastContact + Minutes10) {
		wpFZ.DebugWS(wpFZ.strWARN, "OnlineToggler", "last Contact is 10 Minutes ago, renew Subscribes");
		wpModules.setAllSubscribes();
	}
}

uint16_t helperOnlineToggler::getVersion() {
	String SVN = "$Rev: 141 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperOnlineToggler::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugOnlineToggler, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugOnlineToggler", Debug);
	wpFZ.blink();
}

void helperOnlineToggler::setMqttOffline() {
	wpMqtt.mqttClient.publish(mqttTopicErrorOnline.c_str(), String(1).c_str());
}

void helperOnlineToggler::publishSettings() {
	publishSettings(false);
}
void helperOnlineToggler::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperOnlineToggler::publishValues() {
	publishValues(false);
}
void helperOnlineToggler::publishValues(bool force) {
	if(force) publishCountDebug = wpFZ.publishQoS;
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void helperOnlineToggler::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicOnlineToggler.c_str());
}
void helperOnlineToggler::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicOnlineToggler.c_str()) == 0) {
		int readOnline = msg.toInt();
		if(readOnline != 1) {
			//reset
			wpMqtt.mqttClient.publish(mqttTopicOnlineToggler.c_str(), String(1).c_str());
		}
		lastContact = millis();
		if(Debug) {
			wpFZ.DebugWS(wpFZ.strDEBUG, "TopicOnlineToggler", "get 'Online question' Message From Server, reset counter");
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugOnlineToggler, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
			EEPROM.commit();
			wpFZ.SendWSDebug("DebugOnlineToggler", Debug);
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}

//###################################################################################
// private
//###################################################################################

