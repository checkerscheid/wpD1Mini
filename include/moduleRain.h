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
#ifndef moduleRain_h
#define moduleRain_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class moduleRain {
	public:
		uint16_t rain;
		bool Debug = false;
		bool useAvg = false;
		int8_t correction = 0;
		bool error;
		uint8_t maxCycle = 5;

		// values
		String mqttTopicRain;
		String mqttTopicError;
		// settings
		String mqttTopicMaxCycle;
		String mqttTopicCorrection;
		String mqttTopicUseAvg;
		// commands
		String mqttTopicDebug;

		moduleRain();
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
		uint8_t RainPin;
		uint16_t cycleCounter;
		int16_t rainLast;
		uint16_t publishCountRain;
		bool errorLast;
		uint16_t publishCountError;
		bool DebugLast;
		uint16_t publishCountDebug;
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void printPublishValueDebug(String name, String value, String publishCount);
};
extern moduleRain wpRain;

#endif