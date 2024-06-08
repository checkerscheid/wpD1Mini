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
//# Revision     : $Rev:: 126                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRelais.h 126 2024-06-03 03:11:41Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleRelais_h
#define moduleRelais_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class moduleRelais {
	public:
		moduleRelais();
		bool output;
		bool autoValue;
		bool handValue;
		bool handError;
		bool handSet = false;
		bool handValueSet = false;
		// wpModules.useModuleMoisture {
		bool waterEmptySet = false;
		bool waterEmptyError;
		uint8_t pumpActive;
		uint16_t pumpPause;
		// }

		// values
		String mqttTopicOut;
		String mqttTopicAutoValue;
		String mqttTopicHandValue;
		String mqttTopicErrorHand;
		String mqttTopicErrorWaterEmpty;
		// settings
		// wpModules.useModuleMoisture {
		String mqttTopicPumpActive;
		String mqttTopicPumpPause;
		// }
		// commands
		String mqttTopicSetHand;
		String mqttTopicSetHandValue;
		String mqttTopicSetWaterEmpty;

		// section to copy
		bool sendRest = false;
		bool Debug = false;
		bool error = false;
		uint8_t maxCycle = 5;
		String mqttTopicMaxCycle;
		String mqttTopicSendRest;
		String mqttTopicDebug;
		String mqttTopicError;
		void init();
		void cycle();
		uint16_t getVersion();

		void changeSendRest();
		void changeDebug();
		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		uint8_t relaisPin;
		bool outputLast;
		uint16_t publishCountOutput;
		bool autoValueLast;
		uint16_t publishCountAutoValue;
		bool handValueLast;
		uint16_t publishCountHandValue;
		bool handErrorLast;
		uint16_t publishCountHandError;
		uint16_t publishCountWaterEmptyError;
		// if wpModules.useMoisture
		bool pumpCycleActive;
		bool pumpStarted;
		bool pumpInPause;
		unsigned long pumpTimeStart;
		unsigned long pumpTimePause;
		// }

		void publishValue();
		void calc();
		// if wpModules.useMoisture
		void calcPump();
		// }
		void printPublishValueDebug(String name, String value, String publishCount);
		void printCalcDebug(String name, int16_t value, float raw);

		// section to config and copy
		String ModuleName;
		uint16_t addrMaxCycle;
		uint16_t addrSendRest;
		byte byteSendRest;
		uint8_t bitSendRest;
		uint16_t addrDebug;
		byte byteDebug;
		uint8_t bitDebug;
		String SVNh = "$Rev: 128 $";
		uint8_t cycleCounter;
		bool sendRestLast;
		uint16_t publishCountSendRest;
		bool DebugLast;
		uint16_t publishCountDebug;
		bool errorLast;
		uint16_t publishCountError;
		void publishDefaultSettings(bool force);
		void publishDefaultValues(bool force);
		void setDefaultSubscribes();
		void checkDefaultSubscribes(char* topic, String msg);
};
extern moduleRelais wpRelais;

#endif