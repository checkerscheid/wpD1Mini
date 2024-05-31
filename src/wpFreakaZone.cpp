//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 08.03.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.cpp 117 2024-05-29 01:28:02Z                $ #
//#                                                                                 #
//###################################################################################
#include <wpFreakaZone.h>

wpFreakaZone wpFZ("BasisEmpty");

wpFreakaZone::wpFreakaZone(String deviceName) {
	MajorVersion = 3;
	MinorVersion = 0;
	configTime(TZ, NTP_SERVER);
	DeviceName = deviceName;
	DeviceDescription = deviceName;
	calcValues = true;
	choosenDHT = 0;
	if(wpDHT11) {
		choosenDHT = DHT11;
	}
	if(wpDHT22) {
		choosenDHT = DHT22;
	}

	// values
	mqttTopicRestartRequired = wpFZ.DeviceName + "/RestartRequired";
	// settings
	mqttTopicDeviceName = wpFZ.DeviceName + "/info/DeviceName";
	mqttTopicDeviceDescription = wpFZ.DeviceName + "/info/DeviceDescription";
	mqttTopicVersion = wpFZ.DeviceName + "/info/Version";
	mqttTopicOnSince = wpFZ.DeviceName + "/info/OnSince";
	mqttTopicOnDuration = wpFZ.DeviceName + "/info/OnDuration";
	// commands
	mqttTopicSetDeviceName = wpFZ.DeviceName + "/settings/DeviceName";
	mqttTopicSetDeviceDescription = wpFZ.DeviceName + "/settings/DeviceDescription";
	mqttTopicRestartDevice = wpFZ.DeviceName + "/RestartDevice";
	mqttTopicCalcValues = wpFZ.DeviceName + "/settings/calcValues";

	publishSettings();
	setSubscribes();
}

//###################################################################################
// public
//###################################################################################
void wpFreakaZone::loop() {
	OnDuration = getOnlineTime(false);
}

uint16_t wpFreakaZone::getVersion() {
	String SVN = "$Rev: 118 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

uint16_t wpFreakaZone::getBuild(String Rev) {
	Rev.remove(0, 6);
	Rev.remove(Rev.length() - 2, 2);
	uint16_t returns = Rev.toInt();
	return returns;
}

String wpFreakaZone::getTime() {
	time_t now;
	tm tm;
	time(&now);
	localtime_r(&now, &tm);
	String timeh = ((tm.tm_hour < 10) ? "0" : "") + String(tm.tm_hour);
	String timem = ((tm.tm_min < 10) ? "0" : "") + String(tm.tm_min);
	String times = ((tm.tm_sec < 10) ? "0" : "") + String(tm.tm_sec);
	String timeNow = timeh + ":" + timem + ":" + times;
	return "[" + timeNow + "]";
}

String wpFreakaZone::getDateTime() {
	time_t now;
	tm tm;
	time(&now);
	localtime_r(&now, &tm);
	String dated = ((tm.tm_mday < 10) ? "0" : "") + String(tm.tm_mday);
	String datem = ((tm.tm_mon < 9) ? "0" : "") + String(tm.tm_mon + 1);
	String datey = String(tm.tm_year + 1900);
	String timeh = ((tm.tm_hour < 10) ? "0" : "") + String(tm.tm_hour);
	String timem = ((tm.tm_min < 10) ? "0" : "") + String(tm.tm_min);
	String times = ((tm.tm_sec < 10) ? "0" : "") + String(tm.tm_sec);
	String datetimeNow = dated + "." + datem + "." + datey + " " + timeh + ":" + timem + ":" + times;
	return datetimeNow;
}

String wpFreakaZone::getOnlineTime() {
	return wpFreakaZone::getOnlineTime(true);
}

String wpFreakaZone::getOnlineTime(bool forDebug) {
	unsigned long secall = (unsigned long) millis() / 1000;
	unsigned long minohnesec = (unsigned long) round(secall / 60);
	byte sec = secall % 60;
	unsigned long h = (unsigned long) round(minohnesec / 60);
	byte min = minohnesec % 60;
	String msg = (h < 10 ? "0" + String(h) : String(h)) + ":" + (min < 10 ? "0" + String(min) : String(min)) + ":" + (sec < 10 ? "0" + String(sec) : String(sec));

	if(forDebug) {
		msg.remove(9);
		String e = "";
		for(int i = msg.length(); i < 9; i++) {
			e += ' ';
		}
		return "[" + e + msg + "]";
	} else {
		return msg;
	}
}

String wpFreakaZone::funcToString(String msg) {
	msg.remove(16);
	String e = "";
	for(int i = msg.length(); i < 16; i++) {
		e += ' ';
	}
	return "[" + msg + e + "] ";
}

void wpFreakaZone::blink() {
	int led = digitalRead(LED_BUILTIN);
	short blinkDelay = 50;
	for(int i = 0; i < 2; i++) {
		led = led == 0 ? 1 : 0;
		digitalWrite(LED_BUILTIN, led);
		delay(blinkDelay);
		led = led == 0 ? 1 : 0;
		digitalWrite(LED_BUILTIN, led);
		delay(blinkDelay);
	}
}

String wpFreakaZone::JsonKeyValue(String name, String value) {
	String message = "\"" + name + "\":" + value;
	if(value == "nan") {
		message = "\"" + name + "\":\"" + value + "\"";
	}
	return message;
}
String wpFreakaZone::JsonKeyString(String name, String value) {
	String message = "\"" + name + "\":\"" + value + "\"";
	return message;
}


void wpFreakaZone::checkSubscripes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetDeviceName.c_str()) == 0) {
		if(strcmp(msg.c_str(), wpFZ.DeviceName.c_str()) != 0) {
			wpFZ.DeviceName = msg;
			wpEEPROM.writeStringsToEEPROM();
			wpMqtt.mqttClient.publish(mqttTopicRestartRequired.c_str(), String(1).c_str());
			wpMqtt.mqttClient.publish(mqttTopicRestartDevice.c_str(), String(0).c_str());
			checkSubscripesDebug(mqttTopicDeviceName, wpFZ.DeviceName);
		}
	}
	if(strcmp(topic, mqttTopicSetDeviceDescription.c_str()) == 0) {
		if(strcmp(msg.c_str(), wpFZ.DeviceDescription.c_str()) != 0) {
			wpFZ.DeviceDescription = msg;
			wpEEPROM.writeStringsToEEPROM();
			checkSubscripesDebug(mqttTopicDeviceDescription, wpFZ.DeviceDescription);
		}
	}
	if(strcmp(topic, mqttTopicRestartDevice.c_str()) == 0) {
		int readRestartDevice = msg.toInt();
		if(readRestartDevice > 0) {
			wpOnlineToggler.setMqttOffline();
			checkSubscripesDebug(mqttTopicRestartDevice, String(readRestartDevice));
			ESP.restart();
		}
	}
	if(strcmp(topic, mqttTopicCalcValues.c_str()) == 0) {
		bool readCalcValues = msg.toInt();
		if(wpFZ.calcValues != readCalcValues) {
			wpFZ.calcValues = readCalcValues;
			checkSubscripesDebug(mqttTopicCalcValues, String(wpFZ.calcValues));
		}
	}
}

void wpFreakaZone::publishSettings() {
	wpMqtt.mqttClient.publish(mqttTopicDeviceName.c_str(), DeviceName.c_str());
	wpMqtt.mqttClient.publish(mqttTopicDeviceDescription.c_str(), DeviceDescription.c_str());
	wpMqtt.mqttClient.publish(mqttTopicVersion.c_str(), Version.c_str());
	wpMqtt.mqttClient.publish(mqttTopicOnSince.c_str(), OnSince.c_str());
}

void wpFreakaZone::publishValues() {
	wpMqtt.mqttClient.publish(mqttTopicOnDuration.c_str(), OnDuration.c_str());
}

//###################################################################################
// Debug Messages
//###################################################################################

void wpFreakaZone::DebugWS(String typ, String func, String msg) {
	DebugWS(typ, func, msg, true);
}
void wpFreakaZone::DebugWS(String typ, String func, String msg, bool newline) {
	String toSend = getTime() + getOnlineTime() + typ + funcToString(func) + msg;
	Serial.println(toSend);
	wpWebServer.webSocket.textAll("{\"msg\":\"" + toSend + "\",\"newline\":" + (newline ? "true" : "false") + "}");
}
void wpFreakaZone::SendWS(String msg) {
	wpWebServer.webSocket.textAll("{\"cmd\":\"setDebug\",\"msg\":" + msg + "}");
}

void wpFreakaZone::printStart() {
	Serial.println();
	Serial.print(getTime());
	Serial.print(getOnlineTime());
	Serial.print(strINFO);
	Serial.print(funcToString("StartDevice"));
	Serial.print("Startet as: '");
	Serial.print(DeviceName);
	Serial.println("'");
}

void wpFreakaZone::printRestored() {
	Serial.print(getTime());
	Serial.print(getOnlineTime());
	Serial.print(strINFO);
	Serial.print(funcToString("StartDevice"));
	Serial.print("Restored as: '");
	Serial.print(DeviceName);
	Serial.println("'");
	Serial.print(getTime());
	Serial.print(getOnlineTime());
	Serial.print(strINFO);
	Serial.print(funcToString("StartDevice"));
	Serial.println(getVersion());
}
//###################################################################################
// private
//###################################################################################
void wpFreakaZone::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetDeviceName.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetDeviceDescription.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicRestartDevice.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicCalcValues.c_str());
}

void wpFreakaZone::checkSubscripesDebug(String topic, String value) {
	String logmessage =  "Setting change found on topic: '" + topic + "': " + value;
	wpFZ.DebugWS(wpFZ.strINFO, "checkSubscripes", logmessage);
	wpFZ.blink();
}
