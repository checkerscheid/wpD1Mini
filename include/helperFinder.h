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
#include <ESPAsyncUDP.h>
class helperFinder {
	public:
		bool Debug = false;
		// commands
		String mqttTopicDebug;

		helperFinder();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();
		void setupFinder();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 120 $";
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;
};
extern helperFinder wpFinder;
#endif