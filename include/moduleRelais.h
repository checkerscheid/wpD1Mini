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
//# Revision     : $Rev:: 181                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRelais.h 181 2024-07-27 23:14:47Z                  $ #
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
		uint8 Pin;

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
		uint8 pumpActive; // in seconds
		uint16 pumpPause; // show in minutes, save in seconds
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
		bool outputLast;
		uint32 publishForceOutput;
		bool autoValueLast;
		uint32 publishForceAutoValue;
		bool handValueLast;
		uint32 publishForceHandValue;
		bool handErrorLast;
		uint32 publishForceHandError;
		uint32 publishForceWaterEmptyError;
		// if wpModules.useMoisture
		bool pumpCycleActive;
		bool pumpStarted;
		bool pumpInPause;
		unsigned long pumpTimeStart;
		unsigned long pumpTimePause;

		uint8 debugCalcPumpCounter;
		unsigned long remainPumpTimePause;
		// }

		void publishValue();
		void calc();
		// if wpModules.useMoisture
		void calcPump();
		// }
		void printCalcDebug(String name, int value, float raw);
		void SendPumpStatus();
		String getReadableTime(unsigned long time);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 181 $";
};
extern moduleRelais wpRelais;

#endif