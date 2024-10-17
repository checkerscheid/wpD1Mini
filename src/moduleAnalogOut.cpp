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
//# Revision     : $Rev:: 214                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleAnalogOut.cpp 214 2024-10-17 10:17:02Z             $ #
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
	Pin = D6;

	pinMode(Pin, OUTPUT);
	output = 0;
	hardwareoutMax = 255;
	autoValue = 0;
	handValue = 0;
	handError = false;
	pidType = pidTypeHeating;

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
	// settings
	mqttTopicTempUrl = wpFZ.DeviceName + "/" + ModuleName + "/TempUrl";
	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
		mqttTopicKp = wpFZ.DeviceName + "/settings/" + ModuleName + "/Kp";
		mqttTopicTv = wpFZ.DeviceName + "/settings/" + ModuleName + "/Tv";
		mqttTopicTn = wpFZ.DeviceName + "/settings/" + ModuleName + "/Tn";
		mqttTopicSetPoint = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetPoint";
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
	}
	if(outputLast != output || wpFZ.CheckQoS(publishOutputLast)) {
		publishValue();
	}
	if(autoValueLast != autoValue || wpFZ.CheckQoS(publishAutoValueLast)) {
		autoValueLast = autoValue;
		wpMqtt.mqttClient.publish(mqttTopicAutoValue.c_str(), String(autoValue).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut Auto Value", String(autoValue));
		}
		publishAutoValueLast = wpFZ.loopStartedAt;
	}
	if(handValueLast != handValue || wpFZ.CheckQoS(publishHandValueLast)) {
		handValueLast = handValue;
		wpMqtt.mqttClient.publish(mqttTopicHandValue.c_str(), String(handValue).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("AnalogOut Hand Value", String(handValue));
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
			wpMqtt.mqttClient.publish(mqttTopicSetPoint.c_str(), String(SetPoint).c_str());
			if(wpMqtt.Debug) {
				mb->printPublishValueDebug("AnalogOut Kp, Tv, Tn, SetPoint",
					String(Kp) + ", " + String(Tv) + ", " + String(Tn) + ", " + String(SetPoint));
			}
			publishPIDLast = wpFZ.loopStartedAt;
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
	mb->publishValues(force);
}

void moduleAnalogOut::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetHand.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetHandValue.c_str());
	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
		wpMqtt.mqttClient.subscribe(mqttTopicKp.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicTv.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicTn.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicSetPoint.c_str());
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
			bitWrite(wpEEPROM.bitsSettingsModules0, wpEEPROM.bitAnalogOutHand, handSet);
			EEPROM.write(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0);
			EEPROM.commit();
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
	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22 || mqttTopicTemp != "_") {
		if(strcmp(topic, mqttTopicKp.c_str()) == 0) {
			double readKp = msg.toDouble();
			if(Kp != readKp) {
				Kp = readKp;
				pid->SetTunings(Kp, Tv, Tn);
				EEPROM.put(wpEEPROM.byteAnalogOutKp, (short) (Kp * 10));
				EEPROM.commit();
				wpFZ.DebugcheckSubscribes(mqttTopicKp, String(Kp));
			}
		}
		if(strcmp(topic, mqttTopicTv.c_str()) == 0) {
			double readTv = msg.toDouble();
			if(Tv != readTv) {
				Tv = readTv;
				pid->SetTunings(Kp, Tv, Tn);
				EEPROM.put(wpEEPROM.byteAnalogOutTv, (short) (Tv * 10));
				EEPROM.commit();
				wpFZ.DebugcheckSubscribes(mqttTopicTv, String(Tv));
			}
		}
		if(strcmp(topic, mqttTopicTn.c_str()) == 0) {
			double readTn = msg.toDouble();
			if(Tn != readTn) {
				Tn = readTn;
				pid->SetTunings(Kp, Tv, Tn);
				EEPROM.put(wpEEPROM.byteAnalogOutTn, (short) (Tn * 10));
				EEPROM.commit();
				wpFZ.DebugcheckSubscribes(mqttTopicTn, String(Tn));
			}
		}
		if(strcmp(topic, mqttTopicSetPoint.c_str()) == 0) {
			double readSetPoint = msg.toDouble();
			if(SetPoint != readSetPoint) {
				SetSetPoint(readSetPoint);
				wpFZ.DebugcheckSubscribes(mqttTopicSetPoint, String(SetPoint));
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
void moduleAnalogOut::SetHandValue(uint8 val) {
	handValueSet = val;
	EEPROM.write(wpEEPROM.byteAnalogOutHandValue, handValueSet);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strDEBUG, "SetHandValueSet", "save to EEPROM: 'moduleAnalogOut::handValueSet' = " + String(handValueSet));
}
String moduleAnalogOut::SetSetPoint(double sp) {
	SetPoint = sp;
	uint8 setPointToSave = (uint8) (SetPoint * 10);
	EEPROM.write(wpEEPROM.byteAnalogOutSetPoint, setPointToSave);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "SetSetPoint",
		"save to EEPROM: 'module" + ModuleName + "::SetSetPoint' = " + String(setPointToSave) + ", " +
		"addr: " + String(wpEEPROM.byteAnalogOutSetPoint));
	return "{\"erg\":\"S_OK\"}";
}
String moduleAnalogOut::SetTopicTempUrl(String topic) {
	mqttTopicTemp = topic;
	wpEEPROM.writeStringsToEEPROM();
	//wpMqtt.mqttClient.subscribe(mqttTopicTemp.c_str());
	wpFZ.restartRequired = true;
	return "{\"erg\":\"S_OK\"}";
}
String moduleAnalogOut::SetPidType(uint8 t) {
	switch(t) {
		case pidTypeHeating:
			pidType = pidTypeHeating;
			pid->SetControllerDirection(pidType);
			return F("{\"erg\":\"S_OK\",\"pidType\":\"Heating\"}");
			break;
		case pidTypeAirCondition:
			pidType = pidTypeAirCondition;
			pid->SetControllerDirection(pidType);
			return F("{\"erg\":\"S_OK\",\"pidType\":\"AirCondition\"}");
			break;
		default:
			return F("{\"erg\":\"S_ERROR\",\"msg\":\"pidType not exists\"}");
			// ERROR
			break;
	}
}
String moduleAnalogOut::GetPidType() {
	if(pidType == pidTypeHeating) {
		return F("Heating");
	}
	if(pidType == pidTypeAirCondition) {
		return F("AirCondition");
	}
}

//###################################################################################
// private
//###################################################################################
void moduleAnalogOut::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicOut.c_str(), String(output).c_str());
	outputLast = output;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("AnalogOut", String(output));
	}
	publishOutputLast = wpFZ.loopStartedAt;
}

void moduleAnalogOut::calc() {
	if(handValueSet < 0) handValueSet = 0;
	if(handValueSet > 100) handValueSet = 100;
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
		}
	}
	uint16 hardwareout = wpFZ.Map(output, 0, 100, 0, hardwareoutMax);
	analogWrite(Pin, hardwareout);
}

void moduleAnalogOut::calcOutput() {
	// @ the moment is configured as Ventilator with Humidity
	if(temp == 0) temp = (short) (SetPoint * 10);
	if(mqttTopicTemp != "_") {
		PIDinput = (double) (temp / 10.0);
	} else {
		PIDinput = (double) (wpDHT.humidity / 100.0);
	}
	PIDsetPoint = (double) SetPoint;
	pid->Compute();
	autoValue = PIDoutput;
	if(mb->debug) {
		String logmessage = "PIDinput: '" + String(PIDinput) + "', PIDsetPoint: '" + String(PIDsetPoint) + "', PIDoutput: '" + String(PIDoutput) + "'";
		wpFZ.DebugWS(wpFZ.strDEBUG, "calcOutput", logmessage);
	}
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
void moduleAnalogOut::resetPID() {
	pid->SetOutputLimits(0.0, 1.0);
	pid->SetOutputLimits(-1.0, 0.0);
	pid->SetOutputLimits(minOutput, maxOutput);
}
//###################################################################################
// section to copy
//###################################################################################
uint16 moduleAnalogOut::getVersion() {
	String SVN = "$Rev: 214 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
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
uint32 moduleAnalogOut::CalcCycle() {
	return mb->calcCycle;
}
uint32 moduleAnalogOut::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
