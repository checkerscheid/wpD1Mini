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
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Init");
#endif
	// settings
	mqttTopicOnlineToggler = wpFZ.DeviceName + "/info/Online";
	mqttTopicErrorOnline = wpFZ.DeviceName + "/ERROR/Online";
	// commands
	mqttTopicDebugOnlineToggler = wpFZ.DeviceName + "/settings/Debug/OnlineToggler";

	OfflineTrigger = false;	
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Inited");
#endif
}

//###################################################################################
// public
//###################################################################################
void helperOnlineToggler::cycle() {
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycle");
#endif
	checkOfflineTrigger();
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycled");
#endif
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

//###################################################################################
// private
//###################################################################################

void helperOnlineToggler::checkOfflineTrigger() {
	if(OfflineTrigger) {
		// set offline for reboot
		setMqttOffline();
		OfflineTrigger = false;
	}
}
