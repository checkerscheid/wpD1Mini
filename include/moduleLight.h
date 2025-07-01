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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLight.h 269 2025-07-01 19:25:14Z                   $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleLight_h
#define moduleLight_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <AS_BH1750.h>
#include <Wire.h>

class moduleLight : public IModuleBase {
	public:
		moduleLight();
		moduleBase* mb;
		uint8_t PinSCL;
		uint8_t PinSDA;

		// section for define
		uint32_t light;
		short correction = 0;

		// values
		String mqttTopicLight;
		// settings
		String mqttTopicCorrection;
		
		AS_BH1750 *lightMeter;

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
		void changeDebug();
		String GetJsonSettings();
		// getter / setter
		bool UseAvg();
		bool UseAvg(bool useAvg);
		bool Debug();
		bool Debug(bool debug);
		uint32_t CalcCycle();
		uint32_t CalcCycle(uint32_t calcCycle);
	private:
		uint32_t lightLast;
		unsigned long publishLightLast;
		static const uint8_t avgLength = 128;
		uint32_t avgValues[avgLength];

		void publishValue();
		void calc();
		uint32_t calcAvg(uint32_t raw);
		void printCalcError(String name);
		void printCalcDebug(String name, uint32_t value, float raw);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 269 $";
};
extern moduleLight wpLight;

#endif