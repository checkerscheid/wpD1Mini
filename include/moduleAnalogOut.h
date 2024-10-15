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
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut.h 207 2024-10-07 12:59:22Z               $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleAnalogOut_h
#define moduleAnalogOut_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <PID_v1.h>

class moduleAnalogOut {
	public:
		moduleAnalogOut();
		moduleBase* mb;
		PID* pid;
		uint8 Pin;

		// section for define
		uint8 output;
		uint hardwareoutMax;
		uint8 autoValue;
		uint8 handValue;
		bool handError;
		bool handSet = false;
		uint8 handValueSet = 0;
		double Kp = 1.0;
		double Tv = 0.2;
		double Tn = 0.0;
		double SetPoint = 75.0;

		// values
		String mqttTopicOut;
		String mqttTopicAutoValue;
		String mqttTopicHandValue;
		String mqttTopicErrorHand;
		// settings
		String mqttTopicKp;
		String mqttTopicTv;
		String mqttTopicTn;
		String mqttTopicSetPoint;
		// commands
		String mqttTopicSetHand;
		String mqttTopicSetHandValue;

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
		void InitKp(short kp);
		void InitTv(short tv);
		void InitTn(short tn);
		void InitSetPoint(short setpoint);
		void resetPID();
		void SetHandValueSet(uint8 val);
	private:
		const double minOutput = 0.0;
		const double maxOutput = 100.0;
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

		void publishValue();
		void calc();
		void calcOutput();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 207 $";
};
extern moduleAnalogOut wpAnalogOut;

#endif