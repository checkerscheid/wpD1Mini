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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 269 2025-07-01 19:25:14Z                  $ #
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
#include <IModuleBase.h>

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
#include <moduleDS18B20.h>
#include <moduleRFID.h>
#include <moduleClock.h>
#include <moduleDS18B20.h>



#define NTP_SERVER "172.17.1.1"
#define TZ "CET-1CEST,M3.5.0,M10.5.0/3"

/// @brief Base class with std: consts, EEPROM, WebServer and CalcVersion
class wpFreakaZone {
	public:

		const char* ssid = "Mikrowellen";
		const char* password = "Welc0me@wp-Scheid.com";

		const char* mqttServer = "mqtt.freakazone.com";
		const uint16_t mqttServerPort = 1883;
		const char* updateServer = "d1miniupdate.freakazone.com";
		const uint16_t finderListenPort = 51346;
		const uint32_t minute10  = 1000 * 60 * 10;
		const uint32_t minute5 = 1000 * 60 * 5;
		const uint32_t minute2 = 1000 * 60 * 2;
		const uint32_t sekunde30 = 1000 * 30;
		const uint32_t sekunde10 = 1000 * 10;
		const unsigned long maxWorkingDays = 1000 * 60 * 60 * 24 * 14;
		const uint32_t publishQoS = minute10; // 10 minutes in ms

		unsigned long loopStartedAt;

		uint16_t MajorVersion = 3;
		uint16_t MinorVersion = 4;
		uint16_t Build;
		String Version;

		String OnSince;
		String OnDuration;

		const String strDEBUG  = "[  DEBUG  ]";
		const String strINFO   = "[- INFO  -]";
		const String strWARN   = "[* WARN  *]";
		const String strERRROR = "[! ERROR !]";

		const String jsonOK = "{\"erg\":\"S_OK\"}";
		const String jsonERROR = "{\"erg\":\"S_ERROR\"}";

		const char* Pins[18] = {
			"D3", "TX", "D4", "RX", "D2", "D1",
			"6", "7", "8", "9", "10", "11",
			"D6", "D7", "D5", "D8", "D0", "A0"};

		const uint8_t restartReasonCmd = 1;
		const uint8_t restartReasonMaxWorking = 2;
		const uint8_t restartReasonUpdate = 3;
		const uint8_t restartReasonWiFi = 4;
		const uint8_t restartReasonOnlineToggler = 5;

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
		/// @brief Extract the version from the SVN string
		/// @return SVN version
		uint16_t getVersion();
		/// @brief Extract the build from the SVN string
		/// @param Rev SVN string
		/// @return SVN build
		uint16_t getBuild(String);
		String getTime();
		void getTime(uint8_t &h, uint8_t &m, uint8_t &s);
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
		void SetDeviceDescription(String description);
		void DebugcheckSubscribes(String topic, String value);
		// void DebugSaveBoolToEEPROM(String name, uint16_t addr, uint8_t bit, bool state);
		// void DebugWriteByteToEEPROM(String name, uint16_t addr, uint8_t value);

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
		void InitBootCounter(uint8_t bc);
		void BootCount();
		uint8_t GetBootCounter();
		void ResetBootCounter();
		void InitMaxWorking(bool maxWorking);
		void SetMaxWorking();
		bool GetMaxWorking();
		void InitLastRestartReason(uint8_t restartReason);
		String getLastRestartReason();
		void SetRestartReason(uint8_t restartReason);
	private:
		const uint8_t blinkStatusNothing = 0;
		const uint8_t blinkStatusStart = 1;
		const uint16_t maxWorkingDelay = 1000;
		
		uint8_t _restartReason = 0;
		const String restartReasonStringCmd = "Cmd";
		const String restartReasonStringMaxWorking = "Max Working Counter occurd";
		const String restartReasonStringUpdate = "Update";
		const String restartReasonStringWiFi = "WiFi after Timeout not connected";
		const String restartReasonStringOnlineToggler = "Server Online question after Timeout not recieved";

		String SVNh = "$Rev: 269 $";
		unsigned long publishOnDurationLast;
		bool calcValuesLast;
		unsigned long publishCalcValuesLast;
		bool restartRequiredLast;
		unsigned long publishRestartRequiredLast;
		uint8_t blinkStatus;
		unsigned long blinkStatsusLast;
		uint8_t bootCounter;
		short blinkDelay;
		bool useMaxWorking = false;
		unsigned long maxWorkingMillis;
		void doBlink();
		void RestartAfterMaxWorking();
};
extern wpFreakaZone wpFZ;

#endif
