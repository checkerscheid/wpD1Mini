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
#include <moduleBase.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>

class deviceOneWire {
	public:
		deviceOneWire(uint8 no);
		moduleBase* mb;
		uint8 number;

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
		bool UseAvg();
		bool UseAvg(bool useAvg);

		void initAddress(uint8 adr[8]);
		void setAddress(uint8 adr[8]);
	private:
		uint8 address[8];
		int temperatureLast;
		unsigned long publishTemperatureLast;
		int8 correctionLast;
		unsigned long publishCorrectionLast;
		static const uint8 avgLength = 128;
		int avgValues[avgLength];

		void calc();
		uint16 calcAvg(uint16 raw);

	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 207 $";
};

#endif