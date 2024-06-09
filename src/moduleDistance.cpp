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
//# Revision     : $Rev:: 136                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDistance.cpp 136 2024-06-09 15:37:41Z              $ #
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
	trigPin = D1;
	echoPin = D2;
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT_PULLUP);
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
	publishCountVolume = 0;
	distanceRawLast = 0;
	publishCountDistanceRaw = 0;
	distanceAvgLast = 0;
	publishCountDistanceAvg = 0;

	mb->initRest(wpEEPROM.addrBitsSendRestModules0, wpEEPROM.bitsSendRestModules0, wpEEPROM.bitSendRestDistance);
	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugDistance);
	mb->initError();
	mb->initMaxCycle(wpEEPROM.byteMaxCycleDistance);
}

//###################################################################################
// public
//###################################################################################
void moduleDistance::cycle() {	if(
	wpFZ.calcValues && ++mb->cycleCounter >= mb->maxCycle) {
		calc();
		mb->cycleCounter = 0;
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
		publishCountVolume = wpFZ.publishQoS;
		publishCountDistanceRaw = wpFZ.publishQoS;
		publishCountDistanceAvg = wpFZ.publishQoS;
	}
	if(volumeLast != volume || ++publishCountVolume > wpFZ.publishQoS) {
		publishValue();
	}
	if(distanceRawLast != distanceRaw || ++publishCountDistanceRaw > wpFZ.publishQoS) {
		publishDistanceRaw();
	}
	if(distanceAvgLast != distanceAvg || ++publishCountDistanceAvg > wpFZ.publishQoS) {
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
		uint8_t readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			EEPROM.write(wpEEPROM.byteDistanceCorrection, correction);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicCorrection, String(correction));
		}
	}
	if(strcmp(topic, mqttTopicHeight.c_str()) == 0) {
		uint8_t readHeight = msg.toInt();
		if(height != readHeight) {
			height = readHeight;
			EEPROM.write(wpEEPROM.byteHeight, height);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicHeight, String(height));
		}
	}
	if(strcmp(topic, mqttTopicMaxVolume.c_str()) == 0) {
		uint16_t readMaxVolume = msg.toInt();
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
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("vol", String(volume));
		wpRest.trySend = true;
	}
	volumeLast = volume;
	if(wpMqtt.Debug) {
		printPublishValueDebug("Volume", String(volume), String(publishCountVolume));
	}
	publishCountVolume = 0;
}

void moduleDistance::publishDistanceRaw() {
	wpMqtt.mqttClient.publish(mqttTopicDistanceRaw.c_str(), String(distanceRaw).c_str());
	distanceRawLast = distanceRaw;
	if(wpMqtt.Debug) {
		printPublishValueDebug("DistanceRaw", String(distanceRaw), String(publishCountDistanceRaw));
	}
	publishCountDistanceRaw = 0;
}

void moduleDistance::publishDistanceAvg() {
	wpMqtt.mqttClient.publish(mqttTopicDistanceAvg.c_str(), String(distanceAvg).c_str());
	distanceAvgLast = distanceAvg;
	if(wpMqtt.Debug) {
		printPublishValueDebug("DistanceAvg", String(distanceAvg), String(publishCountDistanceAvg));
	}
	publishCountDistanceAvg = 0;
}

void moduleDistance::calc() {
	unsigned long duration;
	// Sender kurz ausschalten um Störungen des Signal zu vermeiden
	digitalWrite(trigPin, LOW);
	delay(10);
	// Signal senden
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
	// Zeit messen, bis das Signal zurückkommt, mit timeout
	duration = pulseIn(echoPin, HIGH, wpFZ.loopTime * 1000);
	if(duration > 0) {
		distanceRaw = ((duration * 0.03432 / 2) * 10) + correction;
		distanceAvg = calcAvg(distanceRaw);
		if(height <= 0) height = 1; // durch 0
		if(distanceAvg > height * 10) distanceAvg = height * 10;
		volume = maxVolume - round(maxVolume * distanceAvg / (height * 10));
		if(volume > maxVolume) volume = maxVolume;
		mb->error = false;
		if(mb->debug) {
			calcDistanceDebug("Distance", distanceAvg, distanceRaw);
		}
	} else {
		mb->error = true;
		String logmessage = "Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "calcDistance", logmessage);
	}
}
uint16_t moduleDistance::calcAvg(uint16_t raw) {
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
void moduleDistance::calcDistanceDebug(String name, uint16_t avg, uint16_t raw) {
	String logmessage = name + ": " + String(avg) + " (" + String(raw) + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "calcDistance", logmessage);
}
void moduleDistance::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}


//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleDistance::getVersion() {
	String SVN = "$Rev: 136 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleDistance::changeSendRest() {
	mb->changeSendRest();
}
void moduleDistance::changeDebug() {
	mb->changeDebug();
}
bool moduleDistance::SendRest() {
	return mb->sendRest;
}
bool moduleDistance::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleDistance::Debug() {
	return mb->debug;
}
bool moduleDistance::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint8_t moduleDistance::MaxCycle(){
	return mb->maxCycle;
}
uint8_t moduleDistance::MaxCycle(uint8_t maxCycle){
	mb->maxCycle = maxCycle;
	return 0;
}
