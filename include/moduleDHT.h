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
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 117 2024-05-29 01:28:02Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef wpDHT_h
#define wpDHT_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPin D7

class wpDHT {
	public:
		bool errorHT = false;
		int16_t temperature = 0;
		int16_t humidity = 0;
		// values
		String mqttTopicTemperature;
		String mqttTopicHumidity;
		String mqttTopicErrorHT;
		// settings
		String mqttTopicMaxCycleHT;
		String mqttTopicTemperatureCorrection;
		String mqttTopicHumidityCorrection;
		// commands
		String mqttTopicDebugHT;

		int8_t temperatureCorrection;
		int8_t humidityCorrection;

		DHT dht;

		wpDHT(PubSubClient, uint8_t);
		void loop();

		void publishValueTemp(int equalVal);
		void publishValueHum(int equalVal);
		void publishErrorHT();
	private:
		PubSubClient mqttClient;
		uint16_t cycleHT = 0;
		bool errorHTLast = false;
		uint16_t publishCountErrorHT = 0;
		int16_t temperatureLast = 0;
		uint16_t publishCountTemperature = 0;
		int16_t humidityLast = 0;
		uint16_t publishCountHumidity = 0;

		void calcHT();
		void calcHTDebug(String name, int16_t value, float raw);
		void calcHTError(String name);
};

#endif