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
//# Revision     : $Rev:: 203                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperUpdate.h 203 2024-10-04 07:32:26Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef helperUpdate_h
#define helperUpdate_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>

class helperUpdate {
	public:
		bool Debug = false;
		// values
		String mqttTopicMode;
		String mqttTopicNewVersion;
		// settings
		String mqttTopicServer;
		// commands
		String mqttTopicUpdateFW;
		String mqttTopicDebug;

		bool UpdateFW = false;
		unsigned long lastUpdateCheck;
		bool newVersion;
		String serverVersion;
		String installedVersion;

		helperUpdate();
		void init();
		void cycle();
		uint16 getVersion();
		void changeDebug();
		bool setupOta();
		void check();
		void start();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 203 $";
		bool DebugLast;
		unsigned long publishDebugLast;
		bool newVersionLast;
		unsigned long publishNewVersionLast;
		unsigned long twelveHours;
		String file;
		void doCheckUpdate();
		static void started();
		static void finished();
		static void progress(int cur, int total);
		static void error(int err);
};
extern helperUpdate wpUpdate;

#endif