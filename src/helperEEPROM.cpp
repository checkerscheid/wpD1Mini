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

helperEEPROM::helperEEPROM() {
	EEPROM.begin(4095);
	if(false) {
		EEPROM.write(addrBitsModules0, 0);
		EEPROM.write(addrBitsModules1, 0);
		EEPROM.write(addrBitsModules2, 0);
		EEPROM.write(addrBitsSendRestModules0, 0);
		EEPROM.write(addrBitsSendRestModules1, 0);
		EEPROM.write(addrBitsSendRestModules2, 0);
		EEPROM.commit();
	}
}
void helperEEPROM::init() {
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

	byteStartForString = byteStartForString + 1 + wpUnderfloor1.mqttTopicTemp.length();
	wpUnderfloor1.mqttTopicTemp = readStringFromEEPROM(byteStartForString, wpUnderfloor1.mqttTopicTemp);
	byteStartForString = byteStartForString + 1 + wpUnderfloor2.mqttTopicTemp.length();
	wpUnderfloor2.mqttTopicTemp = readStringFromEEPROM(byteStartForString, wpUnderfloor2.mqttTopicTemp);
	byteStartForString = byteStartForString + 1 + wpUnderfloor3.mqttTopicTemp.length();
	wpUnderfloor3.mqttTopicTemp = readStringFromEEPROM(byteStartForString, wpUnderfloor3.mqttTopicTemp);
	byteStartForString = byteStartForString + 1 + wpUnderfloor4.mqttTopicTemp.length();
	wpUnderfloor4.mqttTopicTemp = readStringFromEEPROM(byteStartForString, wpUnderfloor4.mqttTopicTemp);
}

void helperEEPROM::writeStringsToEEPROM() {
	byteStartForString = addrStartForString0; // reset
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceDescription);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpBM.lightToTurnOn);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpWindow.lightToTurnOn);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpUnderfloor1.mqttTopicTemp);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpUnderfloor2.mqttTopicTemp);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpUnderfloor3.mqttTopicTemp);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpUnderfloor4.mqttTopicTemp);
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
	bitsModules2 = EEPROM.read(addrBitsModules2);

	wpModules.useModuleDHT11 = bitRead(bitsModules0, bitUseDHT11);
	wpModules.useModuleDHT22 = bitRead(bitsModules0, bitUseDHT22);
	wpModules.useModuleLDR = bitRead(bitsModules0, bitUseLDR);
	wpModules.useModuleLight = bitRead(bitsModules0, bitUseLight);
	wpModules.useModuleBM = bitRead(bitsModules0, bitUseBM);
	wpModules.useModuleWindow = bitRead(bitsModules1, bitUseWindow);
	wpModules.useModuleCwWw = bitRead(bitsModules2, bitUseCwWw);
	wpModules.useModuleAnalogOut = bitRead(bitsModules1, bitUseAnalogOut);
	wpModules.useModuleAnalogOut2 = bitRead(bitsModules1, bitUseAnalogOut2);
	wpModules.useModuleNeoPixel = bitRead(bitsModules1, bitUseNeoPixel);
	wpModules.useModuleRelais = bitRead(bitsModules0, bitUseRelais);
	wpModules.useModuleRelaisShield = bitRead(bitsModules0, bitUseRelaisShield);
	wpModules.useModuleRpm = bitRead(bitsModules1, bitUseRpm);
	wpModules.useModuleRain = bitRead(bitsModules0, bitUseRain);
	wpModules.useModuleMoisture = bitRead(bitsModules1, bitUseMoisture);
	wpModules.useModuleDistance = bitRead(bitsModules1, bitUseDistance);
	wpModules.useModuleImpulseCounter = bitRead(bitsModules1, bitUseImpulseCounter);
	wpModules.useModuleUnderfloor1 = bitRead(bitsModules2, bitUseUnderfloor1);
	wpModules.useModuleUnderfloor2 = bitRead(bitsModules2, bitUseUnderfloor2);
	wpModules.useModuleUnderfloor3 = bitRead(bitsModules2, bitUseUnderfloor3);
	wpModules.useModuleUnderfloor4 = bitRead(bitsModules2, bitUseUnderfloor4);

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
	wpImpulseCounter.SendRest(bitRead(bitsSendRestModules1, bitSendRestImpulseCounter));

//###################################################################################

	bitsDebugModules0 = EEPROM.read(addrBitsDebugModules0);
	bitsDebugModules1 = EEPROM.read(addrBitsDebugModules1);
	bitsDebugModules2 = EEPROM.read(addrBitsDebugModules2);
	wpDHT.Debug(bitRead(bitsDebugModules0, bitDebugDHT));
	wpLDR.Debug(bitRead(bitsDebugModules0, bitDebugLDR));
	wpLight.Debug(bitRead(bitsDebugModules0, bitDebugLight));
	wpBM.Debug(bitRead(bitsDebugModules0, bitDebugBM));
	wpWindow.Debug(bitRead(bitsDebugModules1, bitDebugWindow));
	wpCwWw.Debug(bitRead(bitsDebugModules1, bitDebugCwWw));
	wpAnalogOut.Debug(bitRead(bitsDebugModules1, bitDebugAnalogOut));
	wpAnalogOut2.Debug(bitRead(bitsDebugModules1, bitDebugAnalogOut2));
	wpNeoPixel.Debug(bitRead(bitsDebugModules1, bitDebugNeoPixel));
	wpRelais.Debug(bitRead(bitsDebugModules0, bitDebugRelais));
	wpRpm.Debug(bitRead(bitsDebugModules1, bitDebugRpm));
	wpRain.Debug(bitRead(bitsDebugModules0, bitDebugRain));
	wpMoisture.Debug(bitRead(bitsDebugModules0, bitDebugMoisture));
	wpDistance.Debug(bitRead(bitsDebugModules0, bitDebugDistance));
	wpImpulseCounter.Debug(bitRead(bitsDebugModules1, bitDebugImpulseCounter));
	wpUnderfloor1.Debug(bitRead(bitsDebugModules2, bitDebugUnderfloor1));
	wpUnderfloor2.Debug(bitRead(bitsDebugModules2, bitDebugUnderfloor2));
	wpUnderfloor3.Debug(bitRead(bitsDebugModules2, bitDebugUnderfloor3));
	wpUnderfloor4.Debug(bitRead(bitsDebugModules2, bitDebugUnderfloor4));

//###################################################################################

	bitsSettingsModules0 = EEPROM.read(addrBitsSettingsModules0);
	bitsSettingsModules1 = EEPROM.read(addrBitsSettingsModules1);
	bitsSettingsModules2 = EEPROM.read(addrBitsSettingsModules2);
	wpLDR.UseAvg(bitRead(bitsSettingsModules0, bitUseLdrAvg));
	wpLight.UseAvg(bitRead(bitsSettingsModules0, bitUseLightAvg));
	wpAnalogOut.handSet = bitRead(bitsSettingsModules0, bitAnalogOutHand);
	wpAnalogOut2.handSet = bitRead(bitsSettingsModules1, bitAnalogOut2Hand);
	wpRelais.handSet = bitRead(bitsSettingsModules0, bitRelaisHand);
	wpRelais.handValueSet = bitRead(bitsSettingsModules0, bitRelaisHandValue);
	wpRelais.waterEmptySet = bitRead(bitsSettingsModules0, bitRelaisWaterEmpty);
	wpRain.UseAvg(bitRead(bitsSettingsModules0, bitUseRainAvg));
	wpMoisture.UseAvg(bitRead(bitsSettingsModules0, bitUseMoistureAvg));
	wpNeoPixel.InitRGB(bitRead(bitsSettingsModules1, bitNeoPixelRGB));
	wpUnderfloor1.handSet = bitRead(bitsSettingsModules2, bitUnderfloor1Hand);
	wpUnderfloor2.handSet = bitRead(bitsSettingsModules2, bitUnderfloor2Hand);
	wpUnderfloor3.handSet = bitRead(bitsSettingsModules2, bitUnderfloor3Hand);
	wpUnderfloor4.handSet = bitRead(bitsSettingsModules2, bitUnderfloor4Hand);
	wpUnderfloor1.handValueSet = bitRead(bitsSettingsModules2, bitUnderfloor1HandValue);
	wpUnderfloor2.handValueSet = bitRead(bitsSettingsModules2, bitUnderfloor2HandValue);
	wpUnderfloor3.handValueSet = bitRead(bitsSettingsModules2, bitUnderfloor3HandValue);
	wpUnderfloor4.handValueSet = bitRead(bitsSettingsModules2, bitUnderfloor4HandValue);


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
