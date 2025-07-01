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
//# Revision     : $Rev:: 246                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRelais.h 246 2025-02-18 16:27:11Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleRelais_h
#define moduleRelais_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleRelais : public IModuleBase {
	public:
		moduleRelais();
		moduleBase* mb;
		uint8_t Pin;

		// section for define
		bool output;
		bool autoValue;
		bool handValue;
		bool handError;
		bool handSet = false;
		bool handValueSet = false;
		// wpModules.useModuleMoisture {
		bool waterEmptySet = false;
		bool waterEmptyError;
		uint8_t pumpActive; // in seconds
		uint16_t pumpPause; // show in minutes, save in seconds
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
		String mqttTopicStartPumpTest;

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
	private:
		bool outputLast;
		unsigned long publishOutputLast;
		bool autoValueLast;
		unsigned long publishAutoValueLast;
		bool handValueLast;
		unsigned long publishHandValueLast;
		bool handErrorLast;
		unsigned long publishHandErrorLast;
		unsigned long publishWaterEmptyErrorLast;
		// if wpModules.useMoisture
		bool pumpCycleActive;
		bool pumpStarted;
		bool pumpInPause;
		unsigned long pumpTimeStart;
		unsigned long pumpTimePause;

		uint8_t debugCalcPumpCounter;
		unsigned long remainPumpTimePause;
		// }
		bool startPumpTestActive;
		unsigned long startPumpTestAt;

		void publishValue();
		void calc();
		// if wpModules.useMoisture
		void calcPump();
		// }
		void printCalcDebug(String name, int value, float raw);
		void SendPumpStatus();
		String getReadableTime(unsigned long time);
		void StartPumpTest();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 246 $";
};
extern moduleRelais wpRelais;

#endif