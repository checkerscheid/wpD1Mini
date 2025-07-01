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
//# Revision     : $Rev:: 258                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBM.cpp 258 2025-04-28 13:34:51Z                    $ #
//#                                                                                 #
//###################################################################################
#include <moduleBM.h>

moduleBM wpBM;

moduleBM::moduleBM() {
	// section to config and copy
	ModuleName = "BM";
	mb = new moduleBase(ModuleName);
}
void moduleBM::init() {
	// section for define
	Pin = D5;
	pinMode(Pin, INPUT_PULLUP);
	digitalWrite(Pin, HIGH);
	bm = true;
	manual = false;
	mqttTopicBM = wpFZ.DeviceName + "/" + ModuleName + "/Output";
	mqttTopicManual = wpFZ.DeviceName + "/" + ModuleName + "/Manual";
	mqttTopicThreshold = wpFZ.DeviceName + "/settings/" + ModuleName + "/Threshold";
	mqttTopicLightToTurnOn = wpFZ.DeviceName + "/settings/" + ModuleName + "/LightToTurnOn";

	bmLast = 0;
	publishBMLast = 0;

	// section to copy

	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugBM);

}

//###################################################################################
void moduleBM::cycle() {
	if(wpFZ.calcValues) {
		calc();
	}
	publishValues();
}
void moduleBM::publishSettings() {
	publishSettings(false);
}
void moduleBM::publishSettings(bool force) {
	if(wpModules.useModuleLDR) {
		wpMqtt.mqttClient.publish(mqttTopicThreshold.c_str(), String(threshold).c_str());
		wpMqtt.mqttClient.publish(mqttTopicLightToTurnOn.c_str(), lightToTurnOn.c_str());
	}
	mb->publishSettings(force);
}
void moduleBM::publishValues() {
	publishValues(false);
}
void moduleBM::publishValues(bool force) {
	if(force) {
		publishBMLast = 0;
		publishManualLast = 0;
	}
	if(bmLast != bm || wpFZ.CheckQoS(publishBMLast)) {
		publishValue();
	}
	if(manualLast != manual || wpFZ.CheckQoS(publishManualLast)) {
		manualLast = manual;
		wpMqtt.mqttClient.publish(mqttTopicManual.c_str(), String(manual).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicManual, String(manual));
		}
		publishManualLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}
void moduleBM::setSubscribes() {
	if(wpModules.useModuleLDR) {
		wpMqtt.mqttClient.subscribe(mqttTopicThreshold.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicLightToTurnOn.c_str());
	}
	mb->setSubscribes();
}
void moduleBM::checkSubscribes(char* topic, String msg) {
	if(wpModules.useModuleLDR) {
		if(strcmp(topic, mqttTopicThreshold.c_str()) == 0) {
			uint16_t readThreshold = msg.toInt();
			if(threshold != readThreshold) {
				threshold = readThreshold;
				wpEEPROM.WriteWordToEEPROM("BM Threshold", wpEEPROM.byteBMThreshold, threshold);
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
	mb->checkSubscribes(topic, msg);
}
void moduleBM::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicBM.c_str(), String(bm).c_str());
	bmLast = bm;
	if(wpModules.useModuleLDR) {
		if(bm && wpLDR.ldr <= threshold && !manual) {
			String lm = "MQTT Set Light (" + String(wpLDR.ldr) + " <= " + String(threshold) + ")";
			if(!lightToTurnOn.startsWith("_")) {
				if(lightToTurnOn.startsWith("http://")) {
					wpFZ.sendRawRest(lightToTurnOn);
					lm += ", send REST '" + lightToTurnOn + "'";
				} else {
					wpMqtt.mqttClient.publish(lightToTurnOn.c_str(), String("on").c_str());
					lm += ", send MQTT '" + lightToTurnOn + "': 'On'";
				}
			}
			wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", lm);
		}
	}
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("BM", String(bm));
	}
	publishBMLast = wpFZ.loopStartedAt;
}
void moduleBM::calc() {
	if(digitalRead(Pin) == LOW) {
		bm = false;
	} else {
		if(bm == false) {
			wpFZ.blink();
			if(mb->debug) {
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcBM", "Bewegung erkannt");
			}
		}
		bm = true;
	}
}

String moduleBM::SetAuto() {
	manual = false;
	return "{\"erg\":\"S_OK\",\"mode\":\"auto\"}";
}
String moduleBM::SetManual() {
	manual = true;
	return "{\"erg\":\"S_OK\",\"mode\":\"manual\"}";
}

//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleBM::getVersion() {
	String SVN = "$Rev: 258 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleBM::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin]));
	if(wpModules.useModuleLDR) {
		json += ",\"LDR\":{" +
			wpFZ.JsonKeyValue(F("Threshold"), String(threshold)) + F(",") +
			wpFZ.JsonKeyString(F("LightToTurnOn"), lightToTurnOn) +
			F("}");
	}
	json += F("}");
	return json;
}

bool moduleBM::Debug() {
	return mb->debug;
}
bool moduleBM::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
void moduleBM::changeDebug() {
	mb->changeDebug();
}
