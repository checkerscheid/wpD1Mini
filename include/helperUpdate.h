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
//# Revision     : $Rev:: 118                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 118 2024-05-29 01:29:33Z                          $ #
//#                                                                                 #
//###################################################################################
#ifndef helperUpdate_h
#define helperUpdate_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

class helperUpdate {
	public:
		// values
		String mqttTopicUpdateMode;
		// settings
		String mqttTopicUpdateFW;

		bool UpdateFW = false;

		helperUpdate();
		void loop();
		uint16_t getVersion();
		void check();
		void start();
		bool setupOta();
	private:
		String SVNh = "$Rev: 118 $";
		static void started();
		static void finished();
		static void progress(int cur, int total);
		static void error(int err);
};
extern helperUpdate wpUpdate;

#endif