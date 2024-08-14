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
//# Revision     : $Rev:: 191                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleImpulseCounter.h 191 2024-08-14 02:36:26Z          $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleImpulseCounter_h
#define moduleImpulseCounter_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleImpulseCounter {
	public:
		moduleImpulseCounter();
		moduleBase* mb;
		uint8 Pin;

		// section for define
		bool bm;
		String mqttTopicCounter;
		// settings
		String mqttTopicSetCounter;
		String mqttTopicSilver;
		String mqttTopicRed;
		unsigned long impulseCounter = 0;
		uint16 counterSilver = 50;
		uint16 counterRed = 255;

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
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
	private:
		bool redIsNow;
		bool redIsNowLast;
		unsigned long publishCounterLast;
		unsigned long impulseCounterLast = 0;
		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 191 $";

};
extern moduleImpulseCounter wpImpulseCounter;

#endif