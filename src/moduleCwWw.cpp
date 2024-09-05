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
//# Revision     : $Rev:: 200                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleCwWw.cpp 200 2024-09-05 23:43:19Z                  $ #
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
	if(mb->debug) wpFZ.DebugWS(wpFZ.strDEBUG, "CwWw::SetSleep", "Off in " + String(sleep) + " sec");
}
String moduleCwWw::SetOn() {
	targetWW = EEPROM.read(wpEEPROM.byteAnalogOutHandValue);
	targetCW = EEPROM.read(wpEEPROM.byteAnalogOut2HandValue);
	calcDuration();
	modeCurrent = ModeBlender;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
String moduleCwWw::SetOff() {
	targetWW = 0;
	targetCW = 0;
	calcDuration();
	modeCurrent = ModeBlender;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}

String moduleCwWw::SetWW(uint ww) {
	targetWW = ww;
	targetCW = wpAnalogOut2.handValue;
	EEPROM.write(wpEEPROM.byteAnalogOutHandValue, targetWW);
	EEPROM.commit();
	calcDuration();
	modeCurrent = ModeBlender;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
String moduleCwWw::SetCW(uint cw) {
	targetWW = wpAnalogOut.handValue;
	targetCW = cw;
	EEPROM.write(wpEEPROM.byteAnalogOut2HandValue, targetCW);
	EEPROM.commit();
	calcDuration();
	modeCurrent = ModeBlender;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
void moduleCwWw::calcDuration() {
	uint8 distWW = abs(wpAnalogOut.handValue - targetWW);
	uint8 distCW = abs(wpAnalogOut2.handValue - targetCW);
	uint dist = distWW >= distCW ? distWW : distCW;
	uint s = (int)(dist / 80.0);
	steps = s == 0 ? 1 : s;
}
void moduleCwWw::SetSmooth() {
	modeCurrent = ModeSmooth;
}

String moduleCwWw::GetModeName(uint actualMode) {
	String returns;
	switch(actualMode) {
		case ModeStatic:
			returns = "Mode Static";
			break;
		case ModeBlender:
			returns = "Mode Blender";
			break;
		case ModeSmooth:
			returns = "Mode Smooth";
			break;
		default:
			returns = String(actualMode);
			break;
	}
	return returns;
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
			SetOff();
			sleep = 0;
			sleepAt = 0;
		} else {
			sleep = (sleepAt - wpFZ.loopStartedAt) / 1000;
		}
	}
	if(wpFZ.loopStartedAt - loopPrevious >= loopTime) {        //  Check for expired time
		loopPrevious = wpFZ.loopStartedAt;                            //  Run current frame
		switch (modeCurrent) {
			case ModeBlender:
				BlenderEffect(); // ModeOn
				break;
			case ModeSmooth:
				SmoothEffect();
				break;
			default:
				modeCurrent = ModeStatic;
				break;
		}
	}
}

bool moduleCwWw::BlenderWWEffect() {
	if(wpAnalogOut.handValueSet != targetWW) {
		if(wpAnalogOut.handValueSet <= targetWW) {
			if(wpAnalogOut.handValueSet + steps <= targetWW) {
				wpAnalogOut.handValueSet += steps;
			} else {
				wpAnalogOut.handValueSet = targetWW;
			}
			if(wpAnalogOut.handValueSet >= targetWW) {
				wpAnalogOut.handValueSet = targetWW;
			}
		} else {
			if(wpAnalogOut.handValueSet - steps >= targetWW) {
				wpAnalogOut.handValueSet -= steps;
			} else {
				wpAnalogOut.handValueSet = targetWW;
			}
			if(wpAnalogOut.handValueSet <= targetWW) {
				wpAnalogOut.handValueSet = targetWW;
			}
		}
	}
	return wpAnalogOut.handValueSet == targetWW;
}
bool moduleCwWw::BlenderCWEffect() {
	if(wpAnalogOut2.handValueSet != targetCW) {
		if(wpAnalogOut2.handValueSet <= targetCW) {
			if(wpAnalogOut2.handValueSet + steps <= targetCW) {
				wpAnalogOut2.handValueSet += steps;
			} else {
				wpAnalogOut2.handValueSet = targetCW;
			}
			if(wpAnalogOut2.handValueSet >= targetCW) {
				wpAnalogOut2.handValueSet = targetCW;
			}
		} else {
			if(wpAnalogOut2.handValueSet - steps >= targetCW) {
				wpAnalogOut2.handValueSet -= steps;
			} else {
				wpAnalogOut2.handValueSet = targetCW;
			}
			if(wpAnalogOut2.handValueSet <= targetCW) {
				wpAnalogOut2.handValueSet = targetCW;
			}
		}
	}
	return wpAnalogOut2.handValueSet == targetCW;
}
void moduleCwWw::BlenderEffect() {
	bool bwwe = BlenderWWEffect();
	bool bcwe = BlenderCWEffect();
	if(bwwe && bcwe) {
		modeCurrent = ModeStatic;
	}
}

void moduleCwWw::SmoothEffect() {
	if(wpAnalogOut.handValueSet >= 255 ||
		wpAnalogOut2.handValueSet <= 0) {
		wpAnalogOut.handValueSet = 255;
		wpAnalogOut2.handValueSet = 0;
		smoothDirection = true;
	}
	if(wpAnalogOut.handValueSet <= 0 ||
		wpAnalogOut2.handValueSet >= 255) {
		wpAnalogOut.handValueSet = 0;
		wpAnalogOut2.handValueSet = 255;
		smoothDirection = false;
	}
	if(smoothDirection) {
		wpAnalogOut.handValueSet -= 5;
		wpAnalogOut2.handValueSet += 5;
	} else {
		wpAnalogOut.handValueSet += 5;
		wpAnalogOut2.handValueSet -= 5;
	}
}


//###################################################################################
// section to copy
//###################################################################################
uint16 moduleCwWw::getVersion() {
	String SVN = "$Rev: 200 $";
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
