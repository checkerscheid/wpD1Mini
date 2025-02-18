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
//# Revision     : $Rev:: 232                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRelais.cpp 232 2024-12-19 15:27:48Z                $ #
//#                                                                                 #
//###################################################################################
#include <moduleRelais.h>

moduleRelais wpRelais;

moduleRelais::moduleRelais() {
	// section to config and copy
	ModuleName = "Relais";
	mb = new moduleBase(ModuleName);

	
	debugCalcPumpCounter = 0;
	remainPumpTimePause = 0;
}
void moduleRelais::init() {
	// section for define
	Pin = D6;
	if(wpModules.useModuleRelaisShield) {
		Pin = D1;
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
	mqttTopicErrorWaterEmpty = wpFZ.DeviceName + "/ERROR/WaterEmpty";
	// settings
	// wpModules.useModuleMoisture {
	mqttTopicPumpActive = wpFZ.DeviceName + "/settings/" + ModuleName + "/PumpActive";
	mqttTopicPumpPause = wpFZ.DeviceName + "/settings/" + ModuleName + "/PumpPause";
	// }
	// commands
	mqttTopicSetHand = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHand";
	mqttTopicSetHandValue = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetHandValue";
	mqttTopicSetWaterEmpty = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetWaterEmpty";
	mqttTopicStartPumpTest = wpFZ.DeviceName + "/settings/" + ModuleName + "/StartPumpTest";

	outputLast = false;
	publishOutputLast = 0;
	autoValueLast = false;
	publishAutoValueLast = 0;
	handValueLast = false;
	publishHandValueLast = 0;
	handErrorLast = false;
	publishHandErrorLast = 0;
	// if wpModules.useMoisture
	pumpCycleActive = false;
	pumpStarted = false;
	pumpInPause = false;
	pumpTimeStart = 0;
	pumpTimePause = 0;
	// }

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugRelais);
}

//###################################################################################
// public
//###################################################################################
void moduleRelais::cycle() {
	if(wpFZ.calcValues) {
		if(wpModules.useModuleMoisture) {
			calcPump();
		}
		calc();
	}
	publishValues();
}

void moduleRelais::publishSettings() {
	publishSettings(false);
}
void moduleRelais::publishSettings(bool force) {
	if(wpModules.useModuleMoisture) {
		wpMqtt.mqttClient.publish(mqttTopicPumpActive.c_str(), String(pumpActive).c_str());
		wpMqtt.mqttClient.publish(mqttTopicPumpPause.c_str(), String(pumpPause / 60).c_str());
	}
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetHand.c_str(), String(handSet).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetHandValue.c_str(), String(handValueSet).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetWaterEmpty.c_str(), String(waterEmptySet).c_str());
	}
	mb->publishSettings(force);
}

void moduleRelais::publishValues() {
	publishValues(false);
}
void moduleRelais::publishValues(bool force) {
	if(force) {
		publishOutputLast = 0;
		publishAutoValueLast = 0;
		publishHandValueLast = 0;
		publishHandErrorLast = 0;
		publishWaterEmptyErrorLast = 0;
	}
	if(outputLast != output || wpFZ.CheckQoS(publishOutputLast)) {
		publishValue();
	}
	if(autoValueLast != autoValue || wpFZ.CheckQoS(publishAutoValueLast)) {
		autoValueLast = autoValue;
		wpMqtt.mqttClient.publish(mqttTopicAutoValue.c_str(), String(autoValue).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("Relais Auto Value", String(autoValue));
		}
		publishAutoValueLast = wpFZ.loopStartedAt;
	}
	if(handValueLast != handValue || wpFZ.CheckQoS(publishHandValueLast)) {
		handValueLast = handValue;
		wpMqtt.mqttClient.publish(mqttTopicHandValue.c_str(), String(handValue).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("Relais Hand Value", String(handValue));
		}
		publishHandValueLast = wpFZ.loopStartedAt;
	}
	if(handErrorLast != handError || wpFZ.CheckQoS(publishHandErrorLast)) {
		handErrorLast = handError;
		wpMqtt.mqttClient.publish(mqttTopicErrorHand.c_str(), String(handError).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("Relais handError", String(handError));
		}
		publishHandErrorLast = wpFZ.loopStartedAt;
	}
	if(waterEmptyError != waterEmptySet || wpFZ.CheckQoS(publishWaterEmptyErrorLast)) {
		waterEmptyError = waterEmptySet;
		wpMqtt.mqttClient.publish(mqttTopicErrorWaterEmpty.c_str(), String(waterEmptyError).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetWaterEmpty.c_str(), String(waterEmptyError).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("Relais waterEmptyError", String(waterEmptyError));
		}
		publishWaterEmptyErrorLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleRelais::setSubscribes() {
	if(wpModules.useModuleMoisture) {
		wpMqtt.mqttClient.subscribe(mqttTopicPumpActive.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicPumpPause.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicSetWaterEmpty.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicSetHand.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetHandValue.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicStartPumpTest.c_str());
	mb->setSubscribes();
}

void moduleRelais::checkSubscribes(char* topic, String msg) {
	if(wpModules.useModuleMoisture) {
		if(strcmp(topic, mqttTopicSetWaterEmpty.c_str()) == 0) {
			bool readSetWaterEmpty = msg.toInt();
			if(waterEmptySet != readSetWaterEmpty) {
				waterEmptySet = readSetWaterEmpty;
				bitWrite(wpEEPROM.bitsSettingsModules0, wpEEPROM.bitRelaisWaterEmpty, waterEmptySet);
				EEPROM.write(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0);
				EEPROM.commit();
				wpFZ.DebugcheckSubscribes(mqttTopicSetWaterEmpty, String(waterEmptySet));
				wpFZ.SendWSDebug("waterEmpty", waterEmptySet);
			}
		}
		if(strcmp(topic, mqttTopicPumpActive.c_str()) == 0) {
			uint8 readPumpActive = msg.toInt();
			if(pumpActive != readPumpActive) {
				pumpActive = readPumpActive;
				EEPROM.write(wpEEPROM.bytePumpActive, pumpActive);
				EEPROM.commit();
				wpFZ.DebugcheckSubscribes(mqttTopicPumpActive, String(pumpActive));
			}
		}
		if(strcmp(topic, mqttTopicPumpPause.c_str()) == 0) {
			uint16 readPumpPause = msg.toInt();
			readPumpPause *= 60;
			if(pumpPause != readPumpPause) {
				pumpPause = readPumpPause;
				EEPROM.put(wpEEPROM.bytePumpPause, pumpPause);
				EEPROM.commit();
				wpFZ.DebugcheckSubscribes(mqttTopicPumpPause, String(pumpPause));
			}
		}
	}
	if(strcmp(topic, mqttTopicSetHand.c_str()) == 0) {
		bool readSetHand = msg.toInt();
		if(handSet != readSetHand) {
			handSet = readSetHand;
			bitWrite(wpEEPROM.bitsSettingsModules0, wpEEPROM.bitRelaisHand, handSet);
			EEPROM.write(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetHand, String(handSet));
		}
	}
	if(strcmp(topic, mqttTopicSetHandValue.c_str()) == 0) {
		bool readSetHandValue = msg.toInt();
		if(handValueSet != readSetHandValue) {
			handValueSet = readSetHandValue;
			bitWrite(wpEEPROM.bitsSettingsModules0, wpEEPROM.bitRelaisHandValue, handValueSet);
			EEPROM.write(wpEEPROM.addrBitsSettingsModules0, wpEEPROM.bitsSettingsModules0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetHandValue, String(handValueSet));
		}
	}
	if(strcmp(topic, mqttTopicStartPumpTest.c_str()) == 0) {
		int readStartPumpTest = msg.toInt();
		if(readStartPumpTest != 0) {
			StartPumpTest();
			wpFZ.DebugcheckSubscribes(mqttTopicStartPumpTest, String(handSet));
			wpMqtt.mqttClient.publish(mqttTopicStartPumpTest.c_str(), String(0).c_str());
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleRelais::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicOut.c_str(), String(output).c_str());
	outputLast = output;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("Relais", String(output));
	}
	publishOutputLast = wpFZ.loopStartedAt;
}

void moduleRelais::calc() {
	if(startPumpTestActive) {
		output = true;
		if(millis() > startPumpTestAt + 500) {
			output = false;
			startPumpTestActive = false;
		}
	} else {
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
	}
	if(output && digitalRead(Pin) == LOW) {
		digitalWrite(Pin, HIGH);
		if(mb->debug) {
			wpFZ.DebugWS(wpFZ.strDEBUG, "calc", "Relais turns on");
		}
	}
	if(!output && digitalRead(Pin) == HIGH) {
		digitalWrite(Pin, LOW);
		if(mb->debug) {
			wpFZ.DebugWS(wpFZ.strDEBUG, "calc", "Relais turns off");
		}
	}
}
void moduleRelais::calcPump() {
	if(wpModules.useModuleMoisture) {
		if(!waterEmptyError) {
			unsigned long m = millis();
			// detect to dry soil
			if(wpMoisture.errorMin && !pumpCycleActive) {
				pumpCycleActive = true;
				pumpStarted = false;
				pumpInPause = false;
				SendPumpStatus();
				if(mb->debug) {
					wpFZ.DebugWS(wpFZ.strDEBUG, "calcPump", "Detect 'toDry', start Pump Cycle");
				}
			}
			// start pump and store timestart
			if(pumpCycleActive && !pumpStarted && !pumpInPause) {
				autoValue = true;
				pumpStarted = true;
				pumpTimeStart = m;
				SendPumpStatus();
				if(mb->debug) {
					wpFZ.DebugWS(wpFZ.strDEBUG, "calcPump", "start 'pump' (" + String(pumpTimeStart) + ")");
				}
			}
			// stop pump and start pause
			if(pumpCycleActive && pumpStarted && !pumpInPause) {
				if(m > (pumpTimeStart + (pumpActive * 1000))) {
					autoValue = false;
					pumpInPause = true;
					pumpTimePause = m;
					remainPumpTimePause = pumpTimePause + (pumpPause * 1000);
					SendPumpStatus();
					if(mb->debug) {
						wpFZ.DebugWS(wpFZ.strDEBUG, "calcPump", "stopped 'pump', start 'pumpPause' (" + String(pumpTimePause) + ")");
					}
				}
			}
			// finished pause, reset all
			if(pumpCycleActive && pumpStarted && pumpInPause) {
				if(m > (pumpTimePause + (pumpPause * 1000))) {
					pumpCycleActive = false;
					pumpStarted = false;
					pumpInPause = false;
					wpFZ.pumpCycleFinished();
					if(mb->debug) {
						wpFZ.DebugWS(wpFZ.strDEBUG, "calcPump", "stopped 'pumpPause' and reset");
					}
				}
				if(mb->debug) {
					if(++debugCalcPumpCounter >= 4) {
						unsigned long calced = remainPumpTimePause - m;
						wpFZ.SendRemainPumpInPause(getReadableTime(calced));
						debugCalcPumpCounter = 0;
					}
				}
			}

		}
	}
}
void moduleRelais::SendPumpStatus() {
	if(mb->debug) {
		wpFZ.SendPumpStatus(
			"\"pumpCycleActive\":" + String(pumpCycleActive) + ","
			"\"pumpStarted\":" + String(pumpStarted) + ","
			"\"pumpInPause\":" + String(pumpInPause)
		);
	}
}

void moduleRelais::StartPumpTest() {
	startPumpTestAt = millis();
	startPumpTestActive = true;
}

String moduleRelais::getReadableTime(unsigned long time) {
	unsigned long secall = (unsigned long) time / 1000;
	unsigned long minohnesec = (unsigned long) round(secall / 60);
	byte sec = secall % 60;
	unsigned long h = (unsigned long) round(minohnesec / 60);
	byte min = minohnesec % 60;
	String msg = (h < 10 ? "0" + String(h) : String(h)) + ":" + (min < 10 ? "0" + String(min) : String(min)) + ":" + (sec < 10 ? "0" + String(sec) : String(sec));
	return msg;
}
// }

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleRelais::getVersion() {
	String SVN = "$Rev: 232 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleRelais::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyValue(F("Hand"), handError ? "true" : "false") + F(",") +
		wpFZ.JsonKeyValue(F("HandValue"), handValue ? "true" : "false");
	if(wpModules.useModuleMoisture) {
		json += F(",\"Moisture\":{") +
			wpFZ.JsonKeyValue(F("waterEmpty"), waterEmptySet ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("pumpActive"), String(pumpActive)) + F(",") +
			wpFZ.JsonKeyValue(F("pumpPause"), String(pumpPause / 60)) +
			F("}");
	}
	json += F("}");
	return json;
}

void moduleRelais::changeDebug() {
	mb->changeDebug();
}
bool moduleRelais::Debug() {
	return mb->debug;
}
bool moduleRelais::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
