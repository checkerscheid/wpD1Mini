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
//# Revision     : $Rev:: 183                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperUpdate.h 183 2024-07-29 03:32:26Z                  $ #
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
		bool newVersion = false;
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
		void start(String file);

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 183 $";
		bool DebugLast;
		unsigned long publishDebugLast;
		bool newVersionLast;
		unsigned long publishNewVersionLast;
		unsigned long twelveHours;
		void doCheckUpdate();
		static void started();
		static void finished();
		static void progress(int cur, int total);
		static void error(int err);
};
extern helperUpdate wpUpdate;

#endif