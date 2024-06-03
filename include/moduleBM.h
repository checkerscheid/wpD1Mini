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
//# File-ID      : $Id:: moduleMoisture.h 126 2024-06-03 03:11:41Z                $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleBM_h
#define moduleBM_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class moduleBM {
	public:
		bool bm;
		bool Debug = false;
		uint16_t threshold = 500;
		String lightToTurnOn = "_";
		bool error;
		uint8_t maxCycle = 5;

		// values
		String mqttTopicBM;
		// settings
		String mqttTopicThreshold;
		String mqttTopicLightToTurnOn;
		// commands
		String mqttTopicDebug;

		moduleBM();
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
		String SVNh = "$Rev: 126 $";
		uint8_t BMPin;
		int16_t bmLast;
		uint16_t publishCountBM;
		bool DebugLast;
		uint16_t publishCountDebug;

		void publishValue();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void printPublishValueDebug(String name, String value, String publishCount);
};
extern moduleBM wpBM;

#endif