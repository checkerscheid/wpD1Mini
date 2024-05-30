//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 29.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.cpp 117 2024-05-29 01:28:02Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperEEPROM.h>

helperEEPROM wpEEPROM();

helperEEPROM::helperEEPROM() {
}

//###################################################################################
// public
//###################################################################################
void helperEEPROM::loop() {
}

uint16_t helperEEPROM::getVersion() {
	String SVN = "$Rev: 118 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperEEPROM::changeDebug() {
	wpFZ.DebugEprom = !wpFZ.DebugEprom;
	bitWrite(wpFZ.bitsDebugBais, wpFZ.bitDebugEprom, wpFZ.DebugEprom);
	EEPROM.write(wpFZ.addrBitsDebugBasis, wpFZ.bitsDebugBais);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugEprom\",\"value\":" + String(wpFZ.DebugEprom ? "true" : "false") + "}");
	wpFZ.blink();
}

//###################################################################################
// private
//###################################################################################
