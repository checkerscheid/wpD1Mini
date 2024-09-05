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
//# Revision     : $Rev:: 199                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleCwWw.h 199 2024-09-05 12:33:52Z                    $ #
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
		static const uint8 ModeOn = 1;
		static const uint8 ModeOff = 2;
		static const uint8 ModeOnBlender = 97;
		static const uint8 ModeOffBlender = 98;

		// values
		String mqttTopicModeName;
		String mqttTopicSleep;
		// settings
		// commands
		String mqttTopicSetMode;
		String mqttTopicSetSleep;

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
		void changeSendRest();
		void changeDebug();
		// getter / setter
		bool SendRest();
		bool SendRest(bool sendRest);
		bool Debug();
		bool Debug(bool debug);
		String GetModeName(uint actualMode);
		void SetMode(uint8 newMode);
		void SetSleep(uint seconds);
		String SetOn();
		String SetOff();
		void SetOnBlender();
		void SetOffBlender();
	private:
		bool staticIsSet;
		unsigned long publishValueLast;
		uint modeCurrent;
		uint modeCurrentLast;
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

		uint8 onBlenderTargetWW;
		uint8 onBlenderTargetCW;
		void OnBlenderEffect();
		void OffBlenderEffect();
		void StaticEffect();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 199 $";
};
extern moduleCwWw wpCwWw;

#endif