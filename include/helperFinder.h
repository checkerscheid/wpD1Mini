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
#ifndef helperFinder_h
#define helperFinder_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <helperEEPROM.h>
#include <ESPAsyncUDP.h>
class helperFinder {
	public:
		bool DebugFinder = false;
		// commands
		String mqttTopicDebugFinder;

		helperFinder();
		void loop();
		uint16_t getVersion();
		void changeDebug();
		void setupFinder();
	private:
		String SVNh = "$Rev: 118 $";
};
extern helperFinder wpFinder;
#endif