//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 22.07.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 199                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleCwWw.cpp 199 2024-09-05 12:33:52Z                  $ #
//#                                                                                 #
//###################################################################################
#include <moduleCwWw.h>

moduleCwWw wpCwWw;
// AnalogOut = W || WW / D6
// AnalogOut2 = CW / D5
moduleCwWw::moduleCwWw() {
	// section to config and copy
	ModuleName = "CwWw";
	mb = new moduleBase(ModuleName);

	loopTime = 25;
	steps = 1;
}
void moduleCwWw::init() {
	sleep = 0;
	modeCurrent = 0;       // Current Pattern Number

	// values
	mqttTopicSleep = wpFZ.DeviceName + "/" + ModuleName + "/Sleep";
	mqttTopicModeName = wpFZ.DeviceName + "/" + ModuleName + "/ModeName";
	// settings
	// commands
	mqttTopicSetMode = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetMode";
	mqttTopicSetSleep = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetSleep";

	publishValueLast = 0;
	publishModeLast = 0;

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules1, wpEEPROM.bitsSendRestModules1, wpEEPROM.bitSendRestNeoPixel);
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugNeoPixel);
}

// loop() function -- runs repeatedly as long as board is on ---------------
void moduleCwWw::cycle() {
	if(wpFZ.calcValues) {
		calc();
	}
	publishValues();

	wpModules.useModuleAnalogOut = true;
	wpModules.useModuleAnalogOut2 = true;
	// RGB LED has CW + WW
	// use AnalogOut for WW or White
	// use AnalogOut2 for CW with WW
	// must have output limitations?
	//wpAnalogOut.hardwareoutMax = 50;
	//wpAnalogOut2.hardwareoutMax = 50;
}

void moduleCwWw::publishSettings() {
	publishSettings(false);
}
void moduleCwWw::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetMode.c_str(), String(modeCurrent).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetSleep.c_str(), String(sleep).c_str());
	}
	mb->publishSettings(force);
}

void moduleCwWw::publishValues() {
	publishValues(false);
}
void moduleCwWw::publishValues(bool force) {
	if(force) {
		publishValueLast = 0;
		publishModeLast = 0;
	}
	if(modeCurrentLast != modeCurrent || wpFZ.CheckQoS(publishModeLast)) {
		modeCurrentLast = modeCurrent;
		wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), GetModeName(modeCurrent).c_str());
		if(mb->sendRest) {
			wpRest.error = wpRest.error | !wpRest.sendRest("modeCurrent", GetModeName(modeCurrent));
			wpRest.trySend = true;
		}
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicModeName, GetModeName(modeCurrent));
		}
		publishModeLast = wpFZ.loopStartedAt;
	}
	if(wpFZ.loopStartedAt > publishSleepLast + (2 * 1000)) {
		if(sleepLast != sleep || wpFZ.CheckQoS(publishSleepLast)) {
			sleepLast = sleep;
			wpMqtt.mqttClient.publish(mqttTopicSleep.c_str(), String(sleep).c_str());
			if(mb->sendRest) {
				wpRest.error = wpRest.error | !wpRest.sendRest("sleep", String(sleep));
				wpRest.trySend = true;
			}
			if(wpMqtt.Debug) {
				mb->printPublishValueDebug(mqttTopicSleep, String(sleep));
			}
			publishSleepLast = wpFZ.loopStartedAt;
		}
	}
	mb->publishValues(force);
}

void moduleCwWw::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetMode.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetSleep.c_str());
	mb->setSubscribes();
}

void moduleCwWw::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetMode.c_str()) == 0) {
		uint readSetMode = msg.toInt();
		if(modeCurrent != readSetMode) {
			modeCurrent = readSetMode;
			wpFZ.DebugcheckSubscribes(mqttTopicSetMode, String(modeCurrent));
		}
	}
	if(strcmp(topic, mqttTopicSetSleep.c_str()) == 0) {
		uint readSleep = msg.toInt();
		SetSleep(readSleep);
		wpFZ.DebugcheckSubscribes(mqttTopicSleep, String(readSleep));
	}
	mb->checkSubscribes(topic, msg);
}
void moduleCwWw::SetSleep(uint seconds) {
	if(seconds == 0) {
		sleep = 0;
		sleepAt = 0;
	} else {
		sleep = seconds;
		sleepAt = wpFZ.loopStartedAt + (seconds * 1000);
	}
	wpFZ.DebugWS(wpFZ.strDEBUG, "CwWw::SetSleep", "Off in " + String(sleep) + " sec");
}
String moduleCwWw::SetOn() {
	wpAnalogOut.handValueSet = EEPROM.read(wpEEPROM.byteAnalogOutHandValue);
	wpFZ.DebugWS(wpFZ.strINFO, "CwWw::SetOn", "setCwWw-WW, restore '" + String(wpAnalogOut.handValueSet) + "'");
	wpAnalogOut2.handValueSet = EEPROM.read(wpEEPROM.byteAnalogOut2HandValue);
	wpFZ.DebugWS(wpFZ.strINFO, "CwWw::SetOn", "setCwWw-CW, restore '" + String(wpAnalogOut2.handValueSet) + "'");
	StaticEffect();
	return "{"
		"," + wpFZ.JsonKeyValue("WW", String(wpAnalogOut.handValue)) +
		"," + wpFZ.JsonKeyValue("CW", String(wpAnalogOut2.handValue)) + "}";
}
String moduleCwWw::SetOff() {
	wpAnalogOut.handValueSet = 0;
	wpFZ.DebugWS(wpFZ.strINFO, "CwWw::SetOff", "setCwWw-WW, '0'");
	wpAnalogOut2.handValueSet = 0;
	wpFZ.DebugWS(wpFZ.strINFO, "CwWw::SetOff", "setCwWw-CW, '0'");
	StaticEffect();
	return "{"
		"," + wpFZ.JsonKeyValue("WW", String(wpAnalogOut.handValue)) +
		"," + wpFZ.JsonKeyValue("CW", String(wpAnalogOut2.handValue)) + "}";
}
void moduleCwWw::SetOnBlender() {
	onBlenderTargetWW = EEPROM.read(wpEEPROM.byteAnalogOutHandValue);
	wpFZ.DebugWS(wpFZ.strINFO, "CwWw::SetOnBlender", "onBlenderTargetWW, restore '" + String(wpAnalogOut.handValueSet) + "'");
	onBlenderTargetCW = EEPROM.read(wpEEPROM.byteAnalogOut2HandValue);
	wpFZ.DebugWS(wpFZ.strINFO, "CwWw::SetOnBlender", "onBlenderTargetCW, restore '" + String(wpAnalogOut2.handValueSet) + "'");
	modeCurrent = ModeOnBlender;
	staticIsSet = false;
	wpFZ.DebugWS(wpFZ.strINFO, "CwWw::SetOn", "SetOnBlender");
}
void moduleCwWw::SetOffBlender() {
	modeCurrent = ModeOffBlender;
	staticIsSet = false;
	wpFZ.DebugWS(wpFZ.strINFO, "CwWw::SetOff", "SetOffBlender");
}

String moduleCwWw::GetModeName(uint actualMode) {
	String returns;
	switch(actualMode) {
		case ModeStatic:
			returns = "ModeStatic";
			break;
		case ModeOnBlender:
			returns = "ModeOnBlender";
			break;
		case ModeOffBlender:
			returns = "ModeOffBlender";
			break;
		default:
			returns = String(actualMode);
			break;
	}
	return returns;
}
void moduleCwWw::SetMode(uint8 newMode) {
	modeCurrent = newMode;
	staticIsSet = false;
	if(modeCurrent == ModeOnBlender) {
		SetOnBlender();
	}
	wpFZ.DebugWS(wpFZ.strINFO, "CwWw::SetMode", GetModeName(newMode));
}
//###################################################################################
// private
//###################################################################################
void moduleCwWw::publishValue() {
	publishValueLast = wpFZ.loopStartedAt;
}

void moduleCwWw::calc() {	
	if(sleepAt > 0) {
		if(wpFZ.loopStartedAt > sleepAt) {
			modeCurrent = ModeOffBlender;
			sleep = 0;
			sleepAt = 0;
		} else {
			sleep = (sleepAt - wpFZ.loopStartedAt) / 1000;
		}
	}
	if(wpFZ.loopStartedAt - loopPrevious >= loopTime) {        //  Check for expired time
		loopPrevious = wpFZ.loopStartedAt;                            //  Run current frame
		switch (modeCurrent) {
			case ModeOn:
				SetOn(); // ModeOn
				break;
			case ModeOff:
				SetOff(); // ModeOff
				break;
			case ModeOnBlender:
				OnBlenderEffect(); // ModeOnBlender
				break;
			case ModeOffBlender:
				OffBlenderEffect(); // ModeOffBlender
				break;
			default:
				if(!staticIsSet) {
					StaticEffect();
					wpFZ.DebugWS(wpFZ.strDEBUG, "CwWw::calc", "Static is set");
				}
				break;
		}
	}
}
void moduleCwWw::OnBlenderEffect() {
	if(wpAnalogOut.handValueSet + steps <= onBlenderTargetWW) {
		wpAnalogOut.handValueSet += steps;
	} else {
		wpAnalogOut.handValueSet = onBlenderTargetWW;
	}
	if(wpAnalogOut2.handValueSet + steps <= onBlenderTargetCW) {
		wpAnalogOut2.handValueSet += steps;
	} else {
		wpAnalogOut2.handValueSet = onBlenderTargetCW;
	}
	if(wpAnalogOut.handValueSet >= onBlenderTargetWW && wpAnalogOut2.handValueSet >= onBlenderTargetCW) {            //  Loop the pattern from the first LED
		StaticEffect();
	}
}
void moduleCwWw::OffBlenderEffect() {
	if(wpAnalogOut.handValueSet >= steps) {
		wpAnalogOut.handValueSet -= steps;
	} else {
		wpAnalogOut.handValueSet = 0;
	}
	if(wpAnalogOut2.handValueSet >= steps) {
		wpAnalogOut2.handValueSet -= steps;
	} else {
		wpAnalogOut2.handValueSet = 0;
	}
	if(wpAnalogOut.handValueSet <= 0 && wpAnalogOut2.handValueSet <= 0) {            //  Loop the pattern from the first LED
		StaticEffect();
	}
}

void moduleCwWw::StaticEffect() {
	modeCurrent = ModeStatic;
	staticIsSet = true;
}

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleCwWw::getVersion() {
	String SVN = "$Rev: 199 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleCwWw::changeSendRest() {
	mb->changeSendRest();
}
void moduleCwWw::changeDebug() {
	mb->changeDebug();
}
bool moduleCwWw::SendRest() {
	return mb->sendRest;
}
bool moduleCwWw::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleCwWw::Debug() {
	return mb->debug;
}
bool moduleCwWw::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
