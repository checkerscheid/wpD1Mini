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
//# Revision     : $Rev:: 246                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut2.h 246 2025-02-18 16:27:11Z              $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleAnalogOut2_h
#define moduleAnalogOut2_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleAnalogOut2 : public IModuleBase {
	public:
		moduleAnalogOut2();
		moduleBase* mb;
		uint8_t Pin;

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

		void InitHand(bool hand);

		void InitHandValue(uint8_t value);
		void SetHandValue(uint8_t value);
		void SetHandValueProzent(uint8_t value);
		uint8_t GetHandValue();

		bool GetHandError();

	private:

		// section for define
		uint8_t output;
		uint8_t autoValue;
		uint8_t handValue;
		bool handError;
		bool handSet = false;
		uint8_t handValueSet = 0;

		uint8_t outputLast;
		unsigned long publishOutputLast;
		uint8_t autoValueLast;
		unsigned long publishAutoValueLast;
		uint8_t handValueLast;
		unsigned long publishHandValueLast;
		bool handErrorLast;
		unsigned long publishHandErrorLast;

		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 246 $";
};
extern moduleAnalogOut2 wpAnalogOut2;

#endif