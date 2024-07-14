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
//# Revision     : $Rev:: 144                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDHT.h 144 2024-06-18 17:20:09Z                     $ #
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
		int16 temperature;
		int16 humidity;
		int8 temperatureCorrection = 0;
		int8 humidityCorrection = 0;

		String mqttTopicTemperature;
		String mqttTopicHumidity;

		String mqttTopicTemperatureCorrection;
		String mqttTopicHumidityCorrection;
		

		// section to copy
		void init();
		void cycle();
		uint16 getVersion();

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
		uint8 MaxCycle();
		uint8 MaxCycle(uint8 maxCycle);
	private:
		uint8 DHTPin;
		int16 temperatureLast;
		uint16 publishCountTemperature;
		int16 humidityLast;
		uint16 publishCountHumidity;

		void publishValueTemp();
		void publishValueHum();
		void calc();
		void printCalcError(String name);
		void printCalcDebug(String name, int16 value, float raw);
		void printPublishValueDebug(String name, String value, String publishCount);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 144 $";
};
extern moduleDHT wpDHT;

#endif