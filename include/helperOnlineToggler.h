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
//# Revision     : $Rev:: 181                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperOnlineToggler.h 181 2024-07-27 23:14:47Z           $ #
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
		uint16 getVersion();
		void changeDebug();
		void setMqttOffline();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 181 $";
		bool DebugLast = false;
		uint32 publishForceDebug = 0;
		unsigned long lastContact;
		const unsigned long Minutes10 = 1000 * 60 * 60;
};
extern helperOnlineToggler wpOnlineToggler;
#endif