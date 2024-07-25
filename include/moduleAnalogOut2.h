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
//# Revision     : $Rev:: 177                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut2.h 177 2024-07-25 17:36:45Z              $ #
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

		// section for define
		uint8 output;
		uint hardwareoutMax;
		uint8 autoValue;
		uint8 handValue;
		bool handError;
		bool handSet = false;
		uint8 handValueSet = false;

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
		uint8 outputLast;
		uint16 publishCountOutput;
		uint8 autoValueLast;
		uint16 publishCountAutoValue;
		uint8 handValueLast;
		uint16 publishCountHandValue;
		bool handErrorLast;
		uint16 publishCountHandError;

		void publishValue();
		void calc();
		void printPublishValueDebug(String name, String value, String publishCount);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 177 $";
};
extern moduleAnalogOut2 wpAnalogOut2;

#endif