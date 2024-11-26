//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 21.11.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLDR.h 207 2024-10-07 12:59:22Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef deviceOneWire_h
#define deviceOneWire_h
#include <Arduino.h>
#include <wpFreakaZone.h>

class deviceOneWire {
	public:
		deviceOneWire(uint8 no);
		uint8 number;
		bool debug;

		// section for define
		int temperature;
		int8 correction = 0;

		// values
		String mqttTopicTemperature;
		String mqttTopicCorrection;
		// settings
		String mqttTopicSetCorrection;

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
		// getter / setter

		void initAddress(uint8 b0, uint8 b1, uint8 b2, uint8 b3, uint8 b4, uint8 b5, uint8 b6, uint8 b7);
		void setAddress(uint8 adr[8]);
		uint8_t* getAddress();

		void setTemperature(float t);
	private:
		uint8 address[8];
		int temperatureLast;
		unsigned long publishTemperatureLast;
		int8 correctionLast;
		unsigned long publishCorrectionLast;

		void calc();

	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 207 $";
};

#endif