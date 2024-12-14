//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 22.07.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut2.h 207 2024-10-07 12:59:22Z              $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleAnalogOut2_h
#define moduleAnalogOut2_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleAnalogOut2 {
	public:
		moduleAnalogOut2();
		moduleBase* mb;
		uint8 Pin;

		// values
		String mqttTopicOut;
		String mqttTopicAutoValue;
		String mqttTopicHandValue;
		String mqttTopicErrorHand;
		// settings
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
		// getter / setter
		bool Debug();
		bool Debug(bool debug);

		void InitHand(bool hand);

		void InitHandValue(uint8 value);
		void SetHandValue(uint8 value);
		void SetHandValueProzent(uint8 value);
		uint8 GetHandValue();

		bool GetHandError();

	private:

		// section for define
		uint8 output;
		uint8 autoValue;
		uint8 handValue;
		bool handError;
		bool handSet = false;
		uint8 handValueSet = 0;

		uint8 outputLast;
		unsigned long publishOutputLast;
		uint8 autoValueLast;
		unsigned long publishAutoValueLast;
		uint8 handValueLast;
		unsigned long publishHandValueLast;
		bool handErrorLast;
		unsigned long publishHandErrorLast;

		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 207 $";
};
extern moduleAnalogOut2 wpAnalogOut2;

#endif