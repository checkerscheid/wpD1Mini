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
//# File-ID      : $Id:: helperWiFi.h 181 2024-07-27 23:14:47Z                    $ #
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
		String SVNh = "$Rev: 181 $";
		bool DebugLast = false;
		uint32 publishForceDebug = 0;
		uint32 publishForceRssi = 0;
		String printEncryptionType(int thisType);
		uint16 addrSendRest;
		byte byteSendRest;
		uint8 bitSendRest;
		bool sendRestLast;
		uint32 publishForceSendRest;
};
extern helperWiFi wpWiFi;
#endif