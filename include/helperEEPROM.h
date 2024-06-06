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
//# File-ID      : $Id:: helperEEPROM.h 135 2024-06-06 14:04:54Z                  $ #
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

/// bool values: byte 0 - 9
		const byte addrBitsModules0 = 0;
		byte bitsModules0;
		const byte bitUseDHT11 = 0; // include in bitsModules0
		const byte bitUseDHT22 = 1; // include in bitsModules0
		const byte bitUseLDR = 2; // include in bitsModules0
		const byte bitUseLight = 3; // include in bitsModules0
		const byte bitUseBM = 4; // include in bitsModules0
		const byte bitUseRelais = 5; // include in bitsModules0
		const byte bitUseRelaisShield = 6; // include in bitsModules0
		const byte bitUseRain = 7; // include in bitsModules0

		const byte addrBitsModules1 = 1;
		byte bitsModules1;
		const byte bitUseMoisture = 0; // include in bitsModules1
		const byte bitUseDistance = 1; // include in bitsModules1
		
		const byte addrBitsDebugBasis0 = 2;
		byte bitsDebugBasis0;
		const byte bitDebugEEPROM = 0; // include in bitsDebugBasis0
		const byte bitDebugFinder = 1; // include in bitsDebugBasis0
		const byte bitDebugModules = 2; // include in bitsDebugBasis0
		const byte bitDebugMqtt = 3; // include in bitsDebugBasis0
		const byte bitDebugOnlineToggler = 4; // include in bitsDebugBasis0
		const byte bitDebugRest = 5; // include in bitsDebugBasis0
		const byte bitDebugUpdate = 6; // include in bitsDebugBasis0
		const byte bitDebugWebServer = 7; // include in bitsDebugBasis0
		
		const byte addrBitsDebugBasis1 = 3;
		byte bitsDebugBasis1;
		const byte bitDebugWiFi = 8; // include in bitsDebugBasis1
		
		const byte addrBitsDebugModules0 = 4;
		byte bitsDebugModules0;
		const byte bitDebugDHT = 0; // include in bitsDebugModules0
		const byte bitDebugLDR = 1; // include in bitsDebugModules0
		const byte bitDebugLight = 2; // include in bitsDebugModules0
		const byte bitDebugBM = 3; // include in bitsDebugModules0
		const byte bitDebugRelais = 4; // include in bitsDebugModules0
		const byte bitDebugRain = 5; // include in bitsDebugModules0
		const byte bitDebugMoisture = 6; // include in bitsDebugModules0
		const byte bitDebugDistance = 7; // include in bitsDebugModules0
		
		const byte addrBitsDebugModules1 = 5;
		byte bitsDebugModules1;

		const byte addrBitsModulesSettings0 = 6;
		byte bitsModulesSettings0;
		const byte bitUseLdrAvg = 0; // include in bitsModulesSettings0
		const byte bitUseLightAvg = 1; // include in bitsModulesSettings0
		const byte bitRelaisHand = 2; // include in bitsModulesSettings0
		const byte bitRelaisHandValue = 3; // include in bitsModulesSettings0
		const byte bitRelaisWaterEmpty = 4; // include in bitsModulesSettings0
		const byte bitUseRainAvg = 5; // include in bitsModulesSettings0
		const byte bitUseMoistureAvg = 6; // include in bitsModulesSettings0

/// byte values: byte 10 - 29
		const byte byteMaxCycleDHT = 10;
		const byte byteTemperatureCorrection = 11; // int8_t * 10
		const byte byteHumidityCorrection = 12; // int8_t * 10
		const byte byteMaxCycleLDR = 13;
		const byte byteLDRCorrection = 14; // int8_t
		const byte byteMaxCycleLight = 15;
		const byte bytePumpActive = 16;
		const byte byteMaxCycleRain = 17;
		const byte byteRainCorrection = 18; // int8_t
		const byte byteMaxCycleMoisture = 19;
		const byte byteMoistureMin = 20;
		const byte byteMaxCycleDistance = 21;
		const byte byteDistanceCorrection = 22; // int8_t
		const byte byteHeight = 23;

/// byte values: 2byte 30 - 59
		const byte byteLightCorrection = 30; // int16_t
		const byte byteThreshold = 32;
		const byte bytePumpPause = 34;
		const byte byteMoistureDry = 36;
		const byte byteMoistureWet = 38;
		const byte byteMaxVolume = 40;

/// byte values: 4byte 60 - 99

		uint16_t byteStartForString;

		String mqttTopicDebug;

		helperEEPROM();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();
		void readStringsFromEEPROM();
		void writeStringsToEEPROM();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 135 $";
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;
		const uint16_t addrStartForString0 = 500;
		String readStringFromEEPROM(int addrOffset, String defaultString);
		int writeStringToEEPROM(int addrOffset, String &strToWrite);
		void readVars();
};
extern helperEEPROM wpEEPROM;
#endif
