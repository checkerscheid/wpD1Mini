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
#include <moduleUnderfloor4.h>

moduleUnderfloor4 wpUnderfloor4;

moduleUnderfloor4::moduleUnderfloor4() {
	// section to config and copy
	ModuleName = "Underfloor4";
	mb = new moduleBase(ModuleName);
	Pin = D7;
}
void moduleUnderfloor4::init() {
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
	mqttTopicSetPoint = wpFZ.DeviceName + "/" + ModuleName + "/SetPoint";
	mqttTopicTempUrl = wpFZ.DeviceName + "/" + ModuleName + "/TempUrl";
	// commands
	mqttTopicSetHand = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHand";
	mqttTopicSetHandValue = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHandValue";
	mqttTopicSetSetPoint = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetPoint";
	mqttTopicSetTempUrl = wpFZ.DeviceName + "/settings/" + ModuleName + "/TempUrl";

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
	tempUrlLast = "";
	publishTempUrlLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules2, wpEEPROM.bitsDebugModules2, wpEEPROM.bitDebugUnderfloor4);
	mb->initCalcCycle(wpEEPROM.byteCalcCycleUnderfloor4);
}

//###################################################################################
// public
//###################################################################################
void moduleUnderfloor4::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calcOutput();
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleUnderfloor4::publishSettings() {
	publishSettings(false);
}
void moduleUnderfloor4::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetHand.c_str(), String(handSet).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetHandValue.c_str(), String(handValueSet).c_str());
		//wpMqtt.mqttClient.publish(mqttTopicSetSetPoint.c_str(), String(setPoint).c_str());
		//wpMqtt.mqttClient.publish(mqttTopicSetTempUrl.c_str(), mqttTopicTemp.c_str());
	}
	mb->publishSettings(force);
}

void moduleUnderfloor4::publishValues() {
	publishValues(false);
}
void moduleUnderfloor4::publishValues(bool force) {
	if(force) {
		publishOutputLast = 0;
		publishAutoValueLast = 0;
		publishHandValueLast = 0;
		publishHandErrorLast = 0;
		publishSetPointLast = 0;
		publishTempUrlLast = 0;
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
	if(setPointLast != setPoint || wpFZ.CheckQoS(publishSetPointLast)) {
		setPointLast = setPoint;
		wpMqtt.mqttClient.publish(mqttTopicSetPoint.c_str(), String(setPoint / 10).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(ModuleName + " setPoint", String(setPoint));
		}
		publishSetPointLast = wpFZ.loopStartedAt;
	}
	if(tempUrlLast != mqttTopicTemp || wpFZ.CheckQoS(publishTempUrlLast)) {
		tempUrlLast = mqttTopicTemp;
		wpMqtt.mqttClient.publish(mqttTopicTempUrl.c_str(), String(mqttTopicTemp).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(ModuleName + " TempUrl", String(mqttTopicTemp));
		}
		publishTempUrlLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleUnderfloor4::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetHand.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetHandValue.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetSetPoint.c_str());
	if(mqttTopicTemp != "_") {
		wpFZ.DebugWS(wpFZ.strDEBUG, "setSubscribes", ModuleName + " subscribe: " + mqttTopicTemp);
		wpMqtt.mqttClient.subscribe(mqttTopicTemp.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicSetTempUrl.c_str());
	mb->setSubscribes();
}

void moduleUnderfloor4::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetHand.c_str()) == 0) {
		bool readSetHand = msg.toInt();
		if(handSet != readSetHand) {
			handSet = readSetHand;
			bitWrite(wpEEPROM.bitsSettingsModules2, wpEEPROM.bitUnderfloor4Hand, handSet);
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
	if(strcmp(topic, mqttTopicSetSetPoint.c_str()) == 0) {
		uint8 readSetPoint = (uint8)(msg.toDouble() * 10);
		if(setPoint != readSetPoint) {
			SetSetPoint(readSetPoint);
			wpFZ.DebugcheckSubscribes(mqttTopicSetSetPoint, String(setPoint));
		}
	}
	if(strcmp(topic, mqttTopicTemp.c_str()) == 0) {
		temp = (uint8)msg.toDouble() * 10;
		wpFZ.DebugcheckSubscribes(mqttTopicTemp, String(temp));
	}
	if(strcmp(topic, mqttTopicSetTempUrl.c_str()) == 0) {
		SetTopicTempUrl(msg);
		wpFZ.DebugcheckSubscribes(mqttTopicSetTempUrl, mqttTopicTemp);
	}
	mb->checkSubscribes(topic, msg);
}
void moduleUnderfloor4::SetHandValueSet(bool val) {
	handValueSet = val;
	bitWrite(wpEEPROM.bitsSettingsModules2, wpEEPROM.bitUnderfloor4HandValue, handValueSet);
	EEPROM.write(wpEEPROM.addrBitsSettingsModules2, wpEEPROM.bitsSettingsModules2);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "SetHandValueSet", "save to EEPROM: 'module" + ModuleName + "::handValueSet' = " + String(handValueSet));
}
void moduleUnderfloor4::InitSetPoint(uint8 setpoint) {
	setPoint = setpoint;
}
void moduleUnderfloor4::SetSetPoint(uint8 setpoint) {
	setPoint = setpoint;
	EEPROM.write(wpEEPROM.byteUnderfloor4Setpoint, setPoint);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "SetSetPoint", "save to EEPROM: 'module" + ModuleName + "::SetSetPoint' = " + String(setPoint));
}
String moduleUnderfloor4::SetTopicTempUrl(String topic) {
	mqttTopicTemp = topic;
	wpEEPROM.writeStringsToEEPROM();
	//wpMqtt.mqttClient.subscribe(mqttTopicTemp.c_str());
	wpFZ.restartRequired = true;
	return "{\"erg\":\"S_OK\"}";
}

//###################################################################################
// private
//###################################################################################
void moduleUnderfloor4::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicOut.c_str(), String(output).c_str());
	outputLast = output;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug(ModuleName, String(output));
	}
	publishOutputLast = wpFZ.loopStartedAt;
}

void moduleUnderfloor4::calc() {
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
void moduleUnderfloor4::calcOutput() {
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
uint16 moduleUnderfloor4::getVersion() {
	String SVN = "$Rev: 198 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleUnderfloor4::changeDebug() {
	mb->changeDebug();
}
bool moduleUnderfloor4::Debug() {
	return mb->debug;
}
bool moduleUnderfloor4::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32 moduleUnderfloor4::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleUnderfloor4::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
