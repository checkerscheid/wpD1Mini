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
//# Revision     : $Rev:: 181                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperRest.h 181 2024-07-27 23:14:47Z                    $ #
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
		bool sendRestRGB(uint8 r, uint8 g, uint8 b, uint8 br);
		bool sendRawRest(String target);
		void publishErrorRest();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 181 $";
		bool errorLast = false;
		uint32 publishForceError = 0;
		bool DebugLast = false;
		uint32 publishForceDebug = 0;
		String macId;
};
extern helperRest wpRest;
#endif