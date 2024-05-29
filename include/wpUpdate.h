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
#ifndef wpUpdate_h
#define wpUpdate_h
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

class wpUpdate {
	public:
		wpUpdate();
		void check();
		void start();
	private:
		const char* server = "http://d1miniupdate.freakazone.com";
		static void started();
		static void finished();
		static void progress(int cur, int total);
		static void error(int err);
};

#endif
