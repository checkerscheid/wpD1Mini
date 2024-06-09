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
//# File-ID      : $Id:: moduleMoisture.cpp 136 2024-06-09 15:37:41Z              $ #
//#                                                                                 #
//###################################################################################
#include <moduleMoisture.h>

moduleMoisture wpMoisture;

moduleMoisture::moduleMoisture() {
	// section to config and copy
	ModuleName = "Moisture";
	mb = new moduleBase(ModuleName);
}
void moduleMoisture::init() {

	// section for define
	moisturePin = A0;
	moisture = 0;
	errorMin = false;

	// values
	mqttTopicMoisture = wpFZ.DeviceName + "/" + ModuleName;
	mqttTopicErrorMin = wpFZ.DeviceName + "/ERROR/" + ModuleName + "Min";
	// settings
	mqttTopicMin = wpFZ.DeviceName + "/settings/" + ModuleName + "/Min";
	mqttTopicDry = wpFZ.DeviceName + "/settings/" + ModuleName + "/Dry";
	mqttTopicWet = wpFZ.DeviceName + "/settings/" + ModuleName + "/Wet";

	moistureLast = 0;
	publishCountMoisture = 0;
	errorMinLast = false;
	publishCountErrorMin = 0;

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules0, wpEEPROM.bitsSendRestModules0, wpEEPROM.bitSendRestMoisture);
	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugMoisture);
	mb->initError();
	mb->initMaxCycle(wpEEPROM.byteMaxCycleMoisture);
}

//###################################################################################
// public
//###################################################################################
void moduleMoisture::cycle() {
	if(wpFZ.calcValues && ++mb->cycleCounter >= mb->maxCycle) {
		calc();
		mb->cycleCounter = 0;
	}
	publishValues();
}

void moduleMoisture::publishSettings() {
	publishSettings(false);
}
void moduleMoisture::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicMin.c_str(), String(minValue).c_str());
	wpMqtt.mqttClient.publish(mqttTopicDry.c_str(), String(dry).c_str());
	wpMqtt.mqttClient.publish(mqttTopicWet.c_str(), String(wet).c_str());
	mb->publishSettings(force);
}

void moduleMoisture::publishValues() {
	publishValues(false);
}
void moduleMoisture::publishValues(bool force) {
	if(force) {
		publishCountMoisture = wpFZ.publishQoS;
		publishCountErrorMin = wpFZ.publishQoS;
	}
	if(moistureLast != moisture || ++publishCountMoisture > wpFZ.publishQoS) {
		publishValue();
	}
	if(errorMinLast != errorMin || ++publishCountErrorMin > wpFZ.publishQoS) {
		errorMinLast = errorMin;
		wpMqtt.mqttClient.publish(mqttTopicErrorMin.c_str(), String(errorMin).c_str());
		publishCountErrorMin = 0;
	}
	mb->publishValues(force);
}

void moduleMoisture::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMin.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDry.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicWet.c_str());
	mb->setSubscribes();
}

void moduleMoisture::checkSubscribes(char* topic, String msg) {
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
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleMoisture::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicMoisture.c_str(), String(moisture).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("moisture", String(moisture));
		wpRest.trySend = true;
	}
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
		if(mb->useAvg) {
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
		mb->error = false;
		if(mb->debug) {
			String logmessage = "Moisture: " + String(moisture) + " (" + String(newMoisture) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "calcMoisture", logmessage);
		}
	} else {
		mb->error = true;
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

//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleMoisture::getVersion() {
	String SVN = "$Rev: 136 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleMoisture::changeSendRest() {
	mb->changeSendRest();
}
void moduleMoisture::changeDebug() {
	mb->changeDebug();
}
bool moduleMoisture::SendRest() {
	return mb->sendRest;
}
bool moduleMoisture::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleMoisture::UseAvg() {
	return mb->useAvg;
}
bool moduleMoisture::UseAvg(bool useAvg) {
	mb->useAvg = useAvg;
	return true;
}
bool moduleMoisture::Debug() {
	return mb->debug;
}
bool moduleMoisture::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint8_t moduleMoisture::MaxCycle(){
	return mb->maxCycle;
}
uint8_t moduleMoisture::MaxCycle(uint8_t maxCycle){
	mb->maxCycle = maxCycle;
	return 0;
}
