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
//# Revision     : $Rev:: 125                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDHT.h 125 2024-06-03 03:11:11Z                     $ #
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
		moduleDHT();
		DHT* dht;

		// section for define
		int16_t temperature;
		int16_t humidity;
		int8_t temperatureCorrection = 0;
		int8_t humidityCorrection = 0;

		String mqttTopicTemperature;
		String mqttTopicHumidity;

		String mqttTopicTemperatureCorrection;
		String mqttTopicHumidityCorrection;
		

		// section to copy
		bool sendRest = false;
		bool Debug = false;
		bool error = false;
		uint8_t maxCycle = 5;
		String mqttTopicMaxCycle;
		String mqttTopicSendRest;
		String mqttTopicDebug;
		String mqttTopicError;
		void init();
		void cycle();
		uint16_t getVersion();

		void changeSendRest();
		void changeDebug();
		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		int16_t temperatureLast;
		uint16_t publishCountTemperature;
		int16_t humidityLast;
		uint16_t publishCountHumidity;

		void publishValueTemp();
		void publishValueHum();
		void calc();
		void printCalcError(String name);
		void printCalcDebug(String name, int16_t value, float raw);
		void printPublishValueDebug(String name, String value, String publishCount);


		// section to config and copy
		String ModuleName;
		uint16_t addrMaxCycle;
		uint16_t addrSendRest;
		byte byteSendRest;
		uint8_t bitSendRest;
		uint16_t addrDebug;
		byte byteDebug;
		uint8_t bitDebug;
		String SVNh = "$Rev: 128 $";
		uint8_t cycleCounter;
		bool sendRestLast;
		uint16_t publishCountSendRest;
		bool DebugLast;
		uint16_t publishCountDebug;
		bool errorLast;
		uint16_t publishCountError;
		void publishDefaultSettings(bool force);
		void publishDefaultValues(bool force);
		void setDefaultSubscribes();
		void checkDefaultSubscribes(char* topic, String msg);
};
extern moduleDHT wpDHT;

#endif