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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleUnderfloor.h 269 2025-07-01 19:25:14Z              $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleUnderfloor1_h
#define moduleUnderfloor1_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleUnderfloor : public IModuleBase {
	public:
		moduleUnderfloor(uint8_t n);
		moduleBase* mb;
		uint8_t Pin;

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
		uint16_t getVersion();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
		void changeDebug();
		String GetJsonSettings();
		void InitSetPoint(uint8_t setpoint);
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		uint32_t CalcCycle();
		uint32_t CalcCycle(uint32_t calcCycle);
		uint8_t GetSetPoint();
		String SetHand(bool val);
		String SetHandValue(bool val);
		String SetSetPoint(uint8_t setpoint);
		String SetTopicTempUrl(String topic);
		String SetWartung();
		void InitSummer(bool summer) { this->summer = summer; };
		String SetSummer(bool summer);
		bool GetSummer() { return summer; }
	private:
		const uint8_t SUMMERTEMP = 12;
		uint8_t no;
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
		uint8_t setPointLast;
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
		String SVNh = "$Rev: 269 $";

		uint8_t bitDebug;
		uint8_t bitHand;
		uint8_t bitHandValue;
		uint8_t bitSummer;
		uint16_t byteSetpoint;
		uint16_t byteCalcCycle;
		uint8_t setPoint;
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