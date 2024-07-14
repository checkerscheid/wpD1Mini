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
//# Revision     : $Rev:: 163                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperFinder.h 163 2024-07-14 19:03:20Z                  $ #
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
		uint16 getVersion();
		void changeDebug();
		void setupFinder();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 163 $";
		bool DebugLast = false;
		uint16 publishCountDebug = 0;
};
extern helperFinder wpFinder;
#endif