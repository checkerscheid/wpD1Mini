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
//# Revision     : $Rev:: 183                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperMqtt.h 183 2024-07-29 03:32:26Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef helperMqtt_h
#define helperMqtt_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
class helperMqtt {
	public:
		bool Debug = false;
		// values
		String mqttTopicMqttSince;
		// settings
		String mqttTopicMqttServer;
		// commands
		String mqttTopicForceMqttUpdate;
		String mqttTopicForceRenewValue;
		String mqttTopicDebug;

		String MqttSince;

		static WiFiClient wifiClient;
		static PubSubClient mqttClient;

		helperMqtt();
		void init();
		void cycle();
		uint16 getVersion();
		void changeDebug();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void connectMqtt();
	private:
		String SVNh = "$Rev: 183 $";
		bool DebugLast;
		unsigned long publishDebugLast;
		static void callbackMqtt(char*, byte*, unsigned int);
		unsigned long lastConnectTry;
};
extern helperMqtt wpMqtt;
#endif