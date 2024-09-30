//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 21.09.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 198                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut2.cpp 198 2024-09-05 12:32:25Z            $ #
//#                                                                                 #
//###################################################################################
#include <moduleUnderfloor.h>

moduleUnderfloor wpUnderfloor1(1);
moduleUnderfloor wpUnderfloor2(2);
moduleUnderfloor wpUnderfloor3(3);
moduleUnderfloor wpUnderfloor4(4);

moduleUnderfloor::moduleUnderfloor(uint8 no) {
	// section to config and copy
	ModuleName = "Underfloor" + String(no);
	mb = new moduleBase(ModuleName);
	switch(no) {
		case 1:
			Pin = D1;
			bitDebug = wpEEPROM.bitDebugUnderfloor1;
			bitHand = wpEEPROM.bitUnderfloor1Hand;
			bitHandValue = wpEEPROM.bitUnderfloor1HandValue;
			byteSetpoint = wpEEPROM.byteUnderfloor1Setpoint;
			break;
		case 2:
			Pin = D2;
			bitDebug = wpEEPROM.bitDebugUnderfloor2;
			bitHand = wpEEPROM.bitUnderfloor2Hand;
			bitHandValue = wpEEPROM.bitUnderfloor2HandValue;
			byteSetpoint = wpEEPROM.byteUnderfloor2Setpoint;
			break;
		case 3:
			Pin = D6;
			bitDebug = wpEEPROM.bitDebugUnderfloor3;
			bitHand = wpEEPROM.bitUnderfloor3Hand;
			bitHandValue = wpEEPROM.bitUnderfloor3HandValue;
			byteSetpoint = wpEEPROM.byteUnderfloor3Setpoint;
			break;
		case 4:
			Pin = D7;
			bitDebug = wpEEPROM.bitDebugUnderfloor4;
			bitHand = wpEEPROM.bitUnderfloor4Hand;
			bitHandValue = wpEEPROM.bitUnderfloor4HandValue;
			byteSetpoint = wpEEPROM.byteUnderfloor4Setpoint;
			break;
	}
}
void moduleUnderfloor::init() {
	pinMode(Pin, OUTPUT);
	output = false;
	autoValue = false;
	handValue = false;
	handError = false;

	// values
	mqttTopicOut = wpFZ.DeviceName + "/" + ModuleName + "/Output";
	mqttTopicAutoValue = wpFZ.DeviceName + "/" + ModuleName + "/Auto";
	mqttTopicHandValue = wpFZ.DeviceName + "/" + ModuleName + "/Hand";
	mqttTopicErrorHand = wpFZ.DeviceName + "/ERROR/" + ModuleName + "Hand";
	// settings
	mqttTopicSetPoint = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetPoint";
	// commands
	mqttTopicSetHand = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHand";
	mqttTopicSetHandValue = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHandValue";
	mqttTopicSetTempURL = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetTempURL";

	outputLast = false;
	publishOutputLast = 0;
	autoValueLast = false;
	publishAutoValueLast = 0;
	handValueLast = false;
	publishHandValueLast = 0;
	handErrorLast = false;
	publishHandErrorLast = 0;
	setPointLast = 0;
	publishSetPointLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules2, wpEEPROM.bitsDebugModules2, bitDebug);
}

//###################################################################################
// public
//###################################################################################
void moduleUnderfloor::cycle() {
	if(wpFZ.calcValues) {
		calcOutput();
		calc();
	}
	publishValues();
}

void moduleUnderfloor::publishSettings() {
	publishSettings(false);
}
void moduleUnderfloor::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetHand.c_str(), String(handSet).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetHandValue.c_str(), String(handValueSet).c_str());
		//wpMqtt.mqttClient.publish(mqttTopicSetTempURL.c_str(), mqttTopicTempURL.c_str());
	}
	mb->publishSettings(force);
}

void moduleUnderfloor::publishValues() {
	publishValues(false);
}
void moduleUnderfloor::publishValues(bool force) {
	if(force) {
		publishOutputLast = 0;
		publishAutoValueLast = 0;
		publishHandValueLast = 0;
		publishHandErrorLast = 0;
	}
	if(outputLast != output || wpFZ.CheckQoS(publishOutputLast)) {
		publishValue();
	}
	if(autoValueLast != autoValue || wpFZ.CheckQoS(publishAutoValueLast)) {
		autoValueLast = autoValue;
		wpMqtt.mqttClient.publish(mqttTopicAutoValue.c_str(), String(autoValue).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(ModuleName + " Auto Value", String(autoValue));
		}
		publishAutoValueLast = wpFZ.loopStartedAt;
	}
	if(handValueLast != handValue || wpFZ.CheckQoS(publishHandValueLast)) {
		handValueLast = handValue;
		wpMqtt.mqttClient.publish(mqttTopicHandValue.c_str(), String(handValue).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(ModuleName + " Hand Value", String(handValue));
		}
		publishHandValueLast = wpFZ.loopStartedAt;
	}
	if(handErrorLast != handError || wpFZ.CheckQoS(publishHandErrorLast)) {
		handErrorLast = handError;
		wpMqtt.mqttClient.publish(mqttTopicErrorHand.c_str(), String(handError).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(ModuleName + " handError", String(handError));
		}
		publishHandErrorLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleUnderfloor::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetHand.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetHandValue.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetPoint.c_str());
	if(mqttTopicTempURL != "_") {
		wpFZ.DebugWS(wpFZ.strDEBUG, "setSubscribes", ModuleName + " subscribe: " + mqttTopicTempURL);
		wpMqtt.mqttClient.subscribe(mqttTopicTempURL.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicSetTempURL.c_str());
	mb->setSubscribes();
}

void moduleUnderfloor::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetHand.c_str()) == 0) {
		bool readSetHand = msg.toInt();
		if(handSet != readSetHand) {
			handSet = readSetHand;
			bitWrite(wpEEPROM.bitsSettingsModules2, bitHand, handSet);
			EEPROM.write(wpEEPROM.addrBitsSettingsModules2, wpEEPROM.bitsSettingsModules2);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetHand, String(handSet));
		}
	}
	if(strcmp(topic, mqttTopicSetHandValue.c_str()) == 0) {
		uint8 readSetHandValue = msg.toInt();
		if(handValueSet != readSetHandValue) {
			SetHandValueSet(readSetHandValue);
			wpFZ.DebugcheckSubscribes(mqttTopicSetHandValue, String(handValueSet));
		}
	}
	if(strcmp(topic, mqttTopicSetPoint.c_str()) == 0) {
		uint8 readSetPoint = (uint8)(msg.toDouble() * 10);
		if(setPoint != readSetPoint) {
			SetSetPoint(readSetPoint);
			wpFZ.DebugcheckSubscribes(mqttTopicSetPoint, String(readSetPoint));
		}
	}
	if(strcmp(topic, mqttTopicTempURL.c_str()) == 0) {
		temp = (uint8)msg.toDouble() * 10;
		wpFZ.DebugcheckSubscribes(mqttTopicTempURL, String(temp));
	}
	if(strcmp(topic, mqttTopicSetTempURL.c_str()) == 0) {
		SetTopicTempURL(msg);
		wpFZ.DebugcheckSubscribes(mqttTopicSetTempURL, String(mqttTopicTempURL));
	}
	mb->checkSubscribes(topic, msg);
}
void moduleUnderfloor::SetHandValueSet(bool val) {
	handValueSet = val;
	bitWrite(wpEEPROM.bitsSettingsModules2, bitHandValue, handValueSet);
	EEPROM.write(wpEEPROM.addrBitsSettingsModules2, wpEEPROM.bitsSettingsModules2);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "SetHandValueSet", "save to EEPROM: 'module" + ModuleName + "::handValueSet' = " + String(handValueSet));
}
void moduleUnderfloor::InitSetPoint(uint8 setpoint) {
	setPoint = setpoint;
}
void moduleUnderfloor::SetSetPoint(uint8 setpoint) {
	setPoint = setpoint;
	EEPROM.write(byteSetpoint, setPoint);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "SetSetPoint", "save to EEPROM: 'module" + ModuleName + "::SetSetPoint' = " + String(setPoint));
}
String moduleUnderfloor::SetTopicTempURL(String topic) {
	mqttTopicTempURL = topic;
	wpEEPROM.writeStringsToEEPROM();
	wpMqtt.mqttClient.subscribe(mqttTopicTempURL.c_str());
	return "{\"erg\":\"S_OK\"}";
}

//###################################################################################
// private
//###################################################################################
void moduleUnderfloor::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicOut.c_str(), String(output).c_str());
	outputLast = output;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug(ModuleName, String(output));
	}
	publishOutputLast = wpFZ.loopStartedAt;
}

void moduleUnderfloor::calc() {
	if(handValue != handValueSet) {
		handValue = handValueSet;
	}
	if(handError != handSet) {
		handError = handSet;
	}
	if(handError) {
		output = handValue;
	} else {
		output = autoValue;
	}
	digitalWrite(Pin, output);
}
void moduleUnderfloor::calcOutput() {
	if(setPoint < temp) {
		autoValue = false;
	}
	if(setPoint > temp) {
		autoValue = true;
	}
}

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleUnderfloor::getVersion() {
	String SVN = "$Rev: 198 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleUnderfloor::changeDebug() {
	mb->changeDebug();
}
bool moduleUnderfloor::Debug() {
	return mb->debug;
}
bool moduleUnderfloor::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
