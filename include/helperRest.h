//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 30.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 163                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperRest.h 163 2024-07-14 19:03:20Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef helperRest_h
#define helperRest_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
class helperRest {
	public:
		bool Debug = false;
		// settings
		String mqttTopicRestServer;
		// commands
		String mqttTopicError;
		String mqttTopicDebug;
		
		bool error = false;
		bool trySend = false;
		bool errorIsSet = false;

		helperRest();
		void init();
		void cycle();
		uint16 getVersion();
		void changeDebug();
		bool sendRest(String name, String value);
		bool sendRawRest(String target);
		void publishErrorRest();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 163 $";
		bool errorLast = false;
		uint16 publishCountError = 0;
		bool DebugLast = false;
		uint16 publishCountDebug = 0;
		String macId;
};
extern helperRest wpRest;
#endif