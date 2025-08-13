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
//# Revision     : $Rev:: 273                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleModbus.cpp 273 2025-08-13 18:46:12Z                $ #
//#                                                                                 #
//###################################################################################
#include <moduleModbus.h>

moduleModbus wpModbus;

moduleModbus::moduleModbus() {
	// section to config and copy
	ModuleName = "Modbus";
	mb = new moduleBase(ModuleName);
}
void moduleModbus::init() {
	// section for define
	PinRX = RX;
	PinTX = TX;
	Pin = D1;

	ss = new SoftwareSerial(PinRX, PinTX);
	ss->begin(9600, SWSERIAL_8N1);
	modbus = new ModbusRTU();
	modbus->begin(ss, Pin);

	if(isMaster) {
		pinMode(Pin, OUTPUT);
		digitalWrite(Pin, HIGH); // Enable RS485 Transmitter mode
		modbus->master();
	} else {
		pinMode(Pin, OUTPUT);
		digitalWrite(Pin, LOW); // Enable RS485 Receiver mode
		modbus->slave(clientid);
		modbus->addHreg(addrtemp);
		modbus->addHreg(addrhum);
	}

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules3, wpEEPROM.bitsDebugModules3, wpEEPROM.bitDebugModbus);
	mb->initError();

	mb->calcLast = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleModbus::cycle() {
	calc();
	publishValues();
}

void moduleModbus::publishSettings() {
	publishSettings(false);
}
void moduleModbus::publishSettings(bool force) {
	mb->publishSettings(force);
}

void moduleModbus::publishValues() {
	publishValues(false);
}
void moduleModbus::publishValues(bool force) {
	if(force) {
	}
	mb->publishValues(force);
}

void moduleModbus::setSubscribes() {
	mb->setSubscribes();
}

void moduleModbus::checkSubscribes(char* topic, String msg) {
	mb->checkSubscribes(topic, msg);
}

void moduleModbus::InitIsMaster(bool isMaster) {
	this->isMaster = isMaster;
}
String moduleModbus::SetIsMaster(bool isMaster) {
	this->isMaster = isMaster;
	wpEEPROM.WriteBoolToEEPROM(ModuleName + " isMaster", wpEEPROM.addrBitsSettingsModules3, wpEEPROM.bitsSettingsModules3, wpEEPROM.bitModbusIsMaster, isMaster);
	return wpFZ.jsonOK;
}
bool moduleModbus::GetIsMaster() {
	return isMaster;
}
void moduleModbus::InitClientId(uint8_t clientid) {
	if(clientid <= 0 || clientid > 3) {
		SetClientId(1);
	}
	this->clientid = clientid;
}
String moduleModbus::SetClientId(uint8_t clientid) {
	this->clientid = clientid;
	wpEEPROM.WriteByteToEEPROM(ModuleName + " clientid", wpEEPROM.byteModbusClientId, clientid);
	return wpFZ.jsonOK;
}
uint8_t moduleModbus::GetClientId() {
	return clientid;
}

//###################################################################################
// private
//###################################################################################

void moduleModbus::calc() {
	if(isMaster) {
		if(!modbus->slave()) {
			if(templast != wpDHT.temperature) {
				templast = wpDHT.temperature;
				modbus->writeHreg(clientid, addrtemp, templast);
			}
			if(humlasst != wpDHT.humidity) {
				humlasst = wpDHT.humidity;
				modbus->writeHreg(clientid, addrhum, humlasst);
			}
			while(modbus->slave()) {
				modbus->task();
			}
		}
	} else {
		if(readedTemp != modbus->Hreg(addrtemp)) {
			readedTemp = modbus->Hreg(addrtemp);
			wpFZ.DebugWS("Modbus", "calc", "Temp: " + String(readedTemp) + " °C");
		}
		if(readedHum != modbus->Hreg(addrhum)) {
			readedHum = modbus->Hreg(addrhum);
			wpFZ.DebugWS("Modbus", "calc", "Humidity: " + String(readedHum) + " %");
		}
		modbus->task();
	}
}


//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleModbus::getVersion() {
	String SVN = "$Rev: 273 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleModbus::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin RX"), String(wpFZ.Pins[PinRX])) + F(",") +
		wpFZ.JsonKeyString(F("Pin TX"), String(wpFZ.Pins[PinTX])) + F(",") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyValue(F("Is Master"), isMaster ? F("true") : F("false")) + F(",") +
		wpFZ.JsonKeyValue(F("Client ID"), String(clientid)) + 
		F("}");
	return json;
}

void moduleModbus::changeDebug() {
	mb->changeDebug();
}
bool moduleModbus::Debug() {
	return mb->debug;
}
bool moduleModbus::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
