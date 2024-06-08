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
//# File-ID      : $Id:: moduleLDR.cpp 132 2024-06-06 11:07:48Z                   $ #
//#                                                                                 #
//###################################################################################
#include <moduleLDR.h>

moduleLDR wpLDR;

moduleLDR::moduleLDR() {}
void moduleLDR::init() {
	// section to config and copy
	ModuleName = "LDR";
	addrMaxCycle = wpEEPROM.byteMaxCycleLDR;
	addrSendRest = wpEEPROM.addrBitsSendRestModules0;
	byteSendRest = wpEEPROM.bitsSendRestModules0;
	bitSendRest = wpEEPROM.bitSendRestLDR;
	addrDebug = wpEEPROM.addrBitsDebugModules0;
	byteDebug = wpEEPROM.bitsDebugModules0;
	bitDebug = wpEEPROM.bitDebugLDR;

	// section for define
	LDRPin = A0;
	LDR = 0;
	// values
	mqttTopicLDR = wpFZ.DeviceName + "/" + ModuleName;
	// settings
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction";
	mqttTopicUseAvg = wpFZ.DeviceName + "/settings/" + ModuleName + "/useAvg";

	LDRLast = 0;
	publishCountLDR = 0;

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
void moduleLDR::cycle() {
	if(wpFZ.calcValues && ++cycleCounter >= maxCycle) {
		calc();
		cycleCounter = 0;
	}
	publishValues();
}

void moduleLDR::publishSettings() {
	publishSettings(false);
}
void moduleLDR::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	wpMqtt.mqttClient.publish(mqttTopicUseAvg.c_str(), String(useAvg).c_str());
	publishDefaultSettings(force);
}

void moduleLDR::publishValues() {
	publishValues(false);
}
void moduleLDR::publishValues(bool force) {
	if(force) {
		publishCountLDR = wpFZ.publishQoS;
	}
	if(LDRLast != LDR || ++publishCountLDR > wpFZ.publishQoS) {
		publishValue();
	}
	publishDefaultValues(force);
}

void moduleLDR::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicUseAvg.c_str());
	setDefaultSubscribes();
}

void moduleLDR::checkSubscribes(char* topic, String msg) {
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
			bitWrite(wpEEPROM.bitsSettingsModules0, wpEEPROM.bitUseLdrAvg, useAvg);
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
void moduleLDR::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicLDR.c_str(), String(LDR).c_str());
	if(sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("ldr", String(LDR));
		wpRest.trySend = true;
	}
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

//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleLDR::getVersion() {
	String SVN = "$Rev: 132 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}
void moduleLDR::changeSendRest() {
	sendRest = !sendRest;
	bitWrite(byteSendRest, bitSendRest, sendRest);
	EEPROM.write(addrSendRest, byteSendRest);
	EEPROM.commit();
	wpFZ.blink();
}
void moduleLDR::changeDebug() {
	Debug = !Debug;
	bitWrite(byteDebug, bitDebug, Debug);
	EEPROM.write(addrDebug, byteDebug);
	EEPROM.commit();
	wpFZ.blink();
}
void moduleLDR::publishDefaultSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
	}
}
void moduleLDR::publishDefaultValues(bool force) {
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
void moduleLDR::setDefaultSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSendRest.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void moduleLDR::checkDefaultSubscribes(char* topic, String msg) {
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