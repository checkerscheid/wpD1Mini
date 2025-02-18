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
//# Revision     : $Rev:: 234                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleWindow.cpp 234 2024-12-19 15:51:45Z                $ #
//#                                                                                 #
//###################################################################################
#include <moduleWindow.h>

moduleWindow wpWindow1(1);
moduleWindow wpWindow2(2);
moduleWindow wpWindow3(3);

moduleWindow::moduleWindow(uint8 n) {
	no = n;
	ModuleName = "Window" + String(no);
	mb = new moduleBase(ModuleName);
}
void moduleWindow::init() {
	switch(no) {
		case 1:
			Pin = D6;
			bitDebug = wpEEPROM.bitDebugWindow;
			bitsDebug = wpEEPROM.bitsDebugModules1;
			addrDebug = wpEEPROM.addrBitsDebugModules1;
			break;
		case 2:
			Pin = D1;
			bitDebug = wpEEPROM.bitDebugWindow2;
			bitsDebug = wpEEPROM.bitsDebugModules2;
			addrDebug = wpEEPROM.addrBitsDebugModules2;
			break;
		case 3:
			Pin = D2;
			bitDebug = wpEEPROM.bitDebugWindow3;
			bitsDebug = wpEEPROM.bitsDebugModules2;
			addrDebug = wpEEPROM.addrBitsDebugModules2;
			break;
		default:
			wpFZ.DebugWS(wpFZ.strERRROR, "Window::init", "FatalError: unknown Window no: " + String(no));
			break;
	}
	// section for define
	pinMode(Pin, INPUT_PULLUP);
	bm = true;
	mqttTopicBM = wpFZ.DeviceName + "/" + ModuleName;

	bmLast = 0;
	publishBMLast = 0;

	// section to copy

	mb->initDebug(addrDebug, bitsDebug, bitDebug);

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
	mb->publishSettings(force);
}
void moduleWindow::publishValues() {
	publishValues(false);
}
void moduleWindow::publishValues(bool force) {
	if(force) {
		publishBMLast = 0;
	}
	if(bmLast != bm || wpFZ.CheckQoS(publishBMLast)) {
		publishValue();
	}
	mb->publishValues(force);
}
void moduleWindow::setSubscribes() {
	mb->setSubscribes();
}
void moduleWindow::checkSubscribes(char* topic, String msg) {
	mb->checkSubscribes(topic, msg);
}
void moduleWindow::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicBM.c_str(), String(bm).c_str());
	bmLast = bm;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug(ModuleName, String(bm));
	}
	publishBMLast = wpFZ.loopStartedAt;
}
void moduleWindow::calc() {
	if(digitalRead(Pin) == LOW) {
		bm = false;
	} else {
		if(bm == false) {
			wpFZ.blink();
			if(mb->debug) {
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcWindow", ModuleName + ": Fenster offen");
			}
		}
		bm = true;
	}
}


//###################################################################################
// section to copy
//###################################################################################
uint16 moduleWindow::getVersion() {
	String SVN = "$Rev: 234 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleWindow::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) +
		F("}");
	return json;
}

bool moduleWindow::Debug() {
	return mb->debug;
}
bool moduleWindow::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
void moduleWindow::changeDebug() {
	mb->changeDebug();
}
