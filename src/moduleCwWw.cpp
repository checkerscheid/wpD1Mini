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
//# Revision     : $Rev:: 231                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleCwWw.cpp 231 2024-12-14 03:25:15Z                  $ #
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
	effectSpeed = 1;
	modeCurrent = 0;       // Current Pattern Number

	// values
	mqttTopicManual = wpFZ.DeviceName + "/" + ModuleName + "/Manual";
	mqttTopicMaxPercent = wpFZ.DeviceName + "/" + ModuleName + "/MaxPercent";
	mqttTopicSleep = wpFZ.DeviceName + "/" + ModuleName + "/Sleep";
	mqttTopicModeName = wpFZ.DeviceName + "/" + ModuleName + "/ModeName";
	mqttTopicSpeed = wpFZ.DeviceName + "/" + ModuleName + "/EffectSpeed";
	// settings
	// commands
	mqttTopicSetMode = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetMode";
	mqttTopicSetSleep = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetSleep";
	mqttTopicSetSpeed = wpFZ.DeviceName + "/settings/" + ModuleName + "/EffectSpeed";

	manual = false;
	maxPercent = 0;
	publishModeLast = 0;
	winkelWW = 0.0;
	winkelCW = 0.0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugCwWw);
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
		publishManualLast = 0;
		publishModeLast = 0;
		publishMaxPercent = 0;
	}
	if(manualLast != manual || wpFZ.CheckQoS(publishManualLast)) {
		manualLast = manual;
		wpMqtt.mqttClient.publish(mqttTopicManual.c_str(), String(manual).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicManual, String(manual));
		}
		publishManualLast = wpFZ.loopStartedAt;
	}
	if(maxPercentLast != maxPercent || wpFZ.CheckQoS(publishMaxPercent)) {
		maxPercentLast = maxPercent;
		wpMqtt.mqttClient.publish(mqttTopicMaxPercent.c_str(), String(maxPercent).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicMaxPercent, String(maxPercent));
		}
		publishMaxPercent = wpFZ.loopStartedAt;
	}
	if(modeCurrentLast != modeCurrent || wpFZ.CheckQoS(publishModeLast)) {
		modeCurrentLast = modeCurrent;
		wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), GetModeName(modeCurrent).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicModeName, GetModeName(modeCurrent));
		}
		publishModeLast = wpFZ.loopStartedAt;
	}
	if(wpFZ.loopStartedAt > publishSleepLast + 1000) {
		if(sleepLast != sleep || wpFZ.CheckQoS(publishSleepLast)) {
			sleepLast = sleep;
			wpMqtt.mqttClient.publish(mqttTopicSleep.c_str(), String(sleep).c_str());
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
void moduleCwWw::SetEffectSpeed(uint8 speed) {
	if(speed < 1) speed = 1;
	if(speed > 9) speed = 9;
	effectSpeed = speed;
}
void moduleCwWw::SetSleep(uint seconds) {
	if(seconds == 0) {
		sleep = 0;
		sleepAt = 0;
	} else {
		sleep = seconds;
		sleepAt = wpFZ.loopStartedAt + (seconds * 1000);
	}
	if(mb->debug)
		wpFZ.DebugWS(wpFZ.strDEBUG, "CwWw::SetSleep", "Off in " + String(sleep) + " sec");
}
String moduleCwWw::SetOn() {
	manual = true;
	SetSleep(0);
	targetWW = EEPROM.read(wpEEPROM.byteAnalogOutHandValue);
	targetCW = EEPROM.read(wpEEPROM.byteAnalogOut2HandValue);
	calcDuration();
	modeCurrent = ModeBlender;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
String moduleCwWw::SetOff() {
	manual = false;
	targetWW = 0;
	targetCW = 0;
	calcDuration();
	modeCurrent = ModeBlender;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
String moduleCwWw::SetWwCw(uint8 ww, uint8 cw) {
	manual = true;
	SetSleep(0);
	targetWW = ww;
	targetCW = cw;
	calcDuration();
	modeCurrent = ModeBlender;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
String moduleCwWw::SetWW(uint8 ww) {
	manual = true;
	SetSleep(0);
	targetWW = ww;
	//targetCW = wpAnalogOut2.handValue;
	EEPROM.write(wpEEPROM.byteAnalogOutHandValue, targetWW);
	EEPROM.commit();
	calcDuration();
	modeCurrent = ModeBlender;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
String moduleCwWw::SetCW(uint8 cw) {
	manual = true;
	SetSleep(0);
	//targetWW = wpAnalogOut.handValue;
	targetCW = cw;
	EEPROM.write(wpEEPROM.byteAnalogOut2HandValue, targetCW);
	EEPROM.commit();
	calcDuration();
	modeCurrent = ModeBlender;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
String moduleCwWw::SetWwCwAuto(uint8 ww, uint8 cw, uint sleep) {
	if(!manual) {
		targetWW = ww;
		targetCW = cw;
		calcDuration();
		modeCurrent = ModeBlender;
		SetSleep(sleep);
		return "{"
			+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
			+ wpFZ.JsonKeyValue("CW", String(targetCW)) + ","
			+ wpFZ.JsonKeyValue("sleep", String(sleep)) + "}";
	} else {
		return "{\"erg\":\"S_OK\",\"mode\":\"isManuell\"}";
	}
}
void moduleCwWw::calcDuration() {
	uint8 distWW = abs(wpAnalogOut.GetHandValue() - targetWW);
	uint8 distCW = abs(wpAnalogOut2.GetHandValue() - targetCW);
	uint dist = distWW >= distCW ? distWW : distCW;
	uint s = (int)(dist / 50.0);
	steps = s == 0 ? 1 : s;
	//if(mb->debug)
		wpFZ.DebugWS(wpFZ.strDEBUG, "CwWw.calcDuration", "calculated steps: '" + String(steps) + "'");
}
String moduleCwWw::SetEffect(uint8 effect) {
	manual = true;
	modeCurrent = effect;
	return "{\"erg\":\"S_OK\",\"effect\":\"" + GetModeName(modeCurrent) + "\"}";
}

String moduleCwWw::GetModeName(uint8 actualMode) {
	String returns;
	switch(actualMode) {
		case ModeStatic:
			returns = "Mode Static";
			break;
		case ModeBlender:
			returns = "Mode Blender";
			break;
		case ModePulse:
			returns = "Mode Pulse";
			break;
		case ModeWwPulse:
			returns = "Mode WW Pulse";
			break;
		case ModeCwPulse:
			returns = "Mode CW Pulse";
			break;
		case ModeSmooth:
			returns = "Mode Smooth";
			break;
		case ModeWwSmooth:
			returns = "Mode WW Smooth";
			break;
		case ModeCwSmooth:
			returns = "Mode CW Smooth";
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
			case ModePulse:
				PulseEffect();
				break;
			case ModeWwPulse:
				WwPulseEffect();
				break;
			case ModeCwPulse:
				CwPulseEffect();
				break;
			case ModeSmooth:
				SmoothEffect();
				break;
			case ModeWwSmooth:
				WwSmoothEffect();
				break;
			case ModeCwSmooth:
				CwSmoothEffect();
				break;
			default:
				modeCurrent = ModeStatic;
				break;
		}
	}
	maxPercent = GetMaxPercent();
}

bool moduleCwWw::BlenderWWEffect() {
	if(wpAnalogOut.GetHandValue() != targetWW) {
		if(wpAnalogOut.GetHandValue() <= targetWW) {
			if(wpAnalogOut.GetHandValue() + steps <= targetWW) {
				wpAnalogOut.InitHandValue(wpAnalogOut.GetHandValue() + steps);
			} else {
				wpAnalogOut.InitHandValue(targetWW);
			}
			if(wpAnalogOut.GetHandValue() >= targetWW) {
				wpAnalogOut.InitHandValue(targetWW);
			}
		} else {
			if(wpAnalogOut.GetHandValue() - steps >= targetWW) {
				wpAnalogOut.InitHandValue(wpAnalogOut.GetHandValue() - steps);
			} else {
				wpAnalogOut.InitHandValue(targetWW);
			}
			if(wpAnalogOut.GetHandValue() <= targetWW) {
				wpAnalogOut.InitHandValue(targetWW);
			}
		}
	}
	return wpAnalogOut.GetHandValue() == targetWW;
}
bool moduleCwWw::BlenderCWEffect() {
	if(wpAnalogOut2.GetHandValue() != targetCW) {
		if(wpAnalogOut2.GetHandValue() <= targetCW) {
			if(wpAnalogOut2.GetHandValue() + steps <= targetCW) {
				wpAnalogOut2.InitHandValue(wpAnalogOut2.GetHandValue() + steps);
			} else {
				wpAnalogOut2.InitHandValue(targetCW);
			}
			if(wpAnalogOut2.GetHandValue() >= targetCW) {
				wpAnalogOut2.InitHandValue(targetCW);
			}
		} else {
			if(wpAnalogOut2.GetHandValue() - steps >= targetCW) {
				wpAnalogOut2.InitHandValue(wpAnalogOut2.GetHandValue() - steps);
			} else {
				wpAnalogOut2.InitHandValue(targetCW);
			}
			if(wpAnalogOut2.GetHandValue() <= targetCW) {
				wpAnalogOut2.InitHandValue(targetCW);
			}
		}
	}
	return wpAnalogOut2.GetHandValue() == targetCW;
}
void moduleCwWw::BlenderEffect() {
	bool bwwe = BlenderWWEffect();
	bool bcwe = BlenderCWEffect();
	if(bwwe && bcwe) {
		modeCurrent = ModeStatic;
	}
}
void moduleCwWw::PulseEffect() {
	if(wpAnalogOut.GetHandValue() >= 255 ||
		wpAnalogOut2.GetHandValue() <= 0) {
		wpAnalogOut.InitHandValue(255);
		wpAnalogOut2.InitHandValue(0);
		smoothDirection = true;
	}
	if(wpAnalogOut.GetHandValue() <= 0 ||
		wpAnalogOut2.GetHandValue() >= 255) {
		wpAnalogOut.InitHandValue(0);
		wpAnalogOut2.InitHandValue(255);
		smoothDirection = false;
	}
	if(smoothDirection) {
		wpAnalogOut.InitHandValue(wpAnalogOut.GetHandValue() - effectSpeed);
		wpAnalogOut2.InitHandValue(wpAnalogOut2.GetHandValue() + effectSpeed);
	} else {
		wpAnalogOut.InitHandValue(wpAnalogOut.GetHandValue() + effectSpeed);
		wpAnalogOut2.InitHandValue(wpAnalogOut2.GetHandValue() - effectSpeed);
	}
}

void moduleCwWw::WwPulseEffect() {
	if(wpAnalogOut.GetHandValue() >= 255) {
		smoothDirection = true;
	}
	if(wpAnalogOut.GetHandValue() <= 10) {
		smoothDirection = false;
	}
	if(smoothDirection) {
		wpAnalogOut.InitHandValue(wpAnalogOut.GetHandValue() - effectSpeed);
	} else {
		wpAnalogOut.InitHandValue(wpAnalogOut.GetHandValue() + effectSpeed);
	}
}
void moduleCwWw::CwPulseEffect() {
	if(wpAnalogOut2.GetHandValue() >= 100) {
		smoothDirection = true;
	}
	if(wpAnalogOut2.GetHandValue() <= 10) {
		smoothDirection = false;
	}
	if(smoothDirection) {
		wpAnalogOut2.InitHandValue(wpAnalogOut2.GetHandValue() - effectSpeed);
	} else {
		wpAnalogOut2.InitHandValue(wpAnalogOut2.GetHandValue() + effectSpeed);
	}
}
void moduleCwWw::SmoothEffect() {
	winkelWW += effectSpeed / 100.0;
	winkelCW += effectSpeed / 100.0;
	if(winkelWW > 2 * pi || winkelCW > 3 * pi) {
		winkelWW = 0;
		winkelCW = pi;
	}
	float sinWW = sin(winkelWW);
	uint8 sinWWByte = (uint8)(127.0 * (sinWW + 1));
	if(sinWWByte < 10) sinWWByte = 10;
	if(mb->debug) wpFZ.DebugWS(wpFZ.strDEBUG, "WWPulseEffekt", "ByteWW: '" + String(sinWWByte) + "', sinWW: '" + String(sinWW) + "' von Bogen: '" + String(winkelWW) + "'");
	wpAnalogOut.InitHandValue(sinWWByte);
	float sinCW = sin(winkelCW);
	uint8 sinCWByte = (uint8)(127.0 * (sinCW + 1));
	if(sinCWByte < 10) sinCWByte = 10;
	if(mb->debug) wpFZ.DebugWS(wpFZ.strDEBUG, "CWPulseEffekt", "ByteCW: '" + String(sinCWByte) + "', sinCW: '" + String(sinCW) + "' von Bogen: '" + String(winkelCW) + "'");
	wpAnalogOut2.InitHandValue(sinCWByte);
}
void moduleCwWw::WwSmoothEffect() {
	winkelWW += effectSpeed / 100.0;
	if(winkelWW >= 2 * pi) {
		winkelWW = 0;
	}
	float sinWW = sin(winkelWW);
	uint8 sinWWByte = (uint8)(127.0 * (sinWW + 1));
	if(sinWWByte < 10) sinWWByte = 10;
	if(mb->debug) wpFZ.DebugWS(wpFZ.strDEBUG, "WWPulseEffekt", "ByteWW: '" + String(sinWWByte) + "', sinWW: '" + String(sinWW) + "' von Bogen: '" + String(winkelWW) + "'");
	wpAnalogOut.InitHandValue(sinWWByte);
}
void moduleCwWw::CwSmoothEffect() {
	winkelCW += effectSpeed / 100.0;
	if(winkelCW >= 2 * pi) {
		winkelCW = 0;
	}
	float sinCW = sin(winkelCW);
	uint8 sinCWByte = (uint8)(127.0 * (sinCW + 1));
	if(sinCWByte < 10) sinCWByte = 10;
	if(mb->debug) wpFZ.DebugWS(wpFZ.strDEBUG, "CWPulseEffekt", "ByteCW: '" + String(sinCWByte) + "', sinCW: '" + String(sinCW) + "' von Bogen: '" + String(winkelCW) + "'");
	wpAnalogOut2.InitHandValue(sinCWByte);
}
uint8 moduleCwWw::GetMaxPercent() {
	uint8 returns = 0;
	returns = wpAnalogOut.GetHandValue() > returns ? wpAnalogOut.GetHandValue() : returns;
	returns = wpAnalogOut2.GetHandValue() > returns ? wpAnalogOut2.GetHandValue() : returns;
	return returns;
}
//###################################################################################
// section to copy
//###################################################################################
uint16 moduleCwWw::getVersion() {
	String SVN = "$Rev: 231 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleCwWw::changeDebug() {
	mb->changeDebug();
}
bool moduleCwWw::Debug() {
	return mb->debug;
}
bool moduleCwWw::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
