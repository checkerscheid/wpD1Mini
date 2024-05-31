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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 120 2024-05-31 03:32:41Z                  $ #
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
		const uint16_t loopTime = 200; // ms
		const uint16_t minute10  = 5 * 60 * 10;
		const uint16_t minute5 = 5 * 60 * 10;
		const uint16_t minute2 = 5 * 60 * 10;
		const uint16_t publishQoS = minute10; // 5 because loopTime = 200

		uint16_t MajorVersion = 3;
		uint16_t MinorVersion = 0;
		String Version;

		String OnSince;
		String OnDuration;

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

		// values
		String mqttTopicRestartRequired;
		// settings
		String mqttTopicDeviceName;
		String mqttTopicDeviceDescription;
		String mqttTopicVersion;
		String mqttTopicOnSince;
		String mqttTopicOnDuration;
		// commands
		String mqttTopicSetDeviceName;
		String mqttTopicSetDeviceDescription;
		String mqttTopicRestartDevice;
		String mqttTopicCalcValues;

		wpFreakaZone();
		void init(String);
		void cycle();
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
		void DebugcheckSubscribes(String topic, String value);

		void printStart();
		void printRestored();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 120 $";
		uint16_t publishCountOnDuration = 0;
};
extern wpFreakaZone wpFZ;

#endif
