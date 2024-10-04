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
//# Revision     : $Rev:: 203                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperModules.h 203 2024-10-04 07:32:26Z                 $ #
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
		String mqttTopicUseAnalogOut;
		String mqttTopicUseAnalogOut2;
		#endif
		#if BUILDWITH == 2
		String mqttTopicUseRpm;
		String mqttTopicUseImpulseCounter;
		#endif
		#if BUILDWITH == 3
		String mqttTopicUseUnderfloor1;
		String mqttTopicUseUnderfloor2;
		String mqttTopicUseUnderfloor3;
		String mqttTopicUseUnderfloor4;
		#endif

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
		void changeModuleRelais(bool newValue);
		void changeModuleRelaisShield(bool newValue);
		void changeModuleRain(bool newValue);
		void changeModuleMoisture(bool newValue);
		void changeModuleDistance(bool newValue);
		#if BUILDWITH == 1
		void changeModuleCwWw(bool newValue);
		void changeModuleNeoPixel(bool newValue);
		void changeModuleAnalogOut(bool newValue);
		void changeModuleAnalogOut2(bool newValue);
		#endif
		#if BUILDWITH == 2
		void changeModuleRpm(bool newValue);
		void changemoduleImpulseCounter(bool newValue);
		#endif
		#if BUILDWITH == 3
		void changemoduleUnderfloor1(bool newValue);
		void changemoduleUnderfloor2(bool newValue);
		void changemoduleUnderfloor3(bool newValue);
		void changemoduleUnderfloor4(bool newValue);
		#endif

		void publishAllSettings();
		void publishAllSettings(bool force);
		void publishAllValues();
		void publishAllValues(bool force);
		void setAllSubscribes();
		void checkAllSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 203 $";
		bool useDHT11Last;
		uint32 publishUseDHT11Last;
		bool useDHT22Last;
		uint32 publishUseDHT22Last;
		bool useLDRLast;
		uint32 publishUseLDRLast;
		bool useLightLast;
		uint32 publishUseLightLast;
		bool useBMLast;
		uint32 publishUseBMLast;
		bool useWindowLast;
		uint32 publishUseWindowLast;
		bool useCwWwLast;
		uint32 publishUseCwWwLast;
		bool useAnalogOutLast;
		uint32 publishUseAnalogOutLast;
		bool useAnalogOut2Last;
		uint32 publishUseAnalogOut2Last;
		bool useNeoPixelLast;
		uint32 publishUseNeoPixelLast;
		bool useRelaisLast;
		uint32 publishUseRelaisLast;
		bool useRelaisShieldLast;
		uint32 publishUseRelaisShieldLast;
		bool useRpmLast;
		uint32 publishUseRpmLast;
		bool useRainLast;
		uint32 publishUseRainLast;
		bool useMoistureLast;
		uint32 publishUseMoistureLast;
		bool useDistanceLast;
		uint32 publishUseDistanceLast;
		bool useImpulseCounterLast;
		uint32 publishUseImpulseCounterLast;
		bool useUnderfloor1Last;
		uint32 publishUseUnderfloor1Last;
		bool useUnderfloor2Last;
		uint32 publishUseUnderfloor2Last;
		bool useUnderfloor3Last;
		uint32 publishUseUnderfloor3Last;
		bool useUnderfloor4Last;
		uint32 publishUseUnderfloor4Last;
		#if BUILDWITH == 1
		#endif
		#if BUILDWITH == 2
		#endif
		#if BUILDWITH == 3
		#endif

		bool DebugLast;
		uint32 publishDebugLast;
};
extern helperModules wpModules;
#endif