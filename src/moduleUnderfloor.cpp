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
//# Revision     : $Rev:: 204                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleUnderfloor.cpp 204 2024-10-04 07:33:17Z            $ #
//#                                                                                 #
//###################################################################################
#include <moduleUnderfloor.h>

moduleUnderfloor wpUnderfloor1(1);
moduleUnderfloor wpUnderfloor2(2);
moduleUnderfloor wpUnderfloor3(3);
moduleUnderfloor wpUnderfloor4(4);

moduleUnderfloor::moduleUnderfloor(uint8 n) {
	no = n;
	ModuleName = "Underfloor" + String(no);
	mb = new moduleBase(ModuleName);
}
void moduleUnderfloor::init() {
	switch(no) {
		case 1:
			Pin = D1;
			bitDebug = wpEEPROM.bitDebugUnderfloor1;
			bitHand = wpEEPROM.bitUnderfloor1Hand;
			bitHandValue = wpEEPROM.bitUnderfloor1HandValue;
			byteSetpoint = wpEEPROM.byteUnderfloor1Setpoint;
			byteCalcCycle = wpEEPROM.byteCalcCycleUnderfloor1;
			break;
		case 2:
			Pin = D2;
			bitDebug = wpEEPROM.bitDebugUnderfloor2;
			bitHand = wpEEPROM.bitUnderfloor2Hand;
			bitHandValue = wpEEPROM.bitUnderfloor2HandValue;
			byteSetpoint = wpEEPROM.byteUnderfloor2Setpoint;
			byteCalcCycle = wpEEPROM.byteCalcCycleUnderfloor2;
			break;
		case 3:
			Pin = D6;
			bitDebug = wpEEPROM.bitDebugUnderfloor3;
			bitHand = wpEEPROM.bitUnderfloor3Hand;
			bitHandValue = wpEEPROM.bitUnderfloor3HandValue;
			byteSetpoint = wpEEPROM.byteUnderfloor3Setpoint;
			byteCalcCycle = wpEEPROM.byteCalcCycleUnderfloor3;
			break;
		case 4:
			Pin = D7;
			bitDebug = wpEEPROM.bitDebugUnderfloor4;
			bitHand = wpEEPROM.bitUnderfloor4Hand;
			bitHandValue = wpEEPROM.bitUnderfloor4HandValue;
			byteSetpoint = wpEEPROM.byteUnderfloor4Setpoint;
			byteCalcCycle = wpEEPROM.byteCalcCycleUnderfloor4;
			break;
		default:
			wpFZ.DebugWS(wpFZ.strERRROR, "Underfloor::init", "FatalError: unknown Underfloor no: " + String(no));
			break;
	}

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
	mb->initDebug(wpEEPROM.addrBitsDebugModules2, wpEEPROM.bitsDebugModules2, bitDebug);
	mb->initCalcCycle(byteCalcCycle);
}

//###################################################################################
// public
//###################################################################################
void moduleUnderfloor::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calcOutput();
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
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
		//wpMqtt.mqttClient.publish(mqttTopicSetSetPoint.c_str(), String(setPoint).c_str());
		//wpMqtt.mqttClient.publish(mqttTopicSetTempUrl.c_str(), mqttTopicTemp.c_str());
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

void moduleUnderfloor::setSubscribes() {
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

void moduleUnderfloor::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetHand.c_str()) == 0) {
		bool readSetHand = msg.toInt();
		if(handSet != readSetHand) {
			SetHand(readSetHand);
			wpFZ.DebugcheckSubscribes(mqttTopicSetHand, String(handSet));
		}
	}
	if(strcmp(topic, mqttTopicSetHandValue.c_str()) == 0) {
		uint8 readSetHandValue = msg.toInt();
		if(handValueSet != readSetHandValue) {
			SetHandValue(readSetHandValue);
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
void moduleUnderfloor::InitSetPoint(uint8 setpoint) {
	setPoint = setpoint;
}
uint8 moduleUnderfloor::GetSetPoint() {
	return setPoint;
}
String moduleUnderfloor::SetHand(bool val) {
	handSet = val;
	bitWrite(wpEEPROM.bitsSettingsModules2, bitHand, handSet);
	EEPROM.write(wpEEPROM.addrBitsSettingsModules2, wpEEPROM.bitsSettingsModules2);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "SetHand", "save to EEPROM: 'module" + ModuleName + "::handSet' = " + String(handSet) + ", bit: " + String(bitHand) + ", addr: " + String(wpEEPROM.addrBitsSettingsModules2));
	return "{\"erg\":\"S_OK\"}";
}
String moduleUnderfloor::SetHandValue(bool val) {
	handValueSet = val;
	bitWrite(wpEEPROM.bitsSettingsModules2, bitHandValue, handValueSet);
	EEPROM.write(wpEEPROM.addrBitsSettingsModules2, wpEEPROM.bitsSettingsModules2);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "SetHandValueSet", "save to EEPROM: 'module" + ModuleName + "::handValueSet' = " + String(handValueSet) + ", bit: " + String(bitHandValue) + ", addr: " + String(wpEEPROM.addrBitsSettingsModules2));
	return "{\"erg\":\"S_OK\"}";
}
String moduleUnderfloor::SetSetPoint(uint8 setpoint) {
	setPoint = setpoint;
	EEPROM.write(byteSetpoint, setPoint);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "SetSetPoint", "save to EEPROM: 'module" + ModuleName + "::SetSetPoint' = " + String(setPoint) + ", addr: " + String(byteSetpoint));
	return "{\"erg\":\"S_OK\"}";
}
String moduleUnderfloor::SetTopicTempUrl(String topic) {
	mqttTopicTemp = topic;
	wpEEPROM.writeStringsToEEPROM();
	//wpMqtt.mqttClient.subscribe(mqttTopicTemp.c_str());
	wpFZ.restartRequired = true;
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
	digitalWrite(Pin, !output);
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
	String SVN = "$Rev: 204 $";
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
uint32 moduleUnderfloor::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleUnderfloor::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
