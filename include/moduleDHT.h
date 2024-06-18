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
//# Revision     : $Rev:: 136                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDHT.h 136 2024-06-09 15:37:41Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleDHT_h
#define moduleDHT_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <DHT.h>

class moduleDHT {
	public:
		moduleDHT();
		moduleBase* mb;
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
		void init();
		void cycle();
		uint16_t getVersion();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
		void changeSendRest();
		void changeDebug();
		// getter / setter
		bool SendRest();
		bool SendRest(bool sendRest);
		bool Debug();
		bool Debug(bool debug);
		uint8_t MaxCycle();
		uint8_t MaxCycle(uint8_t maxCycle);
	private:
		uint8_t DHTPin;
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
		String SVNh = "$Rev: 136 $";
};
extern moduleDHT wpDHT;

#endif