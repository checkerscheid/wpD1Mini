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
//# Revision     : $Rev:: 132                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDistance.h 132 2024-06-06 11:07:48Z                $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleDistance_h
#define moduleDistance_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class moduleDistance {
	public:
		moduleDistance();
		uint16_t volume;
		uint16_t distanceRaw;
		uint16_t distanceAvg;

		uint8_t height = 120;
		uint16_t maxVolume = 6000;
		int8_t correction = 0;

		// values
		String mqttTopicVolume;
		String mqttTopicDistanceRaw;
		String mqttTopicDistanceAvg;
		// settings
		String mqttTopicCorrection;
		String mqttTopicHeight;
		String mqttTopicMaxVolume;


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
		uint8_t trigPin;
		uint8_t echoPin;
		uint16_t volumeLast;
		uint16_t publishCountVolume;
		uint16_t distanceRawLast;
		uint16_t publishCountDistanceRaw;
		uint16_t distanceAvgLast;
		uint16_t publishCountDistanceAvg;
		
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void publishDistanceRaw();
		void publishDistanceAvg();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void calcDistanceDebug(String name, uint16_t avg, uint16_t raw);
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
extern moduleDistance wpDistance;

#endif