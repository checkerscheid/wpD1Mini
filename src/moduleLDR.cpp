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
//# File-ID      : $Id:: moduleLDR.cpp 232 2024-12-19 15:27:48Z                   $ #
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
	Pin = A0;
	ldr = 0;
	// values
	mqttTopicLdr = wpFZ.DeviceName + "/" + ModuleName;
	// settings
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction";

	ldrLast = 0;
	publishLdrLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugLDR);
	mb->initUseAvg(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0, wpEEPROM.bitUseLdrAvg);
	mb->initError();
	mb->initCalcCycle(wpEEPROM.byteCalcCycleLDR);

	mb->calcLast = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleLDR::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
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
		publishLdrLast = 0;
	}
	if(ldrLast != ldr || wpFZ.CheckQoS(publishLdrLast)) {
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
		int8 readCorrection = msg.toInt();
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
	if(wpFZ.loopStartedAt > publishLdrLast + publishLdrMin) {
		wpMqtt.mqttClient.publish(mqttTopicLdr.c_str(), String(ldr).c_str());
		ldrLast = ldr;
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("LDR", String(ldr));
		}
		publishLdrLast = wpFZ.loopStartedAt;
	}
}

void moduleLDR::calc() {
	int read = analogRead(Pin);
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
uint16 moduleLDR::calcAvg(uint16 raw) {
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
uint16 moduleLDR::getVersion() {
	String SVN = "$Rev: 232 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleLDR::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyValue(F("CalcCycle"), String(CalcCycle())) + F(",") +
		wpFZ.JsonKeyValue(F("useAvg"), UseAvg() ? "true" : "false") + F(",") +
		wpFZ.JsonKeyValue(F("Correction"), String(correction)) +
		F("}");
	return json;
}

void moduleLDR::changeDebug() {
	mb->changeDebug();
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
uint32 moduleLDR::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleLDR::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
