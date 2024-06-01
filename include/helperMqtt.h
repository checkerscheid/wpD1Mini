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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperMqtt.h 120 2024-05-31 03:32:41Z                    $ #
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
		uint16_t getVersion();
		void changeDebug();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
	private:
		String SVNh = "$Rev: 120 $";
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;
		void connectMqtt();
		void setSubscribes();
		static void callbackMqtt(char*, byte*, unsigned int);

};
extern helperMqtt wpMqtt;
#endif