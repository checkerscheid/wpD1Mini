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
#include <time.h>
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
		const uint16_t finderListenPort = 51346;
		const uint16_t publishQoS = 4 * 60 * 10;

		uint16_t MajorVersion;
		uint16_t MinorVersion;

		String OnSince;
		String OnDuration;

		const uint16_t loopTime = 200;

		const String strDEBUG  = "[  DEBUG  ]";
		const String strINFO   = "[- INFO  -]";
		const String strWARN   = "[* WARN  *]";
		const String strERRROR = "[! ERROR !]";

		const bool wpDHT11 = false;
		const bool wpDHT22 = false;
		uint8_t choosenDHT;
		const bool wpLDR = false;
		const bool wpLight = false;
		const bool wpBM = false;
		const bool wpRelais = false;
		const bool wpRelaisShield = false;
		const bool wpRain = false;
		const bool wpMoisture = false;
		const bool wpDistance = false;

		String DeviceName;
		String DeviceDescription;
		bool calcValues;

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
		uint16_t getVersion();

		uint16_t getBuild(String);
		String getTime();
		String getDateTime();
		String getOnlineTime();
		String getOnlineTime(bool forDebug);
		String funcToString(String msg);
		void blink();

		static String JsonKeyValue(String name, String value);
		static String JsonKeyString(String name, String value);

		void DebugWS(String typ, String func, String msg);
		void DebugWS(String typ, String func, String msg, bool newline);
		void SendWS(String msg);

		void printStart();
		void printRestored();

	private:
		String SVNh = "$Rev: 117 $";

		String doWebServerDebugChange = "";
		void setWebServerDebugChange(String DebugPlugIn);
		void doTheWebServerDebugChange();
};
extern wpFreakaZone wpFZ;

#endif
