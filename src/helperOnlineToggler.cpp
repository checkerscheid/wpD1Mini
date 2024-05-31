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
//# File-ID      : $Id:: helperOnlineToggler.cpp 120 2024-05-31 03:32:41Z         $ #
//#                                                                                 #
//###################################################################################
#include <helperOnlineToggler.h>

helperOnlineToggler wpOnlineToggler;

helperOnlineToggler::helperOnlineToggler() {}
void helperOnlineToggler::init() {
	// settings
	mqttTopicErrorOnline = wpFZ.DeviceName + "/ERROR/Online";
	// commands
	mqttTopicOnlineToggler = wpFZ.DeviceName + "/info/Online";
	mqttTopicDebugOnlineToggler = wpFZ.DeviceName + "/settings/Debug/OnlineToggler";

}

//###################################################################################
// public
//###################################################################################
void helperOnlineToggler::cycle() {
}

uint16_t helperOnlineToggler::getVersion() {
	String SVN = "$Rev: 120 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperOnlineToggler::changeDebug() {
	DebugOnlineToggler = !DebugOnlineToggler;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugOnlineToggler, DebugOnlineToggler);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugOnlineToggler\",\"value\":" + String(DebugOnlineToggler ? "true" : "false") + "}");
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
		wpMqtt.mqttClient.publish(mqttTopicDebugOnlineToggler.c_str(), String(DebugOnlineToggler).c_str());
	}
}

void helperOnlineToggler::publishValues() {
	publishValues(false);
}
void helperOnlineToggler::publishValues(bool force) {
	if(force) publishCountDebugOnlineToggler = wpFZ.publishQoS;
	if(DebugOnlineTogglerLast != DebugOnlineToggler || ++publishCountDebugOnlineToggler > wpFZ.publishQoS) {
		DebugOnlineTogglerLast = DebugOnlineToggler;
		wpMqtt.mqttClient.publish(mqttTopicDebugOnlineToggler.c_str(), String(DebugOnlineToggler).c_str());
		publishCountDebugOnlineToggler = 0;
	}
}

void helperOnlineToggler::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicOnlineToggler.c_str());
}
void helperOnlineToggler::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicDebugOnlineToggler.c_str()) == 0) {
		int readOnline = msg.toInt();
		if(readOnline != 1) {
			//reset
			wpMqtt.mqttClient.publish(mqttTopicOnlineToggler.c_str(), String(1).c_str());
		}
	}
}

//###################################################################################
// private
//###################################################################################

