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
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLDR.cpp 207 2024-10-07 12:59:22Z                   $ #
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
	for(uint8 i = 0; i < count; i++) {
		devices[i] = new deviceOneWire(i);
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
	mb->publishSettings(force);
}

void moduleDS18B20::publishValues() {
	publishValues(false);
}
void moduleDS18B20::publishValues(bool force) {
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
	mb->setSubscribes();
}

void moduleDS18B20::checkSubscribes(char* topic, String msg) {
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################

void moduleDS18B20::calc() {
}

String moduleDS18B20::scanBus() {
	count = dt->getDeviceCount();
	setCount();
	DeviceAddress address;
	for(uint8 i = 0;  i < count;  i++) {
		dt->getAddress(address, i);
		devices[i]->setAddress(address);
	}
	return wpFZ.jsonOK;
}

void moduleDS18B20::setCount() {
	EEPROM.put(wpEEPROM.byteDS18B20Count, count);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "setCount", "DS18B20 Count: " + String(count));
}
//###################################################################################
// section to copy
//###################################################################################
uint16 moduleDS18B20::getVersion() {
	String SVN = "$Rev: 207 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleDS18B20::changeDebug() {
	mb->changeDebug();
}
bool moduleDS18B20::Debug() {
	return mb->debug;
}
bool moduleDS18B20::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32 moduleDS18B20::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleDS18B20::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
