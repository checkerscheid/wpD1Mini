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
//# Revision     : $Rev:: 136                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleMoisture.h 136 2024-06-09 15:37:41Z                $ #
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
		int16 moisture;
		byte minValue = 30;
		uint16 dry = 1023; // high value
		uint16 wet = 0; // low value
		bool errorMin;

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
		uint8 MaxCycle();
		uint8 MaxCycle(uint8 maxCycle);
	private:
		uint8 moisturePin;
		int16 moistureLast;
		uint16 publishCountMoisture;
		bool errorMinLast;
		uint16 publishCountErrorMin;
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16 calcAvg(uint16 raw);
		void printPublishValueDebug(String name, String value, String publishCount);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 136 $";
};
extern moduleMoisture wpMoisture;

#endif