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
//# Revision     : $Rev:: 232                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleImpulseCounter.cpp 232 2024-12-19 15:27:48Z        $ #
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
	impulseCounterLast = 0;
	mqttTopicCounter = wpFZ.DeviceName + "/" + ModuleName + "/Counter";
	mqttTopicKWh = wpFZ.DeviceName + "/" + ModuleName + "/KWh";
	// settings
	mqttTopicSetKWh = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetKWh";
	mqttTopicUpKWh = wpFZ.DeviceName + "/settings/" + ModuleName + "/UpKWh";
	mqttTopicSilver = wpFZ.DeviceName + "/settings/" + ModuleName + "/Silver";
	mqttTopicRed = wpFZ.DeviceName + "/settings/" + ModuleName + "/Red";

	// section to copy

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
	wpMqtt.mqttClient.publish(mqttTopicSetKWh.c_str(), String(KWh).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUpKWh.c_str(), String(UpKWh).c_str());
	wpMqtt.mqttClient.publish(mqttTopicSilver.c_str(), String(counterSilver).c_str());
	wpMqtt.mqttClient.publish(mqttTopicRed.c_str(), String(counterRed).c_str());
	mb->publishSettings(force);
}
void moduleImpulseCounter::publishValues() {
	publishValues(false);
}
void moduleImpulseCounter::publishValues(bool force) {
	if(force) {
		publishKWhLast = 0;
	}
	if(KWhLast != KWh || wpFZ.CheckQoS(publishKWhLast)) {
		publishValue();
	}
	mb->publishValues(force);
}
void moduleImpulseCounter::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetKWh.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUpKWh.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSilver.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicRed.c_str());
	mb->setSubscribes();
}
void moduleImpulseCounter::checkSubscribes(char* topic, String msg) {
	mb->checkSubscribes(topic, msg);
	if(strcmp(topic, mqttTopicSetKWh.c_str()) == 0) {
		uint16 readSetKWh = msg.toInt();
		if(KWh != readSetKWh) {
			KWh = readSetKWh;
			EEPROM.put(wpEEPROM.byteImpulseCounterKWh, KWh);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetKWh, String(KWh));
		}
	}
	if(strcmp(topic, mqttTopicUpKWh.c_str()) == 0) {
		uint8 readUpKWh = msg.toInt();
		if(UpKWh != readUpKWh) {
			UpKWh = readUpKWh;
			EEPROM.write(wpEEPROM.byteImpulseCounterUpKWh, UpKWh);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicUpKWh, String(UpKWh));
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
	KWhLast = KWh;
	wpMqtt.mqttClient.publish(mqttTopicCounter.c_str(), String(impulseCounter).c_str());
	wpMqtt.mqttClient.publish(mqttTopicKWh.c_str(), String(KWh).c_str());
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("ImpulseCounter", String(impulseCounter));
	}
	publishKWhLast = wpFZ.loopStartedAt;
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
	if(impulseCounter % UpKWh == 0 && impulseCounterLast != impulseCounter) {
		KWh++;
		impulseCounterLast = impulseCounter;
	}
	if(mb->debug) {
		wpFZ.DebugWS(wpFZ.strDEBUG, "moduleImpulseCounter::calc", "ar: '" + String(ar) + "', "
			"redIsNow: '" + (redIsNow ? "true" : "false") + "', "
			"KWh: '" + String(KWh) + "', "
			"impulseCounter: '" + String(impulseCounter) + "'");
	}
}


//###################################################################################
// section to copy
//###################################################################################
uint16 moduleImpulseCounter::getVersion() {
	String SVN = "$Rev: 232 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleImpulseCounter::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyValue(F("CalcCycle"), String(CalcCycle())) + F(",") +
		wpFZ.JsonKeyValue(F("UpKWh"), String(UpKWh)) + F(",") +
		wpFZ.JsonKeyValue(F("Silver"), String(counterSilver)) + F(",") +
		wpFZ.JsonKeyValue(F("Red"), String(counterRed)) +
		F("},");
	return json;
}

bool moduleImpulseCounter::Debug() {
	return mb->debug;
}
bool moduleImpulseCounter::Debug(bool debug) {
	mb->debug = debug;
	return true;
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
