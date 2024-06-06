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
//# Revision     : $Rev:: 135                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.cpp 135 2024-06-06 14:04:54Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperEEPROM.h>

helperEEPROM wpEEPROM;

helperEEPROM::helperEEPROM() {}
void helperEEPROM::init() {
	EEPROM.begin(4095);
	readVars();
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/EEPROM";
}

//###################################################################################
// public
//###################################################################################
void helperEEPROM::cycle() {
	publishValues();
}

uint16_t helperEEPROM::getVersion() {
	String SVN = "$Rev: 135 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperEEPROM::changeDebug() {
	Debug = !Debug;
	bitWrite(bitsDebugBasis0, bitDebugEEPROM, Debug);
	EEPROM.write(addrBitsDebugBasis0, bitsDebugBasis0);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugEEPROM", Debug);
	wpFZ.blink();
}

void helperEEPROM::readStringsFromEEPROM() {
	byteStartForString = addrStartForString0; // reset
	wpFZ.DeviceName = readStringFromEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = byteStartForString + 1 + wpFZ.DeviceName.length();
	wpFZ.DeviceDescription = readStringFromEEPROM(byteStartForString, wpFZ.DeviceDescription);
	byteStartForString = byteStartForString + 1 + wpFZ.DeviceDescription.length();
	wpBM.lightToTurnOn = readStringFromEEPROM(byteStartForString, wpBM.lightToTurnOn);
}

void helperEEPROM::writeStringsToEEPROM() {
	byteStartForString = addrStartForString0; // reset
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceDescription);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpBM.lightToTurnOn);
}

void helperEEPROM::publishSettings() {
	publishSettings(false);
}
void helperEEPROM::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperEEPROM::publishValues() {
	publishValues(false);
}
void helperEEPROM::publishValues(bool force) {
	if(force) publishCountDebug = wpFZ.publishQoS;
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void helperEEPROM::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}

void helperEEPROM::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugEEPROM, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
			EEPROM.commit();
			wpFZ.SendWSDebug("DebugEEPROM", Debug);
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}

//###################################################################################
// private
//###################################################################################
String helperEEPROM::readStringFromEEPROM(int addrOffset, String defaultString) {
	int newStrLen = EEPROM.read(addrOffset);
	if (newStrLen == 255) return defaultString;

	if(Debug) {
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
	if(Debug) {
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
		wpModules.useModuleDHT11 = bitRead(bitsModules0, bitUseDHT11);
		wpModules.useModuleDHT22 = bitRead(bitsModules0, bitUseDHT22);
		wpModules.useModuleLDR = bitRead(bitsModules0, bitUseLDR);
		wpModules.useModuleLight = bitRead(bitsModules0, bitUseLight);
		wpModules.useModuleBM = bitRead(bitsModules0, bitUseBM);
		wpModules.useModuleRelais = bitRead(bitsModules0, bitUseRelais);
		wpModules.useModuleRelaisShield = bitRead(bitsModules0, bitUseRelaisShield);
		wpModules.useModuleRain = bitRead(bitsModules0, bitUseRain);

		bitsModules1 = EEPROM.read(addrBitsModules1);
		wpModules.useModuleMoisture = bitRead(bitsModules1, bitUseMoisture);
		wpModules.useModuleDistance = bitRead(bitsModules1, bitUseDistance);
		
		bitsDebugBasis0 = EEPROM.read(addrBitsDebugBasis0);
		Debug = bitRead(bitsDebugBasis0, bitDebugEEPROM);
		wpWiFi.Debug = bitRead(bitsDebugBasis0, bitDebugWiFi);
		wpMqtt.Debug = bitRead(bitsDebugBasis0, bitDebugMqtt);
		wpFinder.Debug = bitRead(bitsDebugBasis0, bitDebugFinder);
		wpWebServer.Debug = bitRead(bitsDebugBasis0, bitDebugWebServer);
		wpRest.Debug = bitRead(bitsDebugBasis0, bitDebugRest);
		wpOnlineToggler.Debug = bitRead(bitsDebugBasis0, bitDebugOnlineToggler);
		wpModules.Debug = bitRead(bitsDebugBasis0, bitDebugModules);

		bitsDebugBasis1 = EEPROM.read(addrBitsDebugBasis1);

		bitsDebugModules0 = EEPROM.read(addrBitsDebugModules0);
		wpDHT.Debug = bitRead(bitsDebugModules0, bitDebugDHT);
		wpLDR.Debug = bitRead(bitsDebugModules0, bitDebugLDR);
		wpLight.Debug = bitRead(bitsDebugModules0, bitDebugLight);
		wpBM.Debug = bitRead(bitsDebugModules0, bitDebugBM);
		wpRelais.Debug = bitRead(bitsDebugModules0, bitDebugRelais);
		wpRain.Debug = bitRead(bitsDebugModules0, bitDebugRain);
		wpMoisture.Debug = bitRead(bitsDebugModules0, bitDebugMoisture);
		wpDistance.Debug = bitRead(bitsDebugModules0, bitDebugDistance);

		bitsDebugModules1 = EEPROM.read(addrBitsDebugModules1);
		wpWiFi.Debug = bitRead(bitsDebugModules1, bitDebugWiFi);
		
		bitsModulesSettings0 = EEPROM.read(addrBitsModulesSettings0);
		wpLDR.useAvg = bitRead(bitsModulesSettings0, bitUseLdrAvg);
		wpLight.useAvg = bitRead(bitsModulesSettings0, bitUseLightAvg);
		wpRelais.handSet = bitRead(bitsModulesSettings0, bitRelaisHand);
		wpRelais.handValueSet = bitRead(bitsModulesSettings0, bitRelaisHandValue);
		wpRelais.waterEmptySet = bitRead(bitsModulesSettings0, bitRelaisWaterEmpty);
		wpRain.useAvg = bitRead(bitsModulesSettings0, bitUseRainAvg);
		wpMoisture.useAvg = bitRead(bitsModulesSettings0, bitUseMoistureAvg);

/// byte values: byte 10 - 29
		wpDHT.maxCycle = EEPROM.read(byteMaxCycleDHT);
		wpDHT.temperatureCorrection = EEPROM.read(byteTemperatureCorrection);
		wpDHT.humidityCorrection = EEPROM.read(byteHumidityCorrection);
		wpLDR.maxCycle = EEPROM.read(byteMaxCycleLDR);
		wpLDR.correction = EEPROM.read(byteLDRCorrection);
		wpLight.maxCycle = EEPROM.read(byteMaxCycleLight);
		wpRelais.pumpActive = EEPROM.read(bytePumpActive);
		wpRain.maxCycle = EEPROM.read(byteMaxCycleRain);
		wpRain.correction = EEPROM.read(byteRainCorrection);
		wpMoisture.maxCycle = EEPROM.read(byteMaxCycleMoisture);
		wpMoisture.minValue = EEPROM.read(byteMoistureMin);
		wpDistance.maxCycle = EEPROM.read(byteMaxCycleDistance);
		wpDistance.correction = EEPROM.read(byteDistanceCorrection);
		wpDistance.height = EEPROM.read(byteHeight);

/// byte values: 2byte 30 - 59
		EEPROM.get(byteLightCorrection, wpLight.correction); // int16_t
		EEPROM.get(byteThreshold, wpBM.threshold);
		EEPROM.get(bytePumpPause, wpRelais.pumpPause);
		EEPROM.get(byteMoistureDry, wpMoisture.dry);
		EEPROM.get(byteMoistureWet, wpMoisture.wet);
		EEPROM.get(byteMaxVolume, wpDistance.volume);

		readStringsFromEEPROM();
}
