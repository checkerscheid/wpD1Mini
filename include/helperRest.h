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
//# Revision     : $Rev:: 123                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperRest.h 123 2024-06-02 04:37:07Z                    $ #
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
		uint16_t getVersion();
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
		String SVNh = "$Rev: 123 $";
		bool errorLast = false;
		uint16_t publishCountError = 0;
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;
		String macId;
};
extern helperRest wpRest;
#endif