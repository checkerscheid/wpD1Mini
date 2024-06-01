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
//# Revision     : $Rev:: 122                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperUpdate.h 122 2024-06-01 07:52:17Z                  $ #
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
		// settings
		String mqttTopicServer;
		// commands
		String mqttTopicUpdateFW;
		String mqttTopicDebug;

		bool UpdateFW = false;

		helperUpdate();
		void init();
		void cycle();
		uint16_t getVersion();
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
		String SVNh = "$Rev: 122 $";
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;
		static void started();
		static void finished();
		static void progress(int cur, int total);
		static void error(int err);
};
extern helperUpdate wpUpdate;

#endif