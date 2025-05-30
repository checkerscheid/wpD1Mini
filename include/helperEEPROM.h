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
//# Revision     : $Rev:: 264                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.h 264 2025-05-01 17:27:44Z                  $ #
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
		const uint8 bitUseWindow2 = 5; // include in bitsModules2
		const uint8 bitUseWindow3 = 6; // include in bitsModules2
		const uint8 bitUseWeight = 7; // include in bitsModules2

		const uint16 addrBitsModules3 = 3;
		byte bitsModules3;
		const uint8 bitUseRFID = 0; // include in bitsModules3
		const uint8 bitUseClock = 1; // include in bitsModules3
		const uint8 bitUseDS18B20 = 2; // include in bitsModules3

//###################################################################################

		const uint16 addrBitsDebugBasis0 = 4;
		byte bitsDebugBasis0;
		const uint8 bitDebugEEPROM = 0; // include in bitsDebugBasis0
		const uint8 bitDebugFinder = 1; // include in bitsDebugBasis0
		const uint8 bitDebugModules = 2; // include in bitsDebugBasis0
		const uint8 bitDebugMqtt = 3; // include in bitsDebugBasis0
		const uint8 bitDebugOnlineToggler = 4; // include in bitsDebugBasis0
		const uint8 bitDebugUpdate = 6; // include in bitsDebugBasis0
		const uint8 bitDebugWebServer = 7; // include in bitsDebugBasis0
		
		const uint16 addrBitsDebugBasis1 = 5;
		byte bitsDebugBasis1;
		const uint8 bitDebugWiFi = 0; // include in bitsDebugBasis1

//###################################################################################

		const uint16 addrBitsSettingsBasis0 = 6;
		byte bitsSettingsBasis0;
		const uint8 bitUseMaxWorking = 0; // include in bitsSettingsBasis0

		// const uint16 addrBitsDebugModules0 = 8; used in BitsDebugModules3

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
		const uint8 bitDebugRFID = 7; // include in bitsDebugModules1
		
		const uint16 addrBitsDebugModules2 = 11;
		byte bitsDebugModules2;
		const uint8 bitDebugUnderfloor1 = 0; // include in bitsDebugModules2
		const uint8 bitDebugUnderfloor2 = 1; // include in bitsDebugModules2
		const uint8 bitDebugUnderfloor3 = 2; // include in bitsDebugModules2
		const uint8 bitDebugUnderfloor4 = 3; // include in bitsDebugModules2
		const uint8 bitDebugWindow2 = 4; // include in bitsDebugModules2
		const uint8 bitDebugWindow3 = 5; // include in bitsDebugModules2
		const uint8 bitDebugWeight = 6; // include in bitsDebugModules2
		const uint8 bitDebugClock = 7; // include in bitsDebugModules2

		const uint16 addrBitsDebugModules3 = 8;
		byte bitsDebugModules3;
		const uint8 bitDebugDS18B20 = 0; // include in bitsDebugModules3

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
		const uint8 bitUseWeightAvg = 4; // include in bitsSettingsModules1
		const uint8 bitNeoPixelUseWW = 5; // include in bitsSettingsModules1
		const uint8 bitNeoPixelUseCW = 6; // include in bitsSettingsModules1
		const uint8 bitAnalogOutSummer = 7; // include in bitsSettingsModules1

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

		const uint16 addrBitsSettingsModules3 = 15;
		byte bitsSettingsModules3;
		const uint8 bitUnderfloor1Summer = 0; // include in bitsSettingsModules3
		const uint8 bitUnderfloor2Summer = 1; // include in bitsSettingsModules3
		const uint8 bitUnderfloor3Summer = 2; // include in bitsSettingsModules3
		const uint8 bitUnderfloor4Summer = 3; // include in bitsSettingsModules3
		
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
		const uint16 byteAnalogOutPidType = 44;
		const uint16 byteCalcCycleAnalogOut = 45;
		const uint16 byteCalcCycleImpulseCounter = 46;
		const uint16 byteImpulseCounterUpKWh = 47;
		const uint16 byteCalcCycleWeight = 48;
		const uint16 byteCalcCycleRFID = 49;

//###################################################################################
/// byte values: 2byte 50 - 99
		const uint16 byteLightCorrection = 50; // int
		const uint16 byteBMThreshold = 52;
		const uint16 bytePumpPause = 54;
		const uint16 byteMoistureDry = 56;
		const uint16 byteMoistureWet = 58;
		const uint16 byteMaxVolume = 60;
		const uint16 byteAnalogOutKp = 64;
		const uint16 byteAnalogOutTn = 66;
		const uint16 byteAnalogOutTv = 68;
		const uint16 byteAnalogOutSetPoint = 70;
		const uint16 byteNeoPixelPixelCount = 72;
		const uint16 byteImpulseCounterSilver = 74;
		const uint16 byteImpulseCounterRed = 76;

//###################################################################################
/// byte values: 4byte 90 - 149

// kein Platz mehr gehabt...
/// byte values: byte 90 - 119
		const uint16 byteUnderfloor1Setpoint = 90;
		const uint16 byteUnderfloor2Setpoint = 91;
		const uint16 byteUnderfloor3Setpoint = 92;
		const uint16 byteUnderfloor4Setpoint = 93;
		const uint16 byteCalcCycleUnderfloor1 = 94;
		const uint16 byteCalcCycleUnderfloor2 = 95;
		const uint16 byteCalcCycleUnderfloor3 = 96;
		const uint16 byteCalcCycleUnderfloor4 = 97;
		const uint16 byteCalcCycleClock = 98;
		const uint16 byteCalcCycleDS18B20 = 99;
		const uint16 byteDS18B20Count = 100;
		const uint16 byteClockColorHR = 101;
		const uint16 byteClockColorHG = 102;
		const uint16 byteClockColorHB = 103;
		const uint16 byteClockColorMR = 104;
		const uint16 byteClockColorMG = 105;
		const uint16 byteClockColorMB = 106;
		const uint16 byteClockColorSR = 107;
		const uint16 byteClockColorSG = 108;
		const uint16 byteClockColorSB = 109;
		const uint16 byteClockColorQR = 110;
		const uint16 byteClockColorQG = 111;
		const uint16 byteClockColorQB = 112;
		const uint16 byteClockColor5R = 113;
		const uint16 byteClockColor5G = 114;
		const uint16 byteClockColor5B = 115;

// ab hier 4 bytes
		const uint16 byteImpulseCounterKWh = 120;
		const uint16 byteWeightTareValue = 124;
		const uint16 byteWeightTare1kg = 128;


// byte oneWire TempCorrection 200
		const uint16 byteDS18B20Correction[10] = {
			200, 201, 202, 203, 204, 205, 206, 207, 208, 209
		};
// byte oneWire Addresses from 300, each 8 byte
		const uint16 byteDS18B20adr[10][8] = {
			{300, 301, 302, 303, 304, 305, 306, 307},
			{308, 309, 310, 311, 312, 313, 314, 315},
			{316, 317, 318, 319, 320, 321, 322, 323},
			{324, 325, 326, 327, 328, 329, 330, 331},
			{332, 333, 334, 335, 336, 337, 338, 339},
			{340, 341, 342, 343, 344, 345, 346, 347},
			{348, 349, 350, 351, 352, 353, 354, 355},
			{356, 357, 358, 359, 360, 361, 362, 363},
			{364, 365, 366, 367, 368, 369, 370, 371},
			{372, 373, 374, 375, 376, 377, 378, 379}
		};

// **********
		const uint16 addrRestartReason = 495; // 1 byte
		const uint16 addrBootCounter = 496; // 1 byte
		uint16 byteStartForString; // 500


		String mqttTopicDebug;

		helperEEPROM();
		void init();
		void cycle();
		uint16 getVersion();
		void changeDebug();
		void readStringsFromEEPROM();
		void writeStringsToEEPROM();
		void WriteBoolToEEPROM(String name, const uint16 &addr, byte &by, const uint8 &bi, bool &v, bool commit = true);
		void WriteByteToEEPROM(String name, const uint16 &addr, uint8 &v, bool commit = true);
		void WriteByteToEEPROM(String name, const uint16 &addr, int8 &v, bool commit = true);
		void WriteWordToEEPROM(String name, const uint16 &addr, uint16 &v, bool commit = true);
		void WriteWordToEEPROM(String name, const uint16 &addr, short &v, bool commit = true);
		void WriteWordToEEPROM(String name, const uint16 &addr, uint32 &v, bool commit = true);

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 264 $";
		bool DebugLast;
		unsigned long publishDebugLast;
		const uint16 addrStartForString0 = 500;
		String readStringFromEEPROM(int addrOffset, String defaultString);
		int writeStringToEEPROM(int addrOffset, String &strToWrite);
		void readVars();
};
extern helperEEPROM wpEEPROM;
#endif
