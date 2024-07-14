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
//# Revision     : $Rev:: 163                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDistance.h 163 2024-07-14 19:03:20Z                $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleDistance_h
#define moduleDistance_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleDistance {
	public:
		moduleDistance();
		moduleBase* mb;
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
		void changeSendRest();
		void changeDebug();
		// getter / setter
		bool SendRest();
		bool SendRest(bool sendRest);
		bool Debug();
		bool Debug(bool debug);
		uint8 MaxCycle();
		uint8 MaxCycle(uint8 maxCycle);
	private:
		uint8 trigPin;
		uint8 echoPin;
		uint16 volumeLast;
		uint16 publishCountVolume;
		uint16 distanceRawLast;
		uint16 publishCountDistanceRaw;
		uint16 distanceAvgLast;
		uint16 publishCountDistanceAvg;
		
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void publishDistanceRaw();
		void publishDistanceAvg();
		void calc();
		uint16 calcAvg(uint16 raw);
		void calcDistanceDebug(String name, uint16 avg, uint16 raw);
		void printPublishValueDebug(String name, String value, String publishCount);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 163 $";
};
extern moduleDistance wpDistance;

#endif