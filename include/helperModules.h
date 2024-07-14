//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 01.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 163                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperModules.h 163 2024-07-14 19:03:20Z                 $ #
//#                                                                                 #
//###################################################################################
#ifndef helperModules_h
#define helperModules_h
#include <Arduino.h>
#include <wpFreakaZone.h>
class helperModules {
	public:
		bool Debug = false;

		bool useModuleDHT11 = false;
		bool useModuleDHT22 = false;
		uint8 choosenDHTmodul;
		bool useModuleLDR = false;
		bool useModuleLight = false;
		bool useModuleBM = false;
		bool useModuleWindow = false;
		bool useModuleAnalogOut = false;
		bool useModuleRelais = false;
		bool useModuleRelaisShield = false;
		bool useModuleRpm = false;
		bool useModuleRain = false;
		bool useModuleMoisture = false;
		bool useModuleDistance = false;

		// commands
		String mqttTopicDebug;
		// settings
		String mqttTopicUseDHT11;
		String mqttTopicUseDHT22;
		String mqttTopicUseLDR;
		String mqttTopicUseLight;
		String mqttTopicUseBM;
		String mqttTopicUseWindow;
		String mqttTopicUseAnalogOut;
		String mqttTopicUseRelais;
		String mqttTopicUseRelaisShield;
		String mqttTopicUseRpm;
		String mqttTopicUseRain;
		String mqttTopicUseMoisture;
		String mqttTopicUseDistance;

		helperModules();
		void init();
		void cycle();
		uint16 getVersion();
		void changeDebug();
		void setupModules();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);

		void changeModuleDHT11(bool newValue);
		void changeModuleDHT22(bool newValue);
		void changeModuleLDR(bool newValue);
		void changeModuleLight(bool newValue);
		void changeModuleBM(bool newValue);
		void changeModuleWindow(bool newValue);
		void changeModuleAnalogOut(bool newValue);
		void changeModuleRelais(bool newValue);
		void changeModuleRelaisShield(bool newValue);
		void changeModuleRpm(bool newValue);
		void changeModuleRain(bool newValue);
		void changeModuleMoisture(bool newValue);
		void changeModuleDistance(bool newValue);

		void publishAllSettings();
		void publishAllSettings(bool force);
		void publishAllValues();
		void publishAllValues(bool force);
		void setAllSubscribes();
		void checkAllSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 163 $";
		bool useDHT11Last = false;
		uint16 publishCountUseDHT11 = 0;
		bool useDHT22Last = false;
		uint16 publishCountUseDHT22 = 0;
		bool useLDRLast = false;
		uint16 publishCountUseLDR = 0;
		bool useLightLast = false;
		uint16 publishCountUseLight = 0;
		bool useBMLast = false;
		uint16 publishCountUseBM = 0;
		bool useWindowLast = false;
		uint16 publishCountUseWindow = 0;
		bool useAnalogOutLast = false;
		uint16 publishCountUseAnalogOut = 0;
		bool useRelaisLast = false;
		uint16 publishCountUseRelais = 0;
		bool useRelaisShieldLast = false;
		uint16 publishCountUseRelaisShield = 0;
		bool useRpmLast = false;
		uint16 publishCountUseRpm = 0;
		bool useRainLast = false;
		uint16 publishCountUseRain = 0;
		bool useMoistureLast = false;
		uint16 publishCountUseMoisture = 0;
		bool useDistanceLast = false;
		uint16 publishCountUseDistance = 0;
		bool DebugLast = false;
		uint16 publishCountDebug = 0;
};
extern helperModules wpModules;
#endif