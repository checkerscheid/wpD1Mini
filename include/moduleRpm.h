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
//# Revision     : $Rev:: 182                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRpm.h 182 2024-07-28 02:12:39Z                     $ #
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
		uint8 Pin;

		// section for define
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
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
	private:
		short rpmLast;
		unsigned long publishRpmLast;
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16 calcAvg(uint16 raw);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 182 $";
};
extern moduleRpm wpRpm;

#endif