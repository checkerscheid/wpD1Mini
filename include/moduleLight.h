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
//# Revision     : $Rev:: 132                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLight.h 132 2024-06-06 11:07:48Z                   $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleLight_h
#define moduleLight_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <AS_BH1750.h>
#include <Wire.h>

class moduleLight {
	public:
		moduleLight();
		uint16_t light;
		bool useAvg = false;
		int16_t correction = 0;

		// values
		String mqttTopicLight;
		// settings
		String mqttTopicCorrection;
		String mqttTopicUseAvg;
		
		AS_BH1750 *lightMeter;

		// section to copy
		bool sendRest = false;
		bool Debug = false;
		bool error = false;
		uint8_t maxCycle = 5;
		String mqttTopicMaxCycle;
		String mqttTopicSendRest;
		String mqttTopicDebug;
		String mqttTopicError;
		void init();
		void cycle();
		uint16_t getVersion();

		void changeSendRest();
		void changeDebug();
		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		uint16_t lightLast;
		uint16_t publishCountLight;
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void printCalcError(String name);
		void printCalcDebug(String name, int16_t value, float raw);
		void printPublishValueDebug(String name, String value, String publishCount);

		// section to config and copy
		String ModuleName;
		uint16_t addrMaxCycle;
		uint16_t addrSendRest;
		byte byteSendRest;
		uint8_t bitSendRest;
		uint16_t addrDebug;
		byte byteDebug;
		uint8_t bitDebug;
		String SVNh = "$Rev: 128 $";
		uint8_t cycleCounter;
		bool sendRestLast;
		uint16_t publishCountSendRest;
		bool DebugLast;
		uint16_t publishCountDebug;
		bool errorLast;
		uint16_t publishCountError;
		void publishDefaultSettings(bool force);
		void publishDefaultValues(bool force);
		void setDefaultSubscribes();
		void checkDefaultSubscribes(char* topic, String msg);
};
extern moduleLight wpLight;

#endif