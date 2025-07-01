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
//# Revision     : $Rev:: 246                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWiFi.h 246 2025-02-18 16:27:11Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef helperWiFi_h
#define helperWiFi_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <ESP8266WiFi.h>
class helperWiFi {
	public:
		bool Debug = false;
		// values
		String mqttTopicRssi;
		String mqttTopicWiFiSince;
		// settings
		String mqttTopicSsid;
		String mqttTopicIp;
		String mqttTopicMac;
		// commands
		String mqttTopicDebug;

		String WiFiSince;

		helperWiFi();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();
		void setupWiFi();
		void scanWiFi();
		
		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
		void checkDns();
	private:
		String SVNh = "$Rev: 246 $";
		bool DebugLast;
		unsigned long publishDebugLast;
		unsigned long publishRssiLast;
		String printEncryptionType(int thisType);
};
extern helperWiFi wpWiFi;
#endif