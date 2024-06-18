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
//# Revision     : $Rev:: 144                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperModules.h 144 2024-06-18 17:20:09Z                 $ #
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
		uint8_t choosenDHTmodul;
		bool useModuleLDR = false;
		bool useModuleLight = false;
		bool useModuleBM = false;
		bool useModuleBM2 = false;
		bool useModuleRelais = false;
		bool useModuleRelaisShield = false;
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
		String mqttTopicUseBM2;
		String mqttTopicUseRelais;
		String mqttTopicUseRelaisShield;
		String mqttTopicUseRain;
		String mqttTopicUseMoisture;
		String mqttTopicUseDistance;

		helperModules();
		void init();
		void cycle();
		uint16_t getVersion();
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
		void changeModuleBM2(bool newValue);
		void changeModuleRelais(bool newValue);
		void changeModuleRelaisShield(bool newValue);
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
		String SVNh = "$Rev: 144 $";
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
		bool useBM2Last = false;
		uint16 publishCountUseBM2 = 0;
		bool useRelaisLast = false;
		uint16 publishCountUseRelais = 0;
		bool useRelaisShieldLast = false;
		uint16 publishCountUseRelaisShield = 0;
		bool useRainLast = false;
		uint16 publishCountUseRain = 0;
		bool useMoistureLast = false;
		uint16 publishCountUseMoisture = 0;
		bool useDistanceLast = false;
		uint16 publishCountUseDistance = 0;
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;
};
extern helperModules wpModules;
#endif