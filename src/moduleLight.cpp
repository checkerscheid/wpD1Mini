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
//# Revision     : $Rev:: 159                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLight.cpp 159 2024-07-10 19:33:14Z                 $ #
//#                                                                                 #
//###################################################################################
#include <moduleLight.h>

// uses PIN D1 (SCL) & D2 (SDA) for I2C Bus
moduleLight wpLight;

moduleLight::moduleLight() {
	// section to config and copy
	ModuleName = "Light";
	mb = new moduleBase(ModuleName);
}
void moduleLight::init() {

	// section for define
	light = 0;
	// values
	mqttTopicLight = wpFZ.DeviceName + "/" + ModuleName;
	// settings
	mqttTopicCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction";

	lightMeter = new AS_BH1750();
	lightMeter->begin();

	lightLast = 0;
	publishCountLight = 0;

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules0, wpEEPROM.bitsSendRestModules0, wpEEPROM.bitSendRestLight);
	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugLight);
	mb->initUseAvg(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0, wpEEPROM.bitUseLightAvg);
	mb->initError();
	mb->initMaxCycle(wpEEPROM.byteMaxCycleLight);
}

//###################################################################################
// public
//###################################################################################
void moduleLight::cycle() {
	if(wpFZ.calcValues && ++mb->cycleCounter >= mb->maxCycle) {
		calc();
		mb->cycleCounter = 0;
	}
	publishValues();
}

void moduleLight::publishSettings() {
	publishSettings(false);
}
void moduleLight::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
	mb->publishSettings(force);
}

void moduleLight::publishValues() {
	publishValues(false);
}
void moduleLight::publishValues(bool force) {
	if(force) {
		publishCountLight = wpFZ.publishQoS;
	}
	if(lightLast != light || ++publishCountLight > wpFZ.publishQoS) {
		publishValue();
	}
	mb->publishValues(force);
}

void moduleLight::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicCorrection.c_str());
	mb->setSubscribes();
}

void moduleLight::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicCorrection.c_str()) == 0) {
		int8 readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			EEPROM.put(wpEEPROM.byteLightCorrection, correction);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicCorrection, String(correction));
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleLight::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicLight.c_str(), String(light).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("light", String(light));
		wpRest.trySend = true;
	}
	lightLast = light;
	if(wpMqtt.Debug) {
		printPublishValueDebug("Light", String(light), String(publishCountLight));
	}
	publishCountLight = 0;
}

void moduleLight::calc() {
	float ar = lightMeter->readLightLevel();
	uint32 read = (uint32_t)ar;
	uint32 avg;
	if(!isnan(ar) || ar < 0) {
		avg = read;
		if(mb->useAvg) {
			avg = calcAvg(avg);
		}
		light = avg + correction;
		mb->error = false;
		if(mb->debug) {
			String logmessage = "Light: " + String(light) + " ("
				"AnalogRead: " + String(ar) + ", "
				"Read: " + String(read) + ", "
				"Avg: " + String(avg) + ")";
			wpFZ.DebugWS(wpFZ.strDEBUG, "wpLight::calc", logmessage);
		}
	} else {
		mb->error = true;
		String logmessage = "Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "wpLight::calc", logmessage);
	}
}
uint32 moduleLight::calcAvg(uint32 raw) {
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

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleLight::getVersion() {
	String SVN = "$Rev: 159 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleLight::changeSendRest() {
	mb->changeSendRest();
}
void moduleLight::changeDebug() {
	mb->changeDebug();
}
bool moduleLight::SendRest() {
	return mb->sendRest;
}
bool moduleLight::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleLight::UseAvg() {
	return mb->useAvg;
}
bool moduleLight::UseAvg(bool useAvg) {
	mb->useAvg = useAvg;
	return true;
}
bool moduleLight::Debug() {
	return mb->debug;
}
bool moduleLight::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint8 moduleLight::MaxCycle(){
	return mb->maxCycle / (1000 / wpFZ.loopTime);
}
uint8 moduleLight::MaxCycle(uint8 maxCycle){
	mb->maxCycle = maxCycle;
	return 0;
}
