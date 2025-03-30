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
//# Revision     : $Rev:: 246                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDistance.h 246 2025-02-18 16:27:11Z                $ #
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
		uint8 PinTrig;
		uint8 PinEcho;

		// section for define
		uint16 volume;
		uint16 distanceRaw;
		uint16 distanceAvg;

		uint8 height = 120;
		uint16 maxVolume = 6000;
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
		uint16 getVersion();

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
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
	private:
		uint16 volumeLast;
		unsigned long publishVolumeLast;
		uint16 distanceRawLast;
		unsigned long publishDistanceRawLast;
		uint16 distanceAvgLast;
		unsigned long publishDistanceAvgLast;
		
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void publishDistanceRaw();
		void publishDistanceAvg();
		void calc();
		uint16 calcAvg(uint16 raw);
		void calcDistanceDebug(String name, uint16 avg, uint16 raw, unsigned long duration);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 246 $";
};
extern moduleDistance wpDistance;

#endif