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
//# Revision     : $Rev:: 273                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleModbus.h 273 2025-08-13 18:46:12Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleModbus_h
#define moduleModbus_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <ModbusRTU.h>
#include <SoftwareSerial.h>

class moduleModbus : public IModuleBase {
	public:
		moduleModbus();
		moduleBase* mb;
		SoftwareSerial* ss;
		ModbusRTU* modbus;
		uint8_t PinRX;
		uint8_t PinTX;
		uint8_t Pin;

		// section for define

		// values
		// settings

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
		bool Debug();
		bool Debug(bool debug);
		void InitIsMaster(bool isMaster);
		String SetIsMaster(bool isMaster);
		bool GetIsMaster();
		void InitClientId(uint8_t clientid);
		String SetClientId(uint8_t clientid);
		uint8_t GetClientId();
	private:

		const uint16_t addrtemp = 10;
		const uint16_t addrhum = 11;

		void calc();
		bool isMaster = false;
		uint8_t clientid = 1;

		int templast = 0;
		int humlasst = 0;

		int readedTemp;
		int readedHum;
	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 273 $";
};
extern moduleModbus wpModbus;

#endif