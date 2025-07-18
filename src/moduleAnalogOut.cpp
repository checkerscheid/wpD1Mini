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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut.cpp 269 2025-07-01 19:25:14Z             $ #
//#                                                                                 #
//###################################################################################
#include <moduleAnalogOut.h>

moduleAnalogOut wpAnalogOut;

moduleAnalogOut::moduleAnalogOut() {
	// section to config and copy
	ModuleName = "AnalogOut";
	mb = new moduleBase(ModuleName);
	Pin = D6;
	pinMode(Pin, OUTPUT);
	analogWrite(Pin, LOW);
}
void moduleAnalogOut::init() {
	// section for define

	output = 0;
	autoValue = 0;
	handValue = 0;
	handError = false;
	wartungActive = false;

	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
		pid = new PID(&PIDinput, &PIDoutput, &PIDsetPoint, Kp, Tv, Tn, pidType);
		pid->SetMode(AUTOMATIC);
		pid->SetTunings(Kp, Tv, Tn);
		pid->SetOutputLimits(minOutput, maxOutput);
		wpFZ.DebugWS(wpFZ.strINFO, "AnalogOut::init", "PID Regler init: Kp = '" + String(Kp) + "', Tv = '" + String(Tv) + "', Tn = '" + String(Tn) + "'");
	}

	// values
	mqttTopicOut = wpFZ.DeviceName + "/" + ModuleName + "/Output";
	mqttTopicAutoValue = wpFZ.DeviceName + "/" + ModuleName + "/Auto";
	mqttTopicHandValue = wpFZ.DeviceName + "/" + ModuleName + "/Hand";
	mqttTopicReadedTemp = wpFZ.DeviceName + "/" + ModuleName + "/ReadedTemp";
	mqttTopicErrorHand = wpFZ.DeviceName + "/ERROR/" + ModuleName + "Hand";
	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
		mqttTopicWartungActive = wpFZ.DeviceName + "/ERROR/" + ModuleName + "WartungActive";
		mqttTopicSummer = wpFZ.DeviceName + "/ERROR/" + ModuleName + "Summer";
	}
	// settings
	mqttTopicTempUrl = wpFZ.DeviceName + "/" + ModuleName + "/TempUrl";
	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
		mqttTopicKp = wpFZ.DeviceName + "/settings/" + ModuleName + "/Kp";
		mqttTopicTv = wpFZ.DeviceName + "/settings/" + ModuleName + "/Tv";
		mqttTopicTn = wpFZ.DeviceName + "/settings/" + ModuleName + "/Tn";
		mqttTopicSetPoint = wpFZ.DeviceName + "/" + ModuleName + "/SetPoint";
		mqttTopicSetSetPoint = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetPoint";
		mqttTopicSetWartung = wpFZ.DeviceName + "/settings/" + ModuleName + "/Wartung";
		mqttTopicSetSummer = F("WebAutomation/Sommer");
	}
	// commands
	mqttTopicSetHand = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHand";
	mqttTopicSetHandValue = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHandValue";
	mqttTopicSetTempUrl = wpFZ.DeviceName + "/settings/" + ModuleName + "/TempUrl";

	outputLast = 0;
	publishOutputLast = 0;
	autoValueLast = 0;
	publishAutoValueLast = 0;
	handValueLast = 0;
	publishHandValueLast = 0;
	readedTempLast = 0;
	publishReadedTempLast = 0;
	handErrorLast = false;
	publishHandErrorLast = 0;
	publishPIDLast = 0;
	tempUrlLast = "";
	publishTempUrlLast = 0;
	wartungActiveLast = false;
	publishWartungActiveLast = 0;
	summerLast = false;
	publishSummerLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugAnalogOut);
	mb->initCalcCycle(wpEEPROM.byteCalcCycleAnalogOut);
}

//###################################################################################
// public
//###################################################################################
void moduleAnalogOut::cycle() {
	if(wpFZ.calcValues) {
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
			if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
				calcOutput();
				mb->calcLast = wpFZ.loopStartedAt;
			}
		}
		calc();
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
		wpMqtt.mqttClient.publish(mqttTopicSetSetPoint.c_str(), String(SetPoint).c_str());
		//wpMqtt.mqttClient.publish(mqttTopicSetTempUrl.c_str(), mqttTopicTempUrl.c_str());
	}
	mb->publishSettings(force);
}

void moduleAnalogOut::publishValues() {
	publishValues(false);
}
void moduleAnalogOut::publishValues(bool force) {
	if(force) {
		publishOutputLast = 0;
		publishAutoValueLast = 0;
		publishHandValueLast = 0;
		publishReadedTempLast = 0;
		publishHandErrorLast = 0;
		publishPIDLast = 0;
		publishTempUrlLast = 0;
		publishWartungActiveLast = 0;
		publishSummerLast = 0;
	}
	if(outputLast != output || wpFZ.CheckQoS(publishOutputLast)) {
		publishValue();
	}
	if(autoValueLast != autoValue || wpFZ.CheckQoS(publishAutoValueLast)) {
		autoValueLast = autoValue;
		wpMqtt.mqttClient.publish(mqttTopicAutoValue.c_str(), String((uint8)(autoValue / 2.55)).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut Auto Value", String((uint8)(autoValue / 2.55)));
		}
		publishAutoValueLast = wpFZ.loopStartedAt;
	}
	if(handValueLast != handValue || wpFZ.CheckQoS(publishHandValueLast)) {
		handValueLast = handValue;
		wpMqtt.mqttClient.publish(mqttTopicHandValue.c_str(), String((uint8)(handValue / 2.55)).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut Hand Value", String((uint8)(handValue / 2.55)));
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
			mb->printPublishValueDebug("AnalogOut handError", String(handError));
		}
		publishHandErrorLast = wpFZ.loopStartedAt;
	}
	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
		if(KpLast != Kp || TvLast != Tv  || TnLast != Tn || SetPointLast != SetPoint || wpFZ.CheckQoS(publishPIDLast)) {
			KpLast = Kp;
			TvLast = Tv;
			TnLast = Tn;
			SetPointLast = SetPoint;
			wpMqtt.mqttClient.publish(mqttTopicKp.c_str(), String(Kp).c_str());
			wpMqtt.mqttClient.publish(mqttTopicTv.c_str(), String(Tv).c_str());
			wpMqtt.mqttClient.publish(mqttTopicTn.c_str(), String(Tn).c_str());
			if(summer && pidType == pidTypeHeating) {
				wpMqtt.mqttClient.publish(mqttTopicSetPoint.c_str(), String(SUMMERTEMP).c_str());
			} else {
				wpMqtt.mqttClient.publish(mqttTopicSetPoint.c_str(), String(SetPoint).c_str());
			}
			if(wpMqtt.Debug) {
				mb->printPublishValueDebug("AnalogOut Kp, Tv, Tn, SetPoint",
					String(Kp) + ", " + String(Tv) + ", " + String(Tn) + ", " + String(SetPoint));
			}
			publishPIDLast = wpFZ.loopStartedAt;
		}
		if(summerLast != summer || wpFZ.CheckQoS(publishSummerLast)) {
			summerLast = summer;
			wpMqtt.mqttClient.publish(mqttTopicSummer.c_str(), String(summer).c_str());
			if(wpMqtt.Debug) {
				mb->printPublishValueDebug(ModuleName + " Summer", String(summer));
			}
			publishSummerLast = wpFZ.loopStartedAt;
		}
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
	mb->publishValues(force);
}

void moduleAnalogOut::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetHand.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetHandValue.c_str());
	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
		wpMqtt.mqttClient.subscribe(mqttTopicKp.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicTv.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicTn.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicSetSetPoint.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicSetWartung.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicSetSummer.c_str());
	}
	if(mqttTopicTemp != "_") {
		wpFZ.DebugWS(wpFZ.strDEBUG, "setSubscribes", ModuleName + " subscribe: " + mqttTopicTemp);
		wpMqtt.mqttClient.subscribe(mqttTopicTemp.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicSetTempUrl.c_str());
	mb->setSubscribes();
}

void moduleAnalogOut::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetHand.c_str()) == 0) {
		bool readSetHand = msg.toInt();
		if(handSet != readSetHand) {
			resetPID();
			handSet = readSetHand;
			wpEEPROM.WriteBoolToEEPROM("AnalogOut::HandSet", wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0, wpEEPROM.bitAnalogOutHand, handSet);
			wpFZ.DebugcheckSubscribes(mqttTopicSetHand, String(handSet));
		}
	}
	if(strcmp(topic, mqttTopicSetHandValue.c_str()) == 0) {
		uint8_t readSetHandValue = (uint8)(msg.toInt() * 2.55);
		if(handValueSet != readSetHandValue) {
			SetHandValue(readSetHandValue);
			wpFZ.DebugcheckSubscribes(mqttTopicSetHandValue, String(handValueSet));
		}
	}
	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
		if(strcmp(topic, mqttTopicKp.c_str()) == 0) {
			double readKp = msg.toDouble();
			if(Kp != readKp) {
				Kp = readKp;
				pid->SetTunings(Kp, Tv, Tn);
				short kpValue = (short)(Kp * 10);
				wpEEPROM.WriteWordToEEPROM("AnalogOut::Kp", wpEEPROM.byteAnalogOutKp, kpValue);
			}
		}
		if(strcmp(topic, mqttTopicTv.c_str()) == 0) {
			double readTv = msg.toDouble();
			if(Tv != readTv) {
				Tv = readTv;
				pid->SetTunings(Kp, Tv, Tn);
				short tvValue = (short)(Tv * 10);
				wpEEPROM.WriteWordToEEPROM("AnalogOut::Tv", wpEEPROM.byteAnalogOutTv, tvValue);
			}
		}
		if(strcmp(topic, mqttTopicTn.c_str()) == 0) {
			double readTn = msg.toDouble();
			if(Tn != readTn) {
				Tn = readTn;
				pid->SetTunings(Kp, Tv, Tn);
				short tnValue = (short)(Tn * 10);
				wpEEPROM.WriteWordToEEPROM("AnalogOut::Tn", wpEEPROM.byteAnalogOutTn, tnValue);
			}
		}
		if(strcmp(topic, mqttTopicSetSetPoint.c_str()) == 0) {
			double readSetPoint = msg.toDouble();
			if(SetPoint != readSetPoint) {
				SetSetPoint(readSetPoint);
				wpFZ.DebugcheckSubscribes(mqttTopicSetSetPoint, String(SetPoint));
			}
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
	}
	if(strcmp(topic, mqttTopicTemp.c_str()) == 0) {
		temp = (int)(msg.toDouble() * 10);
		wpFZ.DebugcheckSubscribes(mqttTopicTemp, String(temp));
	}
	if(strcmp(topic, mqttTopicSetTempUrl.c_str()) == 0) {
		SetTopicTempUrl(msg);
		wpFZ.DebugcheckSubscribes(mqttTopicSetTempUrl, mqttTopicTemp);
	}
	mb->checkSubscribes(topic, msg);
}
String moduleAnalogOut::SetSetPoint(double sp) {
	SetPoint = sp;
	uint8_t setPointToSave = (uint8) (SetPoint * 10);
	wpEEPROM.WriteByteToEEPROM("AnalogOut::SetPoint", wpEEPROM.byteAnalogOutSetPoint, setPointToSave);
	return wpFZ.jsonOK;
}
String moduleAnalogOut::SetTopicTempUrl(String topic) {
	mqttTopicTemp = topic;
	wpEEPROM.writeStringsToEEPROM();
	//wpMqtt.mqttClient.subscribe(mqttTopicTemp.c_str());
	wpFZ.restartRequired = true;
	return wpFZ.jsonOK;
}
void moduleAnalogOut::InitHand(bool hand) {
	handSet = hand;
}
void moduleAnalogOut::InitHandValue(uint8_t value) {
	handValueSet = value;
}
void moduleAnalogOut::SetHandValue(uint8_t value) {
	handValueSet = value;
	wpEEPROM.WriteByteToEEPROM("AnalogOut::handValueSet", wpEEPROM.byteAnalogOutHandValue, handValueSet);
}
void moduleAnalogOut::SetHandValueProzent(uint8_t value) {
	SetHandValue((uint8)(value * 2.55));
}
uint8_t moduleAnalogOut::GetHandValue() {
	return handValue;
}
bool moduleAnalogOut::GetHandError() {
	return handError;
}
void moduleAnalogOut::InitKp(short kp) {
	Kp = (double) (kp / 10.0);
}
void moduleAnalogOut::InitTv(short tv) {
	Tv = (double) (tv / 10.0);
}
void moduleAnalogOut::InitTn(short tn) {
	Tn = (double) (tn / 10.0);
}
void moduleAnalogOut::InitSetPoint(short setpoint) {
	SetPoint = (double) (setpoint / 10.0);
}
void moduleAnalogOut::InitPidType(uint8_t t) {
	pidType = t;
	wpFZ.DebugWS(wpFZ.strINFO, "startPidType",
		"start PID Type from EEPROM: 'module" + ModuleName + "::loadPidType' = " + GetPidType());
}
String moduleAnalogOut::GetPidType() {
	if(pidType == pidTypeHeating) {
		return F("Heating");
	}
	if(pidType == pidTypeAirCondition) {
		return F("AirCondition");
	}
	return F("unknown Type");
}
String moduleAnalogOut::SetPidType(uint8_t t) {
	switch(t) {
		case pidTypeHeating:
			pidType = pidTypeHeating;
			pid->SetControllerDirection(pidType);
			wpEEPROM.WriteByteToEEPROM("AnalogOut::pidType", wpEEPROM.byteAnalogOutPidType, pidType);
			return F("{\"erg\":\"S_OK\",\"pidType\":\"Heating\"}");
			break;
		case pidTypeAirCondition:
			pidType = pidTypeAirCondition;
			pid->SetControllerDirection(pidType);
			wpEEPROM.WriteByteToEEPROM("AnalogOut::pidType", wpEEPROM.byteAnalogOutPidType, pidType);
			return F("{\"erg\":\"S_OK\",\"pidType\":\"AirCondition\"}");
			break;
		default:
			return F("{\"erg\":\"S_ERROR\",\"msg\":\"pidType not exists\"}");
			// ERROR
			break;
	}
}
String moduleAnalogOut::SetWartung() {
	wartungActive = true;
	wartungStartedAt = wpFZ.loopStartedAt;
	wpFZ.DebugWS(wpFZ.strINFO, "SetWartung", "Wartung activated: 'module" + ModuleName);
	return wpFZ.jsonOK;
}
String moduleAnalogOut::SetSummer(bool summer) {
	this->summer = summer;
	wpEEPROM.WriteBoolToEEPROM("module" + ModuleName + "::summer", wpEEPROM.addrBitsSettingsModules3, wpEEPROM.bitsSettingsModules3, wpEEPROM.bitAnalogOutSummer, this->summer);
	publishPIDLast = 0;
	return wpFZ.jsonOK;
}

//###################################################################################
// private
//###################################################################################
void moduleAnalogOut::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicOut.c_str(), String((uint8)(output / 2.55)).c_str());
	outputLast = output;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("AnalogOut", String((uint8)(output / 2.55)));
	}
	publishOutputLast = wpFZ.loopStartedAt;
}

void moduleAnalogOut::calc() {
	if(handValue != handValueSet) {
		handValue = handValueSet;
	}
	if(wpModules.useModuleNeoPixel || wpModules.useModuleCwWw) { //AnalogOut is used for WW
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
			if(wpModules.useModuleWindow && wpWindow1.bm)
				output = 0;
			if(wpModules.useModuleWindow2 && wpWindow2.bm)
				output = 0;
			if(wpModules.useModuleWindow3 && wpWindow3.bm)
				output = 0;
			
		}
	}
	if(wartungActive) {
		output = maxOutput;
		deactivateWartung();
	}
	analogWrite(Pin, output);
}

void moduleAnalogOut::calcOutput() {
	double aktSetPoint = SetPoint;
	if(summer && pidType == pidTypeHeating) {
		aktSetPoint = (double)SUMMERTEMP;
	}
	if(temp == 0) temp = (short) (aktSetPoint * 10);
	if(mqttTopicTemp == "_") {
		if(pidType == pidTypeHeating) temp = wpDHT.temperature / 10.0;
		if(pidType == pidTypeAirCondition) temp = wpDHT.humidity / 10.0;
	}
	PIDinput = (double) (temp / 10.0);
	PIDsetPoint = (double) aktSetPoint;
	pid->Compute();
	autoValue = PIDoutput;
	if(mb->debug) {
		String logmessage = "PIDinput: '" + String(PIDinput) + "', PIDsetPoint: '" + String(PIDsetPoint) + "', PIDoutput: '" + String(PIDoutput) + "'";
		wpFZ.DebugWS(wpFZ.strDEBUG, "calcOutput", logmessage);
	}
}

void moduleAnalogOut::resetPID() {
	pid->SetOutputLimits(0.0, 1.0);
	pid->SetOutputLimits(-1.0, 0.0);
	pid->SetOutputLimits(minOutput, maxOutput);
}
void moduleAnalogOut::deactivateWartung() {
	int8_t minuten = 5;
	if(wpFZ.loopStartedAt > wartungStartedAt + (minuten * 60 * 1000)) {
		wartungActive = false;
		wpFZ.DebugWS(wpFZ.strINFO, "SetWartung", "Wartung deactivated: 'module" + ModuleName);
	}
}
//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleAnalogOut::getVersion() {
	String SVN = "$Rev: 269 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleAnalogOut::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin]));
	if(!wpModules.useModuleCwWw && !wpModules.useModuleNeoPixel) {
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || wpAnalogOut.mqttTopicTemp != "_") {
			json += F(",") +
				wpFZ.JsonKeyValue(F("CalcCycle"), String(CalcCycle())) + F(",") +
				wpFZ.JsonKeyString(F("pidType"), GetPidType()) + F(",") +
				wpFZ.JsonKeyValue(F("pidTypeRaw"), String(pidType)) + F(",") +
				wpFZ.JsonKeyValue(F("Kp"), String(Kp * 10.0)) + F(",") +
				wpFZ.JsonKeyValue(F("Tv"), String(Tv * 10.0)) + F(",") +
				wpFZ.JsonKeyValue(F("Tn"), String(Tn * 10.0)) + F(",") +
				wpFZ.JsonKeyValue(F("SetPoint"), String((uint8)(SetPoint * 10.0))) + F(",") +
				wpFZ.JsonKeyString(F("TopicTemp"), mqttTopicTemp);
		}
		json += F(",") +
			wpFZ.JsonKeyValue(F("Hand"), handError ? "true" : "false");
	}
	json += F(",") +
		wpFZ.JsonKeyValue(F("HandValue"), String(handValue)) +
		F("}");
	return json;
}

void moduleAnalogOut::changeDebug() {
	mb->changeDebug();
}
bool moduleAnalogOut::Debug() {
	return mb->debug;
}
bool moduleAnalogOut::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32_t moduleAnalogOut::CalcCycle() {
	return mb->calcCycle;
}
uint32_t moduleAnalogOut::CalcCycle(uint32_t calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
