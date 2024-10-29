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
//# Revision     : $Rev:: 220                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRFID.h 220 2024-10-29 10:37:04Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleRFID_h
#define moduleRFID_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <SPI.h>
#include <MFRC522.h>

class moduleRFID {
	public:
		moduleRFID();
		moduleBase* mb;
		MFRC522* rfid;
		uint8 PinSS;
		uint8 PinRST;

		long chipID;

		// section for define

		// values
		String mqttTopicChipID;
		// settings

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
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
	private:
		int chipIDLast;
		unsigned long publishChipIDLast;

		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 220 $";
};
extern moduleRFID wpRFID;

#endif