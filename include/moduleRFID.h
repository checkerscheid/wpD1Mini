//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 28.10.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRFID.h 269 2025-07-01 19:25:14Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleRFID_h
#define moduleRFID_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <SPI.h>
#include <MFRC522.h>

class moduleRFID : public IModuleBase {
	public:
		moduleRFID();
		moduleBase* mb;
		MFRC522* rfid;
		uint8_t PinSS;
		uint8_t PinRST;

		long chipID;

		// section for define

		// values
		String mqttTopicChipID;
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
		uint32_t CalcCycle();
		uint32_t CalcCycle(uint32_t calcCycle);
	private:
		int chipIDLast;
		unsigned long publishChipIDLast;

		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 269 $";
};
extern moduleRFID wpRFID;

#endif