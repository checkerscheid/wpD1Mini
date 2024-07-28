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
//# Revision     : $Rev:: 181                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperFinder.h 181 2024-07-27 23:14:47Z                  $ #
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
		String SVNh = "$Rev: 181 $";
		bool DebugLast = false;
		unsigned long publishDebugLast = 0;
};
extern helperFinder wpFinder;
#endif