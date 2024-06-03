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
#ifndef moduleMoisture_h
#define moduleMoisture_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class moduleMoisture {
	public:
		int16_t moisture;
		bool Debug = false;
		bool useAvg = false;
		byte minValue = 30;
		uint16_t dry = 1023; // high value
		uint16_t wet = 0; // low value
		bool error;
		bool errorMin;
		uint8_t maxCycle = 5;

		// values
		String mqttTopicMoisture;
		String mqttTopicError;
		String mqttTopicErrorMin;
		// settings
		String mqttTopicMaxCycle;
		String mqttTopicUseAvg;
		String mqttTopicMin;
		String mqttTopicDry;
		String mqttTopicWet;
		// commands
		String mqttTopicDebug;

		moduleMoisture();
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
		uint8_t moisturePin;
		uint16_t cycleCounter;
		int16_t moistureLast;
		uint16_t publishCountMoisture;
		bool errorLast;
		uint16_t publishCountError;
		bool errorMinLast;
		uint16_t publishCountErrorMin;
		bool DebugLast;
		uint16_t publishCountDebug;
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void printPublishValueDebug(String name, String value, String publishCount);
};
extern moduleMoisture wpMoisture;

#endif