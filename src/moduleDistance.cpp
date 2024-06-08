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
//# Revision     : $Rev:: 135                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDistance.cpp 135 2024-06-06 14:04:54Z              $ #
//#                                                                                 #
//###################################################################################
#include <moduleDistance.h>

moduleDistance wpDistance;

moduleDistance::moduleDistance() {}
void moduleDistance::init() {
	// section to config and copy
	ModuleName = "Distance";
	addrMaxCycle = wpEEPROM.byteMaxCycleDistance;
	addrSendRest = wpEEPROM.addrBitsSendRestModules0;
	byteSendRest = wpEEPROM.bitsSendRestModules0;
	bitSendRest = wpEEPROM.bitSendRestDistance;
	addrDebug = wpEEPROM.addrBitsDebugModules0;
	byteDebug = wpEEPROM.bitsDebugModules0;
	bitDebug = wpEEPROM.bitDebugDistance;

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

	// section to copy
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/" + ModuleName + "/maxCycle";
	mqttTopicSendRest = wpFZ.DeviceName + "/settings/SendRest/" + ModuleName;
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/" + ModuleName;
	mqttTopicError = wpFZ.DeviceName + "/ERROR/" + ModuleName;

	cycleCounter = 0;
	sendRestLast = false;
	publishCountSendRest = 0;
	DebugLast = false;
	publishCountDebug = 0;
	errorLast = false;
	publishCountError = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleDistance::cycle() {	if(
	wpFZ.calcValues && ++cycleCounter >= maxCycle) {
		calc();
		cycleCounter = 0;
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
	publishDefaultSettings(force);
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
	publishDefaultValues(force);
}

void moduleDistance::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicHeight.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicMaxVolume.c_str());
	setDefaultSubscribes();
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
	checkDefaultSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleDistance::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicVolume.c_str(), String(volume).c_str());
	if(sendRest) {
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
		error = false;
		if(Debug) {
			calcDistanceDebug("Distance", distanceAvg, distanceRaw);
		}
	} else {
		error = true;
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
	String SVN = "$Rev: 132 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}
void moduleDistance::changeSendRest() {
	sendRest = !sendRest;
	bitWrite(byteSendRest, bitSendRest, sendRest);
	EEPROM.write(addrSendRest, byteSendRest);
	EEPROM.commit();
	wpFZ.blink();
}
void moduleDistance::changeDebug() {
	Debug = !Debug;
	bitWrite(byteDebug, bitDebug, Debug);
	EEPROM.write(addrDebug, byteDebug);
	EEPROM.commit();
	wpFZ.blink();
}
void moduleDistance::publishDefaultSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
	}
}
void moduleDistance::publishDefaultValues(bool force) {
	if(force) {
		publishCountSendRest = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
		publishCountError = wpFZ.publishQoS;
	}
	if(sendRestLast != sendRest || ++publishCountSendRest > wpFZ.publishQoS) {
		sendRestLast = sendRest;
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		wpFZ.SendWSSendRest("sendRest" + ModuleName, sendRest);
		publishCountSendRest = 0;
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		wpFZ.SendWSDebug("Debug" + ModuleName, Debug);
		publishCountDebug = 0;
	}
	if(errorLast != error || ++publishCountError > wpFZ.publishQoS) {
		errorLast = error;
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
		publishCountError = 0;
	}
}
void moduleDistance::setDefaultSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSendRest.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void moduleDistance::checkDefaultSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		uint8_t readMaxCycle = msg.toInt();
		if(readMaxCycle <= 0) readMaxCycle = 1;
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			EEPROM.write(addrMaxCycle, maxCycle);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxCycle, String(maxCycle));
		}
	}
	if(strcmp(topic, mqttTopicSendRest.c_str()) == 0) {
		bool readSendRest = msg.toInt();
		if(sendRest != readSendRest) {
			sendRest = readSendRest;
			bitWrite(byteSendRest, bitSendRest, sendRest);
			EEPROM.write(addrSendRest, byteSendRest);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSendRest, String(sendRest));
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(byteDebug, bitDebug, Debug);
			EEPROM.write(addrDebug, byteDebug);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}
