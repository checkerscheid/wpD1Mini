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
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperUpdate.h 207 2024-10-07 12:59:22Z                  $ #
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
		String mqttTopicUpdateChanel;
		// commands
		String mqttTopicUpdateFW;
		String mqttTopicDebug;
		String mqttTopicSetUpdateChanel;

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
		String GetUpdateChanel();
		void SetUpdateChanel(uint8 uc);

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 207 $";
		bool DebugLast;
		unsigned long publishDebugLast;
		bool newVersionLast;
		unsigned long publishNewVersionLast;
		unsigned long twelveHours;
		String file;
		uint8 updateChanel;
		void doCheckUpdate();
		static void started();
		static void finished();
		static void progress(int cur, int total);
		static void error(int err);
};
extern helperUpdate wpUpdate;

#endif