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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.cpp 120 2024-05-31 03:32:41Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperEEPROM.h>

helperEEPROM wpEEPROM;

helperEEPROM::helperEEPROM() {}
void helperEEPROM::init() {
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Init");
#endif
	EEPROM.begin(4095);
	mqttTopicDebugEprom = wpFZ.DeviceName + "/settings/Debug/Eprom";	
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Inited");
#endif
}

//###################################################################################
// public
//###################################################################################
void helperEEPROM::cycle() {
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycle");
#endif
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycled");
#endif
}

uint16_t helperEEPROM::getVersion() {
	String SVN = "$Rev: 120 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperEEPROM::changeDebug() {
	DebugEEPROM = !DebugEEPROM;
	bitWrite(bitsDebugBasis, bitDebugEprom, DebugEEPROM);
	EEPROM.write(addrBitsDebugBasis, bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugEprom\",\"value\":" + String(DebugEEPROM ? "true" : "false") + "}");
	wpFZ.blink();
}

void helperEEPROM::readStringsFromEEPROM() {
	byteStartForString = byteStartForString0; // reset
	wpFZ.DeviceName = readStringFromEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = byteStartForString + 1 + wpFZ.DeviceName.length();
	wpFZ.DeviceDescription = readStringFromEEPROM(byteStartForString, wpFZ.DeviceDescription);
	//byteStartForString = byteStartForString + 1 + wpFZ.DeviceDescription.length();
	//wpFZ.lightToTurnOn = readStringFromEEPROM(byteStartForString, wpFZ.lightToTurnOn);
}

void helperEEPROM::writeStringsToEEPROM() {
	byteStartForString = byteStartForString0; // reset
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceDescription);
	//byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.lightToTurnOn);
}

//###################################################################################
// private
//###################################################################################
String helperEEPROM::readStringFromEEPROM(int addrOffset, String defaultString) {
	int newStrLen = EEPROM.read(addrOffset);
	if (newStrLen == 255) return defaultString;

	if(DebugEEPROM) {
		Serial.printf("newStrLen: %u\n", newStrLen);
	}
	char data[newStrLen];
	for (int i = 0; i < newStrLen; i++) {
		data[i] = EEPROM.read(addrOffset + 1 + i);
	}
	data[newStrLen] = '\0';
	return String(data);
}

int helperEEPROM::writeStringToEEPROM(int addrOffset, String &strToWrite) {
	byte len = strToWrite.length();
	EEPROM.write(addrOffset, len);
	int returns = addrOffset + 1;
	for (int i = 0; i < len; i++) {
		EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
		returns = addrOffset + 1 + i + 1;
	}
	EEPROM.commit();
	if(DebugEEPROM) {
		Serial.println();
		Serial.printf("Start: %u\n", addrOffset);
		Serial.printf("Len: %u\n", len);
		Serial.printf("Start Next: %u\n", returns);
	}
	return returns;
}