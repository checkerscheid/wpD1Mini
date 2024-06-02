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
//# Revision     : $Rev:: 124                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLight.h 124 2024-06-02 04:37:51Z                   $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleRelais_h
#define moduleRelais_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class moduleRelais {
	public:
		bool output;
		bool autoValue;
		bool handValue;
		bool handError;
		bool handSet;
		bool handValueSet;
		// wpModules.useModuleMoisture {
		bool waterEmpty;
		uint8_t pumpActive;
		uint16_t pumpPause;
		// }
		bool Debug;

		// values
		String mqttTopicOut;
		String mqttTopicAutoValue;
		String mqttTopicHandValue;
		String mqttTopicErrorHand;
		// settings
		// wpModules.useModuleMoisture {
		String mqttTopicWaterEmpty;
		String mqttTopicPumpActive;
		String mqttTopicPumpPause;
		// }
		// commands
		String mqttTopicSetHand;
		String mqttTopicSetHandValue;
		String mqttTopicDebug;

		moduleRelais();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 124 $";
		uint8_t relaisPin;
		bool outputLast;
		uint16_t publishCountOutput;
		bool autoValueLast;
		uint16_t publishCountAutoValue;
		bool handValueLast;
		uint16_t publishCountHandValue;
		bool handErrorLast;
		uint16_t publishCountHandError;
		bool DebugLast;
		uint16_t publishCountDebug;
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
};
extern moduleRelais wpRelais;

#endif