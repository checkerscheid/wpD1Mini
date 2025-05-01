//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 21.09.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 264                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleUnderfloor.h 264 2025-05-01 17:27:44Z              $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleUnderfloor1_h
#define moduleUnderfloor1_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleUnderfloor : public IModuleBase {
	public:
		moduleUnderfloor(uint8 n);
		moduleBase* mb;
		uint8 Pin;

		// section for define
		bool output;
		bool autoValue;
		bool handValue;
		bool handError;
		bool handSet = false;
		bool handValueSet = false;

		// values
		String mqttTopicOut;
		String mqttTopicAutoValue;
		String mqttTopicHandValue;
		String mqttTopicReadedTemp;
		String mqttTopicErrorHand;
		String mqttTopicWartungActive;
		// settings
		String mqttTopicSetPoint;
		String mqttTopicTempUrl;
		String mqttTopicTemp = "_";
		String mqttTopicSummer;
		// commands
		String mqttTopicSetHand;
		String mqttTopicSetHandValue;
		String mqttTopicSetSetPoint;
		String mqttTopicSetTempUrl;
		String mqttTopicSetWartung;
		String mqttTopicSetSummer;

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
		String GetJsonSettings();
		void InitSetPoint(uint8 setpoint);
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
		uint8 GetSetPoint();
		String SetHand(bool val);
		String SetHandValue(bool val);
		String SetSetPoint(uint8 setpoint);
		String SetTopicTempUrl(String topic);
		String SetWartung();
		void InitSummer(bool summer) { this->summer = summer; };
		String SetSummer(bool summer);
		bool GetSummer() { return summer; }
	private:
		const uint8 SUMMERTEMP = 12;
		uint8 no;
		bool outputLast;
		unsigned long publishOutputLast;
		bool autoValueLast;
		unsigned long publishAutoValueLast;
		bool handValueLast;
		unsigned long publishHandValueLast;
		int readedTempLast;
		unsigned long publishReadedTempLast;
		bool handErrorLast;
		unsigned long publishHandErrorLast;
		uint8 setPointLast;
		unsigned long publishSetPointLast;
		String tempUrlLast;
		unsigned long publishTempUrlLast;
		bool wartungActiveLast;
		unsigned long publishWartungActiveLast;
		bool summerLast;
		unsigned long publishSummerLast;

		void publishValue();
		void calc();
		void calcOutput();
		void deactivateWartung();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 264 $";

		uint8 bitDebug;
		uint8 bitHand;
		uint8 bitHandValue;
		uint8 bitSummer;
		uint16 byteSetpoint;
		uint16 byteCalcCycle;
		uint8 setPoint;
		int temp;
		bool wartungActive;
		unsigned long wartungStartedAt;
		bool summer = false;
};
extern moduleUnderfloor wpUnderfloor1;
extern moduleUnderfloor wpUnderfloor2;
extern moduleUnderfloor wpUnderfloor3;
extern moduleUnderfloor wpUnderfloor4;

#endif