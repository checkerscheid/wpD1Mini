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
//# Revision     : $Rev:: 137                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBase.h 137 2024-06-09 15:38:30Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleBase_h
#define moduleBase_h
#include <Arduino.h>

class moduleBase {
	public:
		moduleBase(String moduleName);
		bool sendRest = false;
		bool useAvg = false;
		bool debug = false;
		bool error = false;

		uint8 maxCycle = 5;
		uint8 cycleCounter;
		String mqttTopicSendRest;
		String mqttTopicUseAvg;
		String mqttTopicDebug;
		String mqttTopicError;
		String mqttTopicMaxCycle;
		void initRest(uint16 addrSendRest, byte byteSendRest, uint8 bitSendRest);
		void initUseAvg(uint16 addrUseAvg, byte byteUseAvg, uint8 bitUseAvg);
		void initDebug(uint16 addrDebug, byte byteDebug, uint8 bitDebug);
		void initError();
		void initMaxCycle(uint16 addrMaxCycle);
		void changeSendRest();
		void changeDebug();

		void publishSettings(bool force);
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String _name;

		uint16 _addrSendRest;
		byte _byteSendRest;
		uint8 _bitSendRest;
		uint16 _addrUseAvg;
		byte _byteUseAvg;
		uint8 _bitUseAvg;
		uint16 _addrDebug;
		byte _byteDebug;
		uint8 _bitDebug;
		uint16 _addrMaxCycle;
		bool _useUseAvg;
		bool _useMaxCycle;
		bool _useError;

		bool sendRestLast;
		uint16 publishCountSendRest;
		bool DebugLast;
		uint16 publishCountDebug;
		bool errorLast;
		uint16 publishCountError;

		void writeEEPROMsendRest();
		void writeEEPROMuseAvg();
		void writeEEPROMdebug();
		void writeEEPROMmaxCycle();
};

#endif