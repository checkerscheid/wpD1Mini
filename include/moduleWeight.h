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
//# Revision     : $Rev:: 220                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleWeight.h 220 2024-10-29 10:37:04Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleWeight_h
#define moduleWeight_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <HX711.h>

class moduleWeight {
	public:
		moduleWeight();
		moduleBase* mb;
		HX711* scale;
		uint8 Pinout;
		uint8 Pin;

		// section for define
		long weight;
		long tareValue = 50000;

		// values
		String mqttTopicWeight;
		String mqttTopicTareValue;
		// settings
		String mqttTopicSetTare;

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
		void changeDebug();
		void InitTareValue(uint32 tareValue);
		void SetTare();
		// getter / setter
		bool UseAvg();
		bool UseAvg(bool useAvg);
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
	private:
		long weightLast;
		unsigned long publishWeightLast;
		long tareValueLast;
		unsigned long publishTareValueLast;
		static const uint8 avgLength = 128;
		long avgValues[avgLength];
		bool tare;

		void publishValue();
		void calc();
		long calcAvg(long raw);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 220 $";
};
extern moduleWeight wpWeight;

#endif