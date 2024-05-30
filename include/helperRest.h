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
//# Revision     : $Rev:: 118                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 118 2024-05-29 01:29:33Z                          $ #
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
		bool DebugRest = false;
		// settings
		String mqttTopicRestServer;
		// commands
		String mqttTopicErrorRest;
		String mqttTopicDebugRest;
		
		bool errorRest = false;
		bool errorRestLast = false;
		bool trySendRest = false;
		uint16_t publishCountErrorRest = 0;

		helperRest();
		void loop();
		uint16_t getVersion();
		void changeDebug();
		void setupRest();
		bool sendRest(String name, String value);
		bool sendRawRest(String target);
		void publishErrorRest();
	private:
		String SVNh = "$Rev: 118 $";
		void publishInfoDebug(String name, String value, String publishCount);
};
extern helperRest wpRest;
#endif