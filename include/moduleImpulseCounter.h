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
		uint8 Pin;

		// section for define
		bool bm;
		String mqttTopicCounter;
		String mqttTopicKWh;
		// settings
		String mqttTopicSetKWh;
		String mqttTopicSilver;
		String mqttTopicRed;
		String mqttTopicUpKWh;
		uint32 impulseCounter = 0;
		uint32 KWh = 0;
		uint16 counterSilver = 50;
		uint16 counterRed = 255;
		uint8 UpKWh = 150;

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