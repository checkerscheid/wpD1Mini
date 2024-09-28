//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 21.09.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 189                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut2.h 189 2024-08-13 11:58:56Z              $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleUnderfloor_h
#define moduleUnderfloor_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleUnderfloor {
	public:
		moduleUnderfloor(uint8 no);
		moduleBase* mb;
		uint8 Pin;

		// section for define
		bool output;
		bool autoValue;
		bool handValue;
		bool handError;
		bool handSet = false;
		bool handValueSet = false;
		uint8 SetPoint = 190;

		// values
		String mqttTopicOut;
		String mqttTopicAutoValue;
		String mqttTopicHandValue;
		String mqttTopicErrorHand;
		// settings
		String mqttTopicSetPoint;
		String mqttTopicTemp = "_";
		// commands
		String mqttTopicSetHand;
		String mqttTopicSetHandValue;

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
		void InitSetPoint(uint8 setpoint);
		String SetTopicTemp(String topic);
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		void SetHandValueSet(bool val);
	private:
		uint8 valve;
		bool outputLast;
		unsigned long publishOutputLast;
		bool autoValueLast;
		unsigned long publishAutoValueLast;
		bool handValueLast;
		unsigned long publishHandValueLast;
		bool handErrorLast;
		unsigned long publishHandErrorLast;
		uint8 setPointLast;
		unsigned long publishSetPointLast;

		void publishValue();
		void calc();
		void calcOutput();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 189 $";

		uint8 bitDebug;
		uint8 bitHand;
		uint8 bitHandValue;
		uint8 setPoint;
		uint8 temp;
};
extern moduleUnderfloor wpUnderfloor1;
extern moduleUnderfloor wpUnderfloor2;
extern moduleUnderfloor wpUnderfloor3;
extern moduleUnderfloor wpUnderfloor4;

#endif