//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 13.07.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 264                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut.h 264 2025-05-01 17:27:44Z               $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleAnalogOut_h
#define moduleAnalogOut_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <PID_v1.h>

class moduleAnalogOut : public IModuleBase {
	public:
		moduleAnalogOut();
		moduleBase* mb;
		uint8 Pin;

		// values
		String mqttTopicOut;
		String mqttTopicAutoValue;
		String mqttTopicHandValue;
		String mqttTopicReadedTemp;
		String mqttTopicErrorHand;
		String mqttTopicWartungActive;
		// settings
		String mqttTopicKp;
		String mqttTopicTv;
		String mqttTopicTn;
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
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
		void InitKp(short kp);
		void InitTv(short tv);
		void InitTn(short tn);
		void InitSetPoint(short setpoint);
		void resetPID();
		String SetSetPoint(double setpoint);
		String SetTopicTempUrl(String topic);

		void InitHand(bool hand);

		void InitHandValue(uint8 value);
		void SetHandValue(uint8 value);
		void SetHandValueProzent(uint8 value);
		uint8 GetHandValue();

		bool GetHandError();

		void InitPidType(uint8 t);
		String SetPidType(uint8 t);
		String GetPidType();

		String SetWartung();

		void InitSummer(bool summer) { this->summer = summer; };
		String SetSummer(bool summer);
		bool GetSummer() { return summer; }

	private:
		const uint8 SUMMERTEMP = 12;
		PID* pid;
		// section for define
		uint8 output;
		uint8 autoValue;
		uint8 handValue;
		bool handError;
		bool handSet = false;
		uint8 handValueSet = 0;
		static const uint8 pidTypeHeating = DIRECT;
		static const uint8 pidTypeAirCondition = REVERSE;
		double Kp = 1.0;
		double Tv = 0.2;
		double Tn = 0.0;
		double SetPoint = 75.0;
		bool summer = false;

		const double minOutput = 0.0;
		const double maxOutput = 255.0;
		uint8 outputLast;
		unsigned long publishOutputLast;
		uint8 autoValueLast;
		unsigned long publishAutoValueLast;
		uint8 handValueLast;
		unsigned long publishHandValueLast;
		bool handErrorLast;
		unsigned long publishHandErrorLast;
		double PIDinput, PIDoutput, PIDsetPoint;
// Kp: Determines how aggressively the PID reacts to the current amount of error (Proportional)
// Ki (Tv): Determines how aggressively the PID reacts to error over time (Integral)
// Kd (Tn): Determines how aggressively the PID reacts to the change in error (Derivative)
		double KpLast, TvLast, TnLast, SetPointLast;
		unsigned long publishPIDLast;
		int readedTempLast;
		unsigned long publishReadedTempLast;
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
		int temp;
		uint8 pidType = pidTypeHeating;
		bool wartungActive;
		unsigned long wartungStartedAt;
};
extern moduleAnalogOut wpAnalogOut;

#endif