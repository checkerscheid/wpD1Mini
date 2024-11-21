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
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDistance.cpp 207 2024-10-07 12:59:22Z              $ #
//#                                                                                 #
//###################################################################################
#include <moduleDistance.h>

moduleDistance wpDistance;

moduleDistance::moduleDistance() {
	// section to config and copy
	ModuleName = "Distance";
	mb = new moduleBase(ModuleName);
}
void moduleDistance::init() {
	// section for define
	PinTrig = D1;
	PinEcho = D2;
	pinMode(PinTrig, OUTPUT);
	pinMode(PinEcho, INPUT);
	volume = 0;
	distanceRaw = 0;
	distanceAvg = 0;
	// values
	mqttTopicVolume = wpFZ.DeviceName + "/Volume";
	mqttTopicDistanceRaw = wpFZ.DeviceName + "/" + ModuleName + "/Raw";
	mqttTopicDistanceAvg = wpFZ.DeviceName + "/" + ModuleName + "/Avg";
	// settings
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/correction";
	mqttTopicHeight = wpFZ.DeviceName + "/settings/" + ModuleName + "/Height";
	mqttTopicMaxVolume = wpFZ.DeviceName + "/settings/" + ModuleName + "/maxVolume";

	volumeLast = 0;
	publishVolumeLast = 0;
	distanceRawLast = 0;
	publishDistanceRawLast = 0;
	distanceAvgLast = 0;
	publishDistanceAvgLast = 0;

	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugDistance);
	mb->initError();
	mb->initCalcCycle(wpEEPROM.byteCalcCycleDistance);

	mb->calcLast = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleDistance::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleDistance::publishSettings() {
	publishSettings(false);
}
void moduleDistance::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	wpMqtt.mqttClient.publish(mqttTopicHeight.c_str(), String(height).c_str());
	wpMqtt.mqttClient.publish(mqttTopicMaxVolume.c_str(), String(maxVolume).c_str());
	mb->publishSettings(force);
}

void moduleDistance::publishValues() {
	publishValues(false);
}
void moduleDistance::publishValues(bool force) {
	if(force) {
		publishVolumeLast = 0;
		publishDistanceRawLast = 0;
		publishDistanceAvgLast = 0;
	}
	if(volumeLast != volume || wpFZ.CheckQoS(publishVolumeLast)) {
		publishValue();
	}
	if(distanceRawLast != distanceRaw || wpFZ.CheckQoS(publishDistanceRawLast)) {
		publishDistanceRaw();
	}
	if(distanceAvgLast != distanceAvg || wpFZ.CheckQoS(publishDistanceAvgLast)) {
		publishDistanceAvg();
	}
	mb->publishValues(force);
}

void moduleDistance::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicHeight.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicMaxVolume.c_str());
	mb->setSubscribes();
}

void moduleDistance::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicCorrection.c_str()) == 0) {
		uint8 readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			EEPROM.write(wpEEPROM.byteDistanceCorrection, correction);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicCorrection, String(correction));
		}
	}
	if(strcmp(topic, mqttTopicHeight.c_str()) == 0) {
		uint8 readHeight = msg.toInt();
		if(height != readHeight) {
			height = readHeight;
			EEPROM.write(wpEEPROM.byteHeight, height);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicHeight, String(height));
		}
	}
	if(strcmp(topic, mqttTopicMaxVolume.c_str()) == 0) {
		uint16 readMaxVolume = msg.toInt();
		if(maxVolume != readMaxVolume) {
			maxVolume = readMaxVolume;
			EEPROM.put(wpEEPROM.byteMaxVolume, maxVolume);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxVolume, String(maxVolume));
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleDistance::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicVolume.c_str(), String(volume).c_str());
	volumeLast = volume;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("Volume", String(volume));
	}
	publishVolumeLast = wpFZ.loopStartedAt;
}

void moduleDistance::publishDistanceRaw() {
	wpMqtt.mqttClient.publish(mqttTopicDistanceRaw.c_str(), String(distanceRaw).c_str());
	distanceRawLast = distanceRaw;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("DistanceRaw", String(distanceRaw));
	}
	publishDistanceRawLast = wpFZ.loopStartedAt;
}

void moduleDistance::publishDistanceAvg() {
	wpMqtt.mqttClient.publish(mqttTopicDistanceAvg.c_str(), String(distanceAvg).c_str());
	distanceAvgLast = distanceAvg;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("DistanceAvg", String(distanceAvg));
	}
	publishDistanceAvgLast = wpFZ.loopStartedAt;
}

void moduleDistance::calc() {
	unsigned long duration;
	// Sender kurz ausschalten um Störungen des Signal zu vermeiden
	digitalWrite(PinTrig, LOW);
	delay(10);
	// Signal senden
	digitalWrite(PinTrig, HIGH);
	delayMicroseconds(10);
	digitalWrite(PinTrig, LOW);
	// Zeit messen, bis das Signal zurückkommt, mit timeout
	duration = pulseIn(PinEcho, HIGH, 100 * 1000);
	if(duration > 0) {
		distanceRaw = ((duration * 0.03432 / 2) * 10) + correction;
		distanceAvg = calcAvg(distanceRaw);
		if(height <= 0) height = 1; // durch 0
		if(distanceAvg > height * 10) distanceAvg = height * 10;
		volume = maxVolume - round(maxVolume * distanceAvg / (height * 10));
		if(volume > maxVolume) volume = maxVolume;
		mb->error = false;
		if(mb->debug) {
			calcDistanceDebug("Distance", distanceAvg, distanceRaw, duration);
		}
	} else {
		mb->error = true;
		String logmessage = "Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "calcDistance", logmessage);
	}
}
uint16 moduleDistance::calcAvg(uint16 raw) {
	unsigned long avg = 0;
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
void moduleDistance::calcDistanceDebug(String name, uint16 avg, uint16 raw, unsigned long duration) {
	String logmessage = name + " (avg): " + String(avg) + " (raw: " + String(raw) + ", duration: " + String(duration) + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "calcDistance", logmessage);
}

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleDistance::getVersion() {
	String SVN = "$Rev: 207 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleDistance::changeDebug() {
	mb->changeDebug();
}
bool moduleDistance::Debug() {
	return mb->debug;
}
bool moduleDistance::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32 moduleDistance::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleDistance::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
