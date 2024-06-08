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
	// section to config and copy
	ModuleName = "Rain";
	addrMaxCycle = wpEEPROM.byteMaxCycleRain;
	addrSendRest = wpEEPROM.addrBitsSendRestModules0;
	byteSendRest = wpEEPROM.bitsSendRestModules0;
	bitSendRest = wpEEPROM.bitSendRestRain;
	addrDebug = wpEEPROM.addrBitsDebugModules0;
	byteDebug = wpEEPROM.bitsDebugModules0;
	bitDebug = wpEEPROM.bitDebugRain;

	// section for define
	RainPin = A0;
	rain = 0;
	// values
	mqttTopicRain = wpFZ.DeviceName + "/" + ModuleName;
	// settings
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction";
	mqttTopicUseAvg = wpFZ.DeviceName + "/settings/" + ModuleName + "/useAvg";

	rainLast = 0;
	publishCountRain = 0;

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
void moduleRain::cycle() {
	if(wpFZ.calcValues && ++cycleCounter >= maxCycle) {
		calc();
		cycleCounter = 0;
	}
	publishValues();
}

void moduleRain::publishSettings() {
	publishSettings(false);
}
void moduleRain::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseAvg.c_str(), String(useAvg).c_str());
	publishDefaultSettings(force);
}

void moduleRain::publishValues() {
	publishValues(false);
}
void moduleRain::publishValues(bool force) {
	if(force) {
		publishCountRain = wpFZ.publishQoS;
	}
	if(rainLast != rain || ++publishCountRain > wpFZ.publishQoS) {
		publishValue();
	}
	publishDefaultValues(force);
}

void moduleRain::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseAvg.c_str());
	setDefaultSubscribes();
}

void moduleRain::checkSubscribes(char* topic, String msg) {
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
			bitWrite(wpEEPROM.bitsSettingsModules0, wpEEPROM.bitUseRainAvg, useAvg);
			EEPROM.write(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicUseAvg, String(useAvg));
		}
	}
	checkDefaultSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleRain::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicRain.c_str(), String(rain).c_str());
	if(sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("rain", String(rain));
		wpRest.trySend = true;
	}
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

//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleRain::getVersion() {
	String SVN = "$Rev: 132 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}
void moduleRain::changeSendRest() {
	sendRest = !sendRest;
	bitWrite(byteSendRest, bitSendRest, sendRest);
	EEPROM.write(addrSendRest, byteSendRest);
	EEPROM.commit();
	wpFZ.blink();
}
void moduleRain::changeDebug() {
	Debug = !Debug;
	bitWrite(byteDebug, bitDebug, Debug);
	EEPROM.write(addrDebug, byteDebug);
	EEPROM.commit();
	wpFZ.blink();
}
void moduleRain::publishDefaultSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
	}
}
void moduleRain::publishDefaultValues(bool force) {
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
void moduleRain::setDefaultSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSendRest.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void moduleRain::checkDefaultSubscribes(char* topic, String msg) {
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