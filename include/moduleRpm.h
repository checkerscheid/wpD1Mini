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
//# Revision     : $Rev:: 164                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRpm.h 164 2024-07-14 19:05:04Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleRpm_h
#define moduleRpm_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleRpm {
	public:
		moduleRpm();
		moduleBase* mb;
		uint16 rpm;
		int8 correction = 0;

		// values
		String mqttTopicRpm;
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
		uint8 MaxCycle();
		uint8 MaxCycle(uint8 maxCycle);
	private:
		uint8 RpmPin;
		short rpmLast;
		uint16 publishCountRpm;
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16 calcAvg(uint16 raw);
		void printPublishValueDebug(String name, String value, String publishCount);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 164 $";
};
extern moduleRpm wpRpm;

#endif