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
//# Revision     : $Rev:: 139                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLDR.cpp 139 2024-06-11 10:08:54Z                   $ #
//#                                                                                 #
//###################################################################################
#include <moduleLDR.h>

moduleLDR wpLDR;

moduleLDR::moduleLDR() {
	// section to config and copy
	ModuleName = "LDR";
	mb = new moduleBase(ModuleName);
}
void moduleLDR::init() {

	// section for define
	LDRPin = A0;
	ldr = 0;
	// values
	mqttTopicLdr = wpFZ.DeviceName + "/" + ModuleName;
	// settings
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction";

	ldrLast = 0;
	publishCountLdr = 0;

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules0, wpEEPROM.bitsSendRestModules0, wpEEPROM.bitSendRestLDR);
	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugLDR);
	mb->initUseAvg(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0, wpEEPROM.bitUseLdrAvg);
	mb->initError();
	mb->initMaxCycle(wpEEPROM.byteMaxCycleLDR);
}

//###################################################################################
// public
//###################################################################################
void moduleLDR::cycle() {
	if(wpFZ.calcValues && ++mb->cycleCounter >= mb->maxCycle) {
		calc();
		mb->cycleCounter = 0;
	}
	publishValues();
}

void moduleLDR::publishSettings() {
	publishSettings(false);
}
void moduleLDR::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	mb->publishSettings(force);
}

void moduleLDR::publishValues() {
	publishValues(false);
}
void moduleLDR::publishValues(bool force) {
	if(force) {
		publishCountLdr = wpFZ.publishQoS;
	}
	if(ldrLast != ldr || ++publishCountLdr > wpFZ.publishQoS) {
		publishValue();
	}
	mb->publishValues(force);
}

void moduleLDR::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	mb->setSubscribes();
}

void moduleLDR::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicCorrection.c_str()) == 0) {
		int8_t readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			EEPROM.put(wpEEPROM.byteLDRCorrection, correction);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicCorrection, String(correction));
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleLDR::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicLdr.c_str(), String(ldr).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("ldr", String(ldr));
		wpRest.trySend = true;
	}
	ldrLast = ldr;
	if(wpMqtt.Debug) {
		printPublishValueDebug("LDR", String(ldr), String(publishCountLdr));
	}
	publishCountLdr = 0;
}

void moduleLDR::calc() {
	int read = analogRead(LDRPin);
	int minMax, avg, correct;
	if(!isnan(read)) {
		minMax = read;
		if(minMax > 1023) minMax = 1023;
		if(minMax < 0) minMax = 0;
		avg = minMax;
		if(mb->useAvg) {
			avg = calcAvg(avg);
		}
		correct = avg;
		ldr = (1023 - correct) + correction;
		mb->error = false;
		if(mb->debug) {
			String logmessage = "LDR: " + String(ldr) + " ("
				"Read: " + String(read) + ", "
				"MinMax: " + String(minMax) + ", "
				"Avg: " + String(avg) + ")";;
			wpFZ.DebugWS(wpFZ.strDEBUG, "calcLDR", logmessage);
		}
	} else {
		mb->error = true;
		String logmessage = "Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "calcLDR", logmessage);
	}
}
uint16_t moduleLDR::calcAvg(uint16_t raw) {
	long avg = 0;
	long avgCount = avgLength;
	avgValues[avgLength - 1] = raw;
	for(int i = 0; i < avgLength - 1; i++) {
		if(avgValues[i + 1] > 0) {
			avgValues[i] = avgValues[i + 1];
			avg += avgValues[i] * (i + 1);
			avgCount += (i + 1);
		}
	}
	avg += raw * avgLength;
	return round(avg / avgCount);
}

void moduleLDR::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}

//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleLDR::getVersion() {
	String SVN = "$Rev: 139 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleLDR::changeSendRest() {
	mb->changeSendRest();
}
void moduleLDR::changeDebug() {
	mb->changeDebug();
}
bool moduleLDR::SendRest() {
	return mb->sendRest;
}
bool moduleLDR::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleLDR::UseAvg() {
	return mb->useAvg;
}
bool moduleLDR::UseAvg(bool useAvg) {
	mb->useAvg = useAvg;
	return true;
}
bool moduleLDR::Debug() {
	return mb->debug;
}
bool moduleLDR::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint8_t moduleLDR::MaxCycle(){
	return mb->maxCycle;
}
uint8_t moduleLDR::MaxCycle(uint8_t maxCycle){
	mb->maxCycle = maxCycle;
	return 0;
}
