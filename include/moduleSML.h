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
//# Revision     : $Rev:: 271                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleSML.h 271 2025-07-30 22:05:20Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleSML_h
#define moduleSML_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <SoftwareSerial.h>

class moduleSML : public IModuleBase {
	public:
		moduleSML();
		moduleBase* mb;
		SoftwareSerial* SmlSerial;
		uint8_t PinRX;
		uint8_t PinTX;

		// section for define

		// values
		String mqttTopicZsNetz;
		String mqttTopicZsSolar;
		String mqttTopicPNetz;
		String mqttTopicPSolar;
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
		void InitMeterType(uint8_t meterType);
		String SetMeterType(uint8_t meterType);
		String GetMeterTypeName();
		String SetInfo();
		String SetPrintSml();
	private:

		//Variablen SML
		byte bytesmltemp; //Speichern aktueller Wert
		const uint8_t MeterTypeApator = 1;
		const uint8_t MeterTypeDZG = 2;

		uint8_t MeterType = MeterTypeApator;
		bool printSml = false; //SML Ausgabe in der Konsole

		//byte StopSequence; //end sequence of SML protocol

		int SequenceIndex = 0; //Sequenzindex
		int SequenzeTime = 0;
		byte SmlZsNetz[4]; //Speichern der aktuellen Messwerte PSolar in HEX
		byte SmlZsSolar[4]; //Speichern der aktuellen Messwerte PKWH in HEX
		byte SmlPNetz[4]; //Speichern der aktuellen Messwerte PW in HEX
		byte SmlPSolar[4]; //Speichern der aktuellen Messwerte PW in HEX
		byte SmlTime[4];
		//Variablen Berechnung Leistung
		uint8_t SequenceStep = 1; //Sequenzschritt

		uint32_t currentZsNetz = 0; //Gesamtleistung Kwh Ausgabe DEC
		uint32_t currentZsNetzLast;
		unsigned long publishZsNetzLast;

		uint32_t currentZsSolar = 0; //Aktuelle Leistung W Ausgabe DEC
		uint32_t currentZsSolarLast;
		unsigned long publishZsSolarLast;
		
		int32_t currentPNetz = 0; //Aktuelle Leistung W Ausgabe DEC
		int32_t currentPNetzLast;
		unsigned long publishPNetzLast;
		
		int32_t currentPSolar = 0; //Aktuelle Leistung W Ausgabe DEC
		int32_t currentPSolarLast;
		unsigned long publishPSolarLast;
		//double dblcurrentPW = 0; //currentPW/Count
		//double dblcurrentPKwh = 0; //currentPKwh/10000

		void calc();
		uint8_t SetError(uint8_t Step);
	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 271 $";
};
extern moduleSML wpSML;

#endif