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
//# Revision     : $Rev:: 228                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: deviceOneWire.h 228 2024-12-03 08:19:36Z                 $ #
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

		void setAddress(uint8 adr[8]);
		uint8_t* getAddress();
		String getStringAddress();

		void setTemperature(float t);
	private:
		uint8 address[8];
		int temperatureLast;
		unsigned long publishTemperatureLast;
		int8 correctionLast;
		unsigned long publishCorrectionLast;

		void calc();
		void initAddress();

	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 228 $";
};

#endif