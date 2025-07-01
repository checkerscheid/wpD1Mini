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
//# Revision     : $Rev:: 268                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleUnderfloor.cpp 268 2025-06-29 16:21:44Z            $ #
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
			bitSummer = wpEEPROM.bitUnderfloor1Summer;
			byteSetpoint = wpEEPROM.byteUnderfloor1Setpoint;
			byteCalcCycle = wpEEPROM.byteCalcCycleUnderfloor1;
			break;
		case 2:
			Pin = D2;
			bitDebug = wpEEPROM.bitDebugUnderfloor2;
			bitHand = wpEEPROM.bitUnderfloor2Hand;
			bitHandValue = wpEEPROM.bitUnderfloor2HandValue;
			bitSummer = wpEEPROM.bitUnderfloor2Summer;
			byteSetpoint = wpEEPROM.byteUnderfloor2Setpoint;
			byteCalcCycle = wpEEPROM.byteCalcCycleUnderfloor2;
			break;
		case 3:
			Pin = D6;
			bitDebug = wpEEPROM.bitDebugUnderfloor3;
			bitHand = wpEEPROM.bitUnderfloor3Hand;
			bitHandValue = wpEEPROM.bitUnderfloor3HandValue;
			bitSummer = wpEEPROM.bitUnderfloor3Summer;
			byteSetpoint = wpEEPROM.byteUnderfloor3Setpoint;
			byteCalcCycle = wpEEPROM.byteCalcCycleUnderfloor3;
			break;
		case 4:
			Pin = D7;
			bitDebug = wpEEPROM.bitDebugUnderfloor4;
			bitHand = wpEEPROM.bitUnderfloor4Hand;
			bitHandValue = wpEEPROM.bitUnderfloor4HandValue;
			bitSummer = wpEEPROM.bitUnderfloor4Summer;
			byteSetpoint = wpEEPROM.byteUnderfloor4Setpoint;
			byteCalcCycle = wpEEPROM.byteCalcCycleUnderfloor4;
			break;
		default:
			wpFZ.DebugWS(wpFZ.strERRROR, F("Underfloor::init"), F("FatalError: unknown Underfloor no: ") + String(no));
			break;
	}

	pinMode(Pin, OUTPUT);
	output = false;
	autoValue = false;
	handValue = false;
	handError = false;
	wartungActive = false;

	// values
	mqttTopicOut = wpFZ.DeviceName + "/" + ModuleName + "/Output";
	mqttTopicAutoValue = wpFZ.DeviceName + "/" + ModuleName + "/Auto";
	mqttTopicHandValue = wpFZ.DeviceName + "/" + ModuleName + "/Hand";
	mqttTopicReadedTemp = wpFZ.DeviceName + "/" + ModuleName + "/ReadedTemp";
	mqttTopicErrorHand = wpFZ.DeviceName + "/ERROR/" + ModuleName + "Hand";
	mqttTopicWartungActive = wpFZ.DeviceName + "/ERROR/" + ModuleName + "WartungActive";
	mqttTopicSummer = wpFZ.DeviceName + "/ERROR/" + ModuleName + "Summer";
	// settings
	mqttTopicSetPoint = wpFZ.DeviceName + "/" + ModuleName + "/SetPoint";
	mqttTopicTempUrl = wpFZ.DeviceName + "/" + ModuleName + "/TempUrl";
	// commands
	mqttTopicSetHand = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHand";
	mqttTopicSetHandValue = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHandValue";
	mqttTopicSetSetPoint = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetPoint";
	mqttTopicSetTempUrl = wpFZ.DeviceName + "/settings/" + ModuleName + "/TempUrl";
	mqttTopicSetWartung = wpFZ.DeviceName + "/settings/" + ModuleName + "/Wartung";
	mqttTopicSetSummer = F("WebAutomation/Sommer");

	outputLast = false;
	publishOutputLast = 0;
	autoValueLast = false;
	publishAutoValueLast = 0;
	handValueLast = false;
	publishHandValueLast = 0;
	readedTempLast = 0;
	publishReadedTempLast = 0;
	handErrorLast = false;
	publishHandErrorLast = 0;
	setPointLast = 0;
	publishSetPointLast = 0;
	tempUrlLast = "";
	publishTempUrlLast = 0;
	wartungActiveLast = false;
	publishWartungActiveLast = 0;
	summerLast = false;
	publishSummerLast = 0;


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
		wpMqtt.mqttClient.publish(mqttTopicSetWartung.c_str(), String(wartungActive).c_str());
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
		publishReadedTempLast = 0;
		publishHandErrorLast = 0;
		publishSetPointLast = 0;
		publishTempUrlLast = 0;
		publishWartungActiveLast = 0;
		publishSummerLast = 0;
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
	if(readedTempLast != temp || wpFZ.CheckQoS(publishReadedTempLast)) {
		readedTempLast = temp;
		wpMqtt.mqttClient.publish(mqttTopicReadedTemp.c_str(), String(temp / 10.0).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(ModuleName + " readedTemp", String(temp));
		}
		publishReadedTempLast = wpFZ.loopStartedAt;
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
		if(summer) {
			wpMqtt.mqttClient.publish(mqttTopicSetPoint.c_str(), String(SUMMERTEMP).c_str());
		} else {
			wpMqtt.mqttClient.publish(mqttTopicSetPoint.c_str(), String(setPoint / 10.0).c_str());
		}
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
	if(wartungActiveLast != wartungActive || wpFZ.CheckQoS(publishWartungActiveLast)) {
		wartungActiveLast = wartungActive;
		wpMqtt.mqttClient.publish(mqttTopicWartungActive.c_str(), String(wartungActive).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(ModuleName + " WartungActive", String(wartungActive));
		}
		publishWartungActiveLast = wpFZ.loopStartedAt;
	}
	if(summerLast != summer || wpFZ.CheckQoS(publishSummerLast)) {
		summerLast = summer;
		wpMqtt.mqttClient.publish(mqttTopicSummer.c_str(), String(summer).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(ModuleName + " Summer", String(summer));
		}
		publishSummerLast = wpFZ.loopStartedAt;
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
	wpMqtt.mqttClient.subscribe(mqttTopicSetWartung.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetSummer.c_str());
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
		uint8 readSetPoint = (uint8)(msg.toDouble() * 10.0);
		if(setPoint != readSetPoint) {
			SetSetPoint(readSetPoint);
			wpFZ.DebugcheckSubscribes(mqttTopicSetSetPoint, String(setPoint));
		}
	}
	if(strcmp(topic, mqttTopicTemp.c_str()) == 0) {
		temp = (int)(msg.toDouble() * 10.0);
		wpFZ.DebugcheckSubscribes(mqttTopicTemp, String(temp));
	}
	if(strcmp(topic, mqttTopicSetTempUrl.c_str()) == 0) {
		SetTopicTempUrl(msg);
		wpFZ.DebugcheckSubscribes(mqttTopicSetTempUrl, mqttTopicTemp);
	}
	if(strcmp(topic, mqttTopicSetWartung.c_str()) == 0) {
		if(msg.toInt() != 0) {
			SetWartung();
			wpFZ.DebugcheckSubscribes(mqttTopicSetWartung, msg);
		}
	}
	if(strcmp(topic, mqttTopicSetSummer.c_str()) == 0) {
		bool readSetSummer = !(msg == "False");
		if(summer != readSetSummer) {
			SetSummer(readSetSummer);
			wpFZ.DebugcheckSubscribes(mqttTopicSetSummer, String(summer));
		}
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
	wpEEPROM.WriteBoolToEEPROM("module" + ModuleName + "::handSet", wpEEPROM.addrBitsSettingsModules2, wpEEPROM.bitsSettingsModules2, bitHand, handSet);
	return wpFZ.jsonOK;
}
String moduleUnderfloor::SetHandValue(bool val) {
	handValueSet = val;
	wpEEPROM.WriteBoolToEEPROM("module" + ModuleName + "::handValueSet", wpEEPROM.addrBitsSettingsModules2, wpEEPROM.bitsSettingsModules2, bitHandValue, handValueSet);
	return wpFZ.jsonOK;
}
String moduleUnderfloor::SetSetPoint(uint8 setpoint) {
	setPoint = setpoint;
	wpEEPROM.WriteByteToEEPROM("module" + ModuleName + "::SetSetPoint", byteSetpoint, setPoint);
	return wpFZ.jsonOK;
}
String moduleUnderfloor::SetTopicTempUrl(String topic) {
	mqttTopicTemp = topic;
	wpEEPROM.writeStringsToEEPROM();
	//wpMqtt.mqttClient.subscribe(mqttTopicTemp.c_str());
	wpFZ.restartRequired = true;
	return wpFZ.jsonOK;
}
String moduleUnderfloor::SetWartung() {
	wartungActive = true;
	wartungStartedAt = wpFZ.loopStartedAt;
	wpFZ.DebugWS(wpFZ.strINFO, "SetWartung", "Wartung activated: 'module" + ModuleName);
	return wpFZ.jsonOK;
}
String moduleUnderfloor::SetSummer(bool summer) {
	this->summer = summer;
	wpEEPROM.WriteBoolToEEPROM("module" + ModuleName + "::summer", wpEEPROM.addrBitsSettingsModules3, wpEEPROM.bitsSettingsModules3, bitSummer, this->summer);
	publishSetPointLast = 0;
	return wpFZ.jsonOK;
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
	if(wartungActive) {
		output = true;
		deactivateWartung();
	}
	digitalWrite(Pin, !output);
}
void moduleUnderfloor::calcOutput() {
	uint8 aktSetPoint = setPoint;
	if(summer) {
		aktSetPoint = SUMMERTEMP * 10;
	}
	if(aktSetPoint < temp) {
		autoValue = false;
	}
	if(aktSetPoint > temp) {
		autoValue = true;
	}
}
void moduleUnderfloor::deactivateWartung() {
	int8 minuten = 5;
	if(wpFZ.loopStartedAt > wartungStartedAt + (minuten * 60 * 1000)) {
		wartungActive = false;
		wpFZ.DebugWS(wpFZ.strINFO, "SetWartung", "Wartung deactivated: 'module" + ModuleName);
	}
}

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleUnderfloor::getVersion() {
	String SVN = "$Rev: 268 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleUnderfloor::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyValue(F("CalcCycle"), String(CalcCycle())) + F(",") +
		wpFZ.JsonKeyValue(F("SetPoint"), String(GetSetPoint())) + F(",") +
		wpFZ.JsonKeyString(F("TempUrl"), String(mqttTopicTemp)) +
		F("}");
	return json;
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
