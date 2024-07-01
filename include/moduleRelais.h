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
//# Revision     : $Rev:: 151                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRelais.h 151 2024-07-01 20:25:07Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleRelais_h
#define moduleRelais_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleRelais {
	public:
		moduleRelais();
		moduleBase* mb;
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
		String SVNh = "$Rev: 151 $";
};
extern moduleRelais wpRelais;

#endif