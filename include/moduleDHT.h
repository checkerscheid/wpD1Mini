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
//# Revision     : $Rev:: 232                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDHT.h 232 2024-12-19 15:27:48Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleDHT_h
#define moduleDHT_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <DHT.h>

class moduleDHT : public IModuleBase {
	public:
		moduleDHT();
		moduleBase* mb;
		DHT* dht;
		uint8 Pin;

		// section for define
		int temperature;
		int humidity;
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
		void changeDebug();
		String GetJsonSettings();
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
	private:
		int temperatureLast;
		unsigned long publishTemperatureLast;
		int humidityLast;
		unsigned long publishHumidityLast;

		void publishValueTemp();
		void publishValueHum();
		void calc();
		void printCalcError(String name);
		void printCalcDebug(String name, int value, float raw);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 232 $";
};
extern moduleDHT wpDHT;

#endif