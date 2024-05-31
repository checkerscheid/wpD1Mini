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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperFinder.h 120 2024-05-31 03:32:41Z                  $ #
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
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();
		void setupFinder();
	private:
		String SVNh = "$Rev: 120 $";
};
extern helperFinder wpFinder;
#endif