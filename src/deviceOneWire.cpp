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
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleLDR.cpp 207 2024-10-07 12:59:22Z                   $ #
//#                                                                                 #
//###################################################################################
#include <deviceOneWire.h>

/**
 * Device DS18B20
 * hold the settings for one Device in the OneWire Bus
 */
deviceOneWire::deviceOneWire(uint8 no) {
	// section to config and copy
	number = no;
	ModuleName = "DS18B20/deviceOneWire" + String(no + 1);
}
void deviceOneWire::init() {
	// section for define
	temperature = 0;

	// values
	mqttTopicTemperature = wpFZ.DeviceName + "/" + ModuleName + "/Temperature";
	mqttTopicCorrection = wpFZ.DeviceName + "/" + ModuleName + "/Correction";
	// settings
	mqttTopicSetCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction";

	temperatureLast = 0;
	publishTemperatureLast = 0;

	// section to copy
}

//###################################################################################
// public
//###################################################################################
void deviceOneWire::cycle() {
	calc();
	publishValues();
}

void deviceOneWire::publishSettings() {
	publishSettings(false);
}
void deviceOneWire::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
}

void deviceOneWire::publishValues() {
	publishValues(false);
}
void deviceOneWire::publishValues(bool force) {
	if(force) {
		publishTemperatureLast = 0;
	}
	if(temperatureLast != temperature || wpFZ.CheckQoS(publishTemperatureLast)) {
		String sendTemperature = String(float(temperature / 100.0));
		temperatureLast = temperature;
		wpMqtt.mqttClient.publish(mqttTopicTemperature.c_str(), sendTemperature.c_str());
		publishTemperatureLast = wpFZ.loopStartedAt;
	}
	if(correctionLast != correction || wpFZ.CheckQoS(publishCorrectionLast)) {
		correctionLast = correction;
		wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
		publishCorrectionLast = wpFZ.loopStartedAt;
	}
}

void deviceOneWire::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetCorrection.c_str());
}

void deviceOneWire::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetCorrection.c_str()) == 0) {
		int8 readCorrection = msg.toInt();
		if(correction != readCorrection) {
			correction = readCorrection;
			EEPROM.put(wpEEPROM.byteDS18B20Correction[number], correction);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSetCorrection, String(correction));
		}
	}
}
void deviceOneWire::initAddress(uint8 b0, uint8 b1, uint8 b2, uint8 b3, uint8 b4, uint8 b5, uint8 b6, uint8 b7) {
	address[0] = b0;
	address[1] = b1;
	address[2] = b2;
	address[3] = b3;
	address[4] = b4;
	address[5] = b5;
	address[6] = b6;
	address[7] = b7;
}
void deviceOneWire::setAddress(uint8 adr[8]) {
	String print = "save Address: ";
	for(uint8 i = 0; i < 8; i++) {
		address[i] = adr[i];
		EEPROM.write(wpEEPROM.byteDS18B20adr[number][i], adr[i]);
		print += String(adr[i], HEX);
		if(i < 7) print += ":";
	}
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "setAddress", print);
}
uint8_t* deviceOneWire::getAddress() {
	return address;
}
void deviceOneWire::setTemperature(float t) {
	temperature = int(t * 100) + (correction * 10);
}

//###################################################################################
// private
//###################################################################################

void deviceOneWire::calc() {

}

//###################################################################################
// section to copy
//###################################################################################
uint16 deviceOneWire::getVersion() {
	String SVN = "$Rev: 207 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

