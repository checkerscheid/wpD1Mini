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
//# Revision     : $Rev:: 258                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleWeight.cpp 258 2025-04-28 13:34:51Z                $ #
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
	makeTare = false;
	make1kg = false;
	// values
	mqttTopicWeight = wpFZ.DeviceName + "/" + ModuleName + "/Output";
	mqttTopicTareValue = wpFZ.DeviceName + "/" + ModuleName + "/TareValue";
	mqttTopicTare1kg = wpFZ.DeviceName + "/" + ModuleName + "/Tare1kg";
	// settings
	mqttTopicSetTare = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetTare";
	mqttTopicSet1kg = wpFZ.DeviceName + "/settings/" + ModuleName + "/Set1kg";

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
		wpMqtt.mqttClient.publish(mqttTopicSet1kg.c_str(), "0");
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
	if(tare1kgLast != tare1kg || wpFZ.CheckQoS(publishTare1kgLast)) {
		wpMqtt.mqttClient.publish(mqttTopicTare1kg.c_str(), String(tare1kg).c_str());
		tare1kgLast = tare1kg;
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("Tare1kg", String(tare1kg));
		}
		publishTare1kgLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleWeight::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetTare.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSet1kg.c_str());
	mb->setSubscribes();
}

void moduleWeight::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetTare.c_str()) == 0) {
		bool readSetTare = msg.toInt();
		if(readSetTare != 0) {
			SetTare();
			wpMqtt.mqttClient.publish(mqttTopicSetTare.c_str(), "0");
			wpFZ.DebugcheckSubscribes(mqttTopicSetTare, String(readSetTare));
		}
	}
	if(strcmp(topic, mqttTopicSet1kg.c_str()) == 0) {
		bool readSet1kg = msg.toInt();
		if(readSet1kg != 0) {
			Set1kg();
			wpMqtt.mqttClient.publish(mqttTopicSet1kg.c_str(), "0");
			wpFZ.DebugcheckSubscribes(mqttTopicSet1kg, String(readSet1kg));
		}
	}
	mb->checkSubscribes(topic, msg);
}

void moduleWeight::InitTareValue(uint32 tv) {
	tareValue = tv * 1000;
}
void moduleWeight::SetTare() {
	makeTare = true;
	calc();
}
void moduleWeight::InitTare1kg(uint32 t1kg) {
	tare1kg = t1kg * 1000;
}
void moduleWeight::Set1kg() {
	make1kg = true;
	calc();
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
		long raw = -1 * scale->read();
		if(makeTare) {
			tareValue = raw;
			uint32 tv = (uint32) (tareValue / 1000.0);
			wpEEPROM.WriteWordToEEPROM("TareValue", wpEEPROM.byteWeightTareValue, tv);
			makeTare = false;
		}
		if(make1kg) {
			tare1kg = raw;
			uint32 t1kg = (uint32) (tare1kg / 1000.0);
			wpEEPROM.WriteWordToEEPROM("Tare1kg", wpEEPROM.byteWeightTare1kg, t1kg);
			make1kg = false;
		}
		// long correct = tareValue + raw;
		// long mapped = correct * 64.98; // old static
		long mapped = wpFZ.Map(raw, tareValue, tare1kg, 0, 1000, true, false);
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
				//"Correct: " + String(correct) + ", "
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
	String SVN = "$Rev: 258 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleWeight::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyString(F("Pinout"), String(wpFZ.Pins[Pinout])) + F(",") +
		wpFZ.JsonKeyValue(F("Tare 0"), String(tareValue)) + F(",") +
		wpFZ.JsonKeyValue(F("Tare 1 kg"), String(tare1kg)) + F(",") +
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
