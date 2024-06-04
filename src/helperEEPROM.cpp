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
//# Revision     : $Rev:: 130                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.cpp 130 2024-06-04 01:09:41Z                $ #
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
	String SVN = "$Rev: 130 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperEEPROM::changeDebug() {
	Debug = !Debug;
	bitWrite(bitsDebugBasis, bitDebugEEPROM, Debug);
	EEPROM.write(addrBitsDebugBasis, bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugEEPROM\",\"value\":" + String(Debug ? "true" : "false") + "}");
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
			bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugEEPROM, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
			EEPROM.commit();
			wpFZ.SendWS("{\"id\":\"DebugEEPROM\",\"value\":" + String(Debug ? "true" : "false") + "}");
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
		
		bitsDebugBasis = EEPROM.read(addrBitsDebugBasis);
		Debug = bitRead(bitsDebugBasis, bitDebugEEPROM);
		wpWiFi.Debug = bitRead(bitsDebugBasis, bitDebugWiFi);
		wpMqtt.Debug = bitRead(bitsDebugBasis, bitDebugMqtt);
		wpFinder.Debug = bitRead(bitsDebugBasis, bitDebugFinder);
		wpWebServer.Debug = bitRead(bitsDebugBasis, bitDebugWebServer);
		wpRest.Debug = bitRead(bitsDebugBasis, bitDebugRest);
		wpOnlineToggler.Debug = bitRead(bitsDebugBasis, bitDebugOnlineToggler);
		wpModules.Debug = bitRead(bitsDebugBasis, bitDebugModules);
		
		bitsDebugModules = EEPROM.read(addrBitsDebugModules);
		wpDHT.Debug = bitRead(bitsDebugModules, bitDebugDHT);
		wpLDR.Debug = bitRead(bitsDebugModules, bitDebugLDR);
		wpLight.Debug = bitRead(bitsDebugModules, bitDebugLight);
		wpBM.Debug = bitRead(bitsDebugModules, bitDebugBM);
		wpRelais.Debug = bitRead(bitsDebugModules, bitDebugRelais);
		wpRain.Debug = bitRead(bitsDebugModules, bitDebugRain);
		wpMoisture.Debug = bitRead(bitsDebugModules, bitDebugMoisture);
		wpDistance.Debug = bitRead(bitsDebugModules, bitDebugDistance);
		
		bitsModulesSettings = EEPROM.read(addrBitsModulesSettings);
		wpLDR.useAvg = bitRead(bitsModulesSettings, bitUseLdrAvg);
		wpLight.useAvg = bitRead(bitsModulesSettings, bitUseLightAvg);
		wpRelais.handSet = bitRead(bitsModulesSettings, bitRelaisHand);
		wpRelais.handValueSet = bitRead(bitsModulesSettings, bitRelaisHandValue);
		wpRelais.waterEmptySet = bitRead(bitsModulesSettings, bitRelaisWaterEmpty);
		wpRain.useAvg = bitRead(bitsModulesSettings, bitUseRainAvg);
		wpMoisture.useAvg = bitRead(bitsModulesSettings, bitUseMoistureAvg);

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
