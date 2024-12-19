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
//# File-ID      : $Id:: moduleRFID.cpp 232 2024-12-19 15:27:48Z                  $ #
//#                                                                                 #
//###################################################################################
#include <moduleRFID.h>

moduleRFID wpRFID;

moduleRFID::moduleRFID() {
	// section to config and copy
	ModuleName = "RFID";
	mb = new moduleBase(ModuleName);
}
void moduleRFID::init() {
	// section for define
	PinSS = D8;
	PinRST = D3;
	rfid = new MFRC522(PinSS, PinRST);
	// values
	mqttTopicChipID = wpFZ.DeviceName + "/" + ModuleName;
	// settings

	chipIDLast = 0;
	publishChipIDLast = 0;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugRFID);
	mb->initError();
	mb->initCalcCycle(wpEEPROM.byteCalcCycleRFID);

	SPI.begin();
	rfid->PCD_Init();

	mb->calcLast = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleRFID::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleRFID::publishSettings() {
	publishSettings(false);
}
void moduleRFID::publishSettings(bool force) {
	mb->publishSettings(force);
}

void moduleRFID::publishValues() {
	publishValues(false);
}
void moduleRFID::publishValues(bool force) {
	if(force) {
		publishChipIDLast = 0;
	}
	if(chipIDLast != chipID || wpFZ.CheckQoS(publishChipIDLast)) {
		publishValue();
	}
	mb->publishValues(force);
}

void moduleRFID::setSubscribes() {
	mb->setSubscribes();
}

void moduleRFID::checkSubscribes(char* topic, String msg) {
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleRFID::publishValue() {
	wpMqtt.mqttClient.publish(mqttTopicChipID.c_str(), String(chipID).c_str());
	chipIDLast = chipID;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("ChipID", String(chipID));
	}
	publishChipIDLast = wpFZ.loopStartedAt;
}

void moduleRFID::calc() {
	if(rfid->PICC_IsNewCardPresent()) {
		chipID = 0;
		rfid->PICC_ReadCardSerial();
		for(byte i = 0; i < rfid->uid.size; i++ ) {
			chipID = ((chipID + rfid->uid.uidByte[i]) * 10);
		}
		wpFZ.DebugWS(wpFZ.strDEBUG, "RFID::calc", "new Card detected: " + String(chipID));
		wpFZ.blink();
		String minimac = WiFi.macAddress();
		minimac.replace(":", "");
		minimac.toLowerCase();
		wpFZ.sendRawRest(F("http://172.17.1.11:255/?m=") + minimac + "&rfid=" + String(chipID));
	}
}

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleRFID::getVersion() {
	String SVN = "$Rev: 232 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleRFID::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("PinSS"), String(wpFZ.Pins[PinSS])) + F(",") +
		wpFZ.JsonKeyString(F("PinRST"), String(wpFZ.Pins[PinRST])) + F(",") +
		wpFZ.JsonKeyValue(F("CalcCycle"), String(CalcCycle())) +
		F("},");
	return json;
}

void moduleRFID::changeDebug() {
	mb->changeDebug();
}
bool moduleRFID::Debug() {
	return mb->debug;
}
bool moduleRFID::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32 moduleRFID::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleRFID::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
