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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperRest.h 120 2024-05-31 03:32:41Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef helperRest_h
#define helperRest_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <helperEEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
class helperRest {
	public:
		bool Debug = false;
		// settings
		String mqttTopicRestServer;
		// commands
		String mqttTopicErrorRest;
		String mqttTopicDebug;
		
		bool errorRest = false;
		bool errorRestLast = false;
		bool trySendRest = false;
		uint16_t publishCountErrorRest = 0;

		helperRest();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();
		void setupRest();
		bool sendRest(String name, String value);
		bool sendRawRest(String target);
		void publishErrorRest();
	private:
		String SVNh = "$Rev: 120 $";
		void publishInfoDebug(String name, String value, String publishCount);
};
extern helperRest wpRest;
#endif