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
//# Revision     : $Rev:: 271                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleSML.cpp 271 2025-07-30 22:05:20Z                   $ #
//#                                                                                 #
//###################################################################################
#include <moduleSML.h>

moduleSML wpSML;

static const byte StartSequence[] = { 0x1B, 0x1B, 0x1B, 0x1B, 0x01, 0x01, 0x01, 0x01 };

static const byte ZsNetzSequenceAPA[] = { 0x77, 0x07, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x65, 0x00, 0x1C, 0x81, 0x04, 0x01, 0x62, 0x1E, 0x52, 0xFF, 0x69, 0x00, 0x00, 0x00, 0x00 };
static const byte PSequenceAPA[] = { 0x77, 0x07, 0x01, 0x00, 0x10, 0x07, 0x00, 0xFF, 0x01, 0x01, 0x62, 0x1B, 0x52, 0x00, 0x59, 0x00, 0x00, 0x00, 0x00 };

static const byte ZsNetzSequenceDZG[] = { 0x77, 0x07, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x64, 0x00, 0x00, 0x00, 0x72, 0x62, 0x01, 0x65, 0x00, 0x00, 0x00, 0x00, 0x62, 0x1E, 0x52, 0xFF, 0x55 };
static const byte ZsSolarSequenceDZG[] = { 0x77, 0x07, 0x01, 0x00, 0x02, 0x08, 0x00, 0xFF, 0x01, 0x72, 0x62, 0x01, 0x65, 0x00, 0x00, 0x00, 0x00, 0x62, 0x1E, 0x52, 0xFF, 0x55 };
static const byte PSequenceDZG[] = { 0x77, 0x07, 0x01, 0x00, 0x10, 0x07, 0x00, 0xFF, 0x01, 0x01, 0x62, 0x1B, 0x52, 0xFE, 0x00 }; //, 0x00, 0x00, 0x62, 0x1B, 0x52, 0x00, 0x55 };

static const byte InfoSequence[] = { 0x01, 0x00, 0x5E, 0x31, 0x01, 0x02 };

moduleSML::moduleSML() {
	// section to config and copy
	ModuleName = "SML";
	mb = new moduleBase(ModuleName);

	
}
void moduleSML::init() {
	// section for define
	PinRX = D2;
	PinTX = D1;
	SmlSerial = new SoftwareSerial(PinRX, PinTX);
	SmlSerial->begin(9600);


	// values
	mqttTopicZsNetz = wpFZ.DeviceName + "/" + ModuleName + "/ZsNetz";
	mqttTopicZsSolar = wpFZ.DeviceName + "/" + ModuleName + "/ZsSolar";
	mqttTopicPNetz = wpFZ.DeviceName + "/" + ModuleName + "/PNetz";
	mqttTopicPSolar = wpFZ.DeviceName + "/" + ModuleName + "/PSolar";
	// settings

	publishZsNetzLast = 0;
	publishZsSolarLast = 0;
	publishPNetzLast  = 0;
	publishPSolarLast = 0;


	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules3, wpEEPROM.bitsDebugModules3, wpEEPROM.bitDebugSML);
	mb->initError();

}

//###################################################################################
// public
//###################################################################################
void moduleSML::cycle() {
	calc();
	publishValues();
}

void moduleSML::publishSettings() {
	publishSettings(false);
}
void moduleSML::publishSettings(bool force) {
	mb->publishSettings(force);
}

void moduleSML::publishValues() {
	publishValues(false);
}
void moduleSML::publishValues(bool force) {
	if(force) {
		publishZsNetzLast = 0;
		publishZsSolarLast = 0;
		publishPNetzLast  = 0;
		publishPSolarLast = 0;
	}
	if(currentZsNetz != currentZsNetzLast || wpFZ.CheckQoS(publishZsNetzLast)) {
		currentZsNetzLast = currentZsNetz;
		wpMqtt.mqttClient.publish(mqttTopicZsNetz.c_str(), String(currentZsNetz).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("PKwh", String(currentZsNetz));
		}
		publishZsNetzLast = wpFZ.loopStartedAt;
	}
	if(currentZsSolar != currentZsSolarLast || wpFZ.CheckQoS(publishZsSolarLast)) {
		currentZsSolarLast = currentZsSolar;
		wpMqtt.mqttClient.publish(mqttTopicZsSolar.c_str(), String(currentZsSolar).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("PKwh", String(currentZsSolar));
		}
		publishZsSolarLast = wpFZ.loopStartedAt;
	}
	if(currentPNetz != currentPNetzLast || wpFZ.CheckQoS(publishPNetzLast)) {
		currentPNetzLast = currentPNetz;
		wpMqtt.mqttClient.publish(mqttTopicPNetz.c_str(), String(currentPNetz).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("PW", String(currentPNetz));
		}
		publishPNetzLast = wpFZ.loopStartedAt;
	}
	if(currentPSolarLast != currentPSolar || wpFZ.CheckQoS(publishPSolarLast)) {
		currentPSolarLast = currentPSolar;
		wpMqtt.mqttClient.publish(mqttTopicPSolar.c_str(), String(currentPSolar).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("PW", String(currentPSolar));
		}
		publishPSolarLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleSML::setSubscribes() {
	mb->setSubscribes();
}

void moduleSML::checkSubscribes(char* topic, String msg) {
	mb->checkSubscribes(topic, msg);
}

void moduleSML::InitMeterType(uint8_t meterType) {
	MeterType = meterType;
}
String moduleSML::SetMeterType(uint8_t meterType) {
	MeterType = meterType;
	wpEEPROM.WriteByteToEEPROM("MeterType", wpEEPROM.byteMeterType, MeterType, true);
	return "{" + wpFZ.JsonKeyString("MeterType", GetMeterTypeName()) + "}";
}
String moduleSML::GetMeterTypeName() {
	String meterTypeName;
	if(MeterType == MeterTypeApator) {
		meterTypeName = "Apator";
	} else if(MeterType == MeterTypeDZG) {
		meterTypeName = "DZG";
	} else {
		meterTypeName = "Unknown";
	}
	return meterTypeName;
}
String moduleSML::SetInfo() {
	SmlSerial->write(InfoSequence, sizeof(InfoSequence));
	wpFZ.DebugWS(wpFZ.strDEBUG, "moduleSML::SetInfo", "SML Info request sent");
	return wpFZ.jsonOK;
}
String moduleSML::SetPrintSml() {
	printSml = !printSml; // Toggle printSml
	wpFZ.DebugWS(wpFZ.strDEBUG, "moduleSML::SetPrintSml", (printSml ? "SML ausgabe ohne parsen" : "SML parsen aktiv, keine Ausgabe"));
	return wpFZ.jsonOK;
}

//###################################################################################
// private
//###################################################################################

void moduleSML::calc() {
	SequenceStep = SetError(SequenceStep); // Fehlerbehandlung
	// SML-Verarbeitung und speichern der aktullen Werte
	if (SmlSerial->available()) { // wenn Daten vorhanden
		bytesmltemp = SmlSerial->read(); // Temp-Speicher der SML Nachricht
		if(printSml || mb->debug) {
			if(bytesmltemp < 0x10) Serial.print("0");
			Serial.print(bytesmltemp, HEX);
			Serial.print(" ");
		} else {
			if(MeterType == MeterTypeApator) {
				switch (SequenceStep) {
					case 1: // Suche Start-Sequence
						if (bytesmltemp == StartSequence[SequenceIndex]) {
							SequenceIndex++;
							if (SequenceIndex == 8) {
								SequenceIndex = 0;
								SequenceStep = 2;
								if(mb->debug) {
									Serial.println();
								}
							}
						} else if (bytesmltemp != StartSequence[SequenceIndex]) {
							SequenceIndex = 0;
						}
						break;
					case 2: // Suche PKwh-Sequence
						if (bytesmltemp == ZsNetzSequenceAPA[SequenceIndex]) {
							SequenceIndex++;
							if (SequenceIndex == sizeof(ZsNetzSequenceAPA)) {
								SequenceIndex = 0;
								SequenceStep = 3;
								if(mb->debug) {
									Serial.println();
								}
							}
						} else if (bytesmltemp != ZsNetzSequenceAPA[SequenceIndex]) {
							SequenceIndex = 0;
						}
						break;
					case 3: // Gesamtleistung Speichern
						SmlZsNetz[SequenceIndex] = bytesmltemp;
						SequenceIndex++;
						if (SequenceIndex == 4) {
							SequenceIndex = 0;
							SequenceStep = 4;
							currentZsNetz = (uint32_t)(((unsigned long)SmlZsNetz[0] << 24 | (unsigned long)SmlZsNetz[1] << 16 | (unsigned long)SmlZsNetz[2] << 8 | (unsigned long)SmlZsNetz[3]) / 10000);
							if(mb->debug) {
								Serial.println();
							}
						}
						break;
					case 4: // Suche PW-Sequence
						if (bytesmltemp == PSequenceAPA[SequenceIndex]) {
							SequenceIndex++;
							if (SequenceIndex == sizeof(PSequenceAPA)) {
								SequenceIndex = 0;
								SequenceStep = 5;
								if(mb->debug) {
									Serial.println();
								}
							}
						} else if (bytesmltemp != PSequenceAPA[SequenceIndex]) {
							SequenceIndex = 0;
						}
						break;
					case 5: // Aktuelle Wirkleistung speichern
						SmlPNetz[SequenceIndex] = bytesmltemp;
						SequenceIndex++;
						if (SequenceIndex == 4) {
							SequenceIndex = 0;
							SequenceStep = 1;
							currentPNetz = (int32_t)((int32_t)SmlPNetz[0] << 24 | (int32_t)SmlPNetz[1] << 16 | (int32_t)SmlPNetz[2] << 8 | (int32_t)SmlPNetz[3]);
							if(mb->debug) {
								Serial.println();
							}
						}
						break;
				}
			} else if(MeterType == MeterTypeDZG) {
				switch (SequenceStep) {
					case 1: // Suche Start-Sequence
						if (bytesmltemp == StartSequence[SequenceIndex]) {
							SequenceIndex++;
							if (SequenceIndex == 8) {
								SequenceIndex = 0;
								SequenceStep = 2;
								if(mb->debug) {
									Serial.println();
								}
							}
						} else if (bytesmltemp != StartSequence[SequenceIndex]) {
							SequenceIndex = 0;
						}
						break;
					case 2: // Suche PKwh-Sequence
						if (bytesmltemp == ZsNetzSequenceDZG[SequenceIndex] ||
							ZsNetzSequenceDZG[SequenceIndex] == 0x00) {
							SequenceIndex++;
							if (SequenceIndex == sizeof(ZsNetzSequenceDZG)) {
								SequenceIndex = 0;
								SequenceStep = 3;
								if(mb->debug) {
									Serial.println();
								}
							}
						} else if (bytesmltemp != ZsNetzSequenceDZG[SequenceIndex]) {
							SequenceIndex = 0;
						}
						break;
					case 3: // Gesamtleistung Speichern
						SmlZsNetz[SequenceIndex] = bytesmltemp;
						SequenceIndex++;
						if (SequenceIndex == 4) {
							SequenceIndex = 0;
							SequenceStep = 4;
							currentZsNetz = (uint32_t)(((unsigned long)SmlZsNetz[0] << 24 | (unsigned long)SmlZsNetz[1] << 16 | (unsigned long)SmlZsNetz[2] << 8 | (unsigned long)SmlZsNetz[3]) / 10000);
							if(mb->debug) {
								Serial.println();
							}
						}
						break;
					case 4: // Suche PW-Sequence
						if (bytesmltemp == ZsSolarSequenceDZG[SequenceIndex] ||
							ZsSolarSequenceDZG[SequenceIndex] == 0x00) {
							SequenceIndex++;
							if (SequenceIndex == sizeof(ZsSolarSequenceDZG)) {
								SequenceIndex = 0;
								SequenceStep = 5;
								if(mb->debug) {
									Serial.println();
								}
							}
						} else if (bytesmltemp != ZsSolarSequenceDZG[SequenceIndex]) {
							SequenceIndex = 0;
						}
						break;
					case 5: // Aktuelle Wirkleistung speichern
						SmlZsSolar[SequenceIndex] = bytesmltemp;
						SequenceIndex++;
						if (SequenceIndex == 4) {
							SequenceIndex = 0;
							SequenceStep = 6;
							currentZsSolar = (uint32_t)(((int8_t)SmlZsSolar[0] << 24 | (int16_t)SmlZsSolar[1] << 16 | (uint32_t)SmlZsSolar[2] << 8 | (int64_t)SmlZsSolar[3]) / 10000);
							if(mb->debug) {
								Serial.println();
							}
						}
						break;
					case 6: // Suche PW-Sequence
						if (bytesmltemp == PSequenceDZG[SequenceIndex] ||
							PSequenceDZG[SequenceIndex] == 0x00) {
							SequenceIndex++;
							if (SequenceIndex == sizeof(PSequenceDZG)) {
								SequenceIndex = 0;
								if(bytesmltemp == 0x53) {
									SequenceStep = 7;
								} else if(bytesmltemp == 0x54) {
									SequenceStep = 8;
								}
								if(mb->debug) {
									Serial.println();
								}
							}
						} else if (bytesmltemp != PSequenceDZG[SequenceIndex]) {
							SequenceIndex = 0;
						}
						break;
					case 7: // Aktuelle Wirkleistung speichern
						SmlPSolar[SequenceIndex] = bytesmltemp;
						SequenceIndex++;
						if (SequenceIndex == 2) {
							SequenceIndex = 0;
							SequenceStep = 1;
							currentPSolar = (int32_t)(((int8_t)SmlPSolar[0] << 8 | (int16_t)SmlPSolar[1]) / 100);
							if(mb->debug) {
								Serial.println();
							}
						}
						break;
					case 8: // Aktuelle Wirkleistung speichern
						SmlPSolar[SequenceIndex] = bytesmltemp;
						SequenceIndex++;
						if (SequenceIndex == 3) {
							SequenceIndex = 0;
							SequenceStep = 1;
							currentPSolar = (int32_t)(((int8_t)SmlPSolar[0] << 16 | (int16_t)SmlPSolar[1] << 8 | (int32_t)SmlPSolar[2]) / 100);
							if(mb->debug) {
								Serial.println();
							}
						}
						break;

					// case 7: // Aktuelle Wirkleistung speichern
					// 	SmlPNetz[SequenceIndex] = bytesmltemp;
					// 	SequenceIndex++;
					// 	if (SequenceIndex == 1) {
					// 		SequenceIndex = 0;
					// 		SequenceStep = 1;
					// 		currentPNetz = SmlPNetz[0];
					// 		if(mb->debug) {
					// 			Serial.println();
					// 		}
					// 	}
					// 	break;
				}
			}
		}
	}
}
uint8_t moduleSML::SetError(uint8_t Step) {
	uint8_t returns = Step;
	uint32_t static previousMillis = millis(); // speichert wie viele Sekunden seit der letzten Änderung vergangen sind
	uint32_t static interval = 5000; // Interval zwischen zwei Änderungen
	uint8_t static intStepOld; // Alter Schritt speichern

	if(Step != intStepOld) {
		intStepOld = Step; // alten Schritt speichern
		previousMillis = millis();
		mb->error = false;
	}
	if(Step == intStepOld && millis() - previousMillis > interval) {
		previousMillis = millis();
		mb->error = true;
		returns = 1;
	}
	return returns;
}
//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleSML::getVersion() {
	String SVN = "$Rev: 271 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleSML::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("PinRX"), String(wpFZ.Pins[PinRX])) + F(",") +
		wpFZ.JsonKeyString(F("PinTX"), String(wpFZ.Pins[PinTX])) + F(",") +
		wpFZ.JsonKeyValue(F("MeterType"), String(MeterType)) + F(",") +
		wpFZ.JsonKeyString(F("MeterTypeName"), GetMeterTypeName()) +
		F("}");
	return json;
}

void moduleSML::changeDebug() {
	mb->changeDebug();
}
bool moduleSML::Debug() {
	return mb->debug;
}
bool moduleSML::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
