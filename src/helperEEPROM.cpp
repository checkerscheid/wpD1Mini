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
//# Revision     : $Rev:: 262                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.cpp 262 2025-04-30 12:00:50Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperEEPROM.h>

helperEEPROM wpEEPROM;

helperEEPROM::helperEEPROM() {
	EEPROM.begin(4095);
	#if BUILDWITH == 99
		EEPROM.write(addrBitsModules0, 0);
		EEPROM.write(addrBitsModules1, 0);
		EEPROM.write(addrBitsModules2, 0);
		EEPROM.write(addrBitsModules3, 0);
		EEPROM.write(byteDS18B20Count, 0);	
		uint16 pixelCountReset = 0;
		EEPROM.put(byteNeoPixelPixelCount, pixelCountReset);
		readStringsFromEEPROM();
		wpBM.lightToTurnOn = "_";
		wpUnderfloor1.mqttTopicTemp = "_";
		wpUnderfloor2.mqttTopicTemp = "_";
		wpUnderfloor3.mqttTopicTemp = "_";
		wpUnderfloor4.mqttTopicTemp = "_";
		wpAnalogOut.mqttTopicTemp = "_";
		writeStringsToEEPROM();
		EEPROM.commit();
	#endif
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
	String SVN = "$Rev: 262 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperEEPROM::changeDebug() {
	Debug = !Debug;
	WriteBoolToEEPROM("DebugEEPROM", addrBitsDebugBasis0, bitsDebugBasis0, bitDebugEEPROM, Debug);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "writeEEPROM", "DebugEEPROM: " + String(Debug));
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
	wpUnderfloor1.mqttTopicTemp = readStringFromEEPROM(byteStartForString, wpUnderfloor1.mqttTopicTemp);
	byteStartForString = byteStartForString + 1 + wpUnderfloor1.mqttTopicTemp.length();
	wpUnderfloor2.mqttTopicTemp = readStringFromEEPROM(byteStartForString, wpUnderfloor2.mqttTopicTemp);
	byteStartForString = byteStartForString + 1 + wpUnderfloor2.mqttTopicTemp.length();
	wpUnderfloor3.mqttTopicTemp = readStringFromEEPROM(byteStartForString, wpUnderfloor3.mqttTopicTemp);
	byteStartForString = byteStartForString + 1 + wpUnderfloor3.mqttTopicTemp.length();
	wpUnderfloor4.mqttTopicTemp = readStringFromEEPROM(byteStartForString, wpUnderfloor4.mqttTopicTemp);
	byteStartForString = byteStartForString + 1 + wpAnalogOut.mqttTopicTemp.length();
	wpAnalogOut.mqttTopicTemp = readStringFromEEPROM(byteStartForString, wpAnalogOut.mqttTopicTemp);
}

void helperEEPROM::writeStringsToEEPROM() {
	byteStartForString = addrStartForString0; // reset
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpFZ.DeviceDescription);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpBM.lightToTurnOn);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpUnderfloor1.mqttTopicTemp);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpUnderfloor2.mqttTopicTemp);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpUnderfloor3.mqttTopicTemp);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpUnderfloor4.mqttTopicTemp);
	byteStartForString = writeStringToEEPROM(byteStartForString, wpAnalogOut.mqttTopicTemp);
}

void helperEEPROM::WriteBoolToEEPROM(String name, const uint16 &addr, byte &by, const uint8 &bi, bool &v, bool commit) {
	bitWrite(by, bi, v);
	EEPROM.write(addr, by);
	if(commit) EEPROM.commit();
	String logmessage = name + ": addr: " + String(addr) + ", bit: " + String(bi) + ", state: " + String(v);
	wpFZ.DebugWS(wpFZ.strINFO, F("SaveBoolToEEPROM"), logmessage);
}
void helperEEPROM::WriteByteToEEPROM(String name, const uint16 &addr, uint8 &v, bool commit) {
	EEPROM.write(addr, v);
	if(commit) EEPROM.commit();
	String logmessage = name + ": addr: " + String(addr) + ", value: " + String(v);
	wpFZ.DebugWS(wpFZ.strINFO, F("WriteByteToEEPROM"), logmessage);
}
void helperEEPROM::WriteByteToEEPROM(String name, const uint16 &addr, int8 &v, bool commit) {
	EEPROM.write(addr, v);
	if(commit) EEPROM.commit();
	String logmessage = name + ": addr: " + String(addr) + ", value: " + String(v);
	wpFZ.DebugWS(wpFZ.strINFO, F("WriteByteToEEPROM"), logmessage);
}
void helperEEPROM::WriteWordToEEPROM(String name, const uint16 &addr, uint16 &v, bool commit) {
	EEPROM.put(addr, v);
	if(commit) EEPROM.commit();
	String logmessage = name + ": addr: " + String(addr) + ", value: " + String(v);
	wpFZ.DebugWS(wpFZ.strINFO, F("WriteWordToEEPROM"), logmessage);
}
void helperEEPROM::WriteWordToEEPROM(String name, const uint16 &addr, short &v, bool commit) {
	EEPROM.put(addr, v);
	if(commit) EEPROM.commit();
	String logmessage = name + ": addr: " + String(addr) + ", value: " + String(v);
	wpFZ.DebugWS(wpFZ.strINFO, F("WriteWordToEEPROM"), logmessage);
}
void helperEEPROM::WriteWordToEEPROM(String name, const uint16 &addr, uint32 &v, bool commit) {
	EEPROM.put(addr, v);
	if(commit) EEPROM.commit();
	String logmessage = name + ": addr: " + String(addr) + ", value: " + String(v);
	wpFZ.DebugWS(wpFZ.strINFO, F("WriteWordToEEPROM"), logmessage);
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
	bitsModules3 = EEPROM.read(addrBitsModules3);
	wpModules.useModuleDHT11 = bitRead(bitsModules0, bitUseDHT11);
	wpModules.useModuleDHT22 = bitRead(bitsModules0, bitUseDHT22);
	wpModules.useModuleLDR = bitRead(bitsModules0, bitUseLDR);
	wpModules.useModuleLight = bitRead(bitsModules0, bitUseLight);
	wpModules.useModuleBM = bitRead(bitsModules0, bitUseBM);
	wpModules.useModuleWindow = bitRead(bitsModules1, bitUseWindow);
	wpModules.useModuleRelais = bitRead(bitsModules0, bitUseRelais);
	wpModules.useModuleRelaisShield = bitRead(bitsModules0, bitUseRelaisShield);
	wpModules.useModuleRain = bitRead(bitsModules0, bitUseRain);
	wpModules.useModuleMoisture = bitRead(bitsModules1, bitUseMoisture);
	wpModules.useModuleDistance = bitRead(bitsModules1, bitUseDistance);

	wpModules.useModuleNeoPixel = false;
	wpModules.useModuleCwWw = false;
	wpModules.useModuleClock = false;
	wpModules.useModuleAnalogOut = false;
	wpModules.useModuleAnalogOut2 = false;
	wpModules.useModuleRpm = false;
	wpModules.useModuleImpulseCounter = false;
	wpModules.useModuleWindow2 = false;
	wpModules.useModuleWindow3 = false;
	wpModules.useModuleWeight = false;
	wpModules.useModuleUnderfloor1 = false;
	wpModules.useModuleUnderfloor2 = false;
	wpModules.useModuleUnderfloor3 = false;
	wpModules.useModuleUnderfloor4 = false;
#if BUILDWITH == 1
	wpModules.useModuleNeoPixel = bitRead(bitsModules1, bitUseNeoPixel);
	wpModules.useModuleCwWw = bitRead(bitsModules2, bitUseCwWw);
	wpModules.useModuleClock = bitRead(bitsModules3, bitUseClock);
#endif
#if BUILDWITH == 2
	wpModules.useModuleAnalogOut = bitRead(bitsModules1, bitUseAnalogOut);
	wpModules.useModuleAnalogOut2 = bitRead(bitsModules1, bitUseAnalogOut2);
	wpModules.useModuleRpm = bitRead(bitsModules1, bitUseRpm);
	wpModules.useModuleImpulseCounter = bitRead(bitsModules1, bitUseImpulseCounter);
	wpModules.useModuleWindow2 = bitRead(bitsModules2, bitUseWindow2);
	wpModules.useModuleWindow3 = bitRead(bitsModules2, bitUseWindow3);
	wpModules.useModuleWeight = bitRead(bitsModules2, bitUseWeight);
#endif
#if BUILDWITH == 3
	wpModules.useModuleUnderfloor1 = bitRead(bitsModules2, bitUseUnderfloor1);
	wpModules.useModuleUnderfloor2 = bitRead(bitsModules2, bitUseUnderfloor2);
	wpModules.useModuleUnderfloor3 = bitRead(bitsModules2, bitUseUnderfloor3);
	wpModules.useModuleUnderfloor4 = bitRead(bitsModules2, bitUseUnderfloor4);
	wpModules.useModuleDS18B20 = bitRead(bitsModules3, bitUseDS18B20);
#endif
#if BUILDWITH == 4
	wpModules.useModuleRFID = bitRead(bitsModules3, bitUseRFID);
#endif

//###################################################################################

	bitsDebugBasis0 = EEPROM.read(addrBitsDebugBasis0);
	bitsDebugBasis1 = EEPROM.read(addrBitsDebugBasis1);
	Debug = bitRead(bitsDebugBasis0, bitDebugEEPROM);
	wpFinder.Debug = bitRead(bitsDebugBasis0, bitDebugFinder);
	wpModules.Debug = bitRead(bitsDebugBasis0, bitDebugModules);
	wpMqtt.Debug = bitRead(bitsDebugBasis0, bitDebugMqtt);
	wpOnlineToggler.Debug = bitRead(bitsDebugBasis0, bitDebugOnlineToggler);
	wpUpdate.Debug = bitRead(bitsDebugBasis0, bitDebugUpdate);
	wpWebServer.Debug = bitRead(bitsDebugBasis0, bitDebugWebServer);
	wpWiFi.Debug = bitRead(bitsDebugBasis1, bitDebugWiFi);

	bitsSettingsBasis0 = EEPROM.read(addrBitsSettingsBasis0);
	wpFZ.InitMaxWorking(bitRead(bitsSettingsBasis0, bitUseMaxWorking));

//###################################################################################

	bitsDebugModules0 = EEPROM.read(addrBitsDebugModules0);
	bitsDebugModules1 = EEPROM.read(addrBitsDebugModules1);
	bitsDebugModules2 = EEPROM.read(addrBitsDebugModules2);
	bitsDebugModules3 = EEPROM.read(addrBitsDebugModules3);
	wpDHT.Debug(bitRead(bitsDebugModules0, bitDebugDHT));
	wpLDR.Debug(bitRead(bitsDebugModules0, bitDebugLDR));
	wpLight.Debug(bitRead(bitsDebugModules0, bitDebugLight));
	wpBM.Debug(bitRead(bitsDebugModules0, bitDebugBM));
	wpWindow1.Debug(bitRead(bitsDebugModules1, bitDebugWindow));
	wpRelais.Debug(bitRead(bitsDebugModules0, bitDebugRelais));
	wpRain.Debug(bitRead(bitsDebugModules0, bitDebugRain));
	wpMoisture.Debug(bitRead(bitsDebugModules0, bitDebugMoisture));
	wpDistance.Debug(bitRead(bitsDebugModules0, bitDebugDistance));
#if BUILDWITH == 1
	wpCwWw.Debug(bitRead(bitsDebugModules1, bitDebugCwWw));
	wpNeoPixel.Debug(bitRead(bitsDebugModules1, bitDebugNeoPixel));
	wpClock.Debug(bitRead(bitsDebugModules2, bitDebugClock));
#endif
#if BUILDWITH == 2
	wpAnalogOut.Debug(bitRead(bitsDebugModules1, bitDebugAnalogOut));
	wpAnalogOut2.Debug(bitRead(bitsDebugModules1, bitDebugAnalogOut2));
	wpRpm.Debug(bitRead(bitsDebugModules1, bitDebugRpm));
	wpImpulseCounter.Debug(bitRead(bitsDebugModules1, bitDebugImpulseCounter));
	wpWindow2.Debug(bitRead(bitsDebugModules2, bitDebugWindow2));
	wpWindow3.Debug(bitRead(bitsDebugModules2, bitDebugWindow3));
	wpWeight.Debug(bitRead(bitsDebugModules2, bitDebugWeight));
#endif
#if BUILDWITH == 3
	wpUnderfloor1.Debug(bitRead(bitsDebugModules2, bitDebugUnderfloor1));
	wpUnderfloor2.Debug(bitRead(bitsDebugModules2, bitDebugUnderfloor2));
	wpUnderfloor3.Debug(bitRead(bitsDebugModules2, bitDebugUnderfloor3));
	wpUnderfloor4.Debug(bitRead(bitsDebugModules2, bitDebugUnderfloor4));
	wpDS18B20.Debug(bitRead(bitsDebugModules3, bitDebugDS18B20));
#endif
#if BUILDWITH == 4
	wpRFID.Debug(bitRead(bitsDebugModules2, bitDebugRFID));
#endif

//###################################################################################

	bitsSettingsModules0 = EEPROM.read(addrBitsSettingsModules0);
	bitsSettingsModules1 = EEPROM.read(addrBitsSettingsModules1);
	bitsSettingsModules2 = EEPROM.read(addrBitsSettingsModules2);
	wpLDR.UseAvg(bitRead(bitsSettingsModules0, bitUseLdrAvg));
	wpLight.UseAvg(bitRead(bitsSettingsModules0, bitUseLightAvg));
	wpRelais.handSet = bitRead(bitsSettingsModules0, bitRelaisHand);
	wpRelais.handValueSet = bitRead(bitsSettingsModules0, bitRelaisHandValue);
	wpRelais.waterEmptySet = bitRead(bitsSettingsModules0, bitRelaisWaterEmpty);
	wpRain.UseAvg(bitRead(bitsSettingsModules0, bitUseRainAvg));
	wpMoisture.UseAvg(bitRead(bitsSettingsModules0, bitUseMoistureAvg));
#if BUILDWITH == 1
	wpNeoPixel.InitRGB(bitRead(bitsSettingsModules1, bitNeoPixelRGB));
	wpClock.InitRGB(bitRead(bitsSettingsModules1, bitNeoPixelRGB));
	wpNeoPixel.SetUseWW(bitRead(bitsSettingsModules1, bitNeoPixelUseWW));
	wpNeoPixel.SetUseCW(bitRead(bitsSettingsModules1, bitNeoPixelUseCW));
#endif
#if BUILDWITH == 2
	wpAnalogOut.InitHand(bitRead(bitsSettingsModules0, bitAnalogOutHand));
	wpAnalogOut2.InitHand(bitRead(bitsSettingsModules1, bitAnalogOut2Hand));
	wpWeight.UseAvg(bitRead(bitsSettingsModules1, bitUseWeightAvg));
#endif
#if BUILDWITH == 3
	wpUnderfloor1.handSet = bitRead(bitsSettingsModules2, bitUnderfloor1Hand);
	wpUnderfloor2.handSet = bitRead(bitsSettingsModules2, bitUnderfloor2Hand);
	wpUnderfloor3.handSet = bitRead(bitsSettingsModules2, bitUnderfloor3Hand);
	wpUnderfloor4.handSet = bitRead(bitsSettingsModules2, bitUnderfloor4Hand);
	wpUnderfloor1.handValueSet = bitRead(bitsSettingsModules2, bitUnderfloor1HandValue);
	wpUnderfloor2.handValueSet = bitRead(bitsSettingsModules2, bitUnderfloor2HandValue);
	wpUnderfloor3.handValueSet = bitRead(bitsSettingsModules2, bitUnderfloor3HandValue);
	wpUnderfloor4.handValueSet = bitRead(bitsSettingsModules2, bitUnderfloor4HandValue);
#endif
#if BUILDWITH == 4
#endif

//###################################################################################
/// byte values: byte 20 - 49
	wpDHT.CalcCycle(EEPROM.read(byteCalcCycleDHT) * 100);
	wpDHT.temperatureCorrection = EEPROM.read(byteTemperatureCorrection);
	wpDHT.humidityCorrection = EEPROM.read(byteHumidityCorrection);
	wpLDR.CalcCycle(EEPROM.read(byteCalcCycleLDR) * 100);
	wpLDR.correction = EEPROM.read(byteLDRCorrection);
	wpLight.CalcCycle(EEPROM.read(byteCalcCycleLight) * 100);
	wpRelais.pumpActive = EEPROM.read(bytePumpActive);
	wpRain.CalcCycle(EEPROM.read(byteCalcCycleRain) * 100);
	wpRain.correction = EEPROM.read(byteRainCorrection);
	wpMoisture.CalcCycle(EEPROM.read(byteCalcCycleMoisture) * 100);
	wpMoisture.minValue = EEPROM.read(byteMoistureMin);
	wpDistance.CalcCycle(EEPROM.read(byteCalcCycleDistance) * 100);
	wpDistance.correction = EEPROM.read(byteDistanceCorrection);
	wpDistance.height = EEPROM.read(byteHeight);
#if BUILDWITH == 1
	wpNeoPixel.InitValueR(EEPROM.read(byteNeoPixelValueR));
	wpNeoPixel.InitValueG(EEPROM.read(byteNeoPixelValueG));
	wpNeoPixel.InitValueB(EEPROM.read(byteNeoPixelValueB));
	wpClock.CalcCycle(EEPROM.read(byteCalcCycleClock) * 100);
	
	wpClock.ColorHR = EEPROM.read(byteClockColorHR);
	wpClock.ColorHG = EEPROM.read(byteClockColorHG);
	wpClock.ColorHB = EEPROM.read(byteClockColorHB);
	wpClock.ColorMR = EEPROM.read(byteClockColorMR);
	wpClock.ColorMG = EEPROM.read(byteClockColorMG);
	wpClock.ColorMB = EEPROM.read(byteClockColorMB);
	wpClock.ColorSR = EEPROM.read(byteClockColorSR);
	wpClock.ColorSG = EEPROM.read(byteClockColorSG);
	wpClock.ColorSB = EEPROM.read(byteClockColorSB); 
	wpClock.ColorQR = EEPROM.read(byteClockColorQR);
	wpClock.ColorQG = EEPROM.read(byteClockColorQG);
	wpClock.ColorQB = EEPROM.read(byteClockColorQB);
	wpClock.Color5R = EEPROM.read(byteClockColor5R);
	wpClock.Color5G = EEPROM.read(byteClockColor5G);
	wpClock.Color5B = EEPROM.read(byteClockColor5B);
#endif
#if BUILDWITH == 2
	wpAnalogOut.InitHandValue(EEPROM.read(byteAnalogOutHandValue));
	wpAnalogOut.CalcCycle(EEPROM.read(byteCalcCycleAnalogOut) * 100);
	wpAnalogOut.InitPidType(EEPROM.read(byteAnalogOutPidType));
	wpAnalogOut2.InitHand(bitRead(bitsSettingsModules1, bitAnalogOut2Hand));
	wpAnalogOut2.InitHandValue(EEPROM.read(byteAnalogOut2HandValue));
	wpRpm.CalcCycle(EEPROM.read(byteCalcCycleRpm) * 100);
	wpImpulseCounter.CalcCycle(EEPROM.read(byteCalcCycleImpulseCounter) * 100);
	wpImpulseCounter.UpKWh = EEPROM.read(byteImpulseCounterUpKWh);
	wpWeight.CalcCycle(EEPROM.read(byteCalcCycleWeight) * 100);
#endif
#if BUILDWITH == 3
	wpUnderfloor1.InitSetPoint(EEPROM.read(byteUnderfloor1Setpoint));
	wpUnderfloor2.InitSetPoint(EEPROM.read(byteUnderfloor2Setpoint));
	wpUnderfloor3.InitSetPoint(EEPROM.read(byteUnderfloor3Setpoint));
	wpUnderfloor4.InitSetPoint(EEPROM.read(byteUnderfloor4Setpoint));
	wpUnderfloor1.CalcCycle(EEPROM.read(byteCalcCycleUnderfloor1) * 100);
	wpUnderfloor2.CalcCycle(EEPROM.read(byteCalcCycleUnderfloor2) * 100);
	wpUnderfloor3.CalcCycle(EEPROM.read(byteCalcCycleUnderfloor3) * 100);
	wpUnderfloor4.CalcCycle(EEPROM.read(byteCalcCycleUnderfloor4) * 100);
	wpDS18B20.CalcCycle(EEPROM.read(byteCalcCycleDS18B20) * 100);
	wpDS18B20.count = EEPROM.read(byteDS18B20Count);
	for(int i = 0; i < wpDS18B20.count; i++) {
		if(wpDS18B20.devices[i] != NULL)
			wpDS18B20.devices[i]->correction = EEPROM.read(byteDS18B20Correction[i]);
	}
#endif
#if BUILDWITH == 4
	wpRFID.CalcCycle(EEPROM.read(byteCalcCycleRFID) * 100);
#endif

//###################################################################################
/// byte values: 2byte 50 - 79
	EEPROM.get(byteLightCorrection, wpLight.correction); // int
	EEPROM.get(byteBMThreshold, wpBM.threshold);
	EEPROM.get(bytePumpPause, wpRelais.pumpPause);
	EEPROM.get(byteMoistureDry, wpMoisture.dry);
	EEPROM.get(byteMoistureWet, wpMoisture.wet);
	EEPROM.get(byteMaxVolume, wpDistance.volume);
#if BUILDWITH == 1
	uint16 pixelCount;
	EEPROM.get(byteNeoPixelPixelCount, pixelCount);
	wpNeoPixel.InitPixelCount(pixelCount);
	wpClock.InitPixelCount(pixelCount);
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
#endif
#if BUILDWITH == 2
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
	EEPROM.get(byteImpulseCounterKWh, wpImpulseCounter.KWh);
	EEPROM.get(byteImpulseCounterSilver, wpImpulseCounter.counterSilver);
	EEPROM.get(byteImpulseCounterRed, wpImpulseCounter.counterRed);
#endif
#if BUILDWITH == 3
	// wird lokal gemacht
	// for(int i = 0; i < wpDS18B20.count; i++) {
	// 	uint8 b0 = EEPROM.read(byteDS18B20adr[i][0]);
	// 	uint8 b1 = EEPROM.read(byteDS18B20adr[i][1]);
	// 	uint8 b2 = EEPROM.read(byteDS18B20adr[i][2]);
	// 	uint8 b3 = EEPROM.read(byteDS18B20adr[i][3]);
	// 	uint8 b4 = EEPROM.read(byteDS18B20adr[i][4]);
	// 	uint8 b5 = EEPROM.read(byteDS18B20adr[i][5]);
	// 	uint8 b6 = EEPROM.read(byteDS18B20adr[i][6]);
	// 	uint8 b7 = EEPROM.read(byteDS18B20adr[i][7]);
	// 	if(wpDS18B20.devices[i] != NULL)
	// 		wpDS18B20.devices[i]->initAddress(b0, b1, b2, b3, b4, b5, b6, b7);
	// }
#endif
#if BUILDWITH == 4
#endif

//###################################################################################
/// byte values: 4byte 80 - 99
	uint32 tareValueRead = 0;
	EEPROM.get(byteWeightTareValue, tareValueRead);
	wpWeight.InitTareValue(tareValueRead);

	uint32 tare1kgRead = 0;
	EEPROM.get(byteWeightTare1kg, tare1kgRead);
	wpWeight.InitTare1kg(tare1kgRead);

	wpFZ.InitLastRestartReason(EEPROM.read(addrRestartReason));

	uint32 bc = 0;
	EEPROM.get(addrBootCounter, bc);
	wpFZ.InitBootCounter(bc);
	readStringsFromEEPROM();
}
