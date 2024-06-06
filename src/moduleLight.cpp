//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 01.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 131                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLight.cpp 131 2024-06-05 03:01:06Z                 $ #
//#                                                                                 #
//###################################################################################
#include <moduleLight.h>

// uses PIN D1 (SCL) & D2 (SDA) for I2C Bus
moduleLight wpLight;

AS_BH1750 moduleLight::lightMeter;

moduleLight::moduleLight() {}
void moduleLight::init() {
	light = 0;
	error = false;
	// values
	mqttTopicLight = wpFZ.DeviceName + "/Light";
	mqttTopicError = wpFZ.DeviceName + "/ERROR/Light";
	// settings
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/Light/maxCycle";
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/Light/Correction";
	mqttTopicUseAvg = wpFZ.DeviceName + "/settings/Light/useAvg";
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Light";

	lightMeter.begin();

	cycleCounter = 0;
	errorLast = false;
	publishCountError = 0;
	lightLast = 0;
	publishCountLight = 0;
	DebugLast = false;
	publishCountDebug = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleLight::cycle() {
	if(wpFZ.calcValues && ++cycleCounter >= maxCycle) {
		cycleCounter = 0;
		calc();
	}
	publishValues();
}

uint16_t moduleLight::getVersion() {
	String SVN = "$Rev: 131 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleLight::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugLight, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugLight", Debug);
	wpFZ.blink();
}

void moduleLight::publishSettings() {
	publishSettings(false);
}
void moduleLight::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicMaxCycle.c_str(), String(maxCycle).c_str());
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseAvg.c_str(), String(useAvg).c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void moduleLight::publishValues() {
	publishValues(false);
}
void moduleLight::publishValues(bool force) {
	if(force) {
		publishCountLight = wpFZ.publishQoS;
		publishCountError = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
	}
	if(lightLast != light || ++publishCountLight > wpFZ.publishQoS) {
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

void moduleLight::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseAvg.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}

void moduleLight::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		byte readMaxCycle = msg.toInt();
		if(readMaxCycle <= 0) readMaxCycle = 1;
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			EEPROM.write(wpEEPROM.byteMaxCycleLight, maxCycle);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxCycle, String(maxCycle));
		}
	}
	if(strcmp(topic, mqttTopicCorrection.c_str()) == 0) {
		int8_t readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			EEPROM.put(wpEEPROM.byteLightCorrection, correction);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicCorrection, String(correction));
		}
	}
	if(strcmp(topic, mqttTopicUseAvg.c_str()) == 0) {
		bool readAvg = msg.toInt();
		if(useAvg != readAvg) {
			useAvg = readAvg;
			bitWrite(wpEEPROM.bitsModulesSettings0, wpEEPROM.bitUseLightAvg, useAvg);
			EEPROM.write(wpEEPROM.addrBitsModulesSettings0, wpEEPROM.bitsModulesSettings0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicUseAvg, String(useAvg));
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugLight, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
			wpFZ.SendWSDebug("DebugLight", Debug);
		}
	}
}

//###################################################################################
// private
//###################################################################################
void moduleLight::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicLight.c_str(), String(light).c_str());
	wpRest.error = wpRest.error | !wpRest.sendRest("light", String(light));
	wpRest.trySend = true;
	lightLast = light;
	if(wpMqtt.Debug) {
		printPublishValueDebug("Light", String(light), String(publishCountLight));
	}
	publishCountLight = 0;
}

void moduleLight::calc() {
	float ar = lightMeter.readLightLevel();
	uint16_t newLight = (uint16_t)ar;
	if(ar >= 0) {
		if(useAvg) {
			newLight = calcAvg(newLight);
		}
		light = newLight + correction;
		error = false;
		if(Debug) {
			String logmessage = "Light: " + String(light) + " (" + String(newLight) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "wpLight::calc", logmessage);
		}
	} else {
		error = true;
		String logmessage = "Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "wpLight::calc", logmessage);
	}
}
uint16_t moduleLight::calcAvg(uint16_t raw) {
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

void moduleLight::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
