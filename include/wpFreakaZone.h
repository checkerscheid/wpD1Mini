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
//# Revision     : $Rev:: 172                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 172 2024-07-23 22:01:24Z                  $ #
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
#include <moduleBase.h>
#include <moduleDHT.h>
#include <moduleLDR.h>
#include <moduleLight.h>
#include <moduleBM.h>
#include <moduleWindow.h>
#include <moduleAnalogOut.h>
#include <moduleAnalogOut2.h>
#include <moduleNeoPixel.h>
#include <moduleRelais.h>
#include <moduleRpm.h>
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
		const uint16 mqttServerPort = 1883;
		const char* restServer = "light.freakazone.com";
		const uint16 restServerPort = 255;
		const char* updateServer = "d1miniupdate.freakazone.com";
		const uint16 finderListenPort = 51346;
		uint16 loopTime = 200; // ms
		const uint16 minute10  = 5 * 60 * 10;
		const uint16 minute5 = 5 * 60 * 5;
		const uint16 minute2 = 5 * 60 * 2;
		const uint16 sekunde30 = 5 * 30;
		const uint16 sekunde10 = 5 * 10;
		const uint16 publishQoS = minute10; // 5 because loopTime = 200

		uint16 MajorVersion = 3;
		uint16 MinorVersion = 1;
		uint16 Build;
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
		uint16 getVersion();

		uint16 getBuild(String);
		String getTime();
		String getDateTime();
		String getOnlineTime();
		String getOnlineTime(bool forDebug);
		String funcToString(String msg);
		void blink();
		long Map(long in, long inMin, long inMax, long outMin, long outMax);

		static String JsonKeyValue(String name, String value);
		static String JsonKeyString(String name, String value);

		void DebugWS(String typ, String func, String msg);
		//void SendWS(String msg);
		void SendWSModule(String htmlId, bool value);
		void SendWSSendRest(String htmlId, bool value);
		void SendWSDebug(String htmlId, bool value);
		void SendRestartRequired(String msg);
		void SendNewVersion(bool isnew);
		void SendRemainPumpInPause(String readableTime);
		void SendPumpStatus(String pumpStatus);
		void pumpCycleFinished();
		void updateProgress(int percent);
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
		String SVNh = "$Rev: 172 $";
		uint16 publishCountOnDuration;
		bool calcValuesLast;
		uint16 publishCountCalcValues;
		bool restartRequiredLast;
		uint16 publishCountRestartRequired;
};
extern wpFreakaZone wpFZ;

#endif
