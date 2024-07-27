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
//# File-ID      : $Id:: moduleMoisture.h 181 2024-07-27 23:14:47Z                $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleMoisture_h
#define moduleMoisture_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleMoisture {
	public:
		moduleMoisture();
		moduleBase* mb;
		uint8 Pin;

		// section for define
		int moisture;
		bool errorMin;
		byte minValue = 30;
		uint16 dry = 1023; // high value
		uint16 wet = 0; // low value

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
		int moistureLast;
		unsigned long publishMoistureLast;
		bool errorMinLast;
		unsigned long publishErrorMinLast;
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16 calcAvg(uint16 raw);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 181 $";
};
extern moduleMoisture wpMoisture;

#endif