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
//# Revision     : $Rev:: 181                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut2.cpp 181 2024-07-27 23:14:47Z            $ #
//#                                                                                 #
//###################################################################################
#include <moduleAnalogOut2.h>

moduleAnalogOut2 wpAnalogOut2;

moduleAnalogOut2::moduleAnalogOut2() {
	// section to config and copy
	ModuleName = "AnalogOut2";
	mb = new moduleBase(ModuleName);
}
void moduleAnalogOut2::init() {
	// section for define
	Pin = D6;

	pinMode(Pin, OUTPUT_OPEN_DRAIN);
	output = 0;
	hardwareoutMax = 100;
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
	publishForceOutput = 0;
	autoValueLast = 0;
	publishForceAutoValue = 0;
	handValueLast = 0;
	publishForceHandValue = 0;
	handErrorLast = false;
	publishForceHandError = 0;

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules1, wpEEPROM.bitsSendRestModules1, wpEEPROM.bitSendRestAnalogOut2);
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
		publishForceOutput = wpFZ.publishQoS;
		publishForceAutoValue = wpFZ.publishQoS;
		publishForceHandValue = wpFZ.publishQoS;
		publishForceHandError = wpFZ.publishQoS;
	}
	if(outputLast != output || ++publishForceOutput > wpFZ.publishQoS) {
		publishValue();
	}
	if(autoValueLast != autoValue || ++publishForceAutoValue > wpFZ.publishQoS) {
		autoValueLast = autoValue;
		wpMqtt.mqttClient.publish(mqttTopicAutoValue.c_str(), String(autoValue).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut2 Auto Value", String(autoValue));
		}
		publishForceAutoValue = 0;
	}
	if(handValueLast != handValue || ++publishForceHandValue > wpFZ.publishQoS) {
		handValueLast = handValue;
		wpMqtt.mqttClient.publish(mqttTopicHandValue.c_str(), String(handValue).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut2 Hand Value", String(handValue));
		}
		publishForceHandValue = 0;
	}
	if(handErrorLast != handError || ++publishForceHandError > wpFZ.publishQoS) {
		handErrorLast = handError;
		wpMqtt.mqttClient.publish(mqttTopicErrorHand.c_str(), String(handError).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut2 handError", String(handError));
		}
		publishForceHandError = 0;
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
		uint8 readSetHandValue = msg.toInt();
		if(handValueSet != readSetHandValue) {
			handValueSet = readSetHandValue;
			EEPROM.write(wpEEPROM.byteAnalogOut2HandValue, handValueSet);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetHandValue, String(handValueSet));
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleAnalogOut2::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicOut.c_str(), String(output).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("analogout2", String(output));
		wpRest.trySend = true;
	}
	outputLast = output;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("AnalogOut2", String(output));
	}
	publishForceOutput = 0;
}

void moduleAnalogOut2::calc() {
	if(handValue != handValueSet) {
		handValue = handValueSet;
	}
	if(wpModules.useModuleNeoPixel) { //AnalogOut2 is used for CW
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
	uint16 hardwareout = wpFZ.Map(output, 0, 100, 0, hardwareoutMax);
	analogWrite(Pin, hardwareout);
}

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleAnalogOut2::getVersion() {
	String SVN = "$Rev: 181 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleAnalogOut2::changeSendRest() {
	mb->changeSendRest();
}
void moduleAnalogOut2::changeDebug() {
	mb->changeDebug();
}
bool moduleAnalogOut2::SendRest() {
	return mb->sendRest;
}
bool moduleAnalogOut2::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleAnalogOut2::Debug() {
	return mb->debug;
}
bool moduleAnalogOut2::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
