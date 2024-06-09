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
//# Revision     : $Rev:: 136                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperEEPROM.h 136 2024-06-09 15:37:41Z                  $ #
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
		
		const uint16_t addrBitsModules2 = 2;
		byte bitsModules2;

//###################################################################################

		const uint16_t addrBitsSendRestBasis0 = 3;
		byte bitsSendRestBasis0;
		const uint8_t bitSendRestRssi = 0; // include in bitsSendRestBasis0

		const uint16_t addrBitsDebugBasis0 = 4;
		byte bitsDebugBasis0;
		const uint8_t bitDebugEEPROM = 0; // include in bitsDebugBasis0
		const uint8_t bitDebugFinder = 1; // include in bitsDebugBasis0
		const uint8_t bitDebugModules = 2; // include in bitsDebugBasis0
		const uint8_t bitDebugMqtt = 3; // include in bitsDebugBasis0
		const uint8_t bitDebugOnlineToggler = 4; // include in bitsDebugBasis0
		const uint8_t bitDebugRest = 5; // include in bitsDebugBasis0
		const uint8_t bitDebugUpdate = 6; // include in bitsDebugBasis0
		const uint8_t bitDebugWebServer = 7; // include in bitsDebugBasis0
		
		const uint16_t addrBitsDebugBasis1 = 5;
		byte bitsDebugBasis1;
		const uint8_t bitDebugWiFi = 0; // include in bitsDebugBasis1

//###################################################################################

		const uint16_t addrBitsSendRestModules0 = 6;
		byte bitsSendRestModules0;
		const uint8_t bitSendRestDHT = 0; // include in bitsSendRestModules0
		const uint8_t bitSendRestLDR = 1; // include in bitsSendRestModules0
		const uint8_t bitSendRestLight = 2; // include in bitsSendRestModules0
		const uint8_t bitSendRestBM = 3; // include in bitsSendRestModules0
		const uint8_t bitSendRestRelais = 4; // include in bitsSendRestModules0
		const uint8_t bitSendRestRain = 5; // include in bitsSendRestModules0
		const uint8_t bitSendRestMoisture = 6; // include in bitsSendRestModules0
		const uint8_t bitSendRestDistance = 7; // include in bitsSendRestModules0
		
		const uint16_t addrBitsSendRestModules1 = 7;
		byte bitsSendRestModules1;
		
		const uint16_t addrBitsSendRestModules2 = 8;
		byte bitsSendRestModules2;

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
		
		const uint16_t addrBitsDebugModules2 = 11;
		byte bitsDebugModules2;

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

		const uint16_t addrBitsSettingsModules1 = 13;
		byte bitsSettingsModules1;

		const uint16_t addrBitsSettingsModules2 = 14;
		byte bitsSettingsModules2;
		
//###################################################################################
/// byte values: byte 20 - 49
		const uint16_t byteMaxCycleDHT = 20;
		const uint16_t byteTemperatureCorrection = 21; // int8_t * 10
		const uint16_t byteHumidityCorrection = 22; // int8_t * 10
		const uint16_t byteMaxCycleLDR = 23;
		const uint16_t byteLDRCorrection = 24; // int8_t
		const uint16_t byteMaxCycleLight = 25;
		const uint16_t bytePumpActive = 26;
		const uint16_t byteMaxCycleRain = 27;
		const uint16_t byteRainCorrection = 28; // int8_t
		const uint16_t byteMaxCycleMoisture = 29;
		const uint16_t byteMoistureMin = 30;
		const uint16_t byteMaxCycleDistance = 31;
		const uint16_t byteDistanceCorrection = 32; // int8_t
		const uint16_t byteHeight = 33;

//###################################################################################
/// byte values: 2byte 50 - 79
		const uint16_t byteLightCorrection = 50; // int16_t
		const uint16_t byteThreshold = 52;
		const uint16_t bytePumpPause = 54;
		const uint16_t byteMoistureDry = 56;
		const uint16_t byteMoistureWet = 58;
		const uint16_t byteMaxVolume = 60;

//###################################################################################
/// byte values: 4byte 80 - 99

		uint16_t byteStartForString;

		String mqttTopicDebug;

		helperEEPROM();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();
		void readStringsFromEEPROM();
		void writeStringsToEEPROM();
		void saveBool(uint16_t &addr, byte &by, uint8_t &bi, bool v);

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 136 $";
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;
		const uint16_t addrStartForString0 = 500;
		String readStringFromEEPROM(int addrOffset, String defaultString);
		int writeStringToEEPROM(int addrOffset, String &strToWrite);
		void readVars();
};
extern helperEEPROM wpEEPROM;
#endif
