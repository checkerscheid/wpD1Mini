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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleMoisture.h 269 2025-07-01 19:25:14Z                $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleMoisture_h
#define moduleMoisture_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleMoisture : public IModuleBase {
	public:
		moduleMoisture();
		moduleBase* mb;
		uint8_t Pin;

		// section for define
		int moisture;
		bool errorMin;
		byte minValue = 30;
		uint16_t dry = 1023; // high value
		uint16_t wet = 0; // low value

		// values
		String mqttTopicMoisture;
		String mqttTopicErrorMin;
		// settings
		String mqttTopicMin;
		String mqttTopicDry;
		String mqttTopicWet;

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
		// getter / setter
		bool UseAvg();
		bool UseAvg(bool useAvg);
		bool Debug();
		bool Debug(bool debug);
		uint32_t CalcCycle();
		uint32_t CalcCycle(uint32_t calcCycle);
	private:
		int moistureLast;
		unsigned long publishMoistureLast;
		bool errorMinLast;
		unsigned long publishErrorMinLast;
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16_t calcAvg(uint16_t raw);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 269 $";
};
extern moduleMoisture wpMoisture;

#endif