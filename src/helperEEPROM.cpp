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
//# Revision     : $Rev:: 198                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.cpp 198 2024-09-05 12:32:25Z                $ #
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
	String SVN = "$Rev: 198 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperEEPROM::changeDebug() {
	Debug = !Debug;
	saveBool(addrBitsDebugBasis0, bitsDebugBasis0, bitDebugEEPROM, Debug);
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

void helperEEPROM::saveBool(uint16 addr, byte by, uint8 bi, bool v) {
	bitWrite(by, bi, !v);
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
	if(force) publishDebugLast = 0;
	if(DebugLast != Debug || wpFZ.CheckQoS(publishDebugLast)) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishDebugLast = wpFZ.loopStartedAt;
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
			saveBool(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugEEPROM, Debug);
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
	bitsModules2 = EEPROM.read(addrBitsModules2);
	if(bitsModules0 == 255 && bitsModules1 == 255 && bitsModules2 == 255) {
		// first boot
		EEPROM.write(bitsModules0, 0);
		EEPROM.write(bitsModules1, 0);
		EEPROM.write(bitsModules2, 0);
		bitsModules0 = 0;
		bitsModules1 = 0;
		bitsModules2 = 0;
	}

	wpModules.useModuleDHT11 = bitRead(bitsModules0, bitUseDHT11) == false;
	wpModules.useModuleDHT22 = bitRead(bitsModules0, bitUseDHT22) == false;
	wpModules.useModuleLDR = bitRead(bitsModules0, bitUseLDR) == false;
	wpModules.useModuleLight = bitRead(bitsModules0, bitUseLight) == false;
	wpModules.useModuleBM = bitRead(bitsModules0, bitUseBM) == false;
	wpModules.useModuleWindow = bitRead(bitsModules1, bitUseWindow) == false;
	wpModules.useModuleCwWw = bitRead(bitsModules2, bitUseCwWw) == false;
	wpModules.useModuleAnalogOut = bitRead(bitsModules1, bitUseAnalogOut) == false;
	wpModules.useModuleAnalogOut2 = bitRead(bitsModules1, bitUseAnalogOut2) == false;
	wpModules.useModuleNeoPixel = bitRead(bitsModules1, bitUseNeoPixel) == false;
	wpModules.useModuleRelais = bitRead(bitsModules0, bitUseRelais) == false;
	wpModules.useModuleRelaisShield = bitRead(bitsModules0, bitUseRelaisShield) == false;
	wpModules.useModuleRpm = bitRead(bitsModules1, bitUseRpm) == false;
	wpModules.useModuleRain = bitRead(bitsModules0, bitUseRain) == false;
	wpModules.useModuleMoisture = bitRead(bitsModules1, bitUseMoisture) == false;
	wpModules.useModuleDistance = bitRead(bitsModules1, bitUseDistance) == false;
	wpModules.useModuleImpulseCounter = bitRead(bitsModules1, bitUseImpulseCounter) == false;

//###################################################################################

	bitsSendRestBasis0 = EEPROM.read(addrBitsSendRestBasis0);
	wpWiFi.sendRest = !bitRead(bitSendRestRssi, bitsSendRestBasis0);

//###################################################################################

	bitsDebugBasis0 = EEPROM.read(addrBitsDebugBasis0);
	bitsDebugBasis1 = EEPROM.read(addrBitsDebugBasis1);
	Debug = bitRead(bitsDebugBasis0, bitDebugEEPROM) == false;
	wpFinder.Debug = bitRead(bitsDebugBasis0, bitDebugFinder) == false;
	wpModules.Debug = bitRead(bitsDebugBasis0, bitDebugModules) == false;
	wpMqtt.Debug = bitRead(bitsDebugBasis0, bitDebugMqtt) == false;
	wpOnlineToggler.Debug = bitRead(bitsDebugBasis0, bitDebugOnlineToggler) == false;
	wpRest.Debug = bitRead(bitsDebugBasis0, bitDebugRest) == false;
	wpUpdate.Debug = bitRead(bitsDebugBasis0, bitDebugUpdate) == false;
	wpWebServer.Debug = bitRead(bitsDebugBasis0, bitDebugWebServer) == false;
	wpWiFi.Debug = bitRead(bitsDebugBasis1, bitDebugWiFi) == false;

//###################################################################################

	bitsSendRestModules0 = EEPROM.read(addrBitsSendRestModules0);
	bitsSendRestModules1 = EEPROM.read(addrBitsSendRestModules1);
	wpDHT.SendRest(bitRead(bitsSendRestModules0, bitSendRestDHT) == false);
	wpLDR.SendRest(bitRead(bitsSendRestModules0, bitSendRestLDR) == false);
	wpLight.SendRest(bitRead(bitsSendRestModules0, bitSendRestLight) == false);
	wpBM.SendRest(bitRead(bitsSendRestModules0, bitSendRestBM) == false);
	wpWindow.SendRest(bitRead(bitsSendRestModules1, bitSendRestWindow) == false);
	wpAnalogOut.SendRest(bitRead(bitsSendRestModules1, bitSendRestAnalogOut) == false);
	wpAnalogOut2.SendRest(bitRead(bitsSendRestModules1, bitSendRestAnalogOut2) == false);
	wpNeoPixel.SendRest(bitRead(bitsSendRestModules1, bitSendRestNeoPixel) == false);
	wpRelais.SendRest(bitRead(bitsSendRestModules0, bitSendRestRelais) == false);
	wpRpm.SendRest(bitRead(bitsSendRestModules1, bitSendRestRpm) == false);
	wpRain.SendRest(bitRead(bitsSendRestModules0, bitSendRestRain) == false);
	wpMoisture.SendRest(bitRead(bitsSendRestModules0, bitSendRestMoisture) == false);
	wpDistance.SendRest(bitRead(bitsSendRestModules0, bitSendRestDistance) == false);
	wpImpulseCounter.SendRest(bitRead(bitsSendRestModules1, bitSendRestImpulseCounter) == false);

//###################################################################################

	bitsDebugModules0 = EEPROM.read(addrBitsDebugModules0);
	bitsDebugModules1 = EEPROM.read(addrBitsDebugModules1);
	wpDHT.Debug(bitRead(bitsDebugModules0, bitDebugDHT) == false);
	wpLDR.Debug(bitRead(bitsDebugModules0, bitDebugLDR) == false);
	wpLight.Debug(bitRead(bitsDebugModules0, bitDebugLight) == false);
	wpBM.Debug(bitRead(bitsDebugModules0, bitDebugBM) == false);
	wpWindow.Debug(bitRead(bitsDebugModules1, bitDebugWindow) == false);
	wpCwWw.Debug(bitRead(bitsDebugModules1, bitDebugCwWw) == false);
	wpAnalogOut.Debug(bitRead(bitsDebugModules1, bitDebugAnalogOut) == false);
	wpAnalogOut2.Debug(bitRead(bitsDebugModules1, bitDebugAnalogOut2) == false);
	wpNeoPixel.Debug(bitRead(bitsDebugModules1, bitDebugNeoPixel) == false);
	wpRelais.Debug(bitRead(bitsDebugModules0, bitDebugRelais) == false);
	wpRpm.Debug(bitRead(bitsDebugModules1, bitDebugRpm) == false);
	wpRain.Debug(bitRead(bitsDebugModules0, bitDebugRain) == false);
	wpMoisture.Debug(bitRead(bitsDebugModules0, bitDebugMoisture) == false);
	wpDistance.Debug(bitRead(bitsDebugModules0, bitDebugDistance) == false);
	wpImpulseCounter.Debug(bitRead(bitsDebugModules1, bitDebugImpulseCounter) == false);

//###################################################################################

	bitsSettingsModules0 = EEPROM.read(addrBitsSettingsModules0);
	bitsSettingsModules1 = EEPROM.read(addrBitsSettingsModules1);
	wpLDR.UseAvg(bitRead(bitsSettingsModules0, bitUseLdrAvg) == false);
	wpLight.UseAvg(bitRead(bitsSettingsModules0, bitUseLightAvg) == false);
	wpAnalogOut.handSet = bitRead(bitsSettingsModules0, bitAnalogOutHand) == false;
	wpAnalogOut2.handSet = bitRead(bitsSettingsModules1, bitAnalogOut2Hand) == false;
	wpRelais.handSet = bitRead(bitsSettingsModules0, bitRelaisHand) == false;
	wpRelais.handValueSet = bitRead(bitsSettingsModules0, bitRelaisHandValue) == false;
	wpRelais.waterEmptySet = bitRead(bitsSettingsModules0, bitRelaisWaterEmpty) == false;
	wpRain.UseAvg(bitRead(bitsSettingsModules0, bitUseRainAvg) == false);
	wpMoisture.UseAvg(bitRead(bitsSettingsModules0, bitUseMoistureAvg) == false);
	wpNeoPixel.InitRGB(bitRead(bitsSettingsModules1, bitNeoPixelRGB) == false);

//###################################################################################
/// byte values: byte 20 - 49
	wpDHT.CalcCycle(EEPROM.read(byteCalcCycleDHT) * 100);
	wpDHT.temperatureCorrection = EEPROM.read(byteTemperatureCorrection);
	wpDHT.humidityCorrection = EEPROM.read(byteHumidityCorrection);
	wpLDR.CalcCycle(EEPROM.read(byteCalcCycleLDR) * 100);
	wpLDR.correction = EEPROM.read(byteLDRCorrection);
	wpLight.CalcCycle(EEPROM.read(byteCalcCycleLight) * 100);
	wpAnalogOut.handValueSet = EEPROM.read(byteAnalogOutHandValue);
	wpAnalogOut.CalcCycle(EEPROM.read(byteCalcCycleAnalogOut));
	wpAnalogOut2.handValueSet = EEPROM.read(byteAnalogOut2HandValue);
	wpNeoPixel.InitValueR(EEPROM.read(byteNeoPixelValueR));
	wpNeoPixel.InitValueG(EEPROM.read(byteNeoPixelValueG));
	wpNeoPixel.InitValueB(EEPROM.read(byteNeoPixelValueB));
	wpNeoPixel.InitBrightness(EEPROM.read(byteNeoPixelBrightness));
	wpRelais.pumpActive = EEPROM.read(bytePumpActive);
	wpRpm.CalcCycle(EEPROM.read(byteCalcCycleRpm) * 100);
	wpRain.CalcCycle(EEPROM.read(byteCalcCycleRain) * 100);
	wpRain.correction = EEPROM.read(byteRainCorrection);
	wpMoisture.CalcCycle(EEPROM.read(byteCalcCycleMoisture) * 100);
	wpMoisture.minValue = EEPROM.read(byteMoistureMin);
	wpDistance.CalcCycle(EEPROM.read(byteCalcCycleDistance) * 100);
	wpDistance.correction = EEPROM.read(byteDistanceCorrection);
	wpDistance.height = EEPROM.read(byteHeight);
	wpImpulseCounter.CalcCycle(EEPROM.read(byteCalcCycleImpulseCounter) * 100);
	wpImpulseCounter.UpKWh = EEPROM.read(byteImpulseCounterUpKWh);

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
	wpAnalogOut.InitKp(outKp);
	short outTv;
	EEPROM.get(byteAnalogOutTv, outTv);
	wpAnalogOut.InitTv(outTv);
	short outTn;
	EEPROM.get(byteAnalogOutTn, outTn);
	wpAnalogOut.InitTn(outTn);
	short outSetPoint;
	EEPROM.get(byteAnalogOutSetPoint, outSetPoint);
	wpAnalogOut.InitSetPoint(outSetPoint);
	uint16 pixelCount;
	EEPROM.get(byteNeoPixelPixelCount, pixelCount);
	wpNeoPixel.InitPixelCount(pixelCount);
	EEPROM.get(byteImpulseCounterKWh, wpImpulseCounter.KWh);
	EEPROM.get(byteImpulseCounterSilver, wpImpulseCounter.counterSilver);
	EEPROM.get(byteImpulseCounterRed, wpImpulseCounter.counterRed);

//###################################################################################
/// byte values: 4byte 80 - 99

	readStringsFromEEPROM();
}
