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
//# Revision     : $Rev:: 131                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLDR.cpp 131 2024-06-05 03:01:06Z                   $ #
//#                                                                                 #
//###################################################################################
#include <moduleLDR.h>

moduleLDR wpLDR;

moduleLDR::moduleLDR() {}
void moduleLDR::init() {
	LDRPin = A0;
	LDR = 0;
	error = false;
	// values
	mqttTopicLDR = wpFZ.DeviceName + "/LDR";
	mqttTopicError = wpFZ.DeviceName + "/ERROR/LDR";
	// settings
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/LDR/maxCycle";
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/LDR/Correction";
	mqttTopicUseAvg = wpFZ.DeviceName + "/settings/LDR/useAvg";
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/LDR";

	cycleCounter = 0;
	errorLast = false;
	publishCountError = 0;
	LDRLast = 0;
	publishCountLDR = 0;
	DebugLast = false;
	publishCountDebug = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleLDR::cycle() {
	if(wpFZ.calcValues && ++cycleCounter >= maxCycle) {
		cycleCounter = 0;
		calc();
	}
	publishValues();
}

uint16_t moduleLDR::getVersion() {
	String SVN = "$Rev: 131 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleLDR::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugLDR, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugLDR", Debug);
	wpFZ.blink();
}

void moduleLDR::publishSettings() {
	publishSettings(false);
}
void moduleLDR::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicMaxCycle.c_str(), String(maxCycle).c_str());
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseAvg.c_str(), String(useAvg).c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void moduleLDR::publishValues() {
	publishValues(false);
}
void moduleLDR::publishValues(bool force) {
	if(force) {
		publishCountLDR = wpFZ.publishQoS;
		publishCountError = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
	}
	if(LDRLast != LDR || ++publishCountLDR > wpFZ.publishQoS) {
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

void moduleLDR::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseAvg.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}

void moduleLDR::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		byte readMaxCycle = msg.toInt();
		if(readMaxCycle <= 0) readMaxCycle = 1;
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			EEPROM.write(wpEEPROM.byteMaxCycleLDR, maxCycle);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxCycle, String(maxCycle));
		}
	}
	if(strcmp(topic, mqttTopicCorrection.c_str()) == 0) {
		int8_t readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			EEPROM.put(wpEEPROM.byteLDRCorrection, correction);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicCorrection, String(correction));
		}
	}
	if(strcmp(topic, mqttTopicUseAvg.c_str()) == 0) {
		bool readAvg = msg.toInt();
		if(useAvg != readAvg) {
			useAvg = readAvg;
			bitWrite(wpEEPROM.bitsModulesSettings0, wpEEPROM.bitUseLdrAvg, useAvg);
			EEPROM.write(wpEEPROM.addrBitsModulesSettings0, wpEEPROM.bitsModulesSettings0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicUseAvg, String(useAvg));
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugLDR, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
			wpFZ.SendWSDebug("DebugLDR", Debug);
		}
	}
}

//###################################################################################
// private
//###################################################################################
void moduleLDR::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicLDR.c_str(), String(LDR).c_str());
	wpRest.error = wpRest.error | !wpRest.sendRest("ldr", String(LDR));
	wpRest.trySend = true;
	LDRLast = LDR;
	if(wpMqtt.Debug) {
		printPublishValueDebug("LDR", String(LDR), String(publishCountLDR));
	}
	publishCountLDR = 0;
}

void moduleLDR::calc() {
	int newLDR = analogRead(LDRPin);
	if(!isnan(newLDR)) {
		if(newLDR > 1023) newLDR = 1023;
		if(newLDR < 0) newLDR = 0;
		if(useAvg) {
			newLDR = calcAvg(newLDR);
		}
		LDR = (1023 - newLDR) + correction;
		error = false;
		if(Debug) {
			String logmessage = "LDR: " + String(LDR) + " (" + String(newLDR) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "calcLDR", logmessage);
		}
	} else {
		error = true;
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
