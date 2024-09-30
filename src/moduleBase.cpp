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
//# Revision     : $Rev:: 201                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBase.cpp 201 2024-09-08 22:39:09Z                  $ #
//#                                                                                 #
//###################################################################################
#include <moduleBase.h>
#include <wpFreakaZone.h>

moduleBase::moduleBase(String moduleName) {
	_name = moduleName;

	sendRestLast = false;
	publishSendRestLast = 0;
	DebugLast = false;
	publishDebugLast = 0;
	errorLast = false;
	publishErrorLast = 0;
	_useSendRest = false;
	_useUseAvg = false;
	_useCalcCycle = false;
	_useError = false;
}
void moduleBase::initRest(uint16 addrSendRest, byte& byteSendRest, uint8 bitSendRest) {
	_useSendRest = true;
	_addrSendRest = addrSendRest;
	_byteSendRest = byteSendRest;
	_bitSendRest = bitSendRest;
	mqttTopicSendRest = wpFZ.DeviceName + "/settings/SendRest/" + _name;
}
void moduleBase::initUseAvg(uint16 addrUseAvg, byte& byteUseAvg, uint8 bitUseAvg) {
	_useUseAvg = true;
	_addrUseAvg = addrUseAvg;
	_byteUseAvg = byteUseAvg;
	_bitUseAvg = bitUseAvg;
	mqttTopicUseAvg = wpFZ.DeviceName + "/settings/" + _name + "/useAvg";
}
void moduleBase::initDebug(uint16 addrDebug, byte& byteDebug, uint8 bitDebug) {
	_addrDebug = addrDebug;
	_byteDebug = byteDebug;
	_bitDebug = bitDebug;
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/" + _name;
}
void moduleBase::initError() {
	_useError = true;
	mqttTopicError = wpFZ.DeviceName + "/ERROR/" + _name;
}
void moduleBase::initCalcCycle(uint16 addrCalcCycle) {
	_useCalcCycle = true;
	_addrCalcCycle = addrCalcCycle;
	mqttTopicCalcCycle = wpFZ.DeviceName + "/settings/" + _name + "/CalcCycle";
}
void moduleBase::changeSendRest() {
	sendRest = !sendRest;
	writeEEPROMsendRest();
	wpFZ.DebugWS(wpFZ.strDEBUG, "changeSendRest", "new value " + _name + ": sendRest = " + String(sendRest));
	wpFZ.blink();
}
void moduleBase::changeDebug() {
	debug = !debug;
	writeEEPROMdebug();
	wpFZ.DebugWS(wpFZ.strDEBUG, "changeDebug", "new value" + _name + ": debug = " + String(debug));
	wpFZ.blink();
}

void moduleBase::publishSettings(bool force) {
	if(force) {
		if(_useSendRest) {
			wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		}
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
		publishSendRestLast = 0;
		publishDebugLast = 0;
		publishErrorLast = 0;
	}
	if(_useSendRest) {
		if(sendRestLast != sendRest || wpFZ.CheckQoS(publishSendRestLast)) {
			sendRestLast = sendRest;
			wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
			wpFZ.SendWSSendRest("sendRest" + _name, sendRest);
			publishSendRestLast = wpFZ.loopStartedAt;
		}
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
	if(_useSendRest) {
		wpMqtt.mqttClient.subscribe(mqttTopicSendRest.c_str());
	}
	if(_useUseAvg) {
		wpMqtt.mqttClient.subscribe(mqttTopicUseAvg.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
	if(_useCalcCycle) {
		wpMqtt.mqttClient.subscribe(mqttTopicCalcCycle.c_str());
	}
}
void moduleBase::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSendRest.c_str()) == 0) {
		bool readSendRest = msg.toInt();
		if(sendRest != readSendRest) {
			sendRest = readSendRest;
			writeEEPROMsendRest();
			wpFZ.DebugcheckSubscribes(mqttTopicSendRest, String(sendRest));
		}
	}
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
		uint32 readCalcCycle = msg.toInt();
		if(calcCycle != readCalcCycle) {
			calcCycle = readCalcCycle;
			writeEEPROMCalcCycle();
			wpFZ.DebugcheckSubscribes(mqttTopicCalcCycle, String(calcCycle));
		}
	}
}

void moduleBase::writeEEPROMsendRest() {
	bitWrite(_byteSendRest, _bitSendRest, sendRest);
	EEPROM.write(_addrSendRest, _byteSendRest);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "writeEEPROM", _name + " sendRest: " + String(sendRest));
}
void moduleBase::writeEEPROMuseAvg() {
	bitWrite(_byteUseAvg, _bitUseAvg, useAvg);
	EEPROM.write(_addrUseAvg, _byteUseAvg);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "writeEEPROM", _name + " useAvg: " + String(useAvg));
}
void moduleBase::writeEEPROMdebug() {
	bitWrite(_byteDebug, _bitDebug, debug);
	EEPROM.write(_addrDebug, _byteDebug);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "writeEEPROM", _name + " debug: " + String(debug));
}
void moduleBase::writeEEPROMCalcCycle() {
	double r = round(calcCycle / 100);
	if(r < 1) r = 1;
	if(r > 200) r = 200;
	uint8 saveCalcCycle = (uint8) r;
	EEPROM.write(_addrCalcCycle, saveCalcCycle);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "writeEEPROM", _name + " CalcCycle: " + String(saveCalcCycle));
}

void moduleBase::printPublishValueDebug(String name, String value) {
	String logmessage = "MQTT Send '" + name + "': " + value;
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
