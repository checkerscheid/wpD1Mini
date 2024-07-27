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
//# Revision     : $Rev:: 181                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperModules.h 181 2024-07-27 23:14:47Z                 $ #
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
		bool useModuleAnalogOut2 = false;
		bool useModuleNeoPixel = false;
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
		String mqttTopicUseAnalogOut2;
		String mqttTopicUseNeoPixel;
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
		void changeModuleAnalogOut2(bool newValue);
		void changeModuleNeoPixel(bool newValue);
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
		String SVNh = "$Rev: 181 $";
		bool useDHT11Last = false;
		uint32 publishForceUseDHT11 = 0;
		bool useDHT22Last = false;
		uint32 publishForceUseDHT22 = 0;
		bool useLDRLast = false;
		uint32 publishForceUseLDR = 0;
		bool useLightLast = false;
		uint32 publishForceUseLight = 0;
		bool useBMLast = false;
		uint32 publishForceUseBM = 0;
		bool useWindowLast = false;
		uint32 publishForceUseWindow = 0;
		bool useAnalogOutLast = false;
		uint32 publishForceUseAnalogOut = 0;
		bool useAnalogOut2Last = false;
		uint32 publishForceUseAnalogOut2 = 0;
		bool useNeoPixelLast = false;
		uint32 publishForceUseNeoPixel = 0;
		bool useRelaisLast = false;
		uint32 publishForceUseRelais = 0;
		bool useRelaisShieldLast = false;
		uint32 publishForceUseRelaisShield = 0;
		bool useRpmLast = false;
		uint32 publishForceUseRpm = 0;
		bool useRainLast = false;
		uint32 publishForceUseRain = 0;
		bool useMoistureLast = false;
		uint32 publishForceUseMoisture = 0;
		bool useDistanceLast = false;
		uint32 publishForceUseDistance = 0;
		bool DebugLast = false;
		uint32 publishForceDebug = 0;
};
extern helperModules wpModules;
#endif