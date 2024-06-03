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
//# Revision     : $Rev:: 127                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperModules.cpp 127 2024-06-03 11:49:01Z               $ #
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
	String SVN = "$Rev: 127 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperModules::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugModules, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugModules\",\"value\":" + String(Debug ? "true" : "false") + "}");
	wpFZ.blink();
}

void helperModules::publishSettings() {
	publishSettings(false);
}
void helperModules::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicUseDHT11.c_str(), String(useModuleDHT11).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseDHT22.c_str(), String(useModuleDHT22).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseLDR.c_str(), String(useModuleLDR).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseLight.c_str(), String(useModuleLight).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseBM.c_str(), String(useModuleBM).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseRelais.c_str(), String(useModuleRelais).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseRelaisShield.c_str(), String(useModuleRelaisShield).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseRain.c_str(), String(useModuleRain).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseMoisture.c_str(), String(useModuleMoisture).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseDistance.c_str(), String(useModuleDistance).c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperModules::publishValues() {
	publishValues(false);
}
void helperModules::publishValues(bool force) {
	if(force) publishCountDebug = wpFZ.publishQoS;
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
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
		bool readUseDHT11 = msg.toInt();
		if(useModuleDHT11 != readUseDHT11) {
			useModuleDHT11 = readUseDHT11;
			bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseDHT11, useModuleDHT11);
			EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleDHT11\",\"value\":" + String(useModuleDHT11 ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseDHT11, String(Debug));
		}
	}
	if(strcmp(topic, mqttTopicUseDHT22.c_str()) == 0) {
		bool readUseDHT22 = msg.toInt();
		if(useModuleDHT22 != readUseDHT22) {
			useModuleDHT22 = readUseDHT22;
			bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseDHT22, useModuleDHT22);
			EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleDHT22\",\"value\":" + String(useModuleDHT22 ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseDHT22, String(Debug));
		}
	}

	if(strcmp(topic, mqttTopicUseLDR.c_str()) == 0) {
		bool readUseLDR = msg.toInt();
		if(useModuleLDR != readUseLDR) {
			useModuleLDR = readUseLDR;
			bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseLDR, useModuleLDR);
			EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleLDR\",\"value\":" + String(useModuleLDR ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseLDR, String(Debug));
		}
	}

	if(strcmp(topic, mqttTopicUseLight.c_str()) == 0) {
		bool readUseLight = msg.toInt();
		if(useModuleLight != readUseLight) {
			useModuleLight = readUseLight;
			bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseLight, useModuleLight);
			EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleLight\",\"value\":" + String(useModuleLight ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseLight, String(Debug));
		}
	}

	if(strcmp(topic, mqttTopicUseMoisture.c_str()) == 0) {
		bool readUseMoisture = msg.toInt();
		if(useModuleMoisture != readUseMoisture) {
			useModuleMoisture = readUseMoisture;
			bitWrite(wpEEPROM.bitsModules1, wpEEPROM.bitUseMoisture, useModuleMoisture);
			EEPROM.write(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleMoisture\",\"value\":" + String(useModuleMoisture ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseMoisture, String(useModuleMoisture));
		}
	}

	if(strcmp(topic, mqttTopicUseRelais.c_str()) == 0) {
		bool readUseRelais = msg.toInt();
		if(useModuleRelais != readUseRelais) {
			useModuleRelais = readUseRelais;
			bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseRelais, useModuleRelais);
			EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleRelais\",\"value\":" + String(useModuleRelais ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseRelais, String(useModuleRelais));
		}
	}
	if(strcmp(topic, mqttTopicUseRelaisShield.c_str()) == 0) {
		bool readUseRelaisShield = msg.toInt();
		if(useModuleRelaisShield != readUseRelaisShield) {
			useModuleRelaisShield = readUseRelaisShield;
			bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseRelaisShield, useModuleRelaisShield);
			EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleRelaisShield\",\"value\":" + String(useModuleRelaisShield ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseRelaisShield, String(useModuleRelaisShield));
		}
	}

	if(strcmp(topic, mqttTopicUseBM.c_str()) == 0) {
		bool readUseBM = msg.toInt();
		if(useModuleBM != readUseBM) {
			useModuleBM = readUseBM;
			bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseBM, useModuleBM);
			EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleBM\",\"value\":" + String(useModuleBM ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseBM, String(useModuleBM));
		}
	}
	if(strcmp(topic, mqttTopicUseRain.c_str()) == 0) {
		bool readUseRain = msg.toInt();
		if(useModuleRain != readUseRain) {
			useModuleRain = readUseRain;
			bitWrite(wpEEPROM.bitsModules0, wpEEPROM.bitUseRain, useModuleRain);
			EEPROM.write(wpEEPROM.addrBitsModules0, wpEEPROM.bitsModules0);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleRain\",\"value\":" + String(useModuleRain ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseRain, String(useModuleRain));
		}
	}
	if(strcmp(topic, mqttTopicUseDistance.c_str()) == 0) {
		bool readUseDistance = msg.toInt();
		if(useModuleDistance != readUseDistance) {
			useModuleDistance = readUseDistance;
			bitWrite(wpEEPROM.bitsModules1, wpEEPROM.bitUseDistance, useModuleDistance);
			EEPROM.write(wpEEPROM.addrBitsModules1, wpEEPROM.bitsModules1);
			EEPROM.commit();
			wpFZ.restartRequired = true;
			wpFZ.SendWS("{\"id\":\"useModuleDistance\",\"value\":" + String(useModuleDistance ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicUseDistance, String(useModuleDistance));
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugModules, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
			EEPROM.commit();
			wpFZ.SendWS("{\"id\":\"DebugModules\",\"value\":" + String(Debug ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
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
