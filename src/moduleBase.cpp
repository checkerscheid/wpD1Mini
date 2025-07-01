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
//# Revision     : $Rev:: 258                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBase.cpp 258 2025-04-28 13:34:51Z                  $ #
//#                                                                                 #
//###################################################################################
#include <moduleBase.h>
#include <wpFreakaZone.h>

moduleBase::moduleBase(String moduleName) {
	_name = moduleName;

	DebugLast = false;
	publishDebugLast = 0;
	errorLast = false;
	publishErrorLast = 0;
	_useUseAvg = false;
	_useCalcCycle = false;
	_useError = false;
}
void moduleBase::initUseAvg(uint16_t addrUseAvg, byte& byteUseAvg, uint8_t bitUseAvg) {
	_useUseAvg = true;
	_addrUseAvg = addrUseAvg;
	_byteUseAvg = byteUseAvg;
	_bitUseAvg = bitUseAvg;
	mqttTopicUseAvg = wpFZ.DeviceName + "/settings/" + _name + "/useAvg";
}
void moduleBase::initDebug(uint16_t addrDebug, byte& byteDebug, uint8_t bitDebug) {
	_addrDebug = addrDebug;
	_byteDebug = byteDebug;
	_bitDebug = bitDebug;
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/" + _name;
}
void moduleBase::initError() {
	_useError = true;
	mqttTopicError = wpFZ.DeviceName + "/ERROR/" + _name;
}
void moduleBase::initCalcCycle(uint16_t addrCalcCycle) {
	_useCalcCycle = true;
	_addrCalcCycle = addrCalcCycle;
	mqttTopicCalcCycle = wpFZ.DeviceName + "/settings/" + _name + "/CalcCycle";
}
void moduleBase::changeDebug() {
	debug = !debug;
	writeEEPROMdebug();
	wpFZ.DebugWS(wpFZ.strDEBUG, "changeDebug", "new value" + _name + ": debug = " + String(debug));
	wpFZ.blink();
}

void moduleBase::publishSettings(bool force) {
	if(force) {
		if(_useUseAvg) {
			wpMqtt.mqttClient.publish(mqttTopicUseAvg.c_str(), String(useAvg).c_str());
		}
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(debug).c_str());
		if(_useError) {
			wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
		}
		if(_useCalcCycle) {
			wpMqtt.mqttClient.publish(mqttTopicCalcCycle.c_str(), String(calcCycle).c_str());
		}
	}
}
void moduleBase::publishValues(bool force) {
	if(force) {
		publishDebugLast = 0;
		publishErrorLast = 0;
	}
	if(DebugLast != debug || wpFZ.CheckQoS(publishDebugLast)) {
		DebugLast = debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(debug).c_str());
		wpFZ.SendWSDebug("Debug" + _name, debug);
		publishDebugLast = wpFZ.loopStartedAt;
	}
	if(_useError) {
		if(errorLast != error || wpFZ.CheckQoS(publishErrorLast)) {
			errorLast = error;
			wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
			publishErrorLast = wpFZ.loopStartedAt;
		}
	}
}
void moduleBase::setSubscribes() {
	if(_useUseAvg) {
		wpMqtt.mqttClient.subscribe(mqttTopicUseAvg.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
	if(_useCalcCycle) {
		wpMqtt.mqttClient.subscribe(mqttTopicCalcCycle.c_str());
	}
}
void moduleBase::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicUseAvg.c_str()) == 0) {
		bool readUseAvg = msg.toInt();
		if(useAvg != readUseAvg) {
			useAvg = readUseAvg;
			writeEEPROMuseAvg();
			wpFZ.DebugcheckSubscribes(mqttTopicUseAvg, String(useAvg));
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(debug != readDebug) {
			debug = readDebug;
			writeEEPROMdebug();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(debug));
		}
	}
	if(strcmp(topic, mqttTopicCalcCycle.c_str()) == 0) {
		uint32_t readCalcCycle = msg.toInt();
		if(calcCycle != readCalcCycle) {
			calcCycle = readCalcCycle;
			writeEEPROMCalcCycle();
			wpFZ.DebugcheckSubscribes(mqttTopicCalcCycle, String(calcCycle));
		}
	}
}

void moduleBase::writeEEPROMuseAvg() {
	wpEEPROM.WriteBoolToEEPROM(_name + F(" useAvg"), _addrUseAvg, _byteUseAvg, _bitUseAvg, useAvg);
}
void moduleBase::writeEEPROMdebug() {
	wpEEPROM.WriteBoolToEEPROM(_name + F(" debug"), _addrDebug, _byteDebug, _bitDebug, debug);
}
void moduleBase::writeEEPROMCalcCycle() {
	double r = round(calcCycle / 100);
	if(r < 1) r = 1;
	if(r > 200) r = 200;
	uint8_t saveCalcCycle = (uint8) r;
	wpEEPROM.WriteByteToEEPROM(_name + F(" CalcCycle"), _addrCalcCycle, saveCalcCycle);
}

void moduleBase::printPublishValueDebug(String name, String value) {
	String logmessage = "MQTT Send '" + name + "': " + value;
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
