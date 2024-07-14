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
//# Revision     : $Rev:: 163                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBM.h 163 2024-07-14 19:03:20Z                      $ #
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
		int bmLast;
		uint16 publishCountBM;
		void publishValue();
		void printPublishValueDebug(String name, String value, String publishCount);
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 163 $";

};
extern moduleBM wpBM;

#endif