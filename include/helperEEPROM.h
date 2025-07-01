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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.h 269 2025-07-01 19:25:14Z                  $ #
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
		const uint16_t addrBitsModules0 = 0;
		byte bitsModules0;
		const uint8_t bitUseDHT11 = 0; // include in bitsModules0
		const uint8_t bitUseDHT22 = 1; // include in bitsModules0
		const uint8_t bitUseLDR = 2; // include in bitsModules0
		const uint8_t bitUseLight = 3; // include in bitsModules0
		const uint8_t bitUseBM = 4; // include in bitsModules0
		const uint8_t bitUseRelais = 5; // include in bitsModules0
		const uint8_t bitUseRelaisShield = 6; // include in bitsModules0
		const uint8_t bitUseRain = 7; // include in bitsModules0

		const uint16_t addrBitsModules1 = 1;
		byte bitsModules1;
		const uint8_t bitUseMoisture = 0; // include in bitsModules1
		const uint8_t bitUseDistance = 1; // include in bitsModules1
		const uint8_t bitUseWindow = 2; // include in bitsModules1
		const uint8_t bitUseAnalogOut = 3; // include in bitsModules1
		const uint8_t bitUseRpm = 4; // include in bitsModules1
		const uint8_t bitUseAnalogOut2 = 5; // include in bitsModules1
		const uint8_t bitUseNeoPixel = 6; // include in bitsModules1
		const uint8_t bitUseImpulseCounter = 7; // include in bitsModules1
		
		const uint16_t addrBitsModules2 = 2;
		byte bitsModules2;
		const uint8_t bitUseCwWw = 0; // include in bitsModules2
		const uint8_t bitUseUnderfloor1 = 1; // include in bitsModules2
		const uint8_t bitUseUnderfloor2 = 2; // include in bitsModules2
		const uint8_t bitUseUnderfloor3 = 3; // include in bitsModules2
		const uint8_t bitUseUnderfloor4 = 4; // include in bitsModules2
		const uint8_t bitUseWindow2 = 5; // include in bitsModules2
		const uint8_t bitUseWindow3 = 6; // include in bitsModules2
		const uint8_t bitUseWeight = 7; // include in bitsModules2

		const uint16_t addrBitsModules3 = 3;
		byte bitsModules3;
		const uint8_t bitUseRFID = 0; // include in bitsModules3
		const uint8_t bitUseClock = 1; // include in bitsModules3
		const uint8_t bitUseDS18B20 = 2; // include in bitsModules3

//###################################################################################

		const uint16_t addrBitsDebugBasis0 = 4;
		byte bitsDebugBasis0;
		const uint8_t bitDebugEEPROM = 0; // include in bitsDebugBasis0
		const uint8_t bitDebugFinder = 1; // include in bitsDebugBasis0
		const uint8_t bitDebugModules = 2; // include in bitsDebugBasis0
		const uint8_t bitDebugMqtt = 3; // include in bitsDebugBasis0
		const uint8_t bitDebugOnlineToggler = 4; // include in bitsDebugBasis0
		const uint8_t bitDebugUpdate = 6; // include in bitsDebugBasis0
		const uint8_t bitDebugWebServer = 7; // include in bitsDebugBasis0
		
		const uint16_t addrBitsDebugBasis1 = 5;
		byte bitsDebugBasis1;
		const uint8_t bitDebugWiFi = 0; // include in bitsDebugBasis1

//###################################################################################

		const uint16_t addrBitsSettingsBasis0 = 6;
		byte bitsSettingsBasis0;
		const uint8_t bitUseMaxWorking = 0; // include in bitsSettingsBasis0

		// const uint16_t addrBitsDebugModules0 = 8; used in BitsDebugModules3

//###################################################################################

		const uint16_t addrBitsDebugModules0 = 9;
		byte bitsDebugModules0;
		const uint8_t bitDebugDHT = 0; // include in bitsDebugModules0
		const uint8_t bitDebugLDR = 1; // include in bitsDebugModules0
		const uint8_t bitDebugLight = 2; // include in bitsDebugModules0
		const uint8_t bitDebugBM = 3; // include in bitsDebugModules0
		const uint8_t bitDebugRelais = 4; // include in bitsDebugModules0
		const uint8_t bitDebugRain = 5; // include in bitsDebugModules0
		const uint8_t bitDebugMoisture = 6; // include in bitsDebugModules0
		const uint8_t bitDebugDistance = 7; // include in bitsDebugModules0
		
		const uint16_t addrBitsDebugModules1 = 10;
		byte bitsDebugModules1;
		const uint8_t bitDebugWindow = 0; // include in bitsDebugModules1
		const uint8_t bitDebugAnalogOut = 1; // include in bitsDebugModules1
		const uint8_t bitDebugRpm = 2; // include in bitsDebugModules1
		const uint8_t bitDebugAnalogOut2 = 3; // include in bitsDebugModules1
		const uint8_t bitDebugNeoPixel = 4; // include in bitsDebugModules1
		const uint8_t bitDebugImpulseCounter = 5; // include in bitsDebugModules1
		const uint8_t bitDebugCwWw = 6; // include in bitsDebugModules1
		const uint8_t bitDebugRFID = 7; // include in bitsDebugModules1
		
		const uint16_t addrBitsDebugModules2 = 11;
		byte bitsDebugModules2;
		const uint8_t bitDebugUnderfloor1 = 0; // include in bitsDebugModules2
		const uint8_t bitDebugUnderfloor2 = 1; // include in bitsDebugModules2
		const uint8_t bitDebugUnderfloor3 = 2; // include in bitsDebugModules2
		const uint8_t bitDebugUnderfloor4 = 3; // include in bitsDebugModules2
		const uint8_t bitDebugWindow2 = 4; // include in bitsDebugModules2
		const uint8_t bitDebugWindow3 = 5; // include in bitsDebugModules2
		const uint8_t bitDebugWeight = 6; // include in bitsDebugModules2
		const uint8_t bitDebugClock = 7; // include in bitsDebugModules2

		const uint16_t addrBitsDebugModules3 = 8;
		byte bitsDebugModules3;
		const uint8_t bitDebugDS18B20 = 0; // include in bitsDebugModules3

//###################################################################################

		const uint16_t addrBitsSettingsModules0 = 12;
		byte bitsSettingsModules0;
		const uint8_t bitUseLdrAvg = 0; // include in bitsSettingsModules0
		const uint8_t bitUseLightAvg = 1; // include in bitsSettingsModules0
		const uint8_t bitRelaisHand = 2; // include in bitsSettingsModules0
		const uint8_t bitRelaisHandValue = 3; // include in bitsSettingsModules0
		const uint8_t bitRelaisWaterEmpty = 4; // include in bitsSettingsModules0
		const uint8_t bitUseRainAvg = 5; // include in bitsSettingsModules0
		const uint8_t bitUseMoistureAvg = 6; // include in bitsSettingsModules0
		const uint8_t bitAnalogOutHand = 7; // include in bitsSettingsModules0

		const uint16_t addrBitsSettingsModules1 = 13;
		byte bitsSettingsModules1;
		const uint8_t bitUseRpmAvg = 0; // include in bitsSettingsModules1
		const uint8_t bitAnalogOut2Hand = 1; // include in bitsSettingsModules1
		const uint8_t bitNeoPixelRGB = 2; // include in bitsSettingsModules1
		const uint8_t bitUseWeightAvg = 4; // include in bitsSettingsModules1
		const uint8_t bitNeoPixelUseWW = 5; // include in bitsSettingsModules1
		const uint8_t bitNeoPixelUseCW = 6; // include in bitsSettingsModules1
		const uint8_t bitAnalogOutSummer = 7; // include in bitsSettingsModules1

		const uint16_t addrBitsSettingsModules2 = 14;
		byte bitsSettingsModules2;
		const uint8_t bitUnderfloor1Hand = 0; // include in bitsSettingsModules2
		const uint8_t bitUnderfloor2Hand = 1; // include in bitsSettingsModules2
		const uint8_t bitUnderfloor3Hand = 2; // include in bitsSettingsModules2
		const uint8_t bitUnderfloor4Hand = 3; // include in bitsSettingsModules2
		const uint8_t bitUnderfloor1HandValue = 4; // include in bitsSettingsModules2
		const uint8_t bitUnderfloor2HandValue = 5; // include in bitsSettingsModules2
		const uint8_t bitUnderfloor3HandValue = 6; // include in bitsSettingsModules2
		const uint8_t bitUnderfloor4HandValue = 7; // include in bitsSettingsModules2

		const uint16_t addrBitsSettingsModules3 = 15;
		byte bitsSettingsModules3;
		const uint8_t bitUnderfloor1Summer = 0; // include in bitsSettingsModules3
		const uint8_t bitUnderfloor2Summer = 1; // include in bitsSettingsModules3
		const uint8_t bitUnderfloor3Summer = 2; // include in bitsSettingsModules3
		const uint8_t bitUnderfloor4Summer = 3; // include in bitsSettingsModules3
		
//###################################################################################
/// byte values: byte 20 - 49
		const uint16_t byteCalcCycleDHT = 20;
		const uint16_t byteTemperatureCorrection = 21; // int8_t * 10
		const uint16_t byteHumidityCorrection = 22; // int8_t * 10
		const uint16_t byteCalcCycleLDR = 23;
		const uint16_t byteLDRCorrection = 24; // int8
		const uint16_t byteCalcCycleLight = 25;
		const uint16_t bytePumpActive = 26;
		const uint16_t byteCalcCycleRain = 27;
		const uint16_t byteRainCorrection = 28; // int8
		const uint16_t byteCalcCycleMoisture = 29;
		const uint16_t byteMoistureMin = 30;
		const uint16_t byteCalcCycleDistance = 31;
		const uint16_t byteDistanceCorrection = 32; // int8
		const uint16_t byteHeight = 33;
		const uint16_t byteAnalogOutHandValue = 35;
		const uint16_t byteCalcCycleRpm = 36;
		const uint16_t byteRpmCorrection = 37; // int8
		const uint16_t byteAnalogOut2HandValue = 39;
		const uint16_t byteNeoPixelValueR = 41;
		const uint16_t byteNeoPixelValueG = 42;
		const uint16_t byteNeoPixelValueB = 43;
		const uint16_t byteAnalogOutPidType = 44;
		const uint16_t byteCalcCycleAnalogOut = 45;
		const uint16_t byteCalcCycleImpulseCounter = 46;
		const uint16_t byteImpulseCounterUpKWh = 47;
		const uint16_t byteCalcCycleWeight = 48;
		const uint16_t byteCalcCycleRFID = 49;

//###################################################################################
/// byte values: 2byte 50 - 99
		const uint16_t byteLightCorrection = 50; // int
		const uint16_t byteBMThreshold = 52;
		const uint16_t bytePumpPause = 54;
		const uint16_t byteMoistureDry = 56;
		const uint16_t byteMoistureWet = 58;
		const uint16_t byteMaxVolume = 60;
		const uint16_t byteAnalogOutKp = 64;
		const uint16_t byteAnalogOutTn = 66;
		const uint16_t byteAnalogOutTv = 68;
		const uint16_t byteAnalogOutSetPoint = 70;
		const uint16_t byteNeoPixelPixelCount = 72;
		const uint16_t byteImpulseCounterSilver = 74;
		const uint16_t byteImpulseCounterRed = 76;

//###################################################################################
/// byte values: 4byte 90 - 149

// kein Platz mehr gehabt...
/// byte values: byte 90 - 119
		const uint16_t byteUnderfloor1Setpoint = 90;
		const uint16_t byteUnderfloor2Setpoint = 91;
		const uint16_t byteUnderfloor3Setpoint = 92;
		const uint16_t byteUnderfloor4Setpoint = 93;
		const uint16_t byteCalcCycleUnderfloor1 = 94;
		const uint16_t byteCalcCycleUnderfloor2 = 95;
		const uint16_t byteCalcCycleUnderfloor3 = 96;
		const uint16_t byteCalcCycleUnderfloor4 = 97;
		const uint16_t byteCalcCycleClock = 98;
		const uint16_t byteCalcCycleDS18B20 = 99;
		const uint16_t byteDS18B20Count = 100;
		const uint16_t byteClockColorHR = 101;
		const uint16_t byteClockColorHG = 102;
		const uint16_t byteClockColorHB = 103;
		const uint16_t byteClockColorMR = 104;
		const uint16_t byteClockColorMG = 105;
		const uint16_t byteClockColorMB = 106;
		const uint16_t byteClockColorSR = 107;
		const uint16_t byteClockColorSG = 108;
		const uint16_t byteClockColorSB = 109;
		const uint16_t byteClockColorQR = 110;
		const uint16_t byteClockColorQG = 111;
		const uint16_t byteClockColorQB = 112;
		const uint16_t byteClockColor5R = 113;
		const uint16_t byteClockColor5G = 114;
		const uint16_t byteClockColor5B = 115;

// ab hier 4 bytes
		const uint16_t byteImpulseCounterKWh = 120;
		const uint16_t byteWeightTareValue = 124;
		const uint16_t byteWeightTare1kg = 128;


// byte oneWire TempCorrection 200
		const uint16_t byteDS18B20Correction[10] = {
			200, 201, 202, 203, 204, 205, 206, 207, 208, 209
		};
// byte oneWire Addresses from 300, each 8 byte
		const uint16_t byteDS18B20adr[10][8] = {
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
		const uint16_t addrRestartReason = 495; // 1 byte
		const uint16_t addrBootCounter = 496; // 1 byte
		uint16_t byteStartForString; // 500


		String mqttTopicDebug;

		helperEEPROM();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();
		void readStringsFromEEPROM();
		void writeStringsToEEPROM();
		void WriteBoolToEEPROM(String name, const uint16_t &addr, byte &by, const uint8_t &bi, bool &v, bool commit = true);
		void WriteByteToEEPROM(String name, const uint16_t &addr, uint8_t &v, bool commit = true);
		void WriteByteToEEPROM(String name, const uint16_t &addr, int8_t &v, bool commit = true);
		void WriteWordToEEPROM(String name, const uint16_t &addr, uint16_t &v, bool commit = true);
		void WriteWordToEEPROM(String name, const uint16_t &addr, short &v, bool commit = true);
		void WriteWordToEEPROM(String name, const uint16_t &addr, uint32_t &v, bool commit = true);

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 269 $";
		bool DebugLast;
		unsigned long publishDebugLast;
		const uint16_t addrStartForString0 = 500;
		String readStringFromEEPROM(int addrOffset, String defaultString);
		int writeStringToEEPROM(int addrOffset, String &strToWrite);
		void readVars();
};
extern helperEEPROM wpEEPROM;
#endif
