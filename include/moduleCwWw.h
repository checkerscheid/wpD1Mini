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
//# Revision     : $Rev:: 221                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleCwWw.h 221 2024-11-04 15:10:40Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleCwWw_h
#define moduleCwWw_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleCwWw {
	public:
		moduleCwWw();
		moduleBase* mb;
		
		static const uint8 ModeStatic = 0;
		static const uint8 ModePulse = 4;
		static const uint8 ModeWwPulse = 5;
		static const uint8 ModeCwPulse = 6;
		static const uint8 ModeSmooth = 7;
		static const uint8 ModeWwSmooth = 8;
		static const uint8 ModeCwSmooth = 9;
		
		static const uint8 ModeBlender = 90; //CW, WW

		// values
		String mqttTopicManual;
		String mqttTopicMaxPercent;
		String mqttTopicModeName;
		String mqttTopicSleep;
		String mqttTopicSpeed;
		// settings
		// commands
		String mqttTopicSetMode;
		String mqttTopicSetSleep;
		String mqttTopicSetSpeed;

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

		String GetModeName(uint8 actualMode);
		void SetEffectSpeed(uint8 speed);
		void SetSleep(uint seconds);
		String SetOn();
		String SetOff();
		String SetWwCw(uint8 ww, uint8 cw);
		String SetWW(uint8 ww);
		String SetCW(uint8 cw);
		String SetWwCwAuto(uint8 ww, uint8 cw, uint sleep);
		String SetEffect(uint8 effect);
	private:
		bool manual;
		bool manualLast;
		unsigned long publishManualLast;
		uint8 maxPercent;
		uint8 maxPercentLast;
		unsigned long publishMaxPercent;
		uint8 modeCurrent;
		uint8 modeCurrentLast;
		unsigned long publishModeLast;
		uint sleep;
		uint sleepLast;
		unsigned long publishSleepLast;
		unsigned long sleepAt;

		uint8 loopTime;
		unsigned long loopPrevious;
		uint8 steps;

		void publishValue();
		void calc();

		uint8 targetWW;
		uint8 targetCW;
		float winkelWW;
		float winkelCW;
		uint8 effectSpeed;

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
		uint8 GetMaxPercent();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 221 $";
		const float pi = 3.14159267;
};
extern moduleCwWw wpCwWw;

#endif