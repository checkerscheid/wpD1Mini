//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 29.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 163                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLight.h 163 2024-07-14 19:03:20Z                   $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleLight_h
#define moduleLight_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <AS_BH1750.h>
#include <Wire.h>

class moduleLight {
	public:
		moduleLight();
		moduleBase* mb;
		uint32 light;
		int correction = 0;

		// values
		String mqttTopicLight;
		// settings
		String mqttTopicCorrection;
		
		AS_BH1750 *lightMeter;

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
		uint32 lightLast;
		uint16 publishCountLight;
		static const uint8 avgLength = 128;
		uint32 avgValues[avgLength];

		void publishValue();
		void calc();
		uint32 calcAvg(uint32 raw);
		void printCalcError(String name);
		void printCalcDebug(String name, uint32 value, float raw);
		void printPublishValueDebug(String name, String value, String publishCount);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 163 $";
};
extern moduleLight wpLight;

#endif