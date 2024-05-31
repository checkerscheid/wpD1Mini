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
#ifndef helperMqtt_h
#define helperMqtt_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <helperOnlineToggler.h>
#include <helperRest.h>
class helperMqtt {
	public:
		static bool DebugMqtt;
		// settings
		String mqttTopicMqttServer;
		String mqttTopicMqttSince;
		// commands
		String mqttTopicForceMqttUpdate;
		String mqttTopicForceRenewValue;
		String mqttTopicDebugMqtt;
		String MqttSince;

		static WiFiClient wifiClient;
		static PubSubClient mqttClient;

		helperMqtt();
		void loop();
		uint16_t getVersion();
		void changeDebug();
		void publishSettings();
		void publishSettings(bool force);
		void publishInfo();
		void publishInfoDebug(String name, String value, String publishCount);
	private:
		String SVNh = "$Rev: 118 $";
		void connectMqtt();
		static void callbackMqtt(char*, byte*, unsigned int);

};
extern helperMqtt wpMqtt;
#endif