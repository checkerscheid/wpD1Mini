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
//# Revision     : $Rev:: 123                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDHT.h 123 2024-06-02 04:37:07Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleDHT_h
#define moduleDHT_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <DHT.h>

#define DHTPin D7

class moduleDHT {
	public:
		int16_t temperature;
		int16_t humidity;

		bool Debug;
		bool error;
		int8_t temperatureCorrection;
		int8_t humidityCorrection;
		uint8_t maxCycle;
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
		
		DHT* dht;

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
		String SVNh = "$Rev: 123 $";
		uint16_t cycleCounter;
		bool errorLast;
		uint16_t publishCountError;
		int16_t temperatureLast;
		uint16_t publishCountTemperature;
		int16_t humidityLast;
		uint16_t publishCountHumidity;
		bool DebugLast;
		uint16_t publishCountDebug;

		void publishValueTemp();
		void publishValueHum();
		void calc();
		void printCalcError(String name);
		void printCalcDebug(String name, int16_t value, float raw);
		void printPublishValueDebug(String name, String value, String publishCount);
};
extern moduleDHT wpDHT;

#endif