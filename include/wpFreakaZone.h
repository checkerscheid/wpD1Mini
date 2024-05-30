//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 08.03.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 117 2024-05-29 01:28:02Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef wpFreakaZone_h
#define wpFreakaZone_h
#include <Arduino.h>
#include <EEPROM.h>
#include <time.h>
#include <ArduinoOTA.h>
#include <ESPAsyncUDP.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <helperWebServer.h>
#include <DHT.h>

#define NTP_SERVER "172.17.1.1"
#define TZ "CET-1CEST,M3.5.0,M10.5.0/3"

/// @brief Base class with std: consts, EEPROM, WebServer and CalcVersion
class wpFreakaZone {
	public:
		const char* ssid = "Mikrowellen";
		const char* password = "Welc0me@wp-Scheid.com";

		const char* mqttServer = "mqtt.freakazone.com";
		const uint16_t mqttServerPort = 1883;
		const char* restServer = "light.freakazone.com";
		const uint16_t restServerPort = 255;
		const char* updateServer = "d1miniupdate.freakazone.com";
		const uint16_t findListenPort = 51346;
		const uint16_t publishQoS = 4 * 60 * 10;

		uint16_t MajorVersion;
		uint16_t MinorVersion;
		uint16_t Build;
		String MainVersion;

		String OnSince;
		String OnDuration;
		String WiFiSince;
		String MqttSince;

		const String strDEBUG  = "[  DEBUG  ]";
		const String strINFO   = "[- INFO  -]";
		const String strWARN   = "[* WARN  *]";
		const String strERRROR = "[! ERROR !]";

		const bool wpDHT11 = false;
		const bool wpDHT22 = false;
		const bool wpLDR = false;
		const bool wpLight = false;
		const bool wpBM = false;
		const bool wpRelais = false;
		const bool wpRelaisShield = false;
		const bool wpRain = false;
		const bool wpMoisture = false;
		const bool wpDistance = false;

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
		byte bitsDebugBais;
		const byte bitDebugEprom = 0;
		const byte bitDebugWiFi = 1;
		const byte bitDebugMqtt = 2;
		const byte bitDebugFinder = 3;
		const byte bitDebugRest = 4;
		
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

		String DeviceName;
		String DeviceDescription;
		bool OfflineTrigger;
		bool UpdateFW;
		bool calcValues;
		bool errorRest;
		bool trySendRest;

		bool DebugEprom = false;
		bool DebugRest = false;

		int16_t ldrCorrection;
		int16_t lightCorrection;
		uint16_t threshold;
		String lightToTurnOn;
		bool relaisHand;
		bool relaisHandValue;
		bool waterEmpty;
		uint16_t pumpActive;
		uint16_t pumpPause;
		float rainCorrection;
		bool useMoistureAvg;
		byte moistureMin;
		// Dry = high e.g. 622
		int16_t moistureDry;
		// Wet = low e.g. 224
		int16_t moistureWet;
		int8_t distanceCorrection;
		uint16_t maxVolume;
		uint8_t height;

		wpFreakaZone(String);
		void loop();

		String getVersion();
		String getTime();
		String getDateTime();
		String getOnlineTime();
		String getOnlineTime(bool forDebug);
		String funcToString(String msg);
		uint16_t getBuild(String);
		void setVersion(uint16_t v);
		void blink();
		bool setupOta();
		void setupFinder();
		String readStringFromEEPROM(int addrOffset, String defaultString);
		int writeStringToEEPROM(int addrOffset, String &strToWrite);
		bool sendRest(String name, String value);
		bool sendRawRest(String target);

		static String JsonKeyValue(String name, String value);
		static String JsonKeyString(String name, String value);

		void DebugWS(String typ, String func, String msg);
		void DebugWS(String typ, String func, String msg, bool newline);
		void SendWS(String msg);

		void printStart();
		void printRestored();

	private:
		String SVNh = "$Rev: 117 $";
		String Revh;
		String Rev;
		int Buildh;
		int Build;

		String doWebServerDebugChange = "";
		void setWebServerDebugChange(String DebugPlugIn);
		void doTheWebServerDebugChange();

		String printEncryptionType(int thisType);
};
extern wpFreakaZone wpFZ;

#endif
