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
#ifndef moduleDHT_h
#define moduleDHT_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <helperMqtt.h>
#include <helperRest.h>
#include <DHT.h>

#define DHTPin D7

class moduleDHT {
	public:
		int16_t temperature = 0;
		int16_t humidity = 0;
		// values
		String mqttTopicTemperature;
		String mqttTopicHumidity;
		String mqttTopicErrorHT;
		// settings
		String mqttTopicMaxCycle;
		String mqttTopicTemperatureCorrection;
		String mqttTopicHumidityCorrection;
		// commands
		String mqttTopicDebugHT;

		bool DebugHT = false;
		bool errorHT = false;
		int8_t temperatureCorrection = 0;
		int8_t humidityCorrection = 0;
		uint16_t maxCycle = 4;

		moduleDHT();
		void loop();
		uint16_t getVersion();

		void checkSubscripes(char* topic, String msg);
		void publishSettings();
		void publishValues();
		void publishValues(bool force);
	private:
		String SVNh = "$Rev: 118 $";
		uint16_t cycleHT = 0;
		bool errorHTLast = false;
		uint16_t publishCountErrorHT = 0;
		int16_t temperatureLast = 0;
		uint16_t publishCountTemperature = 0;
		int16_t humidityLast = 0;
		uint16_t publishCountHumidity = 0;

		void setSubscribes();
		void publishValueTemp();
		void publishValueHum();
		void publishErrorHT();
		void calcHT();
		void calcHTError(String name);
		void publishInfoDebug(String name, String value, String publishCount);
		void checkSubscripesDebug(String topic, String value);
		void calcHTDebug(String name, int16_t value, float raw);
};
extern moduleDHT wpDHT;

#endif