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
//# Revision     : $Rev:: 128                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBM.h 128 2024-06-03 11:49:32Z                      $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleBM_h
#define moduleBM_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class moduleBM {
	public:
		moduleBM();

		// section for define
		bool bm;
		uint16_t threshold = 500;
		String lightToTurnOn = "_";
		String mqttTopicBM;
		String mqttTopicThreshold;
		String mqttTopicLightToTurnOn;

		// section to copy
		bool sendRest = false;
		bool Debug = false;
		bool error = false;
		uint8_t maxCycle = 5;
		uint8_t cycleCounter = 0;
		String mqttTopicSendRest;
		String mqttTopicDebug;
		String mqttTopicError;
		String mqttTopicMaxCycle;
		void init();
		void cycle();
		uint16_t getVersion();

		void changeSendRest();
		void changeDebug();
		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		uint8_t BMPin;
		int16_t bmLast;
		uint16_t publishCountBM;
		void publishValue();
		void printPublishValueDebug(String name, String value, String publishCount);
		void calc();

		// section to config and copy
		String ModuleName;
		uint16_t addrSendRest;
		byte byteSendRest;
		uint8_t bitSendRest;
		uint16_t addrDebug;
		byte byteDebug;
		uint8_t bitDebug;
		String SVNh = "$Rev: 128 $";
		bool sendRestLast;
		uint16_t publishCountSendRest;
		bool DebugLast;
		uint16_t publishCountDebug;
		bool errorLast;
		uint16_t publishCountError;
		void publishDefaultSettings(bool force);
		void publishDefaultValues(bool force);
		void setDefaultSubscribes();
		void checkDefaultSubscribes(char* topic, String msg);

};
extern moduleBM wpBM;

#endif