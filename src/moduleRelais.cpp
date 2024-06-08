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
//# Revision     : $Rev:: 132                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRelais.cpp 132 2024-06-06 11:07:48Z                $ #
//#                                                                                 #
//###################################################################################
#include <moduleRelais.h>

moduleRelais wpRelais;

moduleRelais::moduleRelais() {}
void moduleRelais::init() {
	// section to config and copy
	ModuleName = "Relais";
	addrSendRest = wpEEPROM.addrBitsSendRestModules0;
	byteSendRest = wpEEPROM.bitsSendRestModules0;
	bitSendRest = wpEEPROM.bitSendRestRelais;
	addrDebug = wpEEPROM.addrBitsDebugModules0;
	byteDebug = wpEEPROM.bitsDebugModules0;
	bitDebug = wpEEPROM.bitDebugRelais;

	// section for define
	relaisPin = D6;
	if(wpModules.useModuleRelaisShield) {
		relaisPin = D1;
	}
	pinMode(relaisPin, OUTPUT);
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

	outputLast = false;
	publishCountOutput = 0;
	autoValueLast = false;
	publishCountAutoValue = 0;
	handValueLast = false;
	publishCountHandValue = 0;
	handErrorLast = false;
	publishCountHandError = 0;
	DebugLast = false;
	publishCountDebug = 0;
	// if wpModules.useMoisture
	pumpCycleActive = false;
	pumpStarted = false;
	pumpInPause = false;
	pumpTimeStart = 0;
	pumpTimePause = 0;
	// }

	// section to copy
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/" + ModuleName + "/maxCycle";
	mqttTopicSendRest = wpFZ.DeviceName + "/settings/SendRest/" + ModuleName;
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/" + ModuleName;
	mqttTopicError = wpFZ.DeviceName + "/ERROR/" + ModuleName;

	cycleCounter = 0;
	sendRestLast = false;
	publishCountSendRest = 0;
	DebugLast = false;
	publishCountDebug = 0;
	errorLast = false;
	publishCountError = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleRelais::cycle() {
	if(wpFZ.calcValues) {
		calc();
		if(wpModules.useModuleMoisture) {
			calcPump();
		}
	}
	publishValues();
}

void moduleRelais::publishSettings() {
	publishSettings(false);
}
void moduleRelais::publishSettings(bool force) {
	if(wpModules.useModuleMoisture) {
		wpMqtt.mqttClient.publish(mqttTopicPumpActive.c_str(), String(pumpActive).c_str());
		wpMqtt.mqttClient.publish(mqttTopicPumpPause.c_str(), String(pumpPause).c_str());
	}
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetHand.c_str(), String(handSet).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetHandValue.c_str(), String(handValueSet).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetWaterEmpty.c_str(), String(waterEmptySet).c_str());
	}
	publishDefaultSettings(force);
}

void moduleRelais::publishValues() {
	publishValues(false);
}
void moduleRelais::publishValues(bool force) {
	if(force) {
		publishCountOutput = wpFZ.publishQoS;
		publishCountAutoValue = wpFZ.publishQoS;
		publishCountHandValue = wpFZ.publishQoS;
		publishCountHandError = wpFZ.publishQoS;
		publishCountWaterEmptyError = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
	}
	if(outputLast != output || ++publishCountOutput > wpFZ.publishQoS) {
		publishValue();
	}
	if(autoValueLast != autoValue || ++publishCountAutoValue > wpFZ.publishQoS) {
		autoValueLast = autoValue;
		wpMqtt.mqttClient.publish(mqttTopicAutoValue.c_str(), String(autoValue).c_str());
		if(wpMqtt.Debug) {
			printPublishValueDebug("Relais Auto Value", String(autoValue), String(publishCountAutoValue));
		}
		publishCountAutoValue = 0;
	}
	if(handValueLast != handValue || ++publishCountHandValue > wpFZ.publishQoS) {
		handValueLast = handValue;
		wpMqtt.mqttClient.publish(mqttTopicHandValue.c_str(), String(handValue).c_str());
		if(wpMqtt.Debug) {
			printPublishValueDebug("Relais Hand Value", String(handValue), String(publishCountHandValue));
		}
		publishCountHandValue = 0;
	}
	if(handErrorLast != handError || ++publishCountHandError > wpFZ.publishQoS) {
		handErrorLast = handError;
		wpMqtt.mqttClient.publish(mqttTopicErrorHand.c_str(), String(handError).c_str());
		if(wpMqtt.Debug) {
			printPublishValueDebug("Relais handError", String(handError), String(publishCountHandError));
		}
		publishCountHandError = 0;
	}
	if(waterEmptyError != waterEmptySet || ++publishCountWaterEmptyError > wpFZ.publishQoS) {
		waterEmptyError = waterEmptySet;
		wpMqtt.mqttClient.publish(mqttTopicErrorWaterEmpty.c_str(), String(waterEmptyError).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetWaterEmpty.c_str(), String(waterEmptyError).c_str());
		if(wpMqtt.Debug) {
			printPublishValueDebug("Relais waterEmptyError", String(waterEmptyError), String(publishCountWaterEmptyError));
		}
		publishCountWaterEmptyError = 0;
	}
	publishDefaultValues(force);
}

void moduleRelais::setSubscribes() {
	if(wpModules.useModuleMoisture) {
		wpMqtt.mqttClient.subscribe(mqttTopicPumpActive.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicPumpPause.c_str());
		wpMqtt.mqttClient.subscribe(mqttTopicSetWaterEmpty.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicSetHand.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetHandValue.c_str());
	setDefaultSubscribes();
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
			uint8_t readPumpActive = msg.toInt();
			if(pumpActive != readPumpActive) {
				pumpActive = readPumpActive;
				EEPROM.write(wpEEPROM.bytePumpActive, pumpActive);
				EEPROM.commit();
				wpFZ.DebugcheckSubscribes(mqttTopicPumpActive, String(pumpActive));
			}
		}
		if(strcmp(topic, mqttTopicPumpPause.c_str()) == 0) {
			uint16_t readPumpPause = msg.toInt();
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
	checkDefaultSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleRelais::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicOut.c_str(), String(output).c_str());
	if(sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("relais", String(output));
		wpRest.trySend = true;
	}
	outputLast = output;
	if(wpMqtt.Debug) {
		printPublishValueDebug("Relais", String(output), String(publishCountOutput));
	}
	publishCountOutput = 0;
}

void moduleRelais::calc() {
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
	if(output && digitalRead(relaisPin) == LOW) {
		digitalWrite(relaisPin, HIGH);
		if(Debug) {
			wpFZ.DebugWS(wpFZ.strDEBUG, "calc", "Relais turns on");
		}
	}
	if(!output && digitalRead(relaisPin) == HIGH) {
		digitalWrite(relaisPin, LOW);
		if(Debug) {
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
				if(Debug) {
					wpFZ.DebugWS(wpFZ.strDEBUG, "calcPump", "Detect 'toDry', start Pump Cycle");
				}
			}
			// start pump and store timestart
			if(pumpCycleActive && !pumpStarted && !pumpInPause) {
				autoValue = true;
				pumpStarted = true;
				pumpTimeStart = m;
				if(Debug) {
					wpFZ.DebugWS(wpFZ.strDEBUG, "calcPump", "start 'pump' (" + String(pumpTimeStart) + ")");
				}
			}
			// stop pump and start pause
			if(pumpCycleActive && pumpStarted && !pumpInPause) {
				if(m > (pumpTimeStart + (pumpActive * 1000))) {
					autoValue = false;
					pumpInPause = true;
					pumpTimePause = m;
					if(Debug) {
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
					if(Debug) {
						wpFZ.DebugWS(wpFZ.strDEBUG, "calcPump", "stopped 'pumpPause' and reset");
					}
				}
			}
		}
	}
}
// }
void moduleRelais::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}

//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleRelais::getVersion() {
	String SVN = "$Rev: 132 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}
void moduleRelais::changeSendRest() {
	sendRest = !sendRest;
	bitWrite(byteSendRest, bitSendRest, sendRest);
	EEPROM.write(addrSendRest, byteSendRest);
	EEPROM.commit();
	wpFZ.blink();
}
void moduleRelais::changeDebug() {
	Debug = !Debug;
	bitWrite(byteDebug, bitDebug, Debug);
	EEPROM.write(addrDebug, byteDebug);
	EEPROM.commit();
	wpFZ.blink();
}
void moduleRelais::publishDefaultSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
	}
}
void moduleRelais::publishDefaultValues(bool force) {
	if(force) {
		publishCountSendRest = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
		publishCountError = wpFZ.publishQoS;
	}
	if(sendRestLast != sendRest || ++publishCountSendRest > wpFZ.publishQoS) {
		sendRestLast = sendRest;
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		wpFZ.SendWSSendRest("sendRest" + ModuleName, sendRest);
		publishCountSendRest = 0;
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		wpFZ.SendWSDebug("Debug" + ModuleName, Debug);
		publishCountDebug = 0;
	}
	if(errorLast != error || ++publishCountError > wpFZ.publishQoS) {
		errorLast = error;
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
		publishCountError = 0;
	}
}
void moduleRelais::setDefaultSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSendRest.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void moduleRelais::checkDefaultSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		uint8_t readMaxCycle = msg.toInt();
		if(readMaxCycle <= 0) readMaxCycle = 1;
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			EEPROM.write(addrMaxCycle, maxCycle);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxCycle, String(maxCycle));
		}
	}
	if(strcmp(topic, mqttTopicSendRest.c_str()) == 0) {
		bool readSendRest = msg.toInt();
		if(sendRest != readSendRest) {
			sendRest = readSendRest;
			bitWrite(byteSendRest, bitSendRest, sendRest);
			EEPROM.write(addrSendRest, byteSendRest);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSendRest, String(sendRest));
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(byteDebug, bitDebug, Debug);
			EEPROM.write(addrDebug, byteDebug);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}