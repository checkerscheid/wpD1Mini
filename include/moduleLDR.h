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
//# Revision     : $Rev:: 128                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLDR.h 128 2024-06-03 11:49:32Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleLDR_h
#define moduleLDR_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class moduleLDR {
	public:
		int16_t LDR;
		bool Debug = false;
		bool useAvg = false;
		int8_t correction = 0;
		bool error;
		uint8_t maxCycle = 5;

		// values
		String mqttTopicLDR;
		String mqttTopicError;
		// settings
		String mqttTopicMaxCycle;
		String mqttTopicCorrection;
		String mqttTopicUseAvg;
		// commands
		String mqttTopicDebug;

		moduleLDR();
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
		String SVNh = "$Rev: 128 $";
		uint8_t LDRPin;
		uint16_t cycleCounter;
		int16_t LDRLast;
		uint16_t publishCountLDR;
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
extern moduleLDR wpLDR;

#endif