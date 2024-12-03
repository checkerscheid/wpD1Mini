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
//# Revision     : $Rev:: 228                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDS18B20.h 228 2024-12-03 08:19:36Z                 $ #
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
class moduleDS18B20 {
	public:
		moduleDS18B20();
		deviceOneWire* devices[10];
		moduleBase* mb;
		uint8 Pin;
		uint8 count = 0;
		OneWire* ow;
		DallasTemperature * dt;

		// section for define

		// values
		String mqttTopicCount;
		// settings

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
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
		String scanBus();
	private:
		uint8 countLast;
		unsigned long publishCountLast;

		void publishValue();
		void calc();
		void setCount();
	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 228 $";
};
extern moduleDS18B20 wpDS18B20;

#endif