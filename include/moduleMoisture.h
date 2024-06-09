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
#ifndef moduleMoisture_h
#define moduleMoisture_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleMoisture {
	public:
		moduleMoisture();
		moduleBase* mb;
		int16_t moisture;
		byte minValue = 30;
		uint16_t dry = 1023; // high value
		uint16_t wet = 0; // low value
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
		uint16_t getVersion();

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
		uint8_t MaxCycle();
		uint8_t MaxCycle(uint8_t maxCycle);
	private:
		uint8_t moisturePin;
		int16_t moistureLast;
		uint16_t publishCountMoisture;
		bool errorMinLast;
		uint16_t publishCountErrorMin;
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void printPublishValueDebug(String name, String value, String publishCount);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 128 $";
};
extern moduleMoisture wpMoisture;

#endif