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
//# Revision     : $Rev:: 179                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBase.cpp 179 2024-07-26 06:43:08Z                  $ #
//#                                                                                 #
//###################################################################################
#include <moduleBase.h>
#include <wpFreakaZone.h>

moduleBase::moduleBase(String moduleName) {
	_name = moduleName;

	sendRestLast = false;
	publishCountSendRest = 0;
	DebugLast = false;
	publishCountDebug = 0;
	errorLast = false;
	publishCountError = 0;
	cycleCounter = 0;
	_useUseAvg = false;
	_useMaxCycle = false;
	_useError = false;
}
void moduleBase::initRest(uint16 addrSendRest, byte byteSendRest, uint8 bitSendRest) {
	_addrSendRest = addrSendRest;
	_byteSendRest = byteSendRest;
	_bitSendRest = bitSendRest;
	mqttTopicSendRest = wpFZ.DeviceName + "/settings/SendRest/" + _name;
}
void moduleBase::initUseAvg(uint16 addrUseAvg, byte byteUseAvg, uint8 bitUseAvg) {
	_useUseAvg = true;
	_addrUseAvg = addrUseAvg;
	_byteUseAvg = byteUseAvg;
	_bitUseAvg = bitUseAvg;
	mqttTopicUseAvg = wpFZ.DeviceName + "/settings/" + _name + "/useAvg";
}
void moduleBase::initDebug(uint16 addrDebug, byte byteDebug, uint8 bitDebug) {
	_addrDebug = addrDebug;
	_byteDebug = byteDebug;
	_bitDebug = bitDebug;
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/" + _name;
}
void moduleBase::initError() {
	_useError = true;
	mqttTopicError = wpFZ.DeviceName + "/ERROR/" + _name;
}
void moduleBase::initMaxCycle(uint16 addrMaxCycle) {
	_useMaxCycle = true;
	_addrMaxCycle = addrMaxCycle;
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/" + _name + "/maxCycle";
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
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		if(_useUseAvg) {
			wpMqtt.mqttClient.publish(mqttTopicUseAvg.c_str(), String(useAvg).c_str());
		}
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(debug).c_str());
		if(_useError) {
			wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
		}
		if(_useMaxCycle) {
			wpMqtt.mqttClient.publish(mqttTopicMaxCycle.c_str(), String(maxCycle).c_str());
		}
	}
}
void moduleBase::publishValues(bool force) {
	if(force) {
		publishCountSendRest = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
		publishCountError = wpFZ.publishQoS;
	}
	if(sendRestLast != sendRest || ++publishCountSendRest > wpFZ.publishQoS) {
		sendRestLast = sendRest;
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		wpFZ.SendWSSendRest("sendRest" + _name, sendRest);
		publishCountSendRest = 0;
	}
	if(DebugLast != debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(debug).c_str());
		wpFZ.SendWSDebug("Debug" + _name, debug);
		publishCountDebug = 0;
	}
	if(_useError) {
		if(errorLast != error || ++publishCountError > wpFZ.publishQoS) {
			errorLast = error;
			wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
			publishCountError = 0;
		}
	}
}
void moduleBase::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSendRest.c_str());
	if(_useUseAvg) {
		wpMqtt.mqttClient.subscribe(mqttTopicUseAvg.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
	if(_useMaxCycle) {
		wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
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
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		uint8 readMaxCycle = msg.toInt();
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			writeEEPROMmaxCycle();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxCycle, String(maxCycle));
		}
	}
}
void moduleBase::writeEEPROMsendRest() {
	bitWrite(_byteSendRest, _bitSendRest, sendRest);
	EEPROM.write(_addrSendRest, _byteSendRest);
	EEPROM.commit();
}
void moduleBase::writeEEPROMuseAvg() {
	bitWrite(_byteUseAvg, _bitUseAvg, useAvg);
	EEPROM.write(_addrUseAvg, _byteUseAvg);
	EEPROM.commit();
}
void moduleBase::writeEEPROMdebug() {
	bitWrite(_byteDebug, _bitDebug, debug);
	EEPROM.write(_addrDebug, _byteDebug);
	EEPROM.commit();
}
void moduleBase::writeEEPROMmaxCycle() {
	EEPROM.write(_addrMaxCycle, maxCycle);
	EEPROM.commit();
}
