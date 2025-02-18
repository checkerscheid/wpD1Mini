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
//# Revision     : $Rev:: 246                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperOnlineToggler.cpp 246 2025-02-18 16:27:11Z         $ #
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
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/OnlineToggler";
}

//###################################################################################
// public
//###################################################################################
void helperOnlineToggler::cycle() {
	publishValues();
	if(wpFZ.loopStartedAt > lastContact + Minutes10) {
		wpFZ.DebugWS(wpFZ.strWARN, "OnlineToggler", "last Contact is 10 Minutes ago, 'checkSubscribes' not fired, restart ...");
		wpFZ.SetRestartReason(wpFZ.restartReasonOnlineToggler);
		ESP.restart();
	}
}

uint16 helperOnlineToggler::getVersion() {
	String SVN = "$Rev: 246 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperOnlineToggler::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugOnlineToggler, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
	wpFZ.DebugSaveBoolToEEPROM("DebugOnlineToggler", wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitDebugOnlineToggler, Debug);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "writeEEPROM", "DebugOnlineToggler: " + String(Debug));
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
	if(force) {
		publishDebugLast = 0;
		wpMqtt.mqttClient.publish(mqttTopicOnlineToggler.c_str(), String(1).c_str());
	}
	if(DebugLast != Debug || wpFZ.CheckQoS(publishDebugLast)) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishDebugLast = wpFZ.loopStartedAt;
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
		lastContact = wpFZ.loopStartedAt;
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

