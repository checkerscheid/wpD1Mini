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
//# Revision     : $Rev:: 226                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 226 2024-11-21 13:14:50Z                  $ #
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
#include <helperUpdate.h>
#include <helperWebServer.h>
#include <helperWiFi.h>
#include <moduleBase.h>

#include <moduleDHT.h>
#include <moduleLDR.h>
#include <moduleLight.h>
#include <moduleBM.h>
#include <moduleWindow.h>
#include <moduleRelais.h>
#include <moduleRain.h>
#include <moduleMoisture.h>
#include <moduleDistance.h>
#include <moduleCwWw.h>
#include <moduleNeoPixel.h>
#include <moduleAnalogOut.h>
#include <moduleAnalogOut2.h>
#include <moduleImpulseCounter.h>
#include <moduleRpm.h>
#include <moduleUnderfloor.h>
#include <moduleWeight.h>
#include <moduleRFID.h>
#include <moduleClock.h>


#define NTP_SERVER "172.17.1.1"
#define TZ "CET-1CEST,M3.5.0,M10.5.0/3"

/// @brief Base class with std: consts, EEPROM, WebServer and CalcVersion
class wpFreakaZone {
	public:

		const char* ssid = "Mikrowellen";
		const char* password = "Welc0me@wp-Scheid.com";

		const char* mqttServer = "mqtt.freakazone.com";
		const uint16 mqttServerPort = 1883;
		const char* updateServer = "d1miniupdate.freakazone.com";
		const uint16 finderListenPort = 51346;
		const uint32 minute10  = 1000 * 60 * 10;
		const uint32 minute5 = 1000 * 60 * 5;
		const uint32 minute2 = 1000 * 60 * 2;
		const uint32 sekunde30 = 1000 * 30;
		const uint32 sekunde10 = 1000 * 10;
		const uint32 publishQoS = minute10; // 10 minutes in ms

		unsigned long loopStartedAt;

		uint16 MajorVersion = 3;
		uint16 MinorVersion = 2;
		uint16 Build;
		String Version;

		String OnSince;
		String OnDuration;

		const String strDEBUG  = "[  DEBUG  ]";
		const String strINFO   = "[- INFO  -]";
		const String strWARN   = "[* WARN  *]";
		const String strERRROR = "[! ERROR !]";

		const String jsonOK = "{\"erg\":\"S_OK\"}";
		const String jsonERROR = "{\"erg\":\"S_ERROR\"}";

		const char* Pins[18] = {"D3", "TX", "D4", "RX", "D2", "D1",
			"6", "7", "8", "9", "10", "11",
			"D6", "D7", "D5", "D8", "D0", "A0"};

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
		void getTime(uint8 &h, uint8 &m, uint8 &s);
		String getDateTime();
		String getOnlineTime();
		String getOnlineTime(bool forDebug);
		String funcToString(String msg);
		void blink();
		bool blinking();
		long Map(long in, long inMin, long inMax, long outMin, long outMax, bool useMin, bool useMax);
		long Map(long in, long inMin, long inMax, long outMin, long outMax);

		static String JsonKeyValue(String name, String value);
		static String JsonKeyString(String name, String value);

		void DebugWS(String typ, String func, String msg);
		//void SendWS(String msg);
		void SendWSModule(String htmlId, bool value);
		void SendWSDebug(String htmlId, bool value);
		void SendRestartRequired(String msg);
		void SendNewVersion(bool isnew);
		void SendRemainPumpInPause(String readableTime);
		void SendPumpStatus(String pumpStatus);
		void pumpCycleFinished();
		void updateProgress(int percent);
		void SetDeviceName(String name);
		void DebugcheckSubscribes(String topic, String value);

		void printStart();
		void printRestored();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
		bool CheckQoS(unsigned long lastSend);
		bool sendRawRest(String target);
		void InitBootCounter(uint32 bc);
		void BootCount();
		uint32 GetBootCounter();
		void ResetBootCounter();
	private:
		const uint8 blinkStatusNothing = 0;
		const uint8 blinkStatusStart = 1;
		String SVNh = "$Rev: 226 $";
		unsigned long publishOnDurationLast;
		bool calcValuesLast;
		unsigned long publishCalcValuesLast;
		bool restartRequiredLast;
		unsigned long publishRestartRequiredLast;
		uint8 blinkStatus;
		unsigned long blinkStatsusLast;
		uint32 bootCounter;
		short blinkDelay;
		void WriteBootCounter();
		void doBlink();
};
extern wpFreakaZone wpFZ;

#endif
