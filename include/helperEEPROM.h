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
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.cpp 117 2024-05-29 01:28:02Z                $ #
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
		const byte bitDHT11 = 0;
		const byte bitDHT22 = 1;
		const byte bitLDR = 2;
		const byte bitLight = 3;
		const byte bitBM = 4;
		const byte bitRelais = 5;
		const byte bitRelaisShield = 6;
		const byte bitRain = 7;

		const byte addrBitsModules1 = 1;
		byte bitsModules1;
		const byte bitMoisture = 0;
		const byte bitDistance = 1;
		
		const byte addrBitsDebugBasis = 2;
		byte bitsDebugBasis;
		const byte bitDebugEprom = 0;
		const byte bitDebugWiFi = 1;
		const byte bitDebugMqtt = 2;
		const byte bitDebugFinder = 3;
		const byte bitDebugRest = 4;
		const byte bitDebugOnlineToggler = 5;
		
		const byte addrBitsDebugModules = 3;
		byte bitsDebugModules;
		const byte bitDebugHT = 0;
		const byte bitDebugLDR = 1;
		const byte bitDebugLight = 2;
		const byte bitDebugBM = 3;
		const byte bitDebugRelais = 4;
		const byte bitDebugRain = 5;
		const byte bitDebugMoisture = 6;
		const byte bitDebugDistance = 7;
		
		const byte addrBitsModulesSettings = 5;
		byte bitsModulesSettings;
		const byte bitUseLdrAvg = 0;
		const byte bitUseLightAvg = 1;
		const byte bitRelaisHand = 2;
		const byte bitRelaisHandValue = 3;
		const byte bitUseRainAvg = 4;
		const byte bitUseMoistureAvg = 5;

/// byte values: byte 10 - 29
		const byte byteMaxCycleHT = 10;
		const byte byteMaxCycleLDR = 11;
		const byte byteLDRCorrection = 12; // int8_t
		const byte byteMaxCycleLight = 13;
		const byte bytePumpActive = 14;
		const byte byteMaxCycleRain = 15;
		const byte byteRainCorrection = 16; // int8_t
		const byte byteMaxCycleMoisture = 17;
		const byte byteMoistureMin = 18;
		const byte byteMaxCycleDistance = 19;
		const byte byteDistanceCorrection = 20; // int8_t
		const byte byteHeight = 21;

/// byte values: 2byte 30 - 59
		const byte byteTemperatureCorrection = 30; // int8_t * 10
		const byte byteHumidityCorrection = 32; // int8_t * 10
		const byte byteLightCorrection = 34; // int16_t
		const byte byteThreshold = 36;
		const byte bytePumpPause = 38;
		const byte byteMoistureDry = 40;
		const byte byteMoistureWet = 42;
		const byte byteMaxVolume = 44;

/// byte values: 4byte 60 - 99

		uint16_t byteStartForString;

		String mqttTopicDebugEprom;

		helperEEPROM();
		void loop();
		uint16_t getVersion();
		void changeDebug();
	private:
		String SVNh = "$Rev: 118 $";
		const byte byteStartForString0 = 100;
		void readStringsFromEEPROM();
		void writeStringsToEEPROM();
		String readStringFromEEPROM(int addrOffset, String defaultString);
		int writeStringToEEPROM(int addrOffset, String &strToWrite);
};
extern helperEEPROM wpEEPROM;
#endif