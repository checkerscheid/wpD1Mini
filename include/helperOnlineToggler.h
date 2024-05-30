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
#ifndef helperOnlineToggler_h
#define helperOnlineToggler_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <helperEEPROM.h>
class helperOnlineToggler {
	public:
// values
		bool DebugOnlineToggler = false;
		// settings
		String mqttTopicOnlineToggler;
		String mqttTopicErrorOnline; // 1 Error
		// commands
		String mqttTopicDebugOnlineToggler;

		bool OfflineTrigger;

		helperOnlineToggler();
		void loop();
		uint16_t getVersion();
		void changeDebug();
	private:
		String SVNh = "$Rev: 118 $";
		void checkOfflineTrigger();
};
extern helperOnlineToggler wpOnlineToggler;
#endif