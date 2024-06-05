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
//# File-ID      : $Id:: moduleMoisture.cpp 131 2024-06-05 03:01:06Z              $ #
//#                                                                                 #
//###################################################################################
#include <moduleMoisture.h>

moduleMoisture wpMoisture;

moduleMoisture::moduleMoisture() {}
void moduleMoisture::init() {
	moisturePin = A0;
	moisture = 0;
	error = false;
	errorMin = false;

	// values
	mqttTopicMoisture = wpFZ.DeviceName + "/Moisture";
	mqttTopicError = wpFZ.DeviceName + "/ERROR/Moisture";
	mqttTopicErrorMin = wpFZ.DeviceName + "/ERROR/MoistureMin";
	// settings
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/Moisture/maxCycle";
	mqttTopicUseAvg = wpFZ.DeviceName + "/settings/Moisture/useAvg";
	mqttTopicMin = wpFZ.DeviceName + "/settings/Moisture/Min";
	mqttTopicDry = wpFZ.DeviceName + "/settings/Moisture/Dry";
	mqttTopicWet = wpFZ.DeviceName + "/settings/Moisture/Wet";
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Moisture";

	cycleCounter = 0;
	moistureLast = 0;
	publishCountMoisture = 0;
	errorLast = false;
	publishCountError = 0;
	errorMinLast = false;
	publishCountErrorMin = 0;
	DebugLast = false;
	publishCountDebug = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleMoisture::cycle() {
	if(wpFZ.calcValues && ++cycleCounter >= maxCycle) {
		cycleCounter = 0;
		calc();
	}
	publishValues();
}

uint16_t moduleMoisture::getVersion() {
	String SVN = "$Rev: 131 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleMoisture::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugMoisture, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugMoisture", Debug);
	wpFZ.blink();
}

void moduleMoisture::publishSettings() {
	publishSettings(false);
}
void moduleMoisture::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicMaxCycle.c_str(), String(maxCycle).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseAvg.c_str(), String(useAvg).c_str());
	wpMqtt.mqttClient.publish(mqttTopicMin.c_str(), String(minValue).c_str());
	wpMqtt.mqttClient.publish(mqttTopicDry.c_str(), String(dry).c_str());
	wpMqtt.mqttClient.publish(mqttTopicWet.c_str(), String(wet).c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void moduleMoisture::publishValues() {
	publishValues(false);
}
void moduleMoisture::publishValues(bool force) {
	if(force) {
		publishCountMoisture = wpFZ.publishQoS;
		publishCountError = wpFZ.publishQoS;
		publishCountErrorMin = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
	}
	if(moistureLast != moisture || ++publishCountMoisture > wpFZ.publishQoS) {
		publishValue();
	}
	if(errorLast != error || ++publishCountError > wpFZ.publishQoS) {
		errorLast = error;
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
		publishCountError = 0;
	}
	if(errorMinLast != errorMin || ++publishCountErrorMin > wpFZ.publishQoS) {
		errorMinLast = errorMin;
		wpMqtt.mqttClient.publish(mqttTopicErrorMin.c_str(), String(errorMin).c_str());
		publishCountErrorMin = 0;
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void moduleMoisture::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseAvg.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicMin.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDry.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicWet.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}

void moduleMoisture::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		byte readMaxCycle = msg.toInt();
		if(readMaxCycle <= 0) readMaxCycle = 1;
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			EEPROM.write(wpEEPROM.byteMaxCycleMoisture, maxCycle);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxCycle, String(maxCycle));
		}
	}
	if(strcmp(topic, mqttTopicUseAvg.c_str()) == 0) {
		bool readAvg = msg.toInt();
		if(useAvg != readAvg) {
			useAvg = readAvg;
			bitWrite(wpEEPROM.bitsModulesSettings, wpEEPROM.bitUseMoistureAvg, useAvg);
			EEPROM.write(wpEEPROM.addrBitsModulesSettings, wpEEPROM.bitsModulesSettings);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicUseAvg, String(useAvg));
		}
	}
	if(strcmp(topic, mqttTopicMin.c_str()) == 0) {
		byte readMin = msg.toInt();
		if(readMin < 0) readMin = 0;
		if(readMin > 100) readMin = 100;
		if(minValue != readMin) {
			minValue = readMin;
			EEPROM.write(wpEEPROM.byteMoistureMin, minValue);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMin, String(minValue));
		}
	}
	if(strcmp(topic, mqttTopicDry.c_str()) == 0) {
		int readDry = msg.toInt();
		if(readDry < 0) readDry = 0;
		if(readDry > 1023) readDry = 1023;
		if(dry != readDry) {
			dry = readDry;
			EEPROM.put(wpEEPROM.byteMoistureDry, dry);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDry, String(dry));
		}
	}
	if(strcmp(topic, mqttTopicWet.c_str()) == 0) {
		int readWet = msg.toInt();
		if(readWet < 0) readWet = 0;
		if(readWet > 1023) readWet = 1023;
		if(wet != readWet) {
			wet = readWet;
			EEPROM.put(wpEEPROM.byteMoistureWet, wet);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicWet, String(wet));
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugMoisture, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
			wpFZ.SendWSDebug("DebugMoisture", Debug);
		}
	}
}

//###################################################################################
// private
//###################################################################################
void moduleMoisture::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicMoisture.c_str(), String(moisture).c_str());
	wpRest.error = wpRest.error | !wpRest.sendRest("moisture", String(moisture));
	wpRest.trySend = true;
	moistureLast = moisture;
	if(wpMqtt.Debug) {
		printPublishValueDebug("Moisture", String(moisture), String(publishCountMoisture));
	}
	publishCountMoisture = 0;
}

void moduleMoisture::calc() {
	int newMoisture = analogRead(moisturePin);
	if(!isnan(newMoisture)) {
		if(newMoisture > 1023) newMoisture = 1023;
		if(newMoisture < 0) newMoisture = 0;
		if(useAvg) {
			newMoisture = calcAvg(newMoisture);
		}
		//Divission 0
		if((wet - dry) == 0) {
			wpFZ.DebugWS(wpFZ.strERRROR, "Moisture::calc", "risky math operation: '" + String(wet) + " (wet) - " + String(dry) + " (dry)'");
			long i = 0;
			do {
				wet = ++i;
			} while((wet - dry) > 0);
		}
		moisture = map(newMoisture, dry, wet, 0, 100);
		if(moisture > 100) moisture = 100;
		if(moisture < 0) moisture = 0;
		if(moisture < minValue) errorMin = true;
		if(moisture > minValue) errorMin = false;
		error = false;
		if(Debug) {
			String logmessage = "Moisture: " + String(moisture) + " (" + String(newMoisture) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "calcMoisture", logmessage);
		}
	} else {
		error = true;
		String logmessage = "Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "calcMoisture", logmessage);
	}
}

uint16_t moduleMoisture::calcAvg(uint16_t raw) {
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

void moduleMoisture::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
