//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 13.07.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 162                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut.h 162 2024-07-13 23:52:17Z               $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleAnalogOut_h
#define moduleAnalogOut_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleAnalogOut {
	public:
		moduleAnalogOut();
		moduleBase* mb;
		uint8_t output;
		uint8_t autoValue;
		uint8_t handValue;
		bool handError;
		bool handSet = false;
		uint8_t handValueSet = false;

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
		void changeSendRest();
		void changeDebug();
		// getter / setter
		bool SendRest();
		bool SendRest(bool sendRest);
		bool Debug();
		bool Debug(bool debug);
		uint8_t MaxCycle();
		uint8_t MaxCycle(uint8_t maxCycle);
	private:
		uint8_t analogOutPin;
		uint8_t outputLast;
		uint16_t publishCountOutput;
		uint8_t autoValueLast;
		uint16_t publishCountAutoValue;
		uint8_t handValueLast;
		uint16_t publishCountHandValue;
		bool handErrorLast;
		uint16_t publishCountHandError;

		void publishValue();
		void calc();
		void printPublishValueDebug(String name, String value, String publishCount);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 162 $";
};
extern moduleAnalogOut wpAnalogOut;

#endif