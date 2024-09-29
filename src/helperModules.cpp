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
//# Revision     : $Rev:: 198                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperModules.cpp 198 2024-09-05 12:32:25Z               $ #
//#                                                                                 #
//###################################################################################
#include <helperModules.h>

helperModules wpModules;

helperModules::helperModules() {}
void helperModules::init() {
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Modules";
	// settings
	
	mqttTopicUseDHT11 = wpFZ.DeviceName + "/settings/useModule/DHT11";
	mqttTopicUseDHT22 = wpFZ.DeviceName + "/settings/useModule/DHT22";
	mqttTopicUseLDR = wpFZ.DeviceName + "/settings/useModule/LDR";
	mqttTopicUseLight = wpFZ.DeviceName + "/settings/useModule/Light";
	mqttTopicUseBM = wpFZ.DeviceName + "/settings/useModule/BM";
	mqttTopicUseWindow = wpFZ.DeviceName + "/settings/useModule/Window";
	mqttTopicUseCwWw = wpFZ.DeviceName + "/settings/useModule/CwWw";
	mqttTopicUseAnalogOut = wpFZ.DeviceName + "/settings/useModule/AnalogOut";
	mqttTopicUseAnalogOut2 = wpFZ.DeviceName + "/settings/useModule/AnalogOut2";
	mqttTopicUseNeoPixel = wpFZ.DeviceName + "/settings/useModule/NeoPixel";
	mqttTopicUseRelais = wpFZ.DeviceName + "/settings/useModule/Relais";
	mqttTopicUseRelaisShield = wpFZ.DeviceName + "/settings/useModule/RelaisShield";
	mqttTopicUseRpm = wpFZ.DeviceName + "/settings/useModule/Rpm";
	mqttTopicUseRain = wpFZ.DeviceName + "/settings/useModule/Rain";
	mqttTopicUseMoisture = wpFZ.DeviceName + "/settings/useModule/Moisture";
	mqttTopicUseDistance = wpFZ.DeviceName + "/settings/useModule/Distance";
	mqttTopicUseImpulseCounter = wpFZ.DeviceName + "/settings/useModule/ImpulseCounter";

	choosenDHTmodul = 0;
	if(useModuleDHT11) {
		choosenDHTmodul = DHT11;
	}
	if(useModuleDHT22) {
		choosenDHTmodul = DHT22;
	}
}

//###################################################################################
// public
//###################################################################################
void helperModules::cycle() {
	publishValues();
}

uint16 helperModules::getVersion() {
	String SVN = "$Rev: 198 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperModules::changeDebug() {
	Debug = !Debug;
	wpEEPROM.saveBool(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugModules, Debug);
	wpFZ.SendWSDebug("DebugModules", Debug);
	wpFZ.blink();
}

void helperModules::publishSettings() {
	publishSettings(false);
}
void helperModules::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperModules::publishValues() {
	publishValues(false);
}
void helperModules::publishValues(bool force) {
	if(force) {
		publishUseDHT11Last = 0;
		publishUseDHT22Last = 0;
		publishUseLDRLast = 0;
		publishUseLightLast = 0;
		publishUseBMLast = 0;
		publishUseWindowLast = 0;
		publishUseCwWwLast = 0;
		publishUseAnalogOutLast = 0;
		publishUseAnalogOut2Last = 0;
		publishUseNeoPixelLast = 0;
		publishUseRelaisLast = 0;
		publishUseRelaisShieldLast = 0;
		publishUseRpmLast = 0;
		publishUseRainLast = 0;
		publishUseMoistureLast = 0;
		publishUseDistanceLast = 0;
		publishUseImpulseCounterLast = 0;
		publishDebugLast = 0;
	}
	if(useDHT11Last != useModuleDHT11 || publishUseDHT11Last == 0 ||
		wpFZ.loopStartedAt > publishUseDHT11Last + wpFZ.publishQoS) {
		useDHT11Last = useModuleDHT11;
		wpMqtt.mqttClient.publish(mqttTopicUseDHT11.c_str(), String(useModuleDHT11).c_str());
		wpFZ.SendWSModule("useDHT11", useModuleDHT11);
		publishUseDHT11Last = wpFZ.loopStartedAt;
	}
	if(useDHT22Last != useModuleDHT22 || publishUseDHT22Last == 0 ||
		wpFZ.loopStartedAt > publishUseDHT22Last + wpFZ.publishQoS) {
		useDHT22Last = useModuleDHT22;
		wpMqtt.mqttClient.publish(mqttTopicUseDHT22.c_str(), String(useModuleDHT22).c_str());
		wpFZ.SendWSModule("useDHT22", useModuleDHT22);
		publishUseDHT22Last = wpFZ.loopStartedAt;
	}
	if(useLDRLast != useModuleLDR || publishUseLDRLast == 0 ||
		wpFZ.loopStartedAt > publishUseLDRLast + wpFZ.publishQoS) {
		useLDRLast = useModuleLDR;
		wpMqtt.mqttClient.publish(mqttTopicUseLDR.c_str(), String(useModuleLDR).c_str());
		wpFZ.SendWSModule("useLDR", useModuleLDR);
		publishUseLDRLast = wpFZ.loopStartedAt;
	}
	if(useLightLast != useModuleLight || publishUseLightLast == 0 ||
		wpFZ.loopStartedAt > publishUseLightLast + wpFZ.publishQoS) {
		useLightLast = useModuleLight;
		wpMqtt.mqttClient.publish(mqttTopicUseLight.c_str(), String(useModuleLight).c_str());
		wpFZ.SendWSModule("useLight", useModuleLight);
		publishUseLightLast = wpFZ.loopStartedAt;
	}
	if(useBMLast != useModuleBM || publishUseBMLast == 0 ||
		wpFZ.loopStartedAt > publishUseBMLast + wpFZ.publishQoS) {
		useBMLast = useModuleBM;
		wpMqtt.mqttClient.publish(mqttTopicUseBM.c_str(), String(useModuleBM).c_str());
		wpFZ.SendWSModule("useBM", useModuleBM);
		publishUseBMLast = wpFZ.loopStartedAt;
	}
	if(useWindowLast != useModuleWindow || publishUseWindowLast == 0 ||
		wpFZ.loopStartedAt > publishUseWindowLast + wpFZ.publishQoS) {
		useWindowLast = useModuleWindow;
		wpMqtt.mqttClient.publish(mqttTopicUseWindow.c_str(), String(useModuleWindow).c_str());
		wpFZ.SendWSModule("useWindow", useModuleWindow);
		publishUseWindowLast = wpFZ.loopStartedAt;
	}
	if(useCwWwLast != useModuleCwWw || publishUseCwWwLast == 0 ||
		wpFZ.loopStartedAt > publishUseCwWwLast + wpFZ.publishQoS) {
		useCwWwLast = useModuleCwWw;
		wpMqtt.mqttClient.publish(mqttTopicUseCwWw.c_str(), String(useModuleCwWw).c_str());
		wpFZ.SendWSModule("useCwWw", useModuleCwWw);
		publishUseCwWwLast = wpFZ.loopStartedAt;
	}
	if(useAnalogOutLast != useModuleAnalogOut || publishUseAnalogOutLast == 0 ||
		wpFZ.loopStartedAt > publishUseAnalogOutLast + wpFZ.publishQoS) {
		useAnalogOutLast = useModuleAnalogOut;
		wpMqtt.mqttClient.publish(mqttTopicUseAnalogOut.c_str(), String(useModuleAnalogOut).c_str());
		wpFZ.SendWSModule("useAnalogOut", useModuleAnalogOut);
		publishUseAnalogOutLast = wpFZ.loopStartedAt;
	}
	if(useAnalogOut2Last != useModuleAnalogOut2 || publishUseAnalogOut2Last == 0 ||
		wpFZ.loopStartedAt > publishUseAnalogOut2Last + wpFZ.publishQoS) {
		useAnalogOut2Last = useModuleAnalogOut2;
		wpMqtt.mqttClient.publish(mqttTopicUseAnalogOut2.c_str(), String(useModuleAnalogOut2).c_str());
		wpFZ.SendWSModule("useAnalogOut2", useModuleAnalogOut2);
		publishUseAnalogOut2Last = wpFZ.loopStartedAt;
	}
	if(useNeoPixelLast != useModuleNeoPixel || publishUseNeoPixelLast == 0 ||
		wpFZ.loopStartedAt > publishUseNeoPixelLast + wpFZ.publishQoS) {
		useNeoPixelLast = useModuleNeoPixel;
		wpMqtt.mqttClient.publish(mqttTopicUseNeoPixel.c_str(), String(useModuleNeoPixel).c_str());
		wpFZ.SendWSModule("useNeoPixel", useModuleNeoPixel);
		publishUseNeoPixelLast = wpFZ.loopStartedAt;
	}
	if(useRelaisLast != useModuleRelais || publishUseRelaisLast == 0 ||
		wpFZ.loopStartedAt > publishUseRelaisLast + wpFZ.publishQoS) {
		useRelaisLast = useModuleRelais;
		wpMqtt.mqttClient.publish(mqttTopicUseRelais.c_str(), String(useModuleRelais).c_str());
		wpFZ.SendWSModule("useRelais", useModuleRelais);
		publishUseRelaisLast = wpFZ.loopStartedAt;
	}
	if(useRelaisShieldLast != useModuleRelaisShield || publishUseRelaisShieldLast == 0 ||
		wpFZ.loopStartedAt > publishUseRelaisShieldLast + wpFZ.publishQoS) {
		useRelaisShieldLast = useModuleRelaisShield;
		wpMqtt.mqttClient.publish(mqttTopicUseRelaisShield.c_str(), String(useModuleRelaisShield).c_str());
		wpFZ.SendWSModule("useRelaisShield", useModuleRelaisShield);
		publishUseRelaisShieldLast = wpFZ.loopStartedAt;
	}
	if(useRpmLast != useModuleRpm || publishUseRpmLast == 0 ||
		wpFZ.loopStartedAt > publishUseRpmLast + wpFZ.publishQoS) {
		useRpmLast = useModuleRpm;
		wpMqtt.mqttClient.publish(mqttTopicUseRpm.c_str(), String(useModuleRpm).c_str());
		wpFZ.SendWSModule("useRpm", useModuleRpm);
		publishUseRpmLast = wpFZ.loopStartedAt;
	}
	if(useRainLast != useModuleRain || publishUseRainLast == 0 ||
		wpFZ.loopStartedAt > publishUseRainLast + wpFZ.publishQoS) {
		useRainLast = useModuleRain;
		wpMqtt.mqttClient.publish(mqttTopicUseRain.c_str(), String(useModuleRain).c_str());
		wpFZ.SendWSModule("useRain", useModuleRain);
		publishUseRainLast = wpFZ.loopStartedAt;
	}
	if(useMoistureLast != useModuleMoisture || publishUseMoistureLast == 0 ||
		wpFZ.loopStartedAt > publishUseMoistureLast + wpFZ.publishQoS) {
		useMoistureLast = useModuleMoisture;
		wpMqtt.mqttClient.publish(mqttTopicUseMoisture.c_str(), String(useModuleMoisture).c_str());
		wpFZ.SendWSModule("useMoisture", useModuleMoisture);
		publishUseMoistureLast = wpFZ.loopStartedAt;
	}
	if(useDistanceLast != useModuleDistance || publishUseDistanceLast == 0 ||
		wpFZ.loopStartedAt > publishUseDistanceLast + wpFZ.publishQoS) {
		useDistanceLast = useModuleDistance;
		wpMqtt.mqttClient.publish(mqttTopicUseDistance.c_str(), String(useModuleDistance).c_str());
		wpFZ.SendWSModule("useDistance", useModuleDistance);
		publishUseDistanceLast = wpFZ.loopStartedAt;
	}
	if(useImpulseCounterLast != useModuleImpulseCounter || publishUseImpulseCounterLast == 0 ||
		wpFZ.loopStartedAt > publishUseImpulseCounterLast + wpFZ.publishQoS) {
		useImpulseCounterLast = useModuleImpulseCounter;
		wpMqtt.mqttClient.publish(mqttTopicUseImpulseCounter.c_str(), String(useModuleImpulseCounter).c_str());
		wpFZ.SendWSModule("useImpulseCounter", useModuleImpulseCounter);
		publishUseImpulseCounterLast = wpFZ.loopStartedAt;
	}
	if(DebugLast != Debug || publishDebugLast == 0 ||
		wpFZ.loopStartedAt > publishDebugLast + wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishDebugLast = wpFZ.loopStartedAt;
	}
}

void helperModules::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicUseDHT11.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseDHT22.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseLDR.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseLight.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseBM.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseWindow.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseCwWw.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseAnalogOut.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseAnalogOut2.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseNeoPixel.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseRelais.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseRelaisShield.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseRpm.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseRain.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseMoisture.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseDistance.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseImpulseCounter.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void helperModules::checkSubscribes(char* topic, String msg) {
	bool readUseModule = msg.toInt();
	if(strcmp(topic, mqttTopicUseDHT11.c_str()) == 0) {
		changeModuleDHT11(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseDHT22.c_str()) == 0) {
		changeModuleDHT22(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseLDR.c_str()) == 0) {
		changeModuleLDR(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseLight.c_str()) == 0) {
		changeModuleLight(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseMoisture.c_str()) == 0) {
		changeModuleMoisture(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseCwWw.c_str()) == 0) {
		changeModuleCwWw(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseAnalogOut.c_str()) == 0) {
		changeModuleAnalogOut(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseAnalogOut2.c_str()) == 0) {
		changeModuleAnalogOut2(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseNeoPixel.c_str()) == 0) {
		changeModuleNeoPixel(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseRelais.c_str()) == 0) {
		changeModuleRelais(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseRelaisShield.c_str()) == 0) {
		changeModuleRelaisShield(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseBM.c_str()) == 0) {
		changeModuleBM(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseWindow.c_str()) == 0) {
		changeModuleWindow(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseRpm.c_str()) == 0) {
		changeModuleRpm(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseRain.c_str()) == 0) {
		changeModuleRain(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseDistance.c_str()) == 0) {
		changeModuleDistance(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseImpulseCounter.c_str()) == 0) {
		changemoduleImpulseCounter(readUseModule);
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		if(Debug != readUseModule) {
			Debug = readUseModule;
			wpEEPROM.saveBool(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugModules, Debug);
			wpFZ.SendWSDebug("DebugModules", Debug);
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}
void helperModules::changeModuleDHT11(bool newValue) {
	if(useModuleDHT11 != newValue) {
		useModuleDHT11 = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0, wpEEPROM.bitUseDHT11, useModuleDHT11);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleDHT11", useModuleDHT11);
		wpFZ.DebugcheckSubscribes(mqttTopicUseDHT11, String(Debug));
	}
}
void helperModules::changeModuleDHT22(bool newValue) {
	if(useModuleDHT22 != newValue) {
		useModuleDHT22 = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0, wpEEPROM.bitUseDHT22, useModuleDHT22);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleDHT22", useModuleDHT22);
		wpFZ.DebugcheckSubscribes(mqttTopicUseDHT22, String(Debug));
	}
}
void helperModules::changeModuleLDR(bool newValue) {
	if(useModuleLDR != newValue) {
		useModuleLDR = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0, wpEEPROM.bitUseLDR, useModuleLDR);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleLDR", useModuleLDR);
		wpFZ.DebugcheckSubscribes(mqttTopicUseLDR, String(Debug));
	}
}
void helperModules::changeModuleLight(bool newValue) {
	if(useModuleLight != newValue) {
		useModuleLight = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0, wpEEPROM.bitUseLight, useModuleLight);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleLight", useModuleLight);
		wpFZ.DebugcheckSubscribes(mqttTopicUseLight, String(Debug));
	}
}
void helperModules::changeModuleBM(bool newValue) {
	if(useModuleBM != newValue) {
		useModuleBM = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0, wpEEPROM.bitUseBM, useModuleBM);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleBM", useModuleBM);
		wpFZ.DebugcheckSubscribes(mqttTopicUseBM, String(useModuleBM));
	}
}
void helperModules::changeModuleWindow(bool newValue) {
	if(useModuleWindow != newValue) {
		useModuleWindow = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1, wpEEPROM.bitUseWindow, useModuleWindow);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleWindow", useModuleWindow);
		wpFZ.DebugcheckSubscribes(mqttTopicUseWindow, String(useModuleWindow));
	}
}
void helperModules::changeModuleCwWw(bool newValue) {
	if(useModuleCwWw != newValue) {
		useModuleCwWw = newValue;
		if(newValue) {
			changeModuleAnalogOut(true);
			changeModuleAnalogOut2(true);
		}
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules2, wpEEPROM.bitsModules2, wpEEPROM.bitUseCwWw, useModuleCwWw);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleCwWw", useModuleCwWw);
		wpFZ.DebugcheckSubscribes(mqttTopicUseCwWw, String(useModuleCwWw));
	}
}
void helperModules::changeModuleAnalogOut(bool newValue) {
	if(useModuleAnalogOut != newValue) {
		useModuleAnalogOut = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1, wpEEPROM.bitUseAnalogOut, useModuleAnalogOut);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleAnalogOut", useModuleAnalogOut);
		wpFZ.DebugcheckSubscribes(mqttTopicUseAnalogOut, String(useModuleAnalogOut));
	}
}
void helperModules::changeModuleAnalogOut2(bool newValue) {
	if(useModuleAnalogOut2 != newValue) {
		useModuleAnalogOut2 = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1, wpEEPROM.bitUseAnalogOut2, useModuleAnalogOut2);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleAnalogOut2", useModuleAnalogOut2);
		wpFZ.DebugcheckSubscribes(mqttTopicUseAnalogOut2, String(useModuleAnalogOut2));
	}
}
void helperModules::changeModuleNeoPixel(bool newValue) {
	if(useModuleNeoPixel != newValue) {
		useModuleNeoPixel = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1, wpEEPROM.bitUseNeoPixel, useModuleNeoPixel);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleNeoPixel", useModuleNeoPixel);
		wpFZ.DebugcheckSubscribes(mqttTopicUseNeoPixel, String(useModuleNeoPixel));
	}
}
void helperModules::changeModuleRelais(bool newValue) {
	if(useModuleRelais != newValue) {
		useModuleRelais = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0, wpEEPROM.bitUseRelais, useModuleRelais);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleRelais", useModuleRelais);
		wpFZ.DebugcheckSubscribes(mqttTopicUseRelais, String(useModuleRelais));
	}
}
void helperModules::changeModuleRelaisShield(bool newValue) {
	if(useModuleRelaisShield != newValue) {
		useModuleRelaisShield = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0, wpEEPROM.bitUseRelaisShield, useModuleRelaisShield);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleRelaisShield", useModuleRelaisShield);
		wpFZ.DebugcheckSubscribes(mqttTopicUseRelaisShield, String(useModuleRelaisShield));
	}
}
void helperModules::changeModuleRpm(bool newValue) {
	if(useModuleRpm != newValue) {
		useModuleRpm = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1, wpEEPROM.bitUseRpm, useModuleRpm);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleRpm", useModuleRpm);
		wpFZ.DebugcheckSubscribes(mqttTopicUseRpm, String(useModuleRpm));
	}
}
void helperModules::changeModuleRain(bool newValue) {
	if(useModuleRain != newValue) {
		useModuleRain = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0, wpEEPROM.bitUseRain, useModuleRain);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleRain", useModuleRain);
		wpFZ.DebugcheckSubscribes(mqttTopicUseRain, String(useModuleRain));
	}
}
void helperModules::changeModuleMoisture(bool newValue) {
	if(useModuleMoisture != newValue) {
		useModuleMoisture = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1, wpEEPROM.bitUseMoisture, useModuleMoisture);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleMoisture", useModuleMoisture);
		wpFZ.DebugcheckSubscribes(mqttTopicUseMoisture, String(useModuleMoisture));
	}
}
void helperModules::changeModuleDistance(bool newValue) {
	if(useModuleDistance != newValue) {
		useModuleDistance = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1, wpEEPROM.bitUseDistance, useModuleDistance);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleDistance", useModuleDistance);
		wpFZ.DebugcheckSubscribes(mqttTopicUseDistance, String(useModuleDistance));
	}
}
void helperModules::changemoduleImpulseCounter(bool newValue) {
	if(useModuleImpulseCounter != newValue) {
		useModuleImpulseCounter = newValue;
		wpEEPROM.saveBool(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1, wpEEPROM.bitUseImpulseCounter, useModuleImpulseCounter);
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleImpulseCounter", useModuleImpulseCounter);
		wpFZ.DebugcheckSubscribes(mqttTopicUseImpulseCounter, String(useModuleImpulseCounter));
	}
}

void helperModules::publishAllSettings() {
	publishAllSettings(false);
}
void helperModules::publishAllSettings(bool force) {
	wpFZ.DebugWS(wpFZ.strDEBUG, "Modules::Settings", "Start publish");
	wpFZ.publishSettings(force);
	wpEEPROM.publishSettings(force);
	wpFinder.publishSettings(force);
	wpModules.publishSettings(force);
	wpMqtt.publishSettings(force);
	wpOnlineToggler.publishSettings(force);
	wpRest.publishSettings(force);
	wpUpdate.publishSettings(force);
	wpWebServer.publishSettings(force);
	wpWiFi.publishSettings(force);

	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
		wpDHT.publishSettings(force);
	}
	if(wpModules.useModuleLDR) {
		wpLDR.publishSettings(force);
	}
	if(wpModules.useModuleLight) {
		wpLight.publishSettings(force);
	}
	if(wpModules.useModuleBM) {
		wpBM.publishSettings(force);
	}
	if(wpModules.useModuleWindow) {
		wpWindow.publishSettings(force);
	}
	if(wpModules.useModuleCwWw) {
		wpCwWw.publishSettings(force);
	}
	if(wpModules.useModuleAnalogOut) {
		wpAnalogOut.publishSettings(force);
	}
	if(wpModules.useModuleAnalogOut2) {
		wpAnalogOut2.publishSettings(force);
	}
	if(wpModules.useModuleNeoPixel) {
		wpNeoPixel.publishSettings(force);
	}
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.publishSettings(force);
	}
	if(wpModules.useModuleRpm) {
		wpRpm.publishSettings(force);
	}
	if(wpModules.useModuleRain) {
		wpRain.publishSettings(force);
	}
	if(wpModules.useModuleMoisture) {
		wpMoisture.publishSettings(force);
	}
	if(wpModules.useModuleDistance) {
		wpDistance.publishSettings(force);
	}
	if(wpModules.useModuleImpulseCounter) {
		wpImpulseCounter.publishSettings(force);
	}
	wpFZ.DebugWS(wpFZ.strDEBUG, "Modules::Settings", "Stop publish");
}

void helperModules::publishAllValues() {
	publishAllValues(false);
}
void helperModules::publishAllValues(bool force) {
	wpFZ.publishValues(force);
	wpEEPROM.publishValues(force);
	wpFinder.publishValues(force);
	wpModules.publishValues(force);
	wpMqtt.publishValues(force);
	wpOnlineToggler.publishValues(force);
	wpRest.publishValues(force);
	wpUpdate.publishValues(force);
	wpWebServer.publishValues(force);
	wpWiFi.publishValues(force);

	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
		wpDHT.publishValues(force);
	}
	if(wpModules.useModuleLDR) {
		wpLDR.publishValues(force);
	}
	if(wpModules.useModuleLight) {
		wpLight.publishValues(force);
	}
	if(wpModules.useModuleBM) {
		wpBM.publishValues(force);
	}
	if(wpModules.useModuleWindow) {
		wpWindow.publishValues(force);
	}
	if(wpModules.useModuleCwWw) {
		wpCwWw.publishValues(force);
	}
	if(wpModules.useModuleAnalogOut) {
		wpAnalogOut.publishValues(force);
	}
	if(wpModules.useModuleAnalogOut2) {
		wpAnalogOut2.publishValues(force);
	}
	if(wpModules.useModuleNeoPixel) {
		wpNeoPixel.publishValues(force);
	}
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.publishValues(force);
	}
	if(wpModules.useModuleRpm) {
		wpRpm.publishValues(force);
	}
	if(wpModules.useModuleRain) {
		wpRain.publishValues(force);
	}
	if(wpModules.useModuleMoisture) {
		wpMoisture.publishValues(force);
	}
	if(wpModules.useModuleDistance) {
		wpDistance.publishValues(force);
	}
	if(wpModules.useModuleImpulseCounter) {
		wpImpulseCounter.publishValues(force);
	}
}

void helperModules::setAllSubscribes() {
	wpFZ.setSubscribes();
	wpEEPROM.setSubscribes();
	wpFinder.setSubscribes();
	wpModules.setSubscribes();
	wpMqtt.setSubscribes();
	wpOnlineToggler.setSubscribes();
	wpRest.setSubscribes();
	wpUpdate.setSubscribes();
	wpWebServer.setSubscribes();
	wpWiFi.setSubscribes();

	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
		wpDHT.setSubscribes();
	}
	if(wpModules.useModuleLDR) {
		wpLDR.setSubscribes();
	}
	if(wpModules.useModuleLight) {
		wpLight.setSubscribes();
	}
	if(wpModules.useModuleBM) {
		wpBM.setSubscribes();
	}
	if(wpModules.useModuleWindow) {
		wpWindow.setSubscribes();
	}
	if(wpModules.useModuleCwWw) {
		wpCwWw.setSubscribes();
	}
	if(wpModules.useModuleAnalogOut) {
		wpAnalogOut.setSubscribes();
	}
	if(wpModules.useModuleAnalogOut2) {
		wpAnalogOut2.setSubscribes();
	}
	if(wpModules.useModuleNeoPixel) {
		wpNeoPixel.setSubscribes();
	}
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.setSubscribes();
	}
	if(wpModules.useModuleRpm) {
		wpRpm.setSubscribes();
	}
	if(wpModules.useModuleRain) {
		wpRain.setSubscribes();
	}
	if(wpModules.useModuleMoisture) {
		wpMoisture.setSubscribes();
	}
	if(wpModules.useModuleDistance) {
		wpDistance.setSubscribes();
	}
	if(wpModules.useModuleImpulseCounter) {
		wpImpulseCounter.setSubscribes();
	}
}
void helperModules::checkAllSubscribes(char* topic, String msg) {
	wpFZ.checkSubscribes(topic, msg);
	wpEEPROM.checkSubscribes(topic, msg);
	wpFinder.checkSubscribes(topic, msg);
	wpModules.checkSubscribes(topic, msg);
	wpOnlineToggler.checkSubscribes(topic, msg);
	wpRest.checkSubscribes(topic, msg);
	wpUpdate.checkSubscribes(topic, msg);
	wpWebServer.checkSubscribes(topic, msg);
	wpWiFi.checkSubscribes(topic, msg);

	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
		wpDHT.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleLDR) {
		wpLDR.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleLight) {
		wpLight.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleBM) {
		wpBM.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleWindow) {
		wpWindow.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleCwWw) {
		wpCwWw.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleAnalogOut) {
		wpAnalogOut.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleAnalogOut2) {
		wpAnalogOut2.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleNeoPixel) {
		wpNeoPixel.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleRpm) {
		wpRpm.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleRain) {
		wpRain.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleMoisture) {
		wpMoisture.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleDistance) {
		wpDistance.checkSubscribes(topic, msg);
	}
	if(wpModules.useModuleImpulseCounter) {
		wpImpulseCounter.checkSubscribes(topic, msg);
	}
}
//###################################################################################
// private
//###################################################################################
