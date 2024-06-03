//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 02.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 125                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLight.cpp 125 2024-06-03 03:11:11Z                 $ #
//#                                                                                 #
//###################################################################################
#include <moduleBM.h>

moduleBM wpBM;

moduleBM::moduleBM() {}
void moduleBM::init() {
	BMPin = D5;
	pinMode(BMPin, INPUT_PULLUP);
	bm = 0;
	// values
	mqttTopicBM = wpFZ.DeviceName + "/BM";
	// settings
	mqttTopicThreshold = wpFZ.DeviceName + "/settings/BM/Threshold";
	mqttTopicLightToTurnOn = wpFZ.DeviceName + "/settings/BM/LightToTurnOn";
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/BM";

	bmLast = 0;
	publishCountBM = 0;
	DebugLast = false;
	publishCountDebug = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleBM::cycle() {
	if(wpFZ.calcValues) {
		calc();
	}
	publishValues();
}

uint16_t moduleBM::getVersion() {
	String SVN = "$Rev: 125 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleBM::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugBM, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugBM\",\"value\":" + String(Debug ? "true" : "false") + "}");
	wpFZ.blink();
}

void moduleBM::publishSettings() {
	publishSettings(false);
}
void moduleBM::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void moduleBM::publishValues() {
	publishValues(false);
}
void moduleBM::publishValues(bool force) {
	if(force) {
		publishCountBM = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
	}
	if(bmLast != bm || ++publishCountBM > wpFZ.publishQoS) {
		publishValue();
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void moduleBM::setSubscribes() {
	if(wpModules.useModuleLDR) {
		wpMqtt.mqttClient.subscribe(mqttTopicThreshold.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicLightToTurnOn.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}

void moduleBM::checkSubscribes(char* topic, String msg) {
	if(wpModules.useModuleLDR) {
		if(strcmp(topic, mqttTopicThreshold.c_str()) == 0) {
			uint16_t readThreshold = msg.toInt();
			if(threshold != readThreshold) {
				threshold = readThreshold;
				EEPROM.put(wpEEPROM.byteThreshold, threshold);
				EEPROM.commit();
				wpFZ.DebugcheckSubscribes(mqttTopicThreshold, String(threshold));
			}
		}
		if(strcmp(topic, mqttTopicLightToTurnOn.c_str()) == 0) {
			if(strcmp(msg.c_str(), lightToTurnOn.c_str()) != 0) {
				lightToTurnOn = msg;
				wpEEPROM.writeStringsToEEPROM();
				wpFZ.DebugcheckSubscribes(mqttTopicLightToTurnOn, lightToTurnOn);
			}
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugDHT, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
			wpFZ.SendWS("{\"id\":\"DebugDHT\",\"value\":" + String(Debug ? "true" : "false") + "}");
		}
	}
}

//###################################################################################
// private
//###################################################################################
void moduleBM::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicBM.c_str(), String(bm).c_str());
	wpRest.error = wpRest.error | !wpRest.sendRest("bm", String(bm));
	wpRest.trySend = true;
	bmLast = bm;
	if(wpModules.useModuleLDR) {
		if(bm && wpLDR.LDR <= threshold) {
			String lm = "MQTT Set Light (" + String(wpLDR.LDR) + " <= " + String(threshold) + ")";
			if(!lightToTurnOn.startsWith("_")) {
				if(lightToTurnOn.startsWith("http://")) {
					wpRest.error = wpRest.error | !wpRest.sendRawRest(lightToTurnOn);
					wpRest.trySend = true;
					lm += ", send REST '" + lightToTurnOn + "': 'On'";
				} else {
					wpMqtt.mqttClient.publish(lightToTurnOn.c_str(), String("on").c_str());
					lm += ", send MQTT '" + lightToTurnOn + "': 'On'";
				}
			}
			wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", lm);
		}
	}
	if(wpMqtt.Debug) {
		printPublishValueDebug("BM", String(bm), String(publishCountBM));
	}
	publishCountBM = 0;
}

void moduleBM::calc() {
	if(digitalRead(BMPin) == LOW) {
		bm = false;
	} else {
		if(bm == false) {
			bm = true;
			wpFZ.blink();
			if(Debug) {
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcBM", "Bewegung erkannt");
			}
		}
	}
}

void moduleBM::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
