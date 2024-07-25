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
//# Revision     : $Rev:: 175                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.cpp 175 2024-07-24 15:31:08Z                $ #
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

uint16 helperEEPROM::getVersion() {
	String SVN = "$Rev: 175 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
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
	byteStartForString = byteStartForString + 1 + wpBM.lightToTurnOn.length();
	wpWindow.lightToTurnOn = readStringFromEEPROM(byteStartForString, wpWindow.lightToTurnOn);
}

void helperEEPROM::writeStringsToEEPROM() {
	byteStartForString = addrStartForString0; // reset
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceDescription);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpBM.lightToTurnOn);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpWindow.lightToTurnOn);
}

void helperEEPROM::saveBool(uint16 &addr, byte &by, uint8 &bi, bool v) {
	bitWrite(by, bi, v);
	EEPROM.write(addr, by);
	EEPROM.commit();
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
/// bool values: byte 0 - 19
	bitsModules0 = EEPROM.read(addrBitsModules0);
	bitsModules1 = EEPROM.read(addrBitsModules1);

	wpModules.useModuleDHT11 = bitRead(bitsModules0, bitUseDHT11);
	wpModules.useModuleDHT22 = bitRead(bitsModules0, bitUseDHT22);
	wpModules.useModuleLDR = bitRead(bitsModules0, bitUseLDR);
	wpModules.useModuleLight = bitRead(bitsModules0, bitUseLight);
	wpModules.useModuleBM = bitRead(bitsModules0, bitUseBM);
	wpModules.useModuleWindow = bitRead(bitsModules1, bitUseWindow);
	wpModules.useModuleAnalogOut = bitRead(bitsModules1, bitUseAnalogOut);
	wpModules.useModuleAnalogOut2 = bitRead(bitsModules1, bitUseAnalogOut2);
	wpModules.useModuleNeoPixel = bitRead(bitsModules1, bitUseNeoPixel);
	wpModules.useModuleRelais = bitRead(bitsModules0, bitUseRelais);
	wpModules.useModuleRelaisShield = bitRead(bitsModules0, bitUseRelaisShield);
	wpModules.useModuleRpm = bitRead(bitsModules1, bitUseRpm);
	wpModules.useModuleRain = bitRead(bitsModules0, bitUseRain);
	wpModules.useModuleMoisture = bitRead(bitsModules1, bitUseMoisture);
	wpModules.useModuleDistance = bitRead(bitsModules1, bitUseDistance);

//###################################################################################

	bitsSendRestBasis0 = EEPROM.read(addrBitsSendRestBasis0);
	wpWiFi.sendRest = bitRead(bitSendRestRssi, bitsSendRestBasis0);

//###################################################################################

	bitsDebugBasis0 = EEPROM.read(addrBitsDebugBasis0);
	bitsDebugBasis1 = EEPROM.read(addrBitsDebugBasis1);
	Debug = bitRead(bitsDebugBasis0, bitDebugEEPROM);
	wpFinder.Debug = bitRead(bitsDebugBasis0, bitDebugFinder);
	wpModules.Debug = bitRead(bitsDebugBasis0, bitDebugModules);
	wpMqtt.Debug = bitRead(bitsDebugBasis0, bitDebugMqtt);
	wpOnlineToggler.Debug = bitRead(bitsDebugBasis0, bitDebugOnlineToggler);
	wpRest.Debug = bitRead(bitsDebugBasis0, bitDebugRest);
	wpUpdate.Debug = bitRead(bitsDebugBasis0, bitDebugUpdate);
	wpWebServer.Debug = bitRead(bitsDebugBasis0, bitDebugWebServer);
	wpWiFi.Debug = bitRead(bitsDebugBasis1, bitDebugWiFi);

//###################################################################################

	bitsSendRestModules0 = EEPROM.read(addrBitsSendRestModules0);
	bitsSendRestModules1 = EEPROM.read(addrBitsSendRestModules1);
	wpDHT.SendRest(bitRead(bitsSendRestModules0, bitSendRestDHT));
	wpLDR.SendRest(bitRead(bitsSendRestModules0, bitSendRestLDR));
	wpLight.SendRest(bitRead(bitsSendRestModules0, bitSendRestLight));
	wpBM.SendRest(bitRead(bitsSendRestModules0, bitSendRestBM));
	wpWindow.SendRest(bitRead(bitsSendRestModules1, bitSendRestWindow));
	wpAnalogOut.SendRest(bitRead(bitsSendRestModules1, bitSendRestAnalogOut));
	wpAnalogOut2.SendRest(bitRead(bitsSendRestModules1, bitSendRestAnalogOut2));
	wpNeoPixel.SendRest(bitRead(bitsSendRestModules1, bitSendRestNeoPixel));
	wpRelais.SendRest(bitRead(bitsSendRestModules0, bitSendRestRelais));
	wpRpm.SendRest(bitRead(bitsSendRestModules1, bitSendRestRpm));
	wpRain.SendRest(bitRead(bitsSendRestModules0, bitSendRestRain));
	wpMoisture.SendRest(bitRead(bitsSendRestModules0, bitSendRestMoisture));
	wpDistance.SendRest(bitRead(bitsSendRestModules0, bitSendRestDistance));

//###################################################################################

	bitsDebugModules0 = EEPROM.read(addrBitsDebugModules0);
	bitsDebugModules1 = EEPROM.read(addrBitsDebugModules1);
	wpDHT.Debug(bitRead(bitsDebugModules0, bitDebugDHT));
	wpLDR.Debug(bitRead(bitsDebugModules0, bitDebugLDR));
	wpLight.Debug(bitRead(bitsDebugModules0, bitDebugLight));
	wpBM.Debug(bitRead(bitsDebugModules0, bitDebugBM));
	wpWindow.Debug(bitRead(bitsDebugModules1, bitDebugWindow));
	wpAnalogOut.Debug(bitRead(bitsDebugModules1, bitDebugAnalogOut));
	wpAnalogOut2.Debug(bitRead(bitsDebugModules1, bitDebugAnalogOut2));
	wpNeoPixel.Debug(bitRead(bitsDebugModules1, bitDebugNeoPixel));
	wpRelais.Debug(bitRead(bitsDebugModules0, bitDebugRelais));
	wpRpm.Debug(bitRead(bitsDebugModules1, bitDebugRpm));
	wpRain.Debug(bitRead(bitsDebugModules0, bitDebugRain));
	wpMoisture.Debug(bitRead(bitsDebugModules0, bitDebugMoisture));
	wpDistance.Debug(bitRead(bitsDebugModules0, bitDebugDistance));

//###################################################################################

	bitsSettingsModules0 = EEPROM.read(addrBitsSettingsModules0);
	wpLDR.UseAvg(bitRead(bitsSettingsModules0, bitUseLdrAvg));
	wpLight.UseAvg(bitRead(bitsSettingsModules0, bitUseLightAvg));
	wpAnalogOut.handSet = bitRead(bitsSettingsModules0, bitAnalogOutHand);
	wpAnalogOut2.handSet = bitRead(bitsSettingsModules1, bitAnalogOut2Hand);
	wpRelais.handSet = bitRead(bitsSettingsModules0, bitRelaisHand);
	wpRelais.handValueSet = bitRead(bitsSettingsModules0, bitRelaisHandValue);
	wpRelais.waterEmptySet = bitRead(bitsSettingsModules0, bitRelaisWaterEmpty);
	wpRain.UseAvg(bitRead(bitsSettingsModules0, bitUseRainAvg));
	wpMoisture.UseAvg(bitRead(bitsSettingsModules0, bitUseMoistureAvg));

//###################################################################################
/// byte values: byte 20 - 49
	wpDHT.MaxCycle(EEPROM.read(byteMaxCycleDHT));
	wpDHT.temperatureCorrection = EEPROM.read(byteTemperatureCorrection);
	wpDHT.humidityCorrection = EEPROM.read(byteHumidityCorrection);
	wpLDR.MaxCycle(EEPROM.read(byteMaxCycleLDR));
	wpLDR.correction = EEPROM.read(byteLDRCorrection);
	wpLight.MaxCycle(EEPROM.read(byteMaxCycleLight));
	wpAnalogOut.MaxCycle(EEPROM.read(byteMaxCycleAnalogOut));
	wpAnalogOut.handValueSet = EEPROM.read(byteAnalogOutHandValue);
	wpAnalogOut2.MaxCycle(EEPROM.read(byteMaxCycleAnalogOut2));
	wpAnalogOut2.handValueSet = EEPROM.read(byteAnalogOut2HandValue);
	wpNeoPixel.MaxCycle(EEPROM.read(byteMaxCycleNeoPixel));
	wpNeoPixel.setValueR(EEPROM.read(byteNeoPixelValueR));
	wpNeoPixel.setValueG(EEPROM.read(byteNeoPixelValueG));
	wpNeoPixel.setValueB(EEPROM.read(byteNeoPixelValueB));
	wpNeoPixel.setBrightness(EEPROM.read(byteNeoPixelBrightness));
	wpRelais.pumpActive = EEPROM.read(bytePumpActive);
	wpRpm.MaxCycle(EEPROM.read(byteMaxCycleRpm));
	wpRain.MaxCycle(EEPROM.read(byteMaxCycleRain));
	wpRain.correction = EEPROM.read(byteRainCorrection);
	wpMoisture.MaxCycle(EEPROM.read(byteMaxCycleMoisture));
	wpMoisture.minValue = EEPROM.read(byteMoistureMin);
	wpDistance.MaxCycle(EEPROM.read(byteMaxCycleDistance));
	wpDistance.correction = EEPROM.read(byteDistanceCorrection);
	wpDistance.height = EEPROM.read(byteHeight);

//###################################################################################
/// byte values: 2byte 50 - 79
	EEPROM.get(byteLightCorrection, wpLight.correction); // int
	EEPROM.get(byteBMThreshold, wpBM.threshold);
	EEPROM.get(byteWindowThreshold, wpWindow.threshold);
	EEPROM.get(bytePumpPause, wpRelais.pumpPause);
	EEPROM.get(byteMoistureDry, wpMoisture.dry);
	EEPROM.get(byteMoistureWet, wpMoisture.wet);
	EEPROM.get(byteMaxVolume, wpDistance.volume);
	short outKp;
	EEPROM.get(byteAnalogOutKp, outKp);
	wpAnalogOut.setKp(outKp);
	short outTv;
	EEPROM.get(byteAnalogOutTv, outTv);
	wpAnalogOut.setTv(outTv);
	short outTn;
	EEPROM.get(byteAnalogOutTn, outTn);
	wpAnalogOut.setTn(outTn);
	short outSetPoint;
	EEPROM.get(byteAnalogOutSetPoint, outSetPoint);
	wpAnalogOut.setSetPoint(outSetPoint);

//###################################################################################
/// byte values: 4byte 80 - 99

	readStringsFromEEPROM();
}
