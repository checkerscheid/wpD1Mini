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
//# Revision     : $Rev:: 136                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWiFi.h 136 2024-06-09 15:37:41Z                    $ #
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
		uint16_t getVersion();
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
		String SVNh = "$Rev: 136 $";
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;
		uint16_t publishCountRssi = 0;
		String printEncryptionType(int thisType);
		uint16_t addrSendRest;
		byte byteSendRest;
		uint8_t bitSendRest;
		bool sendRestLast;
		uint16_t publishCountSendRest;
};
extern helperWiFi wpWiFi;
#endif