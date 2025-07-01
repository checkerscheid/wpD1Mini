//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 22.07.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 267                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleCwWw.h 267 2025-05-27 14:14:36Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleCwWw_h
#define moduleCwWw_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleCwWw : public IModuleBase {
	public:
		moduleCwWw();
		moduleBase* mb;
		
		static const uint8_t ModeStatic = 0;
		static const uint8_t ModePulse = 4;
		static const uint8_t ModeWwPulse = 5;
		static const uint8_t ModeCwPulse = 6;
		static const uint8_t ModeSmooth = 7;
		static const uint8_t ModeWwSmooth = 8;
		static const uint8_t ModeCwSmooth = 9;
		
		static const uint8_t ModeBlender = 90; //CW, WW

		// values
		String mqttTopicWW;
		String mqttTopicCW;
		String mqttTopicManual;
		String mqttTopicMaxPercent;
		String mqttTopicModeName;
		String mqttTopicSleep;
		String mqttTopicSpeed;
		// settings
		// commands
		String mqttTopicSetWW;
		String mqttTopicSetCW;
		String mqttTopicSetMode;
		String mqttTopicSetSleep;
		String mqttTopicSetSpeed;

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

		String SetWW(uint8_t ww);
		String SetCW(uint8_t cw);
		String SetSleep(uint seconds);
		String SetEffect(uint8_t effect);
		String SetEffectSpeed(uint8_t speed);
		String SetOn();
		String SetOff();
		String SetWwCwAuto(uint8_t ww, uint8_t cw, uint sleep);
		String GetModeName(uint8_t actualMode);
	private:
		uint8_t PinWW;
		uint8_t PinCW;
		uint8_t AnalogOutWW = 0;
		uint8_t AnalogOutWWLast = 0;
		unsigned long publishAnalogOutWWLast;
		uint8_t AnalogOutCW = 0;
		uint8_t AnalogOutCWLast = 0;
		unsigned long publishAnalogOutCWLast;
		bool manual;
		bool manualLast;
		unsigned long publishManualLast;
		uint8_t maxPercent;
		uint8_t maxPercentLast;
		unsigned long publishMaxPercent;
		uint8_t modeCurrent;
		uint8_t modeCurrentLast;
		unsigned long publishModeLast;
		uint sleep;
		uint sleepLast;
		unsigned long publishSleepLast;
		unsigned long sleepAt;

		uint8_t loopTime;
		unsigned long loopPrevious;
		uint8_t steps;

		void publishValue();
		void calc();

		uint8_t targetWW;
		uint8_t targetCW;
		float winkelWW;
		float winkelCW;
		uint8_t effectSpeed;

		void calcDuration();
		void BlenderEffect();
		bool BlenderWWEffect();
		bool BlenderCWEffect();
		bool smoothDirection;
		void PulseEffect();
		void WwPulseEffect();
		void CwPulseEffect();
		void SmoothEffect();
		void WwSmoothEffect();
		void CwSmoothEffect();
		uint8_t GetMaxPercent();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 267 $";
		const float pi = 3.14159267;
};
extern moduleCwWw wpCwWw;

#endif