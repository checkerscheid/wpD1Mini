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
//# Revision     : $Rev:: 181                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut2.h 181 2024-07-27 23:14:47Z              $ #
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
	private:
		uint8 outputLast;
		uint32 publishForceOutput;
		uint8 autoValueLast;
		uint32 publishForceAutoValue;
		uint8 handValueLast;
		uint32 publishForceHandValue;
		bool handErrorLast;
		uint32 publishForceHandError;

		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 181 $";
};
extern moduleAnalogOut2 wpAnalogOut2;

#endif