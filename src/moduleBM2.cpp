//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 18.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 139                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBM.cpp 139 2024-06-11 10:08:54Z                    $ #
//#                                                                                 #
//###################################################################################
#include <moduleBM2.h>

moduleBM2 wpBM2;

moduleBM2::moduleBM2() {
	// section to config and copy
	ModuleName = "BM2";
	mb = new moduleBase(ModuleName);
}
void moduleBM2::init() {
	// section for define
	BMPin = D6;
	pinMode(BMPin, INPUT_PULLUP);
	bm = 0;
	mqttTopicBM = wpFZ.DeviceName + "/" + ModuleName;
	mqttTopicThreshold = wpFZ.DeviceName + "/settings/" + ModuleName + "/Threshold";
	mqttTopicLightToTurnOn = wpFZ.DeviceName + "/settings/" + ModuleName + "/LightToTurnOn";

	bmLast = 0;
	publishCountBM = 0;

	// section to copy
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/" + ModuleName + "/maxCycle";

	mb->initRest(wpEEPROM.addrBitsSendRestModules1, wpEEPROM.bitsSendRestModules1, wpEEPROM.bitSendRestBM2);
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugBM2);

}

//###################################################################################
void moduleBM2::cycle() {
	if(wpFZ.calcValues) {
		calc();
	}
	publishValues();
}
void moduleBM2::publishSettings() {
	publishSettings(false);
}
void moduleBM2::publishSettings(bool force) {
	if(wpModules.useModuleLDR) {
		wpMqtt.mqttClient.publish(mqttTopicThreshold.c_str(), String(threshold).c_str());
		wpMqtt.mqttClient.publish(mqttTopicLightToTurnOn.c_str(), lightToTurnOn.c_str());
	}
	mb->publishSettings(force);
}
void moduleBM2::publishValues() {
	publishValues(false);
}
void moduleBM2::publishValues(bool force) {
	if(force) {
		publishCountBM = wpFZ.publishQoS;
	}
	if(bmLast != bm || ++publishCountBM > wpFZ.publishQoS) {
		publishValue();
	}
	mb->publishValues(force);
}
void moduleBM2::setSubscribes() {
	if(wpModules.useModuleLDR) {
		wpMqtt.mqttClient.subscribe(mqttTopicThreshold.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicLightToTurnOn.c_str());
	}
	mb->setSubscribes();
}
void moduleBM2::checkSubscribes(char* topic, String msg) {
	if(wpModules.useModuleLDR) {
		if(strcmp(topic, mqttTopicThreshold.c_str()) == 0) {
			uint16_t readThreshold = msg.toInt();
			if(threshold != readThreshold) {
				threshold = readThreshold;
				EEPROM.put(wpEEPROM.byteBM2Threshold, threshold);
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
	mb->checkSubscribes(topic, msg);
}
void moduleBM2::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicBM.c_str(), String(bm).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("bm2", String(bm));
		wpRest.trySend = true;
	}
	bmLast = bm;
	if(wpModules.useModuleLDR) {
		if(bm && wpLDR.ldr <= threshold) {
			String lm = "MQTT Set Light (" + String(wpLDR.ldr) + " <= " + String(threshold) + ")";
			if(!lightToTurnOn.startsWith("_")) {
				if(lightToTurnOn.startsWith("http://")) {
					wpRest.error = wpRest.error | !wpRest.sendRawRest(lightToTurnOn);
					wpRest.trySend = true;
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
		printPublishValueDebug("BM2", String(bm), String(publishCountBM));
	}
	publishCountBM = 0;
}
void moduleBM2::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
void moduleBM2::calc() {
	if(digitalRead(BMPin) == LOW) {
		bm = false;
	} else {
		if(bm == false) {
			bm = true;
			wpFZ.blink();
			if(mb->debug) {
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcBM2", "Bewegung erkannt");
			}
		}
	}
}


//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleBM2::getVersion() {
	String SVN = "$Rev: 139 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

bool moduleBM2::SendRest() {
	return mb->sendRest;
}
bool moduleBM2::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleBM2::Debug() {
	return mb->debug;
}
bool moduleBM2::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
void moduleBM2::changeSendRest() {
	mb->changeSendRest();
}
void moduleBM2::changeDebug() {
	mb->changeDebug();
}
