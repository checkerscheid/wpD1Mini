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
//# Revision     : $Rev:: 121                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDHT.cpp 121 2024-06-01 05:13:59Z                   $ #
//#                                                                                 #
//###################################################################################
#include <moduleDHT.h>

moduleDHT wpDHT;
DHT dht(DHTPin, wpModules.choosenDHTmodul);

moduleDHT::moduleDHT() {}
void moduleDHT::init() {
	// values
	mqttTopicTemperature = wpFZ.DeviceName + "/Temperature";
	mqttTopicHumidity = wpFZ.DeviceName + "/Humidity";
	mqttTopicError = wpFZ.DeviceName + "/ERROR/DHT";
	// settings
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/DHT/maxCycle";
	mqttTopicTemperatureCorrection = wpFZ.DeviceName + "/settings/DHT/Correction/Temperature";
	mqttTopicHumidityCorrection = wpFZ.DeviceName + "/settings/DHT/Correction/Humidity";
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/DHT";

	temperatureCorrection = 0;
	humidityCorrection = 0;

	dht.begin();
}

//###################################################################################
// public
//###################################################################################
void moduleDHT::cycle() {
	if(wpFZ.calcValues && ++cycleCounter >= maxCycle) {
		cycleCounter = 0;
		calc();
	}
	publishValues();
}

uint16_t moduleDHT::getVersion() {
	String SVN = "$Rev: 121 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleDHT::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugDHT, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugDHT\",\"value\":" + String(Debug ? "true" : "false") + "}");
	wpFZ.blink();
}

void moduleDHT::publishSettings() {
	publishSettings(false);
}
void moduleDHT::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicMaxCycle.c_str(), String(maxCycle).c_str());
	wpMqtt.mqttClient.publish(mqttTopicTemperatureCorrection.c_str(), String(float(temperatureCorrection / 10)).c_str());
	wpMqtt.mqttClient.publish(mqttTopicHumidityCorrection.c_str(), String(float(humidityCorrection / 10)).c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void moduleDHT::publishValues() {
	publishValues(false);
}
void moduleDHT::publishValues(bool force) {
	if(force) {
		publishCountTemperature = wpFZ.publishQoS;
		publishCountHumidity = wpFZ.publishQoS;
		publishCountError = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
	}
	if(temperatureLast != temperature || ++publishCountTemperature > wpFZ.publishQoS) {
		publishValueTemp();
	}
	if(humidityLast != humidity || ++publishCountHumidity > wpFZ.publishQoS) {
		publishValueHum();
	}
	if(errorLast != error || ++publishCountError > wpFZ.publishQoS) {
		errorLast = error;
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
		publishCountError = 0;
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void moduleDHT::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTemperatureCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicHumidityCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}

void moduleDHT::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		byte readMaxCycle = msg.toInt();
		if(readMaxCycle <= 0) readMaxCycle = 1;
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			EEPROM.write(wpEEPROM.byteMaxCycleDHT, maxCycle);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicMaxCycle, String(maxCycle));
		}
	}
	if(strcmp(topic, mqttTopicTemperatureCorrection.c_str()) == 0) {
		int8_t readTemperatureCorrection = int8_t(msg.toFloat() * 10);
		if(temperatureCorrection != readTemperatureCorrection) {
			temperatureCorrection = readTemperatureCorrection;
			EEPROM.put(wpEEPROM.byteTemperatureCorrection, temperatureCorrection);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicTemperatureCorrection, String(temperatureCorrection));
		}
	}
	if(strcmp(topic, mqttTopicHumidityCorrection.c_str()) == 0) {
		int8_t readHumidityCorrection = int8_t(msg.toFloat() * 10);
		if(humidityCorrection != readHumidityCorrection) {
			humidityCorrection = readHumidityCorrection;
			EEPROM.put(wpEEPROM.byteHumidityCorrection, humidityCorrection);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicHumidityCorrection, String(humidityCorrection));
		}
	}
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugDHT, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
			wpFZ.SendWS("{\"id\":\"DebugDHT\",\"value\":" + String(Debug ? "true" : "false") + "}");
		}
	}
}

//###################################################################################
// private
//###################################################################################
void moduleDHT::publishValueTemp() {
	String sendTemperature = String(float(temperature / 100.0));
	wpMqtt.mqttClient.publish(mqttTopicTemperature.c_str(), sendTemperature.c_str());
	wpRest.error = wpRest.error | !wpRest.sendRest("temp", sendTemperature);
	wpRest.trySend = true;
	temperatureLast = temperature;
	if(wpMqtt.Debug) {
		printPublishValueDebug("Temperature", sendTemperature, String(publishCountTemperature));
	}
	publishCountTemperature = 0;
}

void moduleDHT::publishValueHum() {
	String sendHumidity = String(float(humidity / 100.0));
	wpMqtt.mqttClient.publish(mqttTopicHumidity.c_str(), sendHumidity.c_str());
	wpRest.error = wpRest.error | !wpRest.sendRest("hum", sendHumidity);
	wpRest.trySend = true;
	humidityLast = humidity;
	if(wpMqtt.Debug) {
		printPublishValueDebug("Humidity", sendHumidity, String(publishCountHumidity));
	}
	publishCountHumidity = 0;
}

void moduleDHT::calc() {
	bool e = false;
	float newT = dht.readTemperature();
	float newH = dht.readHumidity();
	if(!isnan(newT)) {
		temperature = int16_t(newT * 100) + temperatureCorrection;
		e = e | false;
		if(Debug) {
			printCalcDebug("Temperature", temperature, newT);
		}
	} else {
		e = e | true;
		printCalcError("Temperature");
	}
	if(!isnan(newH)) {
		humidity = newH + humidityCorrection;
		if(Debug) {
			e = e | false;
			printCalcDebug("Humidity", humidity, newH);
		}
	} else {
		e = e | true;
		printCalcError("Humidity");
	}
	error = e;
}

void moduleDHT::printCalcError(String name) {
	wpFZ.blink();
	String logmessage = name + ": Sensor Failure";
	wpFZ.DebugWS(wpFZ.strERRROR, "wpDHT::calc", logmessage);
}

void moduleDHT::printCalcDebug(String name, int16_t value, float raw) {
	String logmessage = name + ": " + String(value) + " (" + String(raw) + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "wpDHT::calc", logmessage);
}

void moduleDHT::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}
