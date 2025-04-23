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
//# Revision     : $Rev:: 246                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLDR.h 246 2025-02-18 16:27:11Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleLDR_h
#define moduleLDR_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleLDR : public IModuleBase {
	public:
		moduleLDR();
		moduleBase* mb;
		uint8 Pin;

		// section for define
		int ldr;
		int8 correction = 0;

		// values
		String mqttTopicLdr;
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
		void changeDebug();
		String GetJsonSettings();
		// getter / setter
		bool UseAvg();
		bool UseAvg(bool useAvg);
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
	private:
		int ldrLast;
		unsigned long publishLdrLast;
		const uint16 publishLdrMin = 2000;
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16 calcAvg(uint16 raw);

	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 246 $";
};
extern moduleLDR wpLDR;

#endif