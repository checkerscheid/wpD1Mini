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
//# Revision     : $Rev:: 128                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDistance.cpp 128 2024-06-03 11:49:32Z              $ #
//#                                                                                 #
//###################################################################################
#include <moduleDistance.h>

moduleDistance wpDistance;

moduleDistance::moduleDistance() {}
void moduleDistance::init() {
	trigPin = D1;
	echoPin = D2;
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT_PULLUP);
	volume = 0;
	height = 0;
	distanceRaw = 0;
	distanceAvg = 0;
	error = false;
	// values
	mqttTopicVolume = wpFZ.DeviceName + "/Volume";
	mqttTopicdistanceRaw = wpFZ.DeviceName + "/Distance/Raw";
	mqttTopicdistanceAvg = wpFZ.DeviceName + "/Distance/Avg";
	mqttTopicError = wpFZ.DeviceName + "/ERROR/Distance";
	// settings
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/Distance/maxCycle";
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/Distance/correction";
	mqttTopicHeight = wpFZ.DeviceName + "/settings/Distance/Height";
	mqttTopicMaxVolume = wpFZ.DeviceName + "/settings/Distance/maxVolume";
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Distance";

	cycleCounter = 0;
	volumeLast = 0;
	publishCountVolume = 0;
	distanceRawLast = 0;
	publishCountDistanceRaw = 0;
	distanceAvgLast = 0;
	publishCountDistanceAvg = 0;
	errorLast = false;
	publishCountError = 0;
	DebugLast = false;
	publishCountDebug = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleDistance::cycle() {
	if(wpFZ.calcValues && ++cycleCounter >= maxCycle) {
		cycleCounter = 0;
		calc();
	}
	publishValues();
}

uint16_t moduleDistance::getVersion() {
	String SVN = "$Rev: 128 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleDistance::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugDistance, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugDistance\",\"value\":" + String(Debug ? "true" : "false") + "}");
	wpFZ.blink();
}

void moduleDistance::publishSettings() {
	publishSettings(false);
}
void moduleDistance::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicMaxCycle.c_str(), String(maxCycle).c_str());
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	wpMqtt.mqttClient.publish(mqttTopicHeight.c_str(), String(height).c_str());
	wpMqtt.mqttClient.publish(mqttTopicMaxVolume.c_str(), String(maxVolume).c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
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
	if(errorLast != error || ++publishCountError > wpFZ.publishQoS) {
		errorLast = error;
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
		publishCountError = 0;
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void moduleDistance::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicHeight.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicMaxVolume.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}

void moduleDistance::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		byte readMaxCycle = msg.toInt();
		if(readMaxCycle <= 0) readMaxCycle = 1;
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			EEPROM.write(wpEEPROM.byteMaxCycleDistance, maxCycle);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxCycle, String(maxCycle));
		}
	}
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
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugDHT, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
			wpFZ.SendWS("{\"id\":\"DebugDHT\",\"value\":" + String(Debug ? "true" : "false") + "}");
		}
	}
}

//###################################################################################
// private
//###################################################################################
void moduleDistance::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicVolume.c_str(), String(volume).c_str());
	wpRest.error = wpRest.error | !wpRest.sendRest("volume", String(volume));
	wpRest.trySend = true;
	volumeLast = volume;
	if(wpMqtt.Debug) {
		printPublishValueDebug("Volume", String(volume), String(publishCountVolume));
	}
	publishCountVolume = 0;
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
		distanceRaw = (duration * 0.03432 / 2);
		distanceAvg = calcAvg(distanceRaw) + correction;
		if(height <= 0) height = 1; // durch 0
		if(distanceAvg > height) distanceAvg = height;
		volume = maxVolume - round(maxVolume * distanceAvg / height);
		if(volume > maxVolume) volume = maxVolume;
		error = false;
		if(Debug) {
			calcDistanceDebug("Distance", distanceAvg, distanceRaw);
		}
		delay(wpFZ.loopTime);
	} else {
		error = true;
		String logmessage = "Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "calcDistance", logmessage);
	}
}
uint16_t moduleDistance::calcAvg(uint16_t raw) {
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
void moduleDistance::calcDistanceDebug(String name, uint8_t avg, uint8_t raw) {
	String logmessage = name + ": " + String(avg) + " (" + String(raw) + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "calcDistance", logmessage);
}
void moduleDistance::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
