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
//# Revision     : $Rev:: 253                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleWeight.h 253 2025-03-17 19:29:41Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleWeight_h
#define moduleWeight_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <HX711.h>

class moduleWeight : public IModuleBase {
	public:
		moduleWeight();
		moduleBase* mb;
		HX711* scale;
		uint8_t Pinout;
		uint8_t Pin;

		// section for define
		long weight;
		long tareValue = 50000;
		long tare1kg = 200000;

		// values
		String mqttTopicWeight;
		String mqttTopicTareValue;
		String mqttTopicTare1kg;
		// settings
		String mqttTopicSetTare;
		String mqttTopicSet1kg;

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
		void InitTareValue(uint32_t tv);
		void SetTare();
		void InitTare1kg(uint32_t t1kg);
		void Set1kg();
		// getter / setter
		bool UseAvg();
		bool UseAvg(bool useAvg);
		bool Debug();
		bool Debug(bool debug);
		uint32_t CalcCycle();
		uint32_t CalcCycle(uint32_t calcCycle);
	private:
		long weightLast;
		unsigned long publishWeightLast;
		long tareValueLast;
		unsigned long publishTareValueLast;
		long tare1kgLast;
		unsigned long publishTare1kgLast;
		static const uint8_t avgLength = 128;
		long avgValues[avgLength];
		bool makeTare;
		bool make1kg;

		void publishValue();
		void calc();
		long calcAvg(long raw);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 253 $";
};
extern moduleWeight wpWeight;

#endif