//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 13.07.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 162                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut.cpp 162 2024-07-13 23:52:17Z             $ #
//#                                                                                 #
//###################################################################################
#include <moduleAnalogOut.h>

moduleAnalogOut wpAnalogOut;

moduleAnalogOut::moduleAnalogOut() {
	// section to config and copy
	ModuleName = "AnalogOut";
	mb = new moduleBase(ModuleName);
}
void moduleAnalogOut::init() {

	// section for define
	analogOutPin = D8;
	//analogOutPin = A0;

	pinMode(analogOutPin, OUTPUT);
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
	// commands
	mqttTopicSetHand = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHand";
	mqttTopicSetHandValue = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHandValue";

	outputLast = false;
	publishCountOutput = 0;
	autoValueLast = false;
	publishCountAutoValue = 0;
	handValueLast = false;
	publishCountHandValue = 0;
	handErrorLast = false;
	publishCountHandError = 0;

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules1, wpEEPROM.bitsSendRestModules1, wpEEPROM.bitSendRestAnalogOut);
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugAnalogOut);
	mb->initMaxCycle(wpEEPROM.byteMaxCycleAnalogOut);
}

//###################################################################################
// public
//###################################################################################
void moduleAnalogOut::cycle() {
	if(wpFZ.calcValues && ++mb->cycleCounter >= mb->maxCycle) {
		calc();
		mb->cycleCounter = 0;
	}
	publishValues();
}

void moduleAnalogOut::publishSettings() {
	publishSettings(false);
}
void moduleAnalogOut::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetHand.c_str(), String(handSet).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetHandValue.c_str(), String(handValueSet).c_str());
	}
	mb->publishSettings(force);
}

void moduleAnalogOut::publishValues() {
	publishValues(false);
}
void moduleAnalogOut::publishValues(bool force) {
	if(force) {
		publishCountOutput = wpFZ.publishQoS;
		publishCountAutoValue = wpFZ.publishQoS;
		publishCountHandValue = wpFZ.publishQoS;
		publishCountHandError = wpFZ.publishQoS;
	}
	if(outputLast != output || ++publishCountOutput > wpFZ.publishQoS) {
		publishValue();
	}
	if(autoValueLast != autoValue || ++publishCountAutoValue > wpFZ.publishQoS) {
		autoValueLast = autoValue;
		wpMqtt.mqttClient.publish(mqttTopicAutoValue.c_str(), String(autoValue).c_str());
		if(wpMqtt.Debug) {
			printPublishValueDebug("AnalogOut Auto Value", String(autoValue), String(publishCountAutoValue));
		}
		publishCountAutoValue = 0;
	}
	if(handValueLast != handValue || ++publishCountHandValue > wpFZ.publishQoS) {
		handValueLast = handValue;
		wpMqtt.mqttClient.publish(mqttTopicHandValue.c_str(), String(handValue).c_str());
		if(wpMqtt.Debug) {
			printPublishValueDebug("AnalogOut Hand Value", String(handValue), String(publishCountHandValue));
		}
		publishCountHandValue = 0;
	}
	if(handErrorLast != handError || ++publishCountHandError > wpFZ.publishQoS) {
		handErrorLast = handError;
		wpMqtt.mqttClient.publish(mqttTopicErrorHand.c_str(), String(handError).c_str());
		if(wpMqtt.Debug) {
			printPublishValueDebug("AnalogOut handError", String(handError), String(publishCountHandError));
		}
		publishCountHandError = 0;
	}
	mb->publishValues(force);
}

void moduleAnalogOut::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetHand.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetHandValue.c_str());
	mb->setSubscribes();
}

void moduleAnalogOut::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetHand.c_str()) == 0) {
		bool readSetHand = msg.toInt();
		if(handSet != readSetHand) {
			handSet = readSetHand;
			bitWrite(wpEEPROM.bitsSettingsModules0, wpEEPROM.bitAnalogOutHand, handSet);
			EEPROM.write(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetHand, String(handSet));
		}
	}
	if(strcmp(topic, mqttTopicSetHandValue.c_str()) == 0) {
		uint8_t readSetHandValue = msg.toInt();
		if(handValueSet != readSetHandValue) {
			handValueSet = readSetHandValue;
			EEPROM.write(wpEEPROM.byteAnalogOutHandValue, handValueSet);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetHandValue, String(handValueSet));
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleAnalogOut::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicOut.c_str(), String(output).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("analogout", output ? "true" : "false");
		wpRest.trySend = true;
	}
	outputLast = output;
	if(wpMqtt.Debug) {
		printPublishValueDebug("AnalogOut", String(output), String(publishCountOutput));
	}
	publishCountOutput = 0;
}

void moduleAnalogOut::calc() {
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
	uint16_t hardwareout = map(output, 0, 100, 0, 1023);
	analogWrite(analogOutPin, hardwareout);
	wpFZ.DebugWS(wpFZ.strDEBUG, "calc", "AnalogOut: " + String(hardwareout));
}
void moduleAnalogOut::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}

//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleAnalogOut::getVersion() {
	String SVN = "$Rev: 162 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleAnalogOut::changeSendRest() {
	mb->changeSendRest();
}
void moduleAnalogOut::changeDebug() {
	mb->changeDebug();
}
bool moduleAnalogOut::SendRest() {
	return mb->sendRest;
}
bool moduleAnalogOut::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleAnalogOut::Debug() {
	return mb->debug;
}
bool moduleAnalogOut::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint8_t moduleAnalogOut::MaxCycle(){
	return mb->maxCycle / (1000 / wpFZ.loopTime);
}
uint8_t moduleAnalogOut::MaxCycle(uint8_t maxCycle){
	mb->maxCycle = maxCycle;
	return 0;
}
