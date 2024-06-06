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
//# Revision     : $Rev:: 135                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperModules.cpp 135 2024-06-06 14:04:54Z               $ #
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
	mqttTopicUseRelais = wpFZ.DeviceName + "/settings/useModule/Relais";
	mqttTopicUseRelaisShield = wpFZ.DeviceName + "/settings/useModule/RelaisShield";
	mqttTopicUseRain = wpFZ.DeviceName + "/settings/useModule/Rain";
	mqttTopicUseMoisture = wpFZ.DeviceName + "/settings/useModule/Moisture";
	mqttTopicUseDistance = wpFZ.DeviceName + "/settings/useModule/Distance";

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

uint16_t helperModules::getVersion() {
	String SVN = "$Rev: 135 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperModules::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugModules, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
	EEPROM.commit();
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
	if(useDHT11Last != useModuleDHT11 || publishCountUseDHT11 > wpFZ.publishQoS) {
		useDHT11Last = useModuleDHT11;
		wpMqtt.mqttClient.publish(mqttTopicUseDHT11.c_str(), String(useModuleDHT11).c_str());
		wpFZ.SendWSModule("useDHT11", useModuleDHT11);
		publishCountUseDHT11 = 0;
	}
	if(useDHT22Last != useModuleDHT22 || publishCountUseDHT22 > wpFZ.publishQoS) {
		useDHT22Last = useModuleDHT22;
		wpMqtt.mqttClient.publish(mqttTopicUseDHT22.c_str(), String(useModuleDHT22).c_str());
		wpFZ.SendWSModule("useDHT22", useModuleDHT22);
		publishCountUseDHT22 = 0;
	}
	if(useLDRLast != useModuleLDR || publishCountUseLDR > wpFZ.publishQoS) {
		useLDRLast = useModuleLDR;
		wpMqtt.mqttClient.publish(mqttTopicUseLDR.c_str(), String(useModuleLDR).c_str());
		wpFZ.SendWSModule("useLDR", useModuleLDR);
		publishCountUseLDR = 0;
	}
	if(useLightLast != useModuleLight || publishCountUseLight > wpFZ.publishQoS) {
		useLightLast = useModuleLight;
		wpMqtt.mqttClient.publish(mqttTopicUseLight.c_str(), String(useModuleLight).c_str());
		wpFZ.SendWSModule("useLight", useModuleLight);
		publishCountUseLight = 0;
	}
	if(useBMLast != useModuleBM || publishCountUseBM > wpFZ.publishQoS) {
		useBMLast = useModuleBM;
		wpMqtt.mqttClient.publish(mqttTopicUseBM.c_str(), String(useModuleBM).c_str());
		wpFZ.SendWSModule("useBM", useModuleBM);
		publishCountUseBM = 0;
	}
	if(useRelaisLast != useModuleRelais || publishCountUseRelais > wpFZ.publishQoS) {
		useRelaisLast = useModuleRelais;
		wpMqtt.mqttClient.publish(mqttTopicUseRelais.c_str(), String(useModuleRelais).c_str());
		wpFZ.SendWSModule("useRelais", useModuleRelais);
		publishCountUseRelais = 0;
	}
	if(useRelaisShieldLast != useModuleRelaisShield || publishCountUseRelaisShield > wpFZ.publishQoS) {
		useRelaisShieldLast = useModuleRelaisShield;
		wpMqtt.mqttClient.publish(mqttTopicUseRelaisShield.c_str(), String(useModuleRelaisShield).c_str());
		wpFZ.SendWSModule("useRelaisShield", useModuleRelaisShield);
		publishCountUseRelaisShield = 0;
	}
	if(useRainLast != useModuleRain || publishCountUseRain > wpFZ.publishQoS) {
		useRainLast = useModuleRain;
		wpMqtt.mqttClient.publish(mqttTopicUseRain.c_str(), String(useModuleRain).c_str());
		wpFZ.SendWSModule("useRain", useModuleRain);
		publishCountUseRain = 0;
	}
	if(useMoistureLast != useModuleMoisture || publishCountUseMoisture > wpFZ.publishQoS) {
		useMoistureLast = useModuleMoisture;
		wpMqtt.mqttClient.publish(mqttTopicUseMoisture.c_str(), String(useModuleMoisture).c_str());
		wpFZ.SendWSModule("useMoisture", useModuleMoisture);
		publishCountUseMoisture = 0;
	}
	if(useDistanceLast != useModuleDistance || publishCountUseDistance > wpFZ.publishQoS) {
		useDistanceLast = useModuleDistance;
		wpMqtt.mqttClient.publish(mqttTopicUseDistance.c_str(), String(useModuleDistance).c_str());
		wpFZ.SendWSModule("useDistance", useModuleDistance);
		publishCountUseDistance = 0;
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
	if(force) publishCountDebug = wpFZ.publishQoS;
}

void helperModules::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicUseDHT11.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseDHT22.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseLDR.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseLight.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseBM.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseRelais.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseRelaisShield.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseRain.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseMoisture.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseDistance.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void helperModules::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicUseDHT11.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleDHT11(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseDHT22.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleDHT22(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseLDR.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleLDR(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseLight.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleLight(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseMoisture.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleBM(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseRelais.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleRelais(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseRelaisShield.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleRelaisShield(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseBM.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleRain(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseRain.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleMoisture(readUseModule);
	}
	if(strcmp(topic, mqttTopicUseDistance.c_str()) == 0) {
		bool readUseModule = msg.toInt();
		changeModuleDistance(readUseModule);
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugModules, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
			EEPROM.commit();
			wpFZ.SendWSDebug("DebugModules", Debug);
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}
void helperModules::changeModuleDHT11(bool newValue) {
	if(useModuleDHT11 != newValue) {
		useModuleDHT11 = newValue;
		bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseDHT11, useModuleDHT11);
		EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleDHT11", useModuleDHT11);
		wpFZ.DebugcheckSubscribes(mqttTopicUseDHT11, String(Debug));
	}
}
void helperModules::changeModuleDHT22(bool newValue) {
	if(useModuleDHT22 != newValue) {
		useModuleDHT22 = newValue;
		bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseDHT22, useModuleDHT22);
		EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleDHT22", useModuleDHT22);
		wpFZ.DebugcheckSubscribes(mqttTopicUseDHT22, String(Debug));
	}
}
void helperModules::changeModuleLDR(bool newValue) {
	if(useModuleLDR != newValue) {
		useModuleLDR = newValue;
		bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseLDR, useModuleLDR);
		EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleLDR", useModuleLDR);
		wpFZ.DebugcheckSubscribes(mqttTopicUseLDR, String(Debug));
	}
}
void helperModules::changeModuleLight(bool newValue) {
	if(useModuleLight != newValue) {
		useModuleLight = newValue;
		bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseLight, useModuleLight);
		EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleLight", useModuleLight);
		wpFZ.DebugcheckSubscribes(mqttTopicUseLight, String(Debug));
	}
}
void helperModules::changeModuleBM(bool newValue) {
	if(useModuleMoisture != newValue) {
		useModuleMoisture = newValue;
		bitWrite(wpEEPROM.bitsModules1, wpEEPROM.bitUseMoisture, useModuleMoisture);
		EEPROM.write(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleMoisture", useModuleMoisture);
		wpFZ.DebugcheckSubscribes(mqttTopicUseMoisture, String(useModuleMoisture));
	}
}
void helperModules::changeModuleRelais(bool newValue) {
	if(useModuleRelais != newValue) {
		useModuleRelais = newValue;
		bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseRelais, useModuleRelais);
		EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleRelais", useModuleRelais);
		wpFZ.DebugcheckSubscribes(mqttTopicUseRelais, String(useModuleRelais));
	}
}
void helperModules::changeModuleRelaisShield(bool newValue) {
	if(useModuleRelaisShield != newValue) {
		useModuleRelaisShield = newValue;
		bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseRelaisShield, useModuleRelaisShield);
		EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleRelaisShield", useModuleRelaisShield);
		wpFZ.DebugcheckSubscribes(mqttTopicUseRelaisShield, String(useModuleRelaisShield));
	}
}
void helperModules::changeModuleRain(bool newValue) {
	if(useModuleBM != newValue) {
		useModuleBM = newValue;
		bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseBM, useModuleBM);
		EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleBM", useModuleBM);
		wpFZ.DebugcheckSubscribes(mqttTopicUseBM, String(useModuleBM));
	}
}
void helperModules::changeModuleMoisture(bool newValue) {
	if(useModuleRain != newValue) {
		useModuleRain = newValue;
		bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseRain, useModuleRain);
		EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleRain", useModuleRain);
		wpFZ.DebugcheckSubscribes(mqttTopicUseRain, String(useModuleRain));
	}
}
void helperModules::changeModuleDistance(bool newValue) {
	if(useModuleDistance != newValue) {
		useModuleDistance = newValue;
		bitWrite(wpEEPROM.bitsModules1, wpEEPROM.bitUseDistance, useModuleDistance);
		EEPROM.write(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1);
		EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.SendWSDebug("useModuleDistance", useModuleDistance);
		wpFZ.DebugcheckSubscribes(mqttTopicUseDistance, String(useModuleDistance));
	}
}


void helperModules::publishAllSettings() {
	publishAllSettings(false);
}
void helperModules::publishAllSettings(bool force) {
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
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.publishSettings(force);
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
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.publishValues(force);
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
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.setSubscribes();
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
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.checkSubscribes(topic, msg);
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
}
//###################################################################################
// private
//###################################################################################
