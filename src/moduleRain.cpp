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
//# Revision     : $Rev:: 132                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRain.cpp 132 2024-06-06 11:07:48Z                  $ #
//#                                                                                 #
//###################################################################################
#include <moduleRain.h>

moduleRain wpRain;

moduleRain::moduleRain() {}
void moduleRain::init() {
	RainPin = A0;
	rain = 0;
	error = false;
	// values
	mqttTopicRain = wpFZ.DeviceName + "/Rain";
	mqttTopicError = wpFZ.DeviceName + "/ERROR/Rain";
	// settings
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/Rain/maxCycle";
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/Rain/Correction";
	mqttTopicUseAvg = wpFZ.DeviceName + "/settings/Rain/useAvg";
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Rain";

	cycleCounter = 0;
	errorLast = false;
	publishCountError = 0;
	rainLast = 0;
	publishCountRain = 0;
	DebugLast = false;
	publishCountDebug = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleRain::cycle() {
	if(wpFZ.calcValues && ++cycleCounter >= maxCycle) {
		cycleCounter = 0;
		calc();
	}
	publishValues();
}

uint16_t moduleRain::getVersion() {
	String SVN = "$Rev: 132 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleRain::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugRain, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugRain", Debug);
	wpFZ.blink();
}

void moduleRain::publishSettings() {
	publishSettings(false);
}
void moduleRain::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicMaxCycle.c_str(), String(maxCycle).c_str());
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseAvg.c_str(), String(useAvg).c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void moduleRain::publishValues() {
	publishValues(false);
}
void moduleRain::publishValues(bool force) {
	if(force) {
		publishCountRain = wpFZ.publishQoS;
		publishCountError = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
	}
	if(rainLast != rain || ++publishCountRain > wpFZ.publishQoS) {
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

void moduleRain::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseAvg.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}

void moduleRain::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		byte readMaxCycle = msg.toInt();
		if(readMaxCycle <= 0) readMaxCycle = 1;
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			EEPROM.write(wpEEPROM.byteMaxCycleRain, maxCycle);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxCycle, String(maxCycle));
		}
	}
	if(strcmp(topic, mqttTopicCorrection.c_str()) == 0) {
		int8_t readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			EEPROM.put(wpEEPROM.byteRainCorrection, correction);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicCorrection, String(correction));
		}
	}
	if(strcmp(topic, mqttTopicUseAvg.c_str()) == 0) {
		bool readAvg = msg.toInt();
		if(useAvg != readAvg) {
			useAvg = readAvg;
			bitWrite(wpEEPROM.bitsModulesSettings0, wpEEPROM.bitUseRainAvg, useAvg);
			EEPROM.write(wpEEPROM.addrBitsModulesSettings0, wpEEPROM.bitsModulesSettings0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicUseAvg, String(useAvg));
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugRain, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
			wpFZ.SendWSDebug("DebugRain", Debug);
		}
	}
}

//###################################################################################
// private
//###################################################################################
void moduleRain::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicRain.c_str(), String(rain).c_str());
	wpRest.error = wpRest.error | !wpRest.sendRest("rain", String(rain));
	wpRest.trySend = true;
	rainLast = rain;
	if(wpMqtt.Debug) {
		printPublishValueDebug("Rain", String(rain), String(publishCountRain));
	}
	publishCountRain = 0;
}

void moduleRain::calc() {
	int newRain = analogRead(RainPin);
	if(!isnan(newRain)) {
		if(newRain > 1023) newRain = 1023;
		if(newRain < 0) newRain = 0;
		int calcedRain = newRain;
		if(useAvg) {
			calcedRain = calcAvg(newRain);
		}
		calcedRain = map(newRain, 1023, 0, 0, 500);
		if(calcedRain > 500) calcedRain = 500;
		if(calcedRain < 0) calcedRain = 0;
		rain = (float)(calcedRain / 10.0) + correction;
		error = false;
		if(Debug) {
			String logmessage = "Rain: " + String(rain) + " (" + String(newRain) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "calcRain", logmessage);
		}
	} else {
		error = true;
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

void moduleRain::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
