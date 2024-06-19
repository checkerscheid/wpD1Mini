//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 18.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 145                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleFK.h 145 2024-06-18 17:20:41Z                      $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleFK_h
#define moduleFK_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleFK {
	public:
		moduleFK();
		moduleBase* mb;

		// section for define
		bool bm;
		uint16_t threshold = 500;
		String lightToTurnOn = "_";
		String mqttTopicBM;
		String mqttTopicThreshold;
		String mqttTopicLightToTurnOn;

		// section to copy
		uint8_t maxCycle = 5;
		uint8_t cycleCounter = 0;
		String mqttTopicMaxCycle;
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
		uint8_t BMPin;
		int16_t bmLast;
		uint16_t publishCountBM;
		void publishValue();
		void printPublishValueDebug(String name, String value, String publishCount);
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 145 $";

};
extern moduleFK wpFK;

#endif
