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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.h 120 2024-05-31 03:32:41Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef helperEEPROM_h
#define helperEEPROM_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <EEPROM.h>
class helperEEPROM {
	public:
		bool DebugEEPROM = false;

/// bool values: byte 0 - 9
		const byte addrBitsModules0 = 0;
		byte bitsModules0;
		const byte bitDHT11 = 0; // include in bitsModules0
		const byte bitDHT22 = 1; // include in bitsModules0
		const byte bitLDR = 2; // include in bitsModules0
		const byte bitLight = 3; // include in bitsModules0
		const byte bitBM = 4; // include in bitsModules0
		const byte bitRelais = 5; // include in bitsModules0
		const byte bitRelaisShield = 6; // include in bitsModules0
		const byte bitRain = 7; // include in bitsModules0

		const byte addrBitsModules1 = 1;
		byte bitsModules1;
		const byte bitMoisture = 0; // include in bitsModules1
		const byte bitDistance = 1; // include in bitsModules1
		
		const byte addrBitsDebugBasis = 2;
		byte bitsDebugBasis;
		const byte bitDebugEEPROM = 0; // include in bitsDebugBasis
		const byte bitDebugWiFi = 1; // include in bitsDebugBasis
		const byte bitDebugMqtt = 2; // include in bitsDebugBasis
		const byte bitDebugFinder = 3; // include in bitsDebugBasis
		const byte bitDebugRest = 4; // include in bitsDebugBasis
		const byte bitDebugOnlineToggler = 5; // include in bitsDebugBasis
		
		const byte addrBitsDebugModules = 3;
		byte bitsDebugModules;
		const byte bitDebugHT = 0; // include in bitsDebugModules
		const byte bitDebugLDR = 1; // include in bitsDebugModules
		const byte bitDebugLight = 2; // include in bitsDebugModules
		const byte bitDebugBM = 3; // include in bitsDebugModules
		const byte bitDebugRelais = 4; // include in bitsDebugModules
		const byte bitDebugRain = 5; // include in bitsDebugModules
		const byte bitDebugMoisture = 6; // include in bitsDebugModules
		const byte bitDebugDistance = 7; // include in bitsDebugModules
		
		const byte addrBitsModulesSettings = 5;
		byte bitsModulesSettings;
		const byte bitUseLdrAvg = 0; // include in bitsModulesSettings
		const byte bitUseLightAvg = 1; // include in bitsModulesSettings
		const byte bitRelaisHand = 2; // include in bitsModulesSettings
		const byte bitRelaisHandValue = 3; // include in bitsModulesSettings
		const byte bitUseRainAvg = 4; // include in bitsModulesSettings
		const byte bitUseMoistureAvg = 5; // include in bitsModulesSettings

/// byte values: byte 10 - 29
		const byte byteMaxCycleHT = 10;
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

		String mqttTopicDebugEEPROM;

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
		String SVNh = "$Rev: 120 $";
		bool DebugEEPROMLast = false;
		uint16_t publishCountDebugEEPROM = 0;
		const byte byteStartForString0 = 100;
		String readStringFromEEPROM(int addrOffset, String defaultString);
		int writeStringToEEPROM(int addrOffset, String &strToWrite);
		void readVars();
};
extern helperEEPROM wpEEPROM;
#endif