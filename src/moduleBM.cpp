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
//# Revision     : $Rev:: 172                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBM.cpp 172 2024-07-23 22:01:24Z                    $ #
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
	BMPin = D5;
	pinMode(BMPin, INPUT_PULLUP);
	bm = true;
	mqttTopicBM = wpFZ.DeviceName + "/" + ModuleName;
	mqttTopicThreshold = wpFZ.DeviceName + "/settings/" + ModuleName + "/Threshold";
	mqttTopicLightToTurnOn = wpFZ.DeviceName + "/settings/" + ModuleName + "/LightToTurnOn";

	bmLast = 0;
	publishCountBM = 0;

	// section to copy
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/" + ModuleName + "/maxCycle";

	mb->initRest(wpEEPROM.addrBitsSendRestModules0, wpEEPROM.bitsSendRestModules0, wpEEPROM.bitSendRestBM);
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
		publishCountBM = wpFZ.publishQoS;
	}
	if(bmLast != bm || ++publishCountBM > wpFZ.publishQoS) {
		publishValue();
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
			uint16 readThreshold = msg.toInt();
			if(threshold != readThreshold) {
				threshold = readThreshold;
				EEPROM.put(wpEEPROM.byteBMThreshold, threshold);
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
void moduleBM::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicBM.c_str(), String(bm).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("bm", bm ? "true" : "false");
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
		printPublishValueDebug("BM", String(bm), String(publishCountBM));
	}
	publishCountBM = 0;
}
void moduleBM::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
void moduleBM::calc() {
	if(digitalRead(BMPin) == LOW) {
		if(bm == false) {
			wpFZ.blink();
			if(mb->debug) {
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcBM", "Bewegung erkannt");
			}
		}
		bm = true;
	} else {
		bm = false;
	}
}


//###################################################################################
// section to copy
//###################################################################################
uint16 moduleBM::getVersion() {
	String SVN = "$Rev: 172 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

bool moduleBM::SendRest() {
	return mb->sendRest;
}
bool moduleBM::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleBM::Debug() {
	return mb->debug;
}
bool moduleBM::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
void moduleBM::changeSendRest() {
	mb->changeSendRest();
}
void moduleBM::changeDebug() {
	mb->changeDebug();
}
