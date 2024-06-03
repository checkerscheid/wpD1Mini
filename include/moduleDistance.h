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
#ifndef moduleDistance_h
#define moduleDistance_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class moduleDistance {
	public:
		uint16_t volume;
		uint8_t distanceRaw = 0;
		uint8_t distanceAvg = 0;
		bool Debug = false;
		uint8_t height = 120;
		uint16_t maxVolume = 6000;
		int8_t correction = 0;
		bool error;
		uint8_t maxCycle = 5;

		// values
		String mqttTopicVolume;
		String mqttTopicdistanceRaw;
		String mqttTopicdistanceAvg;
		String mqttTopicError;
		// settings
		String mqttTopicMaxCycle;
		String mqttTopicCorrection;
		String mqttTopicHeight;
		String mqttTopicMaxVolume;
		// commands
		String mqttTopicDebug;

		moduleDistance();
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
		uint8_t trigPin;
		uint8_t echoPin;
		uint16_t cycleCounter;
		uint8_t volumeLast;
		uint16_t publishCountVolume;
		uint8_t distanceRawLast;
		uint16_t publishCountDistanceRaw;
		uint8_t distanceAvgLast;
		uint16_t publishCountDistanceAvg;
		bool errorLast;
		uint16_t publishCountError;
		bool DebugLast;
		uint16_t publishCountDebug;
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void calcDistanceDebug(String name, uint8_t avg, uint8_t raw);
		void printPublishValueDebug(String name, String value, String publishCount);
};
extern moduleDistance wpDistance;

#endif