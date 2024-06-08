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
//# Revision     : $Rev:: 133                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 133 2024-06-06 12:40:21Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef wpFreakaZone_h
#define wpFreakaZone_h
#include <Arduino.h>
#include <time.h>
#include <DHT.h>
#include <helperEEPROM.h>
#include <helperFinder.h>
#include <helperModules.h>
#include <helperMqtt.h>
#include <helperOnlineToggler.h>
#include <helperRest.h>
#include <helperUpdate.h>
#include <helperWebServer.h>
#include <helperWiFi.h>
#include <moduleDHT.h>
#include <moduleLDR.h>
#include <moduleLight.h>
#include <moduleBM.h>
#include <moduleRelais.h>
#include <moduleRain.h>
#include <moduleMoisture.h>
#include <moduleDistance.h>

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
		const char* updateServer = "http://d1miniupdate.freakazone.com";
		const uint16_t finderListenPort = 51346;
		const uint16_t loopTime = 200; // ms
		const uint16_t minute10  = 5 * 60 * 10;
		const uint16_t minute5 = 5 * 60 * 5;
		const uint16_t minute2 = 5 * 60 * 2;
		const uint16_t sekunde30 = 5 * 30;
		const uint16_t sekunde10 = 5 * 10;
		const uint16_t publishQoS = minute10; // 5 because loopTime = 200

		uint16_t MajorVersion = 3;
		uint16_t MinorVersion = 0;
		uint16_t Build;
		String Version;

		String OnSince;
		String OnDuration;

		const String strDEBUG  = "[  DEBUG  ]";
		const String strINFO   = "[- INFO  -]";
		const String strWARN   = "[* WARN  *]";
		const String strERRROR = "[! ERROR !]";

		String DeviceName = "BasisEmpty";
		String DeviceDescription = "BasisEmpty";
		bool calcValues;
		bool restartRequired;

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
		//void SendWS(String msg);
		void SendWSModule(String htmlId, bool value);
		void SendWSSendRest(String htmlId, bool value);
		void SendWSDebug(String htmlId, bool value);
		void SendRestartRequired(String msg);
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
		String SVNh = "$Rev: 133 $";
		uint16_t publishCountOnDuration;
		bool calcValuesLast;
		uint16_t publishCountCalcValues;
		bool restartRequiredLast;
		uint16_t publishCountRestartRequired;
};
extern wpFreakaZone wpFZ;

#endif
