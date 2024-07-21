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
//# File-ID      : $Id:: helperWiFi.h 163 2024-07-14 19:03:20Z                    $ #
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
		bool sendRest = false;
		// values
		String mqttTopicRssi;
		String mqttTopicWiFiSince;
		// settings
		String mqttTopicSsid;
		String mqttTopicIp;
		String mqttTopicMac;
		// commands
		String mqttTopicDebug;
		String mqttTopicSendRest;

		String WiFiSince;

		helperWiFi();
		void init();
		void cycle();
		uint16 getVersion();
		void changeDebug();
		void changeSendRest();
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
		String SVNh = "$Rev: 163 $";
		bool DebugLast = false;
		uint16 publishCountDebug = 0;
		uint16 publishCountRssi = 0;
		String printEncryptionType(int thisType);
		uint16 addrSendRest;
		byte byteSendRest;
		uint8 bitSendRest;
		bool sendRestLast;
		uint16 publishCountSendRest;
};
extern helperWiFi wpWiFi;
#endif