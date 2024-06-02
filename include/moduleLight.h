//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 29.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 124                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLight.h 124 2024-06-02 04:37:51Z                   $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleLight_h
#define moduleLight_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <Wire.h>
#include <AS_BH1750.h>

class moduleLight {
	public:
		int16_t light;
		bool useAvg;

		bool Debug;
		bool error;
		int16_t correction;
		uint8_t maxCycle;
		// values
		String mqttTopicLight;
		String mqttTopicError;
		// settings
		String mqttTopicMaxCycle;
		String mqttTopicCorrection;
		String mqttTopicUseAvg;
		// commands
		String mqttTopicDebug;
		
		static AS_BH1750 lightMeter;

		moduleLight();
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
		uint16_t cycleCounter;
		bool errorLast;
		uint16_t publishCountError;
		int16_t lightLast;
		uint16_t publishCountLight;
		bool DebugLast;
		uint16_t publishCountDebug;
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void printCalcError(String name);
		void printCalcDebug(String name, int16_t value, float raw);
		void printPublishValueDebug(String name, String value, String publishCount);
};
extern moduleLight wpLight;

#endif