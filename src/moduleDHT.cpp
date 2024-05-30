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
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 117 2024-05-29 01:28:02Z                  $ #
//#                                                                                 #
//###################################################################################
#include <moduleDHT.h>

moduleDHT::moduleDHT(PubSubClient mc, uint8_t wpHTtype) {
	if(wpFZ.installedModules[wpFZ.wpDHT11]) {
		wpHTtype = DHT11;
	}
	if(wpFZ.installedModules[wpFZ.wpDHT22]) {
		wpHTtype = DHT11;
	}
	dht = new DHT(DHTPin, wpHTtype);
	mqttClient = mc;
	// values
	mqttTopicTemperature = wpFZ.DeviceName + "/Temperature";
	mqttTopicHumidity = wpFZ.DeviceName + "/Humidity";
	mqttTopicErrorHT = wpFZ.DeviceName + "/ERROR/HT";
	// settings
	mqttTopicMaxCycleHT = wpFZ.DeviceName + "/settings/HT/maxCycle";
	mqttTopicTemperatureCorrection = wpFZ.DeviceName + "/settings/HT/Correction/Temperature";
	mqttTopicHumidityCorrection = wpFZ.DeviceName + "/settings/HT/Correction/Humidity";
	// commands
	mqttTopicDebugHT = wpFZ.DeviceName + "/settings/Debug/HT";

	temperatureCorrection = 0;
	humidityCorrection = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleDHT::loop() {
	if(++cycleHT >= wpFZ.maxCycleHT) {
		cycleHT = 0;
		calcHT();
	}
}

uint16_t moduleDHT::getVersion() {
	String SVN = "$Rev: 118 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleDHT::publishValueTemp(int equalVal) {
	mqttClient.publish(mqttTopicTemperature.c_str(), String(temperature).c_str());
	wpFZ.errorRest = wpFZ.errorRest | !wpFZ.sendRest("temp", String(temperature));
	wpFZ.trySendRest = true;
	temperatureLast = equalVal;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("Temperature", String(temperature), String(publishCountTemperature));
	}
	publishCountTemperature = 0;
}

void moduleDHT::publishValueHum(int equalVal) {
	mqttClient.publish(mqttTopicHumidity.c_str(), String(humidity).c_str());
	wpFZ.errorRest = wpFZ.errorRest | !wpFZ.sendRest("hum", String(humidity));
	wpFZ.trySendRest = true;
	humidityLast = equalVal;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("Humidity", String(humidity), String(publishCountHumidity));
	}
	publishCountHumidity = 0;
}

void moduleDHT::publishInfoDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}

void moduleDHT::publishErrorHT() {
	mqttClient.publish(mqttTopicErrorHT.c_str(), String(errorHT).c_str());
	errorHTLast = errorHT;
	publishCountErrorHT = 0;
}

//###################################################################################
// Allgemein
//###################################################################################
void moduleDHT::calcHT() {
	bool e = false;
	float newT = dht.readTemperature();
	float newH = dht.readHumidity();
	if(!isnan(newT)) {
		temperature = int16_t(newT * 100) + temperatureCorrection;
		e = e | false;
		if(wpFZ.DebugHT) {
			calcHTDebug("Temperature", temperature, newT);
		}
	} else {
		e = e | true;
		calcHTError("Temperature");
	}
	if(!isnan(newH)) {
		humidity = newH + humidityCorrection;
		if(wpFZ.DebugHT) {
			e = e | false;
			calcHTDebug("Humidity", humidity, newH);
		}
	} else {
		e = e | true;
		calcHTError("Humidity");
	}
	errorHT = e;
}

void moduleDHT::calcHTDebug(String name, int16_t value, float raw) {
	String logmessage = name + ": " + String(value) + " (" + String(raw) + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "calcHT", logmessage);
}

void moduleDHT::calcHTError(String name) {
	wpFZ.blink();
	String logmessage = name + ": Sensor Failure";
	wpFZ.DebugWS(wpFZ.strERRROR, "calcHT", logmessage);
}
