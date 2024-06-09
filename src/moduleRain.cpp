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

moduleRain::moduleRain() {
	// section to config and copy
	ModuleName = "Rain";
	mb = new moduleBase(ModuleName);
}
void moduleRain::init() {

	// section for define
	RainPin = A0;
	rain = 0;
	// values
	mqttTopicRain = wpFZ.DeviceName + "/" + ModuleName;
	// settings
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction";

	rainLast = 0;
	publishCountRain = 0;

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules0, wpEEPROM.bitsSendRestModules0, wpEEPROM.bitSendRestRain);
	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugRain);
	mb->initError();
	mb->initMaxCycle(wpEEPROM.byteMaxCycleRain);
}

//###################################################################################
// public
//###################################################################################
void moduleRain::cycle() {
	if(wpFZ.calcValues && ++mb->cycleCounter >= mb->maxCycle) {
		calc();
		mb->cycleCounter = 0;
	}
	publishValues();
}

void moduleRain::publishSettings() {
	publishSettings(false);
}
void moduleRain::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	mb->publishSettings(force);
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
	mb->publishValues(force);
}

void moduleRain::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	mb->setSubscribes();
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
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleRain::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicRain.c_str(), String(rain).c_str());
	if(mb->sendRest) {
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
		if(mb->useAvg) {
			calcedRain = calcAvg(newRain);
		}
		calcedRain = map(newRain, 1023, 0, 0, 500);
		if(calcedRain > 500) calcedRain = 500;
		if(calcedRain < 0) calcedRain = 0;
		rain = (float)(calcedRain / 10.0) + correction;
		mb->error = false;
		if(mb->debug) {
			String logmessage = "Rain: " + String(rain) + " (" + String(newRain) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "calcRain", logmessage);
		}
	} else {
		mb->error = true;
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
	mb->changeSendRest();
}
void moduleRain::changeDebug() {
	mb->changeDebug();
}
bool moduleRain::SendRest() {
	return mb->sendRest;
}
bool moduleRain::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleRain::UseAvg() {
	return mb->useAvg;
}
bool moduleRain::UseAvg(bool useAvg) {
	mb->useAvg = useAvg;
	return true;
}
bool moduleRain::Debug() {
	return mb->debug;
}
bool moduleRain::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint8_t moduleRain::MaxCycle(){
	return mb->maxCycle;
}
uint8_t moduleRain::MaxCycle(uint8_t maxCycle){
	mb->maxCycle = maxCycle;
	return 0;
}
