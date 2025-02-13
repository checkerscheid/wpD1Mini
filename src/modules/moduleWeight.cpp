//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 28.10.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 235                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleWeight.cpp 235 2024-12-20 07:34:35Z                $ #
//#                                                                                 #
//###################################################################################
#include <moduleWeight.h>

moduleWeight wpWeight;

moduleWeight::moduleWeight() {
	// section to config and copy
	ModuleName = "Weight";
	mb = new moduleBase(ModuleName);
	scale = new HX711();
}
void moduleWeight::init() {
	// section for define
	Pinout = D6; // DT / DAT
	Pin = D7; // CLK / SCK
	scale->begin(Pinout, Pin);
	weight = 0;
	tare = false;
	// values
	mqttTopicWeight = wpFZ.DeviceName + "/" + ModuleName + "/Output";
	mqttTopicTareValue = wpFZ.DeviceName + "/" + ModuleName + "/Tare";
	// settings
	mqttTopicSetTare = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetTare";

	weightLast = 0;
	publishWeightLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules2, wpEEPROM.bitsDebugModules2, wpEEPROM.bitDebugWeight);
	mb->initUseAvg(wpEEPROM.addrBitsSettingsModules1, wpEEPROM.bitsSettingsModules1, wpEEPROM.bitUseWeightAvg);
	mb->initError();
	mb->initCalcCycle(wpEEPROM.byteCalcCycleWeight);

	mb->calcLast = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleWeight::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleWeight::publishSettings() {
	publishSettings(false);
}
void moduleWeight::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetTare.c_str(), "0");
	}
	mb->publishSettings(force);
}

void moduleWeight::publishValues() {
	publishValues(false);
}
void moduleWeight::publishValues(bool force) {
	if(force) {
		publishWeightLast = 0;
		publishTareValueLast = 0;
	}
	if(weightLast != weight || wpFZ.CheckQoS(publishWeightLast)) {
		publishValue();
	}
	if(tareValueLast != tareValue || wpFZ.CheckQoS(publishTareValueLast)) {
		wpMqtt.mqttClient.publish(mqttTopicTareValue.c_str(), String(tareValue).c_str());
		tareValueLast = tareValue;
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("TareValue", String(tareValue));
		}
		publishTareValueLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleWeight::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetTare.c_str());
	mb->setSubscribes();
}

void moduleWeight::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetTare.c_str()) == 0) {
		bool readSetTare = msg.toInt();
		if(readSetTare != 0) {
			tare = true;
			wpMqtt.mqttClient.publish(mqttTopicSetTare.c_str(), "0");
			wpFZ.DebugcheckSubscribes(mqttTopicSetTare, String(readSetTare));
		}
	}
	mb->checkSubscribes(topic, msg);
}

void moduleWeight::InitTareValue(uint32 tv) {
	tareValue = tv * 1000;
}
void moduleWeight::SetTare() {
	tare = true;
}

//###################################################################################
// private
//###################################################################################
void moduleWeight::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicWeight.c_str(), String(weight).c_str());
	weightLast = weight;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("Weight", String(weight));
	}
	publishWeightLast = wpFZ.loopStartedAt;
}

void moduleWeight::calc() {
	if(scale->is_ready()) {
		long raw = scale->read();
		if(tare) {
			tareValue = -1 * raw;
			EEPROM.put(wpEEPROM.byteWeightTareValue, (uint32) (tareValue / 1000.0));
			EEPROM.commit();
			tare = false;
		}
		long correct = tareValue + raw;
		long mapped = correct * 64.98; //wpFZ.Map(correct, 0, 427800, 0, 1000000, true, false);
		long avg = mapped;
		if(mb->useAvg) {
			avg = calcAvg(avg);
		}
		weight = avg;
		mb->error = false;
		if(mb->debug) {
			String logmessage = "Weight: " + String(weight) + " ("
				"Avg: " + String(avg) + ", "
				"Mapped: " + String(mapped) + ", "
				"Correct: " + String(correct) + ", "
				"Raw: " + String(raw) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "calcWeight", logmessage);
		}
	} else {
		mb->error = true;
		String logmessage = "Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "calcWeight", logmessage);
	}
}
long moduleWeight::calcAvg(long raw) {
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
uint16 moduleWeight::getVersion() {
	String SVN = "$Rev: 235 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleWeight::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyString(F("Pinout"), String(wpFZ.Pins[Pinout])) + F(",") +
		wpFZ.JsonKeyValue(F("CalcCycle"), String(CalcCycle())) + F(",") +
		wpFZ.JsonKeyValue(F("useAvg"), UseAvg() ? "true" : "false") +
		F("}");
	return json;
}

void moduleWeight::changeDebug() {
	mb->changeDebug();
}
bool moduleWeight::UseAvg() {
	return mb->useAvg;
}
bool moduleWeight::UseAvg(bool useAvg) {
	mb->useAvg = useAvg;
	return true;
}
bool moduleWeight::Debug() {
	return mb->debug;
}
bool moduleWeight::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32 moduleWeight::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleWeight::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
