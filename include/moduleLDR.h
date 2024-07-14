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
//# Revision     : $Rev:: 139                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLDR.h 139 2024-06-11 10:08:54Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleLDR_h
#define moduleLDR_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleLDR {
	public:
		moduleLDR();
		moduleBase* mb;
		int16 ldr;
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
		uint8 LDRPin;
		int16 ldrLast;
		uint16 publishCountLdr;
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16 calcAvg(uint16 raw);
		void printPublishValueDebug(String name, String value, String publishCount);

	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 139 $";
};
extern moduleLDR wpLDR;

#endif