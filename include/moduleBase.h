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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBase.h 269 2025-07-01 19:25:14Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleBase_h
#define moduleBase_h
#include <Arduino.h>

class moduleBase {
	public:
		moduleBase(String moduleName);
		bool useAvg = false;
		bool debug = false;
		bool error = false;
		uint32_t calcCycle = 1000 * 5;
		unsigned long calcLast;

		String mqttTopicUseAvg;
		String mqttTopicDebug;
		String mqttTopicError;
		String mqttTopicCalcCycle;
		void initUseAvg(uint16_t addrUseAvg, byte& byteUseAvg, uint8_t bitUseAvg);
		void initDebug(uint16_t addrDebug, byte& byteDebug, uint8_t bitDebug);
		void initError();
		void initCalcCycle(uint16_t addrCalcCycle);
		void changeDebug();

		void publishSettings(bool force);
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
		void printPublishValueDebug(String name, String value);
	private:
		String _name;

		uint16_t _addrUseAvg;
		byte _byteUseAvg;
		uint8_t _bitUseAvg;
		uint16_t _addrDebug;
		byte _byteDebug;
		uint8_t _bitDebug;
		uint16_t _addrCalcCycle;
		bool _useUseAvg;
		bool _useCalcCycle;
		bool _useError;

		bool DebugLast;
		unsigned long publishDebugLast;
		bool errorLast;
		unsigned long publishErrorLast;

		void writeEEPROMuseAvg();
		void writeEEPROMdebug();
		void writeEEPROMCalcCycle();
};

#endif