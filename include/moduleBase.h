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
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBase.h 207 2024-10-07 12:59:22Z                    $ #
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
		uint32 calcCycle = 1000 * 5;
		unsigned long calcLast;

		String mqttTopicUseAvg;
		String mqttTopicDebug;
		String mqttTopicError;
		String mqttTopicCalcCycle;
		void initUseAvg(uint16 addrUseAvg, byte& byteUseAvg, uint8 bitUseAvg);
		void initDebug(uint16 addrDebug, byte& byteDebug, uint8 bitDebug);
		void initError();
		void initCalcCycle(uint16 addrCalcCycle);
		void changeDebug();

		void publishSettings(bool force);
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
		void printPublishValueDebug(String name, String value);
	private:
		String _name;

		uint16 _addrUseAvg;
		byte _byteUseAvg;
		uint8 _bitUseAvg;
		uint16 _addrDebug;
		byte _byteDebug;
		uint8 _bitDebug;
		uint16 _addrCalcCycle;
		bool _useUseAvg;
		bool _useCalcCycle;
		bool _useError;

		bool DebugLast;
		uint32 publishDebugLast;
		bool errorLast;
		uint32 publishErrorLast;

		void writeEEPROMuseAvg();
		void writeEEPROMdebug();
		void writeEEPROMCalcCycle();
};

#endif