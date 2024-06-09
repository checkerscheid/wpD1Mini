//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 09.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 128                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBM.h 128 2024-06-03 11:49:32Z                      $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleBase_h
#define moduleBase_h
#include <Arduino.h>

class moduleBase {
	public:
		moduleBase(String moduleName);
		bool sendRest = false;
		bool debug = false;
		bool error = false;
		uint8_t maxCycle = 5;
		uint8_t cycleCounter;
		String mqttTopicError;
		String mqttTopicSendRest;
		String mqttTopicDebug;
		String mqttTopicMaxCycle;
		void initRest(uint16_t addrSendRest, byte byteSendRest, uint8_t bitSendRest);
		void initDebug(uint16_t addrDebug, byte byteDebug, uint8_t bitDebug);
		void initError();
		void initMaxCycle(uint16_t addrMaxCycle);
		void changeSendRest();
		void changeDebug();

		void publishSettings(bool force);
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String _name;

		uint16_t _addrSendRest;
		byte _byteSendRest;
		uint8_t _bitSendRest;
		uint16_t _addrDebug;
		byte _byteDebug;
		uint8_t _bitDebug;
		uint16_t _addrMaxCycle;
		bool _useMaxCycle;
		bool _useError;

		bool sendRestLast;
		uint16_t publishCountSendRest;
		bool DebugLast;
		uint16_t publishCountDebug;
		bool errorLast;
		uint16_t publishCountError;

		void writeEEPROMsendRest();
		void writeEEPROMdebug();
		void writeEEPROMmaxCycle();
};

#endif