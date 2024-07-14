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
//# Revision     : $Rev:: 136                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBM.h 136 2024-06-09 15:37:41Z                      $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleBM_h
#define moduleBM_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleBM {
	public:
		moduleBM();
		moduleBase* mb;

		// section for define
		bool bm;
		uint16 threshold = 500;
		String lightToTurnOn = "_";
		String mqttTopicBM;
		String mqttTopicThreshold;
		String mqttTopicLightToTurnOn;

		// section to copy
		uint8 maxCycle = 5;
		uint8 cycleCounter = 0;
		String mqttTopicMaxCycle;
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
		uint8 BMPin;
		int16 bmLast;
		uint16 publishCountBM;
		void publishValue();
		void printPublishValueDebug(String name, String value, String publishCount);
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 136 $";

};
extern moduleBM wpBM;

#endif