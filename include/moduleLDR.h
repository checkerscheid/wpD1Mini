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
		int16_t ldr;
		int8_t correction = 0;

		// values
		String mqttTopicLdr;
		// settings
		String mqttTopicCorrection;

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
		uint8_t LDRPin;
		int16_t ldrLast;
		uint16_t publishCountLdr;
		static const uint8_t avgLength = 128;
		int avgValues[avgLength];

		void publishValue();
		void calc();
		uint16_t calcAvg(uint16_t raw);
		void printPublishValueDebug(String name, String value, String publishCount);

	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 139 $";
};
extern moduleLDR wpLDR;

#endif