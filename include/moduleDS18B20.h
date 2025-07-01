//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 21.11.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDS18B20.h 269 2025-07-01 19:25:14Z                 $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleDS18B20_h
#define moduleDS18B20_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <deviceOneWire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/// @brief  forward declaration
class deviceOneWire;
class moduleDS18B20 : public IModuleBase {
	public:
		moduleDS18B20();
		deviceOneWire* devices[10];
		moduleBase* mb;
		uint8_t Pin;
		uint8_t count = 0;
		OneWire* ow;
		DallasTemperature * dt;

		// section for define

		// values
		String mqttTopicCount;
		// settings

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
		void changeDebug();
		String GetJsonSettings();
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		uint32_t CalcCycle();
		uint32_t CalcCycle(uint32_t calcCycle);
		String scanBus();
	private:
		uint8_t countLast;
		unsigned long publishCountLast;

		void publishValue();
		void calc();
	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 269 $";
};
extern moduleDS18B20 wpDS18B20;

#endif