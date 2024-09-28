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
//# File-ID      : $Id:: helperEEPROM.h 198 2024-09-05 12:32:25Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef helperEEPROM_h
#define helperEEPROM_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <EEPROM.h>
class helperEEPROM {
	public:
		bool Debug = false;

//###################################################################################
/// bool values: byte 0 - 19
		const uint16 addrBitsModules0 = 0;
		byte bitsModules0;
		const uint8 bitUseDHT11 = 0; // include in bitsModules0
		const uint8 bitUseDHT22 = 1; // include in bitsModules0
		const uint8 bitUseLDR = 2; // include in bitsModules0
		const uint8 bitUseLight = 3; // include in bitsModules0
		const uint8 bitUseBM = 4; // include in bitsModules0
		const uint8 bitUseRelais = 5; // include in bitsModules0
		const uint8 bitUseRelaisShield = 6; // include in bitsModules0
		const uint8 bitUseRain = 7; // include in bitsModules0

		const uint16 addrBitsModules1 = 1;
		byte bitsModules1;
		const uint8 bitUseMoisture = 0; // include in bitsModules1
		const uint8 bitUseDistance = 1; // include in bitsModules1
		const uint8 bitUseWindow = 2; // include in bitsModules1
		const uint8 bitUseAnalogOut = 3; // include in bitsModules1
		const uint8 bitUseRpm = 4; // include in bitsModules1
		const uint8 bitUseAnalogOut2 = 5; // include in bitsModules1
		const uint8 bitUseNeoPixel = 6; // include in bitsModules1
		const uint8 bitUseImpulseCounter = 7; // include in bitsModules1
		
		const uint16 addrBitsModules2 = 2;
		byte bitsModules2;
		const uint8 bitUseCwWw = 0; // include in bitsModules2
		const uint8 bitUseUnderfloor1 = 1; // include in bitsModules2
		const uint8 bitUseUnderfloor2 = 2; // include in bitsModules2
		const uint8 bitUseUnderfloor3 = 3; // include in bitsModules2
		const uint8 bitUseUnderfloor4 = 4; // include in bitsModules2

//###################################################################################

		const uint16 addrBitsSendRestBasis0 = 3;
		byte bitsSendRestBasis0;
		const uint8 bitSendRestRssi = 0; // include in bitsSendRestBasis0

		const uint16 addrBitsDebugBasis0 = 4;
		byte bitsDebugBasis0;
		const uint8 bitDebugEEPROM = 0; // include in bitsDebugBasis0
		const uint8 bitDebugFinder = 1; // include in bitsDebugBasis0
		const uint8 bitDebugModules = 2; // include in bitsDebugBasis0
		const uint8 bitDebugMqtt = 3; // include in bitsDebugBasis0
		const uint8 bitDebugOnlineToggler = 4; // include in bitsDebugBasis0
		const uint8 bitDebugRest = 5; // include in bitsDebugBasis0
		const uint8 bitDebugUpdate = 6; // include in bitsDebugBasis0
		const uint8 bitDebugWebServer = 7; // include in bitsDebugBasis0
		
		const uint16 addrBitsDebugBasis1 = 5;
		byte bitsDebugBasis1;
		const uint8 bitDebugWiFi = 0; // include in bitsDebugBasis1

//###################################################################################

		const uint16 addrBitsSendRestModules0 = 6;
		byte bitsSendRestModules0;
		const uint8 bitSendRestDHT = 0; // include in bitsSendRestModules0
		const uint8 bitSendRestLDR = 1; // include in bitsSendRestModules0
		const uint8 bitSendRestLight = 2; // include in bitsSendRestModules0
		const uint8 bitSendRestBM = 3; // include in bitsSendRestModules0
		const uint8 bitSendRestRelais = 4; // include in bitsSendRestModules0
		const uint8 bitSendRestRain = 5; // include in bitsSendRestModules0
		const uint8 bitSendRestMoisture = 6; // include in bitsSendRestModules0
		const uint8 bitSendRestDistance = 7; // include in bitsSendRestModules0
		
		const uint16 addrBitsSendRestModules1 = 7;
		byte bitsSendRestModules1;
		const uint8 bitSendRestWindow = 0; // include in bitsSendRestModules1
		const uint8 bitSendRestAnalogOut = 1; // include in bitsSendRestModules1
		const uint8 bitSendRestRpm = 2; // include in bitsSendRestModules1
		const uint8 bitSendRestAnalogOut2 = 3; // include in bitsSendRestModules1
		const uint8 bitSendRestNeoPixel = 4; // include in bitsSendRestModules1
		const uint8 bitSendRestImpulseCounter = 5; // include in bitsSendRestModules1
		
		const uint16 addrBitsSendRestModules2 = 8;
		byte bitsSendRestModules2;

//###################################################################################

		const uint16 addrBitsDebugModules0 = 9;
		byte bitsDebugModules0;
		const uint8 bitDebugDHT = 0; // include in bitsDebugModules0
		const uint8 bitDebugLDR = 1; // include in bitsDebugModules0
		const uint8 bitDebugLight = 2; // include in bitsDebugModules0
		const uint8 bitDebugBM = 3; // include in bitsDebugModules0
		const uint8 bitDebugRelais = 4; // include in bitsDebugModules0
		const uint8 bitDebugRain = 5; // include in bitsDebugModules0
		const uint8 bitDebugMoisture = 6; // include in bitsDebugModules0
		const uint8 bitDebugDistance = 7; // include in bitsDebugModules0
		
		const uint16 addrBitsDebugModules1 = 10;
		byte bitsDebugModules1;
		const uint8 bitDebugWindow = 0; // include in bitsDebugModules1
		const uint8 bitDebugAnalogOut = 1; // include in bitsDebugModules1
		const uint8 bitDebugRpm = 2; // include in bitsDebugModules1
		const uint8 bitDebugAnalogOut2 = 3; // include in bitsDebugModules1
		const uint8 bitDebugNeoPixel = 4; // include in bitsDebugModules1
		const uint8 bitDebugImpulseCounter = 5; // include in bitsDebugModules1
		const uint8 bitDebugCwWw = 6; // include in bitsDebugModules1
		
		const uint16 addrBitsDebugModules2 = 11;
		byte bitsDebugModules2;
		const uint8 bitDebugUnderfloor1 = 0; // include in bitsDebugModules2
		const uint8 bitDebugUnderfloor2 = 1; // include in bitsDebugModules2
		const uint8 bitDebugUnderfloor3 = 2; // include in bitsDebugModules2
		const uint8 bitDebugUnderfloor4 = 3; // include in bitsDebugModules2

//###################################################################################

		const uint16 addrBitsSettingsModules0 = 12;
		byte bitsSettingsModules0;
		const uint8 bitUseLdrAvg = 0; // include in bitsSettingsModules0
		const uint8 bitUseLightAvg = 1; // include in bitsSettingsModules0
		const uint8 bitRelaisHand = 2; // include in bitsSettingsModules0
		const uint8 bitRelaisHandValue = 3; // include in bitsSettingsModules0
		const uint8 bitRelaisWaterEmpty = 4; // include in bitsSettingsModules0
		const uint8 bitUseRainAvg = 5; // include in bitsSettingsModules0
		const uint8 bitUseMoistureAvg = 6; // include in bitsSettingsModules0
		const uint8 bitAnalogOutHand = 7; // include in bitsSettingsModules0

		const uint16 addrBitsSettingsModules1 = 13;
		byte bitsSettingsModules1;
		const uint8 bitUseRpmAvg = 0; // include in bitsSettingsModules1
		const uint8 bitAnalogOut2Hand = 1; // include in bitsSettingsModules1
		const uint8 bitNeoPixelRGB = 2; // include in bitsSettingsModules1

		const uint16 addrBitsSettingsModules2 = 14;
		byte bitsSettingsModules2;
		const uint8 bitUnderfloor1Hand = 0; // include in bitsSettingsModules2
		const uint8 bitUnderfloor2Hand = 1; // include in bitsSettingsModules2
		const uint8 bitUnderfloor3Hand = 2; // include in bitsSettingsModules2
		const uint8 bitUnderfloor4Hand = 3; // include in bitsSettingsModules2
		const uint8 bitUnderfloor1HandValue = 4; // include in bitsSettingsModules2
		const uint8 bitUnderfloor2HandValue = 5; // include in bitsSettingsModules2
		const uint8 bitUnderfloor3HandValue = 6; // include in bitsSettingsModules2
		const uint8 bitUnderfloor4HandValue = 7; // include in bitsSettingsModules2
		
//###################################################################################
/// byte values: byte 20 - 49
		const uint16 byteCalcCycleDHT = 20;
		const uint16 byteTemperatureCorrection = 21; // int8 * 10
		const uint16 byteHumidityCorrection = 22; // int8 * 10
		const uint16 byteCalcCycleLDR = 23;
		const uint16 byteLDRCorrection = 24; // int8
		const uint16 byteCalcCycleLight = 25;
		const uint16 bytePumpActive = 26;
		const uint16 byteCalcCycleRain = 27;
		const uint16 byteRainCorrection = 28; // int8
		const uint16 byteCalcCycleMoisture = 29;
		const uint16 byteMoistureMin = 30;
		const uint16 byteCalcCycleDistance = 31;
		const uint16 byteDistanceCorrection = 32; // int8
		const uint16 byteHeight = 33;
		const uint16 byteAnalogOutHandValue = 35;
		const uint16 byteCalcCycleRpm = 36;
		const uint16 byteRpmCorrection = 37; // int8
		const uint16 byteAnalogOut2HandValue = 39;
		const uint16 byteNeoPixelValueR = 41;
		const uint16 byteNeoPixelValueG = 42;
		const uint16 byteNeoPixelValueB = 43;
		const uint16 byteNeoPixelBrightness = 44;
		const uint16 byteCalcCycleAnalogOut = 45;
		const uint16 byteCalcCycleImpulseCounter = 46;
		const uint16 byteImpulseCounterUpKWh = 47;

//###################################################################################
/// byte values: 2byte 50 - 79
		const uint16 byteLightCorrection = 50; // int
		const uint16 byteBMThreshold = 52;
		const uint16 bytePumpPause = 54;
		const uint16 byteMoistureDry = 56;
		const uint16 byteMoistureWet = 58;
		const uint16 byteMaxVolume = 60;
		const uint16 byteWindowThreshold = 62;
		const uint16 byteAnalogOutKp = 64;
		const uint16 byteAnalogOutTn = 66;
		const uint16 byteAnalogOutTv = 68;
		const uint16 byteAnalogOutSetPoint = 70;
		const uint16 byteNeoPixelPixelCount = 72;
		const uint16 byteImpulseCounterSilver = 74;
		const uint16 byteImpulseCounterRed = 76;

//###################################################################################
/// byte values: 4byte 80 - 99

		const uint16 byteImpulseCounterKWh = 80;

		uint16 byteStartForString;

		String mqttTopicDebug;

		helperEEPROM();
		void init();
		void cycle();
		uint16 getVersion();
		void changeDebug();
		void readStringsFromEEPROM();
		void writeStringsToEEPROM();
		void saveBool(uint16 &addr, byte &by, uint8 &bi, bool v);

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 198 $";
		bool DebugLast;
		unsigned long publishDebugLast;
		const uint16 addrStartForString0 = 500;
		String readStringFromEEPROM(int addrOffset, String defaultString);
		int writeStringToEEPROM(int addrOffset, String &strToWrite);
		void readVars();
};
extern helperEEPROM wpEEPROM;
#endif
