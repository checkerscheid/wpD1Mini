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
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLight.h 207 2024-10-07 12:59:22Z                   $ #
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
		uint8 PinSCL;
		uint8 PinSDA;

		// section for define
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
		void changeDebug();
		// getter / setter
		bool UseAvg();
		bool UseAvg(bool useAvg);
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
	private:
		uint32 lightLast;
		unsigned long publishLightLast;
		static const uint8 avgLength = 128;
		uint32 avgValues[avgLength];

		void publishValue();
		void calc();
		uint32 calcAvg(uint32 raw);
		void printCalcError(String name);
		void printCalcDebug(String name, uint32 value, float raw);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 207 $";
};
extern moduleLight wpLight;

#endif