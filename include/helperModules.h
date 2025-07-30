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
//# Revision     : $Rev:: 270                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperModules.h 270 2025-07-30 22:04:37Z                 $ #
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
		bool useModuleWindow = false;
		bool useModuleRelais = false;
		bool useModuleRelaisShield = false;
		bool useModuleRain = false;
		bool useModuleMoisture = false;
		bool useModuleDistance = false;
		
		bool useModuleCwWw = false;
		bool useModuleNeoPixel = false;
		bool useModuleAnalogOut = false;
		bool useModuleAnalogOut2 = false;
		bool useModuleRpm = false;
		bool useModuleImpulseCounter = false;
		bool useModuleUnderfloor1 = false;
		bool useModuleUnderfloor2 = false;
		bool useModuleUnderfloor3 = false;
		bool useModuleUnderfloor4 = false;
		bool useModuleDS18B20 = false;
		bool useModuleWindow2 = false;
		bool useModuleWindow3 = false;
		bool useModuleWeight = false;
		bool useModuleRFID = false;
		bool useModuleClock = false;
		bool useModuleSML = false;
		// commands
		String mqttTopicDebug;
		// settings
		String mqttTopicUseDHT11;
		String mqttTopicUseDHT22;
		String mqttTopicUseLDR;
		String mqttTopicUseLight;
		String mqttTopicUseBM;
		String mqttTopicUseWindow;
		String mqttTopicUseRelais;
		String mqttTopicUseRelaisShield;
		String mqttTopicUseRain;
		String mqttTopicUseMoisture;
		String mqttTopicUseDistance;
		#if BUILDWITH == 1
		String mqttTopicUseCwWw;
		String mqttTopicUseNeoPixel;
		String mqttTopicUseClock;
		#endif
		#if BUILDWITH == 2
		String mqttTopicUseAnalogOut;
		String mqttTopicUseAnalogOut2;
		String mqttTopicUseRpm;
		String mqttTopicUseImpulseCounter;
		String mqttTopicUseWindow2;
		String mqttTopicUseWindow3;
		String mqttTopicUseWeight;
		#endif
		#if BUILDWITH == 3
		String mqttTopicUseUnderfloor1;
		String mqttTopicUseUnderfloor2;
		String mqttTopicUseUnderfloor3;
		String mqttTopicUseUnderfloor4;
		String mqttTopicUseDS18B20;
		String mqttTopicUseSML;
		#endif
		#if BUILDWITH == 4
		String mqttTopicUseRFID;
		#endif

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
		//void SaveBoolToEEPROM(String name, uint16_t addr, byte byte, uint8_t bit, bool value);

		void changeModuleDHT11(bool newValue);
		void changeModuleDHT22(bool newValue);
		void changeModuleLDR(bool newValue);
		void changeModuleLight(bool newValue);
		void changeModuleBM(bool newValue);
		void changeModuleWindow(bool newValue);
		void changeModuleRelais(bool newValue);
		void changeModuleRelaisShield(bool newValue);
		void changeModuleRain(bool newValue);
		void changeModuleMoisture(bool newValue);
		void changeModuleDistance(bool newValue);
		#if BUILDWITH == 1
		void changeModuleCwWw(bool newValue);
		void changeModuleNeoPixel(bool newValue);
		void changemoduleClock(bool newValue);
		#endif
		#if BUILDWITH == 2
		void changeModuleAnalogOut(bool newValue);
		void changeModuleAnalogOut2(bool newValue);
		void changeModuleRpm(bool newValue);
		void changemoduleImpulseCounter(bool newValue);
		void changeModuleWindow2(bool newValue);
		void changeModuleWindow3(bool newValue);
		void changeModuleWeight(bool newValue);
		#endif
		#if BUILDWITH == 3
		void changemoduleUnderfloor1(bool newValue);
		void changemoduleUnderfloor2(bool newValue);
		void changemoduleUnderfloor3(bool newValue);
		void changemoduleUnderfloor4(bool newValue);
		void changemoduleDS18B20(bool newValue);
		void changemoduleSML(bool newValue);
		#endif
		#if BUILDWITH == 4
		void changemoduleRFID(bool newValue);
		#endif

		void publishAllSettings();
		void publishAllSettings(bool force);
		void publishAllValues();
		void publishAllValues(bool force);
		void setAllSubscribes();
		void checkAllSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 270 $";
		bool useDHT11Last;
		unsigned long publishUseDHT11Last;
		bool useDHT22Last;
		unsigned long publishUseDHT22Last;
		bool useLDRLast;
		unsigned long publishUseLDRLast;
		bool useLightLast;
		unsigned long publishUseLightLast;
		bool useBMLast;
		unsigned long publishUseBMLast;
		bool useWindowLast;
		unsigned long publishUseWindowLast;
		bool useCwWwLast;
		unsigned long publishUseCwWwLast;
		bool useAnalogOutLast;
		unsigned long publishUseAnalogOutLast;
		bool useAnalogOut2Last;
		unsigned long publishUseAnalogOut2Last;
		bool useNeoPixelLast;
		unsigned long publishUseNeoPixelLast;
		bool useRelaisLast;
		unsigned long publishUseRelaisLast;
		bool useRelaisShieldLast;
		unsigned long publishUseRelaisShieldLast;
		bool useRpmLast;
		unsigned long publishUseRpmLast;
		bool useRainLast;
		unsigned long publishUseRainLast;
		bool useMoistureLast;
		unsigned long publishUseMoistureLast;
		bool useDistanceLast;
		unsigned long publishUseDistanceLast;
		bool useImpulseCounterLast;
		unsigned long publishUseImpulseCounterLast;
		bool useWindow2Last;
		unsigned long publishUseWindow2Last;
		bool useWindow3Last;
		unsigned long publishUseWindow3Last;
		bool useWeightLast;
		unsigned long publishUseWeightLast;
		bool useUnderfloor1Last;
		unsigned long publishUseUnderfloor1Last;
		bool useUnderfloor2Last;
		unsigned long publishUseUnderfloor2Last;
		bool useUnderfloor3Last;
		unsigned long publishUseUnderfloor3Last;
		bool useUnderfloor4Last;
		unsigned long publishUseUnderfloor4Last;
		bool useDS18B20Last;
		unsigned long publishUseDS18B20Last;
		bool useRFIDLast;
		unsigned long publishUseRFIDLast;
		bool useClockLast;
		unsigned long publishUseClockLast;
		bool useSMLLast;
		unsigned long publishUseSMLLast;
		#if BUILDWITH == 1
		#endif
		#if BUILDWITH == 2
		#endif
		#if BUILDWITH == 3
		#endif
		#if BUILDWITH == 4
		#endif

		bool DebugLast;
		unsigned long publishDebugLast;
};
extern helperModules wpModules;
#endif