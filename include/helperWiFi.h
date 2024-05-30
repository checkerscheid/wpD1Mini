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
//# Revision     : $Rev:: 118                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 118 2024-05-29 01:29:33Z                          $ #
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
		// settings
		String mqttTopicSsid;
		String mqttTopicIp;
		String mqttTopicMac;
		String mqttTopicWiFiSince;
		String mqttTopicRssi;
		// commands
		String mqttTopicDebugWiFi;
		String WiFiSince;

		helperWiFi();
		void loop();
		uint16_t getVersion();
		void changeDebug();
		void setupWiFi();
		void scanWiFi();
	private:
		String SVNh = "$Rev: 118 $";
		String printEncryptionType(int thisType);
};
extern helperWiFi wpWiFi;
#endif