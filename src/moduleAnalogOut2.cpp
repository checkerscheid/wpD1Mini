//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 22.07.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 251                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut2.cpp 251 2025-03-13 09:20:04Z            $ #
//#                                                                                 #
//###################################################################################
#include <moduleAnalogOut2.h>

moduleAnalogOut2 wpAnalogOut2;

moduleAnalogOut2::moduleAnalogOut2() {
	// section to config and copy
	ModuleName = "AnalogOut2";
	mb = new moduleBase(ModuleName);
	Pin = D5;
	pinMode(Pin, OUTPUT);
	analogWrite(Pin, LOW);
}
void moduleAnalogOut2::init() {
	// section for define

	output = 0;
	autoValue = 0;
	handValue = 0;
	handError = false;

	// values
	mqttTopicOut = wpFZ.DeviceName + "/" + ModuleName + "/Output";
	mqttTopicAutoValue = wpFZ.DeviceName + "/" + ModuleName + "/Auto";
	mqttTopicHandValue = wpFZ.DeviceName + "/" + ModuleName + "/Hand";
	mqttTopicErrorHand = wpFZ.DeviceName + "/ERROR/" + ModuleName + "Hand";
	// settings
	// commands
	mqttTopicSetHand = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHand";
	mqttTopicSetHandValue = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHandValue";

	outputLast = 0;
	publishOutputLast = 0;
	autoValueLast = 0;
	publishAutoValueLast = 0;
	handValueLast = 0;
	publishHandValueLast = 0;
	handErrorLast = false;
	publishHandErrorLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugAnalogOut2);
}

//###################################################################################
// public
//###################################################################################
void moduleAnalogOut2::cycle() {
	if(wpFZ.calcValues) {
		calc();
	}
	publishValues();
}

void moduleAnalogOut2::publishSettings() {
	publishSettings(false);
}
void moduleAnalogOut2::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetHand.c_str(), String(handSet).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetHandValue.c_str(), String(handValueSet).c_str());
	}
	mb->publishSettings(force);
}

void moduleAnalogOut2::publishValues() {
	publishValues(false);
}
void moduleAnalogOut2::publishValues(bool force) {
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
		wpMqtt.mqttClient.publish(mqttTopicAutoValue.c_str(), String((uint8)(autoValue / 2.55)).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut2 Auto Value", String((uint8)(autoValue / 2.55)));
		}
		publishAutoValueLast = wpFZ.loopStartedAt;
	}
	if(handValueLast != handValue || wpFZ.CheckQoS(publishHandValueLast)) {
		handValueLast = handValue;
		wpMqtt.mqttClient.publish(mqttTopicHandValue.c_str(), String((uint8)(handValue / 2.55)).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut2 Hand Value", String((uint8)(handValue / 2.55)));
		}
		publishHandValueLast = wpFZ.loopStartedAt;
	}
	if(handErrorLast != handError || wpFZ.CheckQoS(publishHandErrorLast)) {
		handErrorLast = handError;
		wpMqtt.mqttClient.publish(mqttTopicErrorHand.c_str(), String(handError).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut2 handError", String(handError));
		}
		publishHandErrorLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleAnalogOut2::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetHand.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetHandValue.c_str());
	mb->setSubscribes();
}

void moduleAnalogOut2::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetHand.c_str()) == 0) {
		bool readSetHand = msg.toInt();
		if(handSet != readSetHand) {
			handSet = readSetHand;
			bitWrite(wpEEPROM.bitsSettingsModules1, wpEEPROM.bitAnalogOut2Hand, handSet);
			EEPROM.write(wpEEPROM.addrBitsSettingsModules1, wpEEPROM.bitsSettingsModules1);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetHand, String(handSet));
		}
	}
	if(strcmp(topic, mqttTopicSetHandValue.c_str()) == 0) {
		uint8 readSetHandValue = (uint8)(msg.toInt() * 2.55);
		if(handValueSet != readSetHandValue) {
			SetHandValue(readSetHandValue);
			wpFZ.DebugcheckSubscribes(mqttTopicSetHandValue, String(handValueSet));
		}
	}
	mb->checkSubscribes(topic, msg);
}
void moduleAnalogOut2::InitHand(bool hand) {
	handSet = hand;
}
void moduleAnalogOut2::InitHandValue(uint8 value) {
	handValueSet = value;
}
void moduleAnalogOut2::SetHandValue(uint8 value) {
	handValueSet = value;
	EEPROM.write(wpEEPROM.byteAnalogOut2HandValue, handValueSet);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strDEBUG, "SetHandValueSet", "save to EEPROM: 'moduleAnalogOut2::handValueSet' = " + String(handValueSet));
}
void moduleAnalogOut2::SetHandValueProzent(uint8 value) {
	SetHandValue((uint8)(value * 2.55));
}
uint8 moduleAnalogOut2::GetHandValue() {
	return handValue;
}
bool moduleAnalogOut2::GetHandError() {
	return handError;
}

//###################################################################################
// private
//###################################################################################
void moduleAnalogOut2::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicOut.c_str(), String((uint8)(output / 2.55)).c_str());
	outputLast = output;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("AnalogOut2", String((uint8)(output / 2.55)));
	}
	publishOutputLast = wpFZ.loopStartedAt;
}

void moduleAnalogOut2::calc() {
	if(handValue != handValueSet) {
		handValue = handValueSet;
	}
	if(wpModules.useModuleNeoPixel || wpModules.useModuleCwWw) { //AnalogOut2 is used for CW
		handError = false;
		output = handValue;
	} else {
		if(handError != handSet) {
			handError = handSet;
		}
		if(handError) {
			output = handValue;
		} else {
			output = autoValue;
		}
	}
	uint16 hardwareout = output;
	analogWrite(Pin, hardwareout);
}

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleAnalogOut2::getVersion() {
	String SVN = "$Rev: 251 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleAnalogOut2::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin]));
	if(!wpModules.useModuleCwWw && !wpModules.useModuleNeoPixel) {
		json += F(",") +
			wpFZ.JsonKeyValue(F("Hand"), handError ? "true" : "false");
	}
	json += F(",") +
		wpFZ.JsonKeyValue(F("HandValue"), String(handValue)) +
		F("}");
	return json;
}

void moduleAnalogOut2::changeDebug() {
	mb->changeDebug();
}
bool moduleAnalogOut2::Debug() {
	return mb->debug;
}
bool moduleAnalogOut2::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
