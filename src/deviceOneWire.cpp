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
	ModuleName = "deviceOneWire-" + String(no);
	mb = new moduleBase(ModuleName);
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
	//mb->initUseAvg(wpEEPROM.addrBitsSettingsOneWire, wpEEPROM.bitsSettingsOneWire, wpEEPROM.bitUseAvgDS18B20[number]);

	mb->calcLast = 0;
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
	mb->publishSettings(force);
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
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("Temperature", sendTemperature);
		}
		publishTemperatureLast = wpFZ.loopStartedAt;
	}
	if(correctionLast != correction || wpFZ.CheckQoS(publishCorrectionLast)) {
		correctionLast = correction;
		wpMqtt.mqttClient.publish(mqttTopicCorrection.c_str(), String(correction).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("Correction", String(correction));
		}
		publishCorrectionLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void deviceOneWire::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetCorrection.c_str());
	mb->setSubscribes();
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
	mb->checkSubscribes(topic, msg);
}
void deviceOneWire::initAddress(uint8 adr[8]) {
	for(uint8 i = 0; i < 8; i++) {
		address[i] = adr[i];
	}
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

//###################################################################################
// private
//###################################################################################

void deviceOneWire::calc() {

}
uint16 deviceOneWire::calcAvg(uint16 raw) {
	long avg = 0;
	long avgCount = avgLength;
	avgValues[avgLength - 1] = raw;
	for(int i = 0; i < avgLength - 1; i++) {
		if(avgValues[i + 1] > 0) {
			avgValues[i] = avgValues[i + 1];
			avg += avgValues[i] * (i + 1);
			avgCount += (i + 1);
		}
	}
	avg += raw * avgLength;
	return round(avg / avgCount);
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

bool deviceOneWire::UseAvg() {
	return mb->useAvg;
}
bool deviceOneWire::UseAvg(bool useAvg) {
	mb->useAvg = useAvg;
	return true;
}
