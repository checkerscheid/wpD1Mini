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
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleDHT.cpp 120 2024-05-31 03:32:41Z                   $ #
//#                                                                                 #
//###################################################################################
#include <moduleDHT.h>

moduleDHT wpDHT;
DHT dht(DHTPin, wpFZ.choosenDHT);

moduleDHT::moduleDHT() {}
void moduleDHT::init() {
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Init");
#endif
	// values
	mqttTopicTemperature = wpFZ.DeviceName + "/Temperature";
	mqttTopicHumidity = wpFZ.DeviceName + "/Humidity";
	mqttTopicErrorHT = wpFZ.DeviceName + "/ERROR/HT";
	// settings
	mqttTopicMaxCycle = wpFZ.DeviceName + "/settings/HT/maxCycle";
	mqttTopicTemperatureCorrection = wpFZ.DeviceName + "/settings/HT/Correction/Temperature";
	mqttTopicHumidityCorrection = wpFZ.DeviceName + "/settings/HT/Correction/Humidity";
	// commands
	mqttTopicDebugHT = wpFZ.DeviceName + "/settings/Debug/HT";

	temperatureCorrection = 0;
	humidityCorrection = 0;

	dht.begin();
	publishSettings();
	setSubscribes();	
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Inited");
#endif
}

//###################################################################################
// public
//###################################################################################
void moduleDHT::cycle() {
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycle");
#endif
	if(wpFZ.calcValues && ++cycleHT >= maxCycle) {
		cycleHT = 0;
		calcHT();
	}
	publishValues();
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycled");
#endif
}

uint16_t moduleDHT::getVersion() {
	String SVN = "$Rev: 120 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleDHT::changeDebug() {
	DebugHT = !DebugHT;
	bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugHT, DebugHT);
	EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugHT\",\"value\":" + String(DebugHT ? "true" : "false") + "}");
	wpFZ.blink();
}

void moduleDHT::checkSubscripes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicMaxCycle.c_str()) == 0) {
		byte readMaxCycle = msg.toInt();
		if(readMaxCycle <= 0) readMaxCycle = 1;
		if(maxCycle != readMaxCycle) {
			maxCycle = readMaxCycle;
			EEPROM.write(wpEEPROM.byteMaxCycleHT, maxCycle);
			EEPROM.commit();
			checkSubscripesDebug(mqttTopicMaxCycle, String(maxCycle));
		}
	}
	if(strcmp(topic, mqttTopicTemperatureCorrection.c_str()) == 0) {
		int8_t readTemperatureCorrection = int8_t(msg.toFloat() * 10);
		if(temperatureCorrection != readTemperatureCorrection) {
			temperatureCorrection = readTemperatureCorrection;
			EEPROM.put(wpEEPROM.byteTemperatureCorrection, temperatureCorrection);
			EEPROM.commit();
			checkSubscripesDebug(mqttTopicTemperatureCorrection, String(temperatureCorrection));
		}
	}
	if(strcmp(topic, mqttTopicHumidityCorrection.c_str()) == 0) {
		int8_t readHumidityCorrection = int8_t(msg.toFloat() * 10);
		if(humidityCorrection != readHumidityCorrection) {
			humidityCorrection = readHumidityCorrection;
			EEPROM.put(wpEEPROM.byteHumidityCorrection, humidityCorrection);
			EEPROM.commit();
			checkSubscripesDebug(mqttTopicHumidityCorrection, String(humidityCorrection));
		}
	}
	if(strcmp(topic, mqttTopicDebugHT.c_str()) == 0) {
		bool readDebugHT = msg.toInt();
		if(DebugHT != readDebugHT) {
			DebugHT = readDebugHT;
			bitWrite(wpEEPROM.bitsDebugModules, wpEEPROM.bitDebugHT, DebugHT);
			EEPROM.write(wpEEPROM.addrBitsDebugModules, wpEEPROM.bitsDebugModules);
			EEPROM.commit();
			checkSubscripesDebug(mqttTopicDebugHT, String(DebugHT));
			wpFZ.SendWS("{\"id\":\"DebugHT\",\"value\":" + String(DebugHT ? "true" : "false") + "}");
		}
	}
}

void moduleDHT::publishSettings() {
	wpMqtt.mqttClient.publish(mqttTopicMaxCycle.c_str(), String(maxCycle).c_str());
	wpMqtt.mqttClient.publish(mqttTopicTemperatureCorrection.c_str(), String(float(temperatureCorrection / 10)).c_str());
	wpMqtt.mqttClient.publish(mqttTopicHumidityCorrection.c_str(), String(float(humidityCorrection / 10)).c_str());
	wpMqtt.mqttClient.publish(mqttTopicDebugHT.c_str(), String(DebugHT).c_str());
	wpMqtt.mqttClient.publish(mqttTopicErrorHT.c_str(), String(errorHT).c_str());
}

void moduleDHT::publishValues() {
	publishValues(false);
}
void moduleDHT::publishValues(bool force) {
	if(force) {
		publishCountTemperature  = wpFZ.publishQoS;
		publishCountHumidity = wpFZ.publishQoS;
		publishCountErrorHT = wpFZ.publishQoS;
	}
	if(temperatureLast != temperature || ++publishCountTemperature > wpFZ.publishQoS) {
		publishValueTemp();
	}
	if(humidityLast != humidity || ++publishCountHumidity > wpFZ.publishQoS) {
		publishValueHum();
	}
	if(errorHTLast != errorHT || ++publishCountErrorHT > wpFZ.publishQoS) {
		publishErrorHT();
	}
}

//###################################################################################
// private
//###################################################################################
void moduleDHT::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicMaxCycle.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTemperatureCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicHumidityCorrection.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebugHT.c_str());
}

void moduleDHT::publishValueTemp() {
	String sendTemperature = String(float(temperature / 100.0));
	wpMqtt.mqttClient.publish(mqttTopicTemperature.c_str(), sendTemperature.c_str());
	wpRest.errorRest = wpRest.errorRest | !wpRest.sendRest("temp", sendTemperature);
	wpRest.trySendRest = true;
	temperatureLast = temperature;
	if(wpMqtt.DebugMqtt) {
		publishInfoDebug("Temperature", sendTemperature, String(publishCountTemperature));
	}
	publishCountTemperature = 0;
}

void moduleDHT::publishValueHum() {
	String sendHumidity = String(float(humidity / 100.0));
	wpMqtt.mqttClient.publish(mqttTopicHumidity.c_str(), sendHumidity.c_str());
	wpRest.errorRest = wpRest.errorRest | !wpRest.sendRest("hum", sendHumidity);
	wpRest.trySendRest = true;
	humidityLast = humidity;
	if(wpMqtt.DebugMqtt) {
		publishInfoDebug("Humidity", sendHumidity, String(publishCountHumidity));
	}
	publishCountHumidity = 0;
}

void moduleDHT::publishErrorHT() {
	wpMqtt.mqttClient.publish(mqttTopicErrorHT.c_str(), String(errorHT).c_str());
	errorHTLast = errorHT;
	publishCountErrorHT = 0;
}

void moduleDHT::calcHT() {
	bool e = false;
	float newT = dht.readTemperature();
	float newH = dht.readHumidity();
	if(!isnan(newT)) {
		temperature = int16_t(newT * 100) + temperatureCorrection;
		e = e | false;
		if(DebugHT) {
			calcHTDebug("Temperature", temperature, newT);
		}
	} else {
		e = e | true;
		calcHTError("Temperature");
	}
	if(!isnan(newH)) {
		humidity = newH + humidityCorrection;
		if(DebugHT) {
			e = e | false;
			calcHTDebug("Humidity", humidity, newH);
		}
	} else {
		e = e | true;
		calcHTError("Humidity");
	}
	errorHT = e;
}

void moduleDHT::calcHTError(String name) {
	wpFZ.blink();
	String logmessage = name + ": Sensor Failure";
	wpFZ.DebugWS(wpFZ.strERRROR, "calcHT", logmessage);
}

void moduleDHT::publishInfoDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}

void moduleDHT::checkSubscripesDebug(String topic, String value) {
	String logmessage =  "Setting change found on topic: '" + topic + "': " + value;
	wpFZ.DebugWS(wpFZ.strINFO, "checkSubscripes", logmessage);
	wpFZ.blink();
}

void moduleDHT::calcHTDebug(String name, int16_t value, float raw) {
	String logmessage = name + ": " + String(value) + " (" + String(raw) + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "calcHT", logmessage);
}
