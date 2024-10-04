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
//# Revision     : $Rev:: 204                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleUnderfloor.h 204 2024-10-04 07:33:17Z              $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleUnderfloor1_h
#define moduleUnderfloor1_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleUnderfloor {
	public:
		moduleUnderfloor(uint8 n);
		moduleBase* mb;
		uint8 Pin;

		// section for define
		bool output;
		bool autoValue;
		bool handValue;
		bool handError;
		bool handSet = false;
		bool handValueSet = false;

		// values
		String mqttTopicOut;
		String mqttTopicAutoValue;
		String mqttTopicHandValue;
		String mqttTopicErrorHand;
		// settings
		String mqttTopicSetPoint;
		String mqttTopicTempUrl;
		String mqttTopicTemp = "_";
		// commands
		String mqttTopicSetHand;
		String mqttTopicSetHandValue;
		String mqttTopicSetSetPoint;
		String mqttTopicSetTempUrl;

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
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
		uint8 GetSetPoint();
		String SetHand(bool val);
		String SetHandValue(bool val);
		String SetSetPoint(uint8 setpoint);
		String SetTopicTempUrl(String topic);
	private:
		uint8 no;
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
		String tempUrlLast;
		unsigned long publishTempUrlLast;

		void publishValue();
		void calc();
		void calcOutput();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 204 $";

		uint8 bitDebug;
		uint8 bitHand;
		uint8 bitHandValue;
		uint16 byteSetpoint;
		uint16 byteCalcCycle;
		uint8 setPoint;
		uint8 temp;
};
extern moduleUnderfloor wpUnderfloor1;
extern moduleUnderfloor wpUnderfloor2;
extern moduleUnderfloor wpUnderfloor3;
extern moduleUnderfloor wpUnderfloor4;

#endif