//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 08.03.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 117 2024-05-29 01:28:02Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef wpMqtt_h
#define wpMqtt_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
class wpMqtt {
	public:
		// settings
		String mqttTopicMqttServer;
		String mqttTopicMqttSince;
		// commands
		String mqttTopicForceMqttUpdate;
		String mqttTopicForceRenewValue;

		static WiFiClient wifiClient;
		static PubSubClient mqttClient;

		wpMqtt();
		void loop();
	private:
		void connectMqtt();
		static void callbackMqtt(char*, byte*, unsigned int);

};
extern wpMqtt mqtt;
#endif