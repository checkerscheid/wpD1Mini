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
//# File-ID      : $Id:: moduleDistance.h 269 2025-07-01 19:25:14Z                $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleDistance_h
#define moduleDistance_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleDistance : public IModuleBase {
	public:
		moduleDistance();
		moduleBase* mb;
		uint8_t PinTrig;
		uint8_t PinEcho;

		// section for define
		uint16_t volume;
		uint16_t distanceRaw;
		uint16_t distanceAvg;

		uint8_t height = 120;
		uint16_t maxVolume = 6000;
		int8 correction = 0;

		// values
		String mqttTopicVolume;
		String mqttTopicDistanceRaw;
		String mqttTopicDistanceAvg;
		// settings
		String mqttTopicCorrection;
		String mqttTopicHeight;
		String mqttTopicMaxVolume;


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
		bool Debug();
		bool Debug(bool debug);
		uint32_t CalcCycle();
		uint32_t CalcCycle(uint32_t calcCycle);
	private:
		uint16_t volumeLast;
		unsigned long publishVolumeLast;
		uint16_t distanceRawLast;
		unsigned long publishDistanceRawLast;
		uint16_t distanceAvgLast;
		unsigned long publishDistanceAvgLast;
		
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void publishDistanceRaw();
		void publishDistanceAvg();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void calcDistanceDebug(String name, uint16_t avg, uint16_t raw, unsigned long duration);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 269 $";
};
extern moduleDistance wpDistance;

#endif