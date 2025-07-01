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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBM.h 269 2025-07-01 19:25:14Z                      $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleBM_h
#define moduleBM_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleBM : public IModuleBase {
	public:
		moduleBM();
		moduleBase* mb;
		uint8_t Pin;

		// section for define
		bool bm;
		uint16_t threshold = 500;
		String lightToTurnOn = "_";
		String mqttTopicBM;
		String mqttTopicManual;
		String mqttTopicThreshold;
		String mqttTopicLightToTurnOn;

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
		String SetAuto();
		String SetManual();
	private:
		int bmLast;
		unsigned long publishBMLast;
		bool manual;
		bool manualLast;
		unsigned long publishManualLast;
		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 269 $";

};
extern moduleBM wpBM;

#endif