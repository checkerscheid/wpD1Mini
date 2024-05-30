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
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.cpp 117 2024-05-29 01:28:02Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperOnlineToggler.h>

helperOnlineToggler wpOnlineToggler();

helperOnlineToggler::helperOnlineToggler() {
	// settings
	mqttTopicOnlineToggler = wpFZ.DeviceName + "/info/Online";
	mqttTopicErrorOnline = wpFZ.DeviceName + "/ERROR/Online";
	// commands
	mqttTopicDebugOnlineToggler = wpFZ.DeviceName + "/settings/Debug/OnlineToggler";

	OfflineTrigger = false;
}

//###################################################################################
// public
//###################################################################################
void helperOnlineToggler::loop() {
	checkOfflineTrigger();
}

uint16_t helperOnlineToggler::getVersion() {
	String SVN = "$Rev: 118 $";
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

//###################################################################################
// private
//###################################################################################

void helperOnlineToggler::checkOfflineTrigger() {
	if(OfflineTrigger) {
		// set offline for reboot
		wpMqtt.setMqttOffline();
		OfflineTrigger = false;
	}
}
