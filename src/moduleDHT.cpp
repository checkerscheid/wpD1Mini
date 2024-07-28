//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 29.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 182                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDHT.cpp 182 2024-07-28 02:12:39Z                   $ #
//#                                                                                 #
//###################################################################################
#include <moduleDHT.h>

moduleDHT wpDHT;

moduleDHT::moduleDHT() {
	// section to config and copy
	ModuleName = "DHT";
	mb = new moduleBase(ModuleName);
}
void moduleDHT::init() {
	// section for define
	Pin = D7;
	temperature = 0;
	humidity = 0;
	// values
	mqttTopicTemperature = wpFZ.DeviceName + "/Temperature";
	mqttTopicHumidity = wpFZ.DeviceName + "/Humidity";
	// settings
	mqttTopicTemperatureCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction/Temperature";
	mqttTopicHumidityCorrection = wpFZ.DeviceName + "/settings/" + ModuleName + "/Correction/Humidity";

	temperatureLast = 0;
	publishTemperatureLast = 0;
	humidityLast = 0;
	publishHumidityLast = 0;

	dht = new DHT(Pin, wpModules.choosenDHTmodul);
	dht->begin();

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules0, wpEEPROM.bitsSendRestModules0, wpEEPROM.bitSendRestDHT);
	mb->initDebug(wpEEPROM.addrBitsDebugModules0, wpEEPROM.bitsDebugModules0, wpEEPROM.bitDebugDHT);
	mb->initError();
	mb->initCalcCycle(wpEEPROM.byteCalcCycleDHT);

	mb->calcLast = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleDHT::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleDHT::publishSettings() {
	publishSettings(false);
}
void moduleDHT::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicTemperatureCorrection.c_str(), String(float(temperatureCorrection / 10.0)).c_str());
	wpMqtt.mqttClient.publish(mqttTopicHumidityCorrection.c_str(), String(float(humidityCorrection / 10.0)).c_str());
	mb->publishSettings(force);
}

void moduleDHT::publishValues() {
	publishValues(false);
}
void moduleDHT::publishValues(bool force) {
	if(force) {
		publishTemperatureLast = 0;
		publishHumidityLast = 0;
	}
	if(temperatureLast != temperature || mb->CheckQoS(publishTemperatureLast)) {
		publishValueTemp();
	}
	if(humidityLast != humidity || mb->CheckQoS(publishHumidityLast)) {
		publishValueHum();
	}
	mb->publishValues(force);
}

void moduleDHT::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicTemperatureCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicHumidityCorrection.c_str());
	mb->setSubscribes();
}

void moduleDHT::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicTemperatureCorrection.c_str()) == 0) {
		int8 readTemperatureCorrection = int8_t(msg.toFloat() * 10.0);
		if(temperatureCorrection != readTemperatureCorrection) {
			temperatureCorrection = readTemperatureCorrection;
			EEPROM.put(wpEEPROM.byteTemperatureCorrection, temperatureCorrection);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicTemperatureCorrection, String(temperatureCorrection));
		}
	}
	if(strcmp(topic, mqttTopicHumidityCorrection.c_str()) == 0) {
		int8 readHumidityCorrection = int8_t(msg.toFloat() * 10);
		if(humidityCorrection != readHumidityCorrection) {
			humidityCorrection = readHumidityCorrection;
			EEPROM.put(wpEEPROM.byteHumidityCorrection, humidityCorrection);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicHumidityCorrection, String(humidityCorrection));
		}
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleDHT::publishValueTemp() {
	String sendTemperature = String(float(temperature / 100.0));
	wpMqtt.mqttClient.publish(mqttTopicTemperature.c_str(), sendTemperature.c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("temp", sendTemperature);
		wpRest.trySend = true;
	}
	temperatureLast = temperature;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("Temperature", sendTemperature);
	}
	publishTemperatureLast = wpFZ.loopStartedAt;
}

void moduleDHT::publishValueHum() {
	String sendHumidity = String(float(humidity / 100.0));
	wpMqtt.mqttClient.publish(mqttTopicHumidity.c_str(), sendHumidity.c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRest("hum", sendHumidity);
		wpRest.trySend = true;
	}
	humidityLast = humidity;
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug("Humidity", sendHumidity);
	}
	publishHumidityLast = wpFZ.loopStartedAt;
}

void moduleDHT::calc() {
	bool e = false;
	float newT = dht->readTemperature();
	float newH = dht->readHumidity();
	if(!isnan(newT)) {
		temperature = int(newT * 100) + (temperatureCorrection * 10);
		e = e | false;
		if(mb->debug) {
			printCalcDebug("Temperature", temperature, newT);
		}
	} else {
		e = e | true;
		printCalcError("Temperature");
	}
	if(!isnan(newH)) {
		humidity = int(newH * 100) + (humidityCorrection * 10);
		e = e | false;
		if(mb->debug) {
			printCalcDebug("Humidity", humidity, newH);
		}
	} else {
		e = e | true;
		printCalcError("Humidity");
	}
	mb->error = e;
}

void moduleDHT::printCalcError(String name) {
	wpFZ.blink();
	String logmessage = name + ": Sensor Failure";
	wpFZ.DebugWS(wpFZ.strERRROR, "wpDHT::calc", logmessage);
}

void moduleDHT::printCalcDebug(String name, int value, float raw) {
	String logmessage = name + ": " + String(value) + " (" + String(raw) + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "wpDHT::calc", logmessage);
}

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleDHT::getVersion() {
	String SVN = "$Rev: 182 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleDHT::changeSendRest() {
	mb->changeSendRest();
}
void moduleDHT::changeDebug() {
	mb->changeDebug();
}
bool moduleDHT::SendRest() {
	return mb->sendRest;
}
bool moduleDHT::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleDHT::Debug() {
	return mb->debug;
}
bool moduleDHT::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32 moduleDHT::CalcCycle() {
	return mb->calcCycle;
}
uint32 moduleDHT::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
