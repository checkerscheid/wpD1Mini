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
	EEPROM.begin(4095);
	mqttTopicDebugEEPROM = wpFZ.DeviceName + "/settings/Debug/EEPROM";
}

//###################################################################################
// public
//###################################################################################
void helperEEPROM::cycle() {
}

uint16_t helperEEPROM::getVersion() {
	String SVN = "$Rev: 120 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperEEPROM::changeDebug() {
	DebugEEPROM = !DebugEEPROM;
	bitWrite(bitsDebugBasis, bitDebugEEPROM, DebugEEPROM);
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

void helperEEPROM::publishSettings() {
	publishSettings(false);
}
void helperEEPROM::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebugEEPROM.c_str(), String(DebugEEPROM).c_str());
	}
}

void helperEEPROM::publishValues() {
	publishValues(false);
}
void helperEEPROM::publishValues(bool force) {
	if(force) publishCountDebugEEPROM = wpFZ.publishQoS;
	if(DebugEEPROMLast != DebugEEPROM || ++publishCountDebugEEPROM > wpFZ.publishQoS) {
		DebugEEPROMLast = DebugEEPROM;
		wpMqtt.mqttClient.publish(mqttTopicDebugEEPROM.c_str(), String(DebugEEPROM).c_str());
		publishCountDebugEEPROM = 0;
	}
}

void helperEEPROM::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicDebugEEPROM.c_str());
}

void helperEEPROM::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicDebugEEPROM.c_str()) == 0) {
		bool readDebugEEPROM = msg.toInt();
		if(DebugEEPROM != readDebugEEPROM) {
			DebugEEPROM = readDebugEEPROM;
			bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugEEPROM, DebugEEPROM);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
			EEPROM.commit();
			wpFZ.SendWS("{\"id\":\"DebugEEPROM\",\"value\":" + String(DebugEEPROM ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicDebugEEPROM, String(DebugEEPROM));
		}
	}
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

void helperEEPROM::readVars() {
	/// bool values: byte 0 - 9
		bitsModules0 = EEPROM.read(addrBitsModules0);
		// bitDHT11 = 0;
		// bitDHT22 = 1;
		// bitLDR = 2;
		// bitLight = 3;
		// bitBM = 4;
		// bitRelais = 5;
		// bitRelaisShield = 6;
		// bitRain = 7;

		bitsModules1 = EEPROM.read(addrBitsModules1);
		// bitMoisture = 0;
		// bitDistance = 1;
		
		bitsDebugBasis = EEPROM.read(addrBitsDebugBasis);
		DebugEEPROM = bitRead(bitsDebugBasis, bitDebugEEPROM);
		wpWiFi.DebugWiFi = bitRead(bitsDebugBasis, bitDebugWiFi);
		wpMqtt.DebugMqtt = bitRead(bitsDebugBasis, bitDebugMqtt);
		wpFinder.DebugFinder = bitRead(bitsDebugBasis, bitDebugFinder);
		wpRest.DebugRest = bitRead(bitsDebugBasis, bitDebugRest);
		wpOnlineToggler.DebugOnlineToggler = bitRead(bitsDebugBasis, bitDebugOnlineToggler);
		
		bitsDebugModules = EEPROM.read(addrBitsDebugModules);
		wpDHT.DebugHT = bitRead(bitsDebugModules, bitDebugHT);
		// bitDebugLDR = 1;
		// bitDebugLight = 2;
		// bitDebugBM = 3;
		// bitDebugRelais = 4;
		// bitDebugRain = 5;
		// bitDebugMoisture = 6;
		// bitDebugDistance = 7;
		
		bitsModulesSettings = EEPROM.read(addrBitsModulesSettings);
		// bitUseLdrAvg = 0;
		// bitUseLightAvg = 1;
		// bitRelaisHand = 2;
		// bitRelaisHandValue = 3;
		// bitUseRainAvg = 4;
		// bitUseMoistureAvg = 5;

/// byte values: byte 10 - 29
		wpDHT.maxCycle = EEPROM.read(byteMaxCycleHT);
		wpDHT.temperatureCorrection = EEPROM.read(byteTemperatureCorrection);
		wpDHT.humidityCorrection = EEPROM.read(byteHumidityCorrection);
		// byteMaxCycleLDR = 11;
		// byteLDRCorrection = 12; // int8_t
		// byteMaxCycleLight = 13;
		// bytePumpActive = 14;
		// byteMaxCycleRain = 15;
		// byteRainCorrection = 16; // int8_t
		// byteMaxCycleMoisture = 17;
		// byteMoistureMin = 18;
		// byteMaxCycleDistance = 19;
		// byteDistanceCorrection = 20; // int8_t
		// byteHeight = 21;

/// byte values: 2byte 30 - 59
		// byteLightCorrection = 34; // int16_t
		// byteThreshold = 36;
		// bytePumpPause = 38;
		// byteMoistureDry = 40;
		// byteMoistureWet = 42;
		// byteMaxVolume = 44;

		readStringsFromEEPROM();
}
