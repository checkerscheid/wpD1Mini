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
//# Revision     : $Rev:: 246                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperOnlineToggler.h 246 2025-02-18 16:27:11Z           $ #
//#                                                                                 #
//###################################################################################
#ifndef helperOnlineToggler_h
#define helperOnlineToggler_h
#include <Arduino.h>
#include <wpFreakaZone.h>
class helperOnlineToggler {
	public:
// values
		bool Debug = false;
		// settings
		String mqttTopicOnlineToggler;
		String mqttTopicErrorOnline; // 1 Error
		// commands
		String mqttTopicDebug;

		helperOnlineToggler();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();
		void setMqttOffline();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 246 $";
		bool DebugLast;
		unsigned long publishDebugLast;
		unsigned long lastContact;
		const unsigned long Minutes10 = 1000 * 60 * 60;
};
extern helperOnlineToggler wpOnlineToggler;
#endif