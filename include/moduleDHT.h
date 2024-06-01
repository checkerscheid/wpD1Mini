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
//# Revision     : $Rev:: 121                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDHT.h 121 2024-06-01 05:13:59Z                     $ #
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

		bool Debug = false;
		bool error = false;
		int8_t temperatureCorrection = 0;
		int8_t humidityCorrection = 0;
		uint16_t maxCycle = 5;
		// values
		String mqttTopicTemperature;
		String mqttTopicHumidity;
		String mqttTopicError;
		// settings
		String mqttTopicMaxCycle;
		String mqttTopicTemperatureCorrection;
		String mqttTopicHumidityCorrection;
		// commands
		String mqttTopicDebug;

		moduleDHT();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 121 $";
		uint16_t cycleCounter = 0;
		bool errorLast = false;
		uint16_t publishCountError = 0;
		int16_t temperatureLast = 0;
		uint16_t publishCountTemperature = 0;
		int16_t humidityLast = 0;
		uint16_t publishCountHumidity = 0;
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;

		void publishValueTemp();
		void publishValueHum();
		void calc();
		void printCalcError(String name);
		void printCalcDebug(String name, int16_t value, float raw);
		void printPublishValueDebug(String name, String value, String publishCount);
};
extern moduleDHT wpDHT;

#endif