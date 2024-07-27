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
//# Revision     : $Rev:: 179                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleWindow.cpp 179 2024-07-26 06:43:08Z                $ #
//#                                                                                 #
//###################################################################################
#include <moduleWindow.h>

moduleWindow wpWindow;

moduleWindow::moduleWindow() {
	// section to config and copy
	ModuleName = "Window";
	mb = new moduleBase(ModuleName);
}
void moduleWindow::init() {
	// section for define
	Pin = D6;
	pinMode(Pin, INPUT_PULLUP);
	digitalWrite(Pin, HIGH);
	bm = true;
	mqttTopicBM = wpFZ.DeviceName + "/" + ModuleName;
	mqttTopicThreshold = wpFZ.DeviceName + "/settings/" + ModuleName + "/Threshold";
	mqttTopicLightToTurnOn = wpFZ.DeviceName + "/settings/" + ModuleName + "/LightToTurnOn";

	bmLast = 0;
	publishCountBM = 0;

	// section to copy

	mb->initRest(wpEEPROM.addrBitsSendRestModules1, wpEEPROM.bitsSendRestModules1, wpEEPROM.bitSendRestWindow);
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugWindow);

}

//###################################################################################
void moduleWindow::cycle() {
	if(wpFZ.calcValues) {
		calc();
	}
	publishValues();
}
void moduleWindow::publishSettings() {
	publishSettings(false);
}
void moduleWindow::publishSettings(bool force) {
	if(wpModules.useModuleLDR) {
		wpMqtt.mqttClient.publish(mqttTopicThreshold.c_str(), String(threshold).c_str());
		wpMqtt.mqttClient.publish(mqttTopicLightToTurnOn.c_str(), lightToTurnOn.c_str());
	}
	mb->publishSettings(force);
}
void moduleWindow::publishValues() {
	publishValues(false);
}
void moduleWindow::publishValues(bool force) {
	if(force) {
		publishCountBM = wpFZ.publishQoS;
	}
	if(bmLast != bm || ++publishCountBM > wpFZ.publishQoS) {
		publishValue();
	}
	mb->publishValues(force);
}
void moduleWindow::setSubscribes() {
	if(wpModules.useModuleLDR) {
		wpMqtt.mqttClient.subscribe(mqttTopicThreshold.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicLightToTurnOn.c_str());
	}
	mb->setSubscribes();
}
void moduleWindow::checkSubscribes(char* topic, String msg) {
	if(wpModules.useModuleLDR) {
		if(strcmp(topic, mqttTopicThreshold.c_str()) == 0) {
			uint16 readThreshold = msg.toInt();
			if(threshold != readThreshold) {
				threshold = readThreshold;
				EEPROM.put(wpEEPROM.byteWindowThreshold, threshold);
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
void moduleWindow::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicBM.c_str(), String(bm).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("window", bm ? "true" : "false");
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
		printPublishValueDebug("Window", String(bm), String(publishCountBM));
	}
	publishCountBM = 0;
}
void moduleWindow::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
void moduleWindow::calc() {
	if(digitalRead(Pin) == LOW) {
		bm = false;
	} else {
		if(bm == false) {
			wpFZ.blink();
			if(mb->debug) {
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcWindow", "Fenster offen");
			}
		}
		bm = true;
	}
}


//###################################################################################
// section to copy
//###################################################################################
uint16 moduleWindow::getVersion() {
	String SVN = "$Rev: 179 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

bool moduleWindow::SendRest() {
	return mb->sendRest;
}
bool moduleWindow::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleWindow::Debug() {
	return mb->debug;
}
bool moduleWindow::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
void moduleWindow::changeSendRest() {
	mb->changeSendRest();
}
void moduleWindow::changeDebug() {
	mb->changeDebug();
}
