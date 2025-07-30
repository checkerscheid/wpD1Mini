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
//# Revision     : $Rev:: 270                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRain.cpp 270 2025-07-30 22:04:37Z                  $ #
//#                                                                                 #
//###################################################################################
#include <moduleRain.h>

moduleRain wpRain;

moduleRain::moduleRain() {
	// section to config and copy
	ModuleName = "Rain";
	mb = new moduleBase(ModuleName);
}
void moduleRain::init() {
	// section for define
	Pin = A0;
	rain = 0;
	// values
	mqttTopicRain = wpFZ.DeviceName + "/" + ModuleName;
	// settings
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction";

	rainLast = 0;
	publishRainLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugRain);
	mb->initUseAvg(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0, wpEEPROM.bitUseRainAvg);
	mb->initError();
	mb->initCalcCycle(wpEEPROM.byteCalcCycleRain);

	mb->calcLast = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleRain::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleRain::publishSettings() {
	publishSettings(false);
}
void moduleRain::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	mb->publishSettings(force);
}

void moduleRain::publishValues() {
	publishValues(false);
}
void moduleRain::publishValues(bool force) {
	if(force) {
		publishRainLast = 0;
	}
	if(rainLast != rain || wpFZ.CheckQoS(publishRainLast)) {
		publishValue();
	}
	mb->publishValues(force);
}

void moduleRain::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	mb->setSubscribes();
}

void moduleRain::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicCorrection.c_str()) == 0) {
		int8_t readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			wpEEPROM.WriteByteToEEPROM("RainCorrection", wpEEPROM.byteRainCorrection, correction);
			wpFZ.DebugcheckSubscribes(mqttTopicCorrection, String(correction));
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleRain::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicRain.c_str(), String(rain).c_str());
	rainLast = rain;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("Rain", String(rain));
	}
	publishRainLast = wpFZ.loopStartedAt;
}

void moduleRain::calc() {
	int read = analogRead(Pin);
	int minMax, avg, correct, raw;
	if(!isnan(read)) {
		minMax = read;
		if(minMax > 1023) minMax = 1023;
		if(minMax < 0) minMax = 0;
		avg = minMax;
		if(mb->useAvg) {
			avg = calcAvg(avg);
		}
		correct = avg;
		raw = wpFZ.Map(correct, 1023, 0, 0, 500);
		/// 0 - 50 l/m²
		rain = (float)(raw / 10.0) + correction;
		mb->error = false;
		if(mb->debug) {
			String logmessage = "Rain: " + String(rain) + " ("
				"Read: " + String(read) + ", "
				"MinMax: " + String(minMax) + ", "
				"Avg: " + String(correct) + ", "
				"Raw: " + String(raw) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "calcRain", logmessage);
		}
	} else {
		mb->error = true;
		String logmessage = "Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "calcRain", logmessage);
	}
}
uint16_t moduleRain::calcAvg(uint16_t raw) {
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
uint16_t moduleRain::getVersion() {
	String SVN = "$Rev: 270 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleRain::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyValue(F("CalcCycle"), String(CalcCycle())) + F(",") +
		wpFZ.JsonKeyValue(F("useAvg"), UseAvg() ? "true" : "false") + F(",") +
		wpFZ.JsonKeyValue(F("Correction"), String(correction)) +
		F("}");
	return json;
}

void moduleRain::changeDebug() {
	mb->changeDebug();
}
bool moduleRain::UseAvg() {
	return mb->useAvg;
}
bool moduleRain::UseAvg(bool useAvg) {
	mb->useAvg = useAvg;
	return true;
}
bool moduleRain::Debug() {
	return mb->debug;
}
bool moduleRain::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32_t moduleRain::CalcCycle(){
	return mb->calcCycle;
}
uint32_t moduleRain::CalcCycle(uint32_t calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
