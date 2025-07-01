//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 02.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 258                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleImpulseCounter.h 258 2025-04-28 13:34:51Z          $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleImpulseCounter_h
#define moduleImpulseCounter_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleImpulseCounter : public IModuleBase {
	public:
		moduleImpulseCounter();
		moduleBase* mb;
		uint8_t Pin;

		// section for define
		bool bm;
		String mqttTopicCounter;
		String mqttTopicKWh;
		// settings
		String mqttTopicSetKWh;
		String mqttTopicSilver;
		String mqttTopicRed;
		String mqttTopicUpKWh;
		uint32_t impulseCounter = 0;
		uint32_t KWh = 0;
		uint16_t counterSilver = 50;
		uint16_t counterRed = 255;
		uint8_t UpKWh = 150;

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
	private:
		bool redIsNow;
		bool redIsNowLast;
		unsigned long impulseCounterLast;
		unsigned long publishKWhLast;
		unsigned long KWhLast = 0;
		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 258 $";

};
extern moduleImpulseCounter wpImpulseCounter;

#endif