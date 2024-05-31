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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperOnlineToggler.h 120 2024-05-31 03:32:41Z           $ #
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
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();
		void setMqttOffline();
	private:
		String SVNh = "$Rev: 120 $";
		void checkOfflineTrigger();
};
extern helperOnlineToggler wpOnlineToggler;
#endif