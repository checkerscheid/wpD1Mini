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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWiFi.h 120 2024-05-31 03:32:41Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef helperWiFi_h
#define helperWiFi_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <helperEEPROM.h>
#include <ESP8266WiFi.h>
class helperWiFi {
	public:
		bool DebugWiFi = false;
		// values
		String mqttTopicRssi;
		String mqttTopicWiFiSince;
		// settings
		String mqttTopicSsid;
		String mqttTopicIp;
		String mqttTopicMac;
		// commands
		String mqttTopicDebugWiFi;

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
		void checkSubscripes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 120 $";
		bool DebugWiFiLast = false;
		uint16_t publishCountDebugWiFi = 0;
		uint16_t publishCountRssi = 0;
		void setSubscribes();
		String printEncryptionType(int thisType);
};
extern helperWiFi wpWiFi;
#endif