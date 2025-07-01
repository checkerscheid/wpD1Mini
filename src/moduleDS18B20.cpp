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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDS18B20.cpp 269 2025-07-01 19:25:14Z               $ #
//#                                                                                 #
//###################################################################################
#include <moduleDS18B20.h>

moduleDS18B20 wpDS18B20;
/**
 * Module DS18B20
 * hold the settings and the List of all DS18B20 in the OneWire Bus
 */
moduleDS18B20::moduleDS18B20() {
	// section to config and copy
	ModuleName = "DS18B20";
	mb = new moduleBase(ModuleName);
}
void moduleDS18B20::init() {
	// section for define
	Pin = D7;
	for(uint8_t i = 0; i < count; i++) {
		devices[i] = new deviceOneWire(i);
		devices[i]->init();
	}
	ow = new OneWire(Pin);
	dt = new DallasTemperature(ow);
	// values
	mqttTopicCount = wpFZ.DeviceName + "/" + ModuleName + "/Count";
	// settings
	countLast = 0;
	publishCountLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules3, wpEEPROM.bitsDebugModules3, wpEEPROM.bitDebugDS18B20);
	mb->initCalcCycle(wpEEPROM.byteCalcCycleDS18B20);
	
	for(uint8_t i = 0; i < count; i++) {
		if(devices[i] != NULL)
			devices[i]->debug = mb->debug;
	}

	mb->calcLast = 0;

	dt->begin();
}

//###################################################################################
// public
//###################################################################################
void moduleDS18B20::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleDS18B20::publishSettings() {
	publishSettings(false);
}
void moduleDS18B20::publishSettings(bool force) {
	for(int i = 0; i < count; i++) {
		if(devices[i] != NULL) {
			devices[i]->publishSettings(force);
		}
	}
	mb->publishSettings(force);
}

void moduleDS18B20::publishValues() {
	publishValues(false);
}
void moduleDS18B20::publishValues(bool force) {
	for(int i = 0; i < count; i++) {
		if(devices[i] != NULL) {
			devices[i]->publishValues(force);
		}
	}
	if(force) {
		publishCountLast = 0;
	}
	if(countLast != count || wpFZ.CheckQoS(publishCountLast)) {
		countLast = count;
		wpMqtt.mqttClient.publish(mqttTopicCount.c_str(), String(count).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("Count", String(count));
		}
		publishCountLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleDS18B20::setSubscribes() {
	for(int i = 0; i < count; i++) {
		if(devices[i] != NULL) {
			devices[i]->setSubscribes();
		}
	}
	mb->setSubscribes();
}

void moduleDS18B20::checkSubscribes(char* topic, String msg) {
	for(int i = 0; i < count; i++) {
		if(devices[i] != NULL) {
			devices[i]->checkSubscribes(topic, msg);
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################

void moduleDS18B20::calc() {
	dt->requestTemperatures();
	for(int i = 0; i < count; i++) {
		if(devices[i] != NULL) {
			float ti = dt->getTempCByIndex(i);
			float t = dt->getTempC(devices[i]->getAddress());
			devices[i]->setTemperature(ti);
			if(mb->debug)
				wpFZ.DebugWS(wpFZ.strDEBUG, "DS18B20::calc", "index: " + String(i) + ", ti: " + String(ti) + ", t: " + String(t));
		}
	}
}

String moduleDS18B20::scanBus() {
	uint8_t cds18 = dt->getDS18Count();
	wpFZ.DebugWS(wpFZ.strINFO, "scanBus", "count ds18: " + String(cds18));
	count = dt->getDeviceCount();
	wpEEPROM.WriteByteToEEPROM("DS18B20Count", wpEEPROM.byteDS18B20Count, count);
	DeviceAddress address;
	String adr = "";
	for(uint8_t i = 0;  i < count;  i++) {
		dt->getAddress(address, i);
		if(devices[i] != NULL) {
			devices[i]->setAddress(address);
			if(i > 0) adr += ",";
			adr += "\"" + String(i) + "\":\"" + devices[i]->getStringAddress() + "\"";
		}
	}
	return "{\"found\":" + String(count) + ",\"adr\":{" + adr + "}}"; //wpFZ.jsonOK;
}

//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleDS18B20::getVersion() {
	String SVN = "$Rev: 269 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleDS18B20::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",");
	//wpFZ.JsonKeyValue(F("CalcCycle"), String(wpDS18B20.CalcCycle(1))) + F(",") +
	//wpFZ.JsonKeyValue(F("useAvg1"), wpDS18B20.UseAvg(1) ? "true" : "false") + F(",") +
	for(int c = 0; c < count; c++) {
		json += wpFZ.JsonKeyString(F("Device") + String(c) + F("Address"), devices[c]->getStringAddress()) + F(",");
	}
	json += 
		wpFZ.JsonKeyValue(F("Count"), String(count)) +
		F("}");
	return json;
}

void moduleDS18B20::changeDebug() {
	mb->changeDebug();
}
bool moduleDS18B20::Debug() {
	return mb->debug;
}
bool moduleDS18B20::Debug(bool debug) {
	mb->debug = debug;
	for(uint8_t i = 0; i < count; i++) {
		if(devices[i] != NULL)
			devices[i]->debug = mb->debug;
	}
	return true;
}
uint32_t moduleDS18B20::CalcCycle(){
	return mb->calcCycle;
}
uint32_t moduleDS18B20::CalcCycle(uint32_t calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
