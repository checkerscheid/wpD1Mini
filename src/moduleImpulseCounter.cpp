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
//# Revision     : $Rev:: 191                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleImpulseCounter.cpp 191 2024-08-14 02:36:26Z        $ #
//#                                                                                 #
//###################################################################################
#include <moduleImpulseCounter.h>

moduleImpulseCounter wpImpulseCounter;

moduleImpulseCounter::moduleImpulseCounter() {
	// section to config and copy
	ModuleName = "ImpulseCounter";
	mb = new moduleBase(ModuleName);
}
void moduleImpulseCounter::init() {
	// section for define
	Pin = A0;
	bm = true;
	mqttTopicCounter = wpFZ.DeviceName + "/" + ModuleName;
	// settings
	mqttTopicSetCounter = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetCounter";
	mqttTopicSilver = wpFZ.DeviceName + "/settings/" + ModuleName + "/Silver";
	mqttTopicRed = wpFZ.DeviceName + "/settings/" + ModuleName + "/Red";

	// section to copy

	mb->initRest(wpEEPROM.addrBitsSendRestModules1, wpEEPROM.bitsSendRestModules1, wpEEPROM.bitSendRestImpulseCounter);
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugImpulseCounter);
	mb->initCalcCycle(wpEEPROM.byteCalcCycleImpulseCounter);
}

//###################################################################################
void moduleImpulseCounter::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}
void moduleImpulseCounter::publishSettings() {
	publishSettings(false);
}
void moduleImpulseCounter::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicSetCounter.c_str(), String(impulseCounter).c_str());
	wpMqtt.mqttClient.publish(mqttTopicSilver.c_str(), String(counterSilver).c_str());
	wpMqtt.mqttClient.publish(mqttTopicRed.c_str(), String(counterRed).c_str());
	mb->publishSettings(force);
}
void moduleImpulseCounter::publishValues() {
	publishValues(false);
}
void moduleImpulseCounter::publishValues(bool force) {
	if(force) {
		publishCounterLast = 0;
	}
	if(impulseCounterLast != impulseCounter || wpFZ.CheckQoS(publishCounterLast)) {
		publishValue();
	}
	mb->publishValues(force);
}
void moduleImpulseCounter::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetCounter.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSilver.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicRed.c_str());
	mb->setSubscribes();
}
void moduleImpulseCounter::checkSubscribes(char* topic, String msg) {
	mb->checkSubscribes(topic, msg);
	if(strcmp(topic, mqttTopicSetCounter.c_str()) == 0) {
		uint16 readSetCounter = msg.toInt();
		if(impulseCounter != readSetCounter) {
			impulseCounter = readSetCounter;
			EEPROM.put(wpEEPROM.byteImpulseCounter, impulseCounter);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetCounter, String(impulseCounter));
		}
	}
	if(strcmp(topic, mqttTopicSilver.c_str()) == 0) {
		int8 readSilver = msg.toInt();
		if(counterSilver != readSilver) {
			counterSilver = readSilver;
			EEPROM.put(wpEEPROM.byteImpulseCounterSilver, counterSilver);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSilver, String(counterSilver));
		}
	}
	if(strcmp(topic, mqttTopicRed.c_str()) == 0) {
		int8 readRed = msg.toInt();
		if(counterRed != readRed) {
			counterRed = readRed;
			EEPROM.put(wpEEPROM.byteImpulseCounterRed, counterRed);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicRed, String(counterRed));
		}
	}
}
void moduleImpulseCounter::publishValue() {
	impulseCounterLast = impulseCounter;
	wpMqtt.mqttClient.publish(mqttTopicCounter.c_str(), String(impulseCounter).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("counter", String(impulseCounter));
		wpRest.trySend = true;
	}
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("ImpulseCounter", String(impulseCounter));
	}
	publishCounterLast = wpFZ.loopStartedAt;
}
void moduleImpulseCounter::calc() {
	int ar = analogRead(Pin);
	if(redIsNow) {
		if(ar <= counterSilver) {
			redIsNow = false;
		}
	} else {
		if(ar >= counterRed) {
			redIsNow = true;
		}
	}
	if(redIsNowLast != redIsNow) {
		if(redIsNow) impulseCounter++;
		redIsNowLast = redIsNow;
	}
	if(mb->debug) {
		wpFZ.DebugWS(wpFZ.strDEBUG, "moduleImpulseCounter::calc", "ar: '" + String(ar) + "', "
			"redIsNow: '" + (redIsNow ? "true" : "false") + "', "
			"impulseCounter: '" + String(impulseCounter) + "'");
	}
}


//###################################################################################
// section to copy
//###################################################################################
uint16 moduleImpulseCounter::getVersion() {
	String SVN = "$Rev: 191 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

bool moduleImpulseCounter::SendRest() {
	return mb->sendRest;
}
bool moduleImpulseCounter::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleImpulseCounter::Debug() {
	return mb->debug;
}
bool moduleImpulseCounter::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
void moduleImpulseCounter::changeSendRest() {
	mb->changeSendRest();
}
void moduleImpulseCounter::changeDebug() {
	mb->changeDebug();
}
uint32 moduleImpulseCounter::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleImpulseCounter::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
