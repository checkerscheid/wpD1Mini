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
//# Revision     : $Rev:: 181                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRain.h 181 2024-07-27 23:14:47Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleRain_h
#define moduleRain_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleRain {
	public:
		moduleRain();
		moduleBase* mb;
		uint8 Pin;

		// section for define
		uint16 rain;
		int8 correction = 0;

		// values
		String mqttTopicRain;
		// settings
		String mqttTopicCorrection;

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
		bool UseAvg();
		bool UseAvg(bool useAvg);
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint8 calcCycle);
	private:
		int rainLast;
		unsigned long publishRainLast;
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16 calcAvg(uint16 raw);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 181 $";
};
extern moduleRain wpRain;

#endif