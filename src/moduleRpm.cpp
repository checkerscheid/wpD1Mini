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
//# Revision     : $Rev:: 258                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRpm.cpp 258 2025-04-28 13:34:51Z                   $ #
//#                                                                                 #
//###################################################################################
#include <moduleRpm.h>

moduleRpm wpRpm;

moduleRpm::moduleRpm() {
	// section to config and copy
	ModuleName = "Rpm";
	mb = new moduleBase(ModuleName);
}
void moduleRpm::init() {
	// section for define
	Pin = D5;
	pinMode(Pin, INPUT);
	rpm = 0;
	// values
	mqttTopicRpm = wpFZ.DeviceName + "/" + ModuleName;
	// settings
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction";

	rpmLast = 0;
	publishRpmLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugRpm);
	mb->initUseAvg(wpEEPROM.addrBitsSettingsModules1, wpEEPROM.bitsSettingsModules1, wpEEPROM.bitUseRpmAvg);
	mb->initError();
	mb->initCalcCycle(wpEEPROM.byteCalcCycleRpm);

	mb->calcLast = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleRpm::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleRpm::publishSettings() {
	publishSettings(false);
}
void moduleRpm::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	mb->publishSettings(force);
}

void moduleRpm::publishValues() {
	publishValues(false);
}
void moduleRpm::publishValues(bool force) {
	if(force) {
		publishRpmLast = 0;
	}
	if(rpmLast != rpm || wpFZ.CheckQoS(publishRpmLast)) {
		publishValue();
	}
	mb->publishValues(force);
}

void moduleRpm::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	mb->setSubscribes();
}

void moduleRpm::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicCorrection.c_str()) == 0) {
		int8 readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			wpEEPROM.WriteByteToEEPROM("RpmCorrection", wpEEPROM.byteRpmCorrection, correction);
			wpFZ.DebugcheckSubscribes(mqttTopicCorrection, String(correction));
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleRpm::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicRpm.c_str(), String(rpm).c_str());
	rpmLast = rpm;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("Rpm", String(rpm));
	}
	publishRpmLast = wpFZ.loopStartedAt;
}

void moduleRpm::calc() {
	uint32 raw = pulseIn(Pin, HIGH, 200 * 1000);
	if(raw > 0) {
		uint32 duration = raw / 1000;
		uint32 read = 1000 / duration / 4 * 60;
		uint32 avg = read;
		if(mb->useAvg) {
			avg = calcAvg(avg);
		}
		uint32 correct = avg + correction;
		rpm = correct;
		mb->error = false;
		if(mb->debug) {
			String logmessage = "Rpm: " + String(rpm) + " ("
				"Read: " + String(duration) + " ms, "
				"Avg: " + String(correct) + ", "
				"Raw: " + String(raw) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "calcRpm", logmessage);
		}
	} else {
		mb->error = true;
		String logmessage = "Sensor Failure: " + String(raw);
		wpFZ.DebugWS(wpFZ.strERRROR, "calcRpm", logmessage);
	}
}
uint16 moduleRpm::calcAvg(uint16 raw) {
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

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleRpm::getVersion() {
	String SVN = "$Rev: 258 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleRpm::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyValue(F("CalcCycle"), String(CalcCycle())) + F(",") +
		wpFZ.JsonKeyValue(F("useAvg"), UseAvg() ? "true" : "false") + F(",") +
		wpFZ.JsonKeyValue(F("Correction"), String(correction)) +
		F("},");
	return json;
}

void moduleRpm::changeDebug() {
	mb->changeDebug();
}
bool moduleRpm::UseAvg() {
	return mb->useAvg;
}
bool moduleRpm::UseAvg(bool useAvg) {
	mb->useAvg = useAvg;
	return true;
}
bool moduleRpm::Debug() {
	return mb->debug;
}
bool moduleRpm::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32 moduleRpm::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleRpm::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
