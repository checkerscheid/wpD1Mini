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
//# Revision     : $Rev:: 163                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.cpp 163 2024-07-14 19:03:20Z                $ #
//#                                                                                 #
//###################################################################################
#include <wpFreakaZone.h>

wpFreakaZone wpFZ;
wpFreakaZone::wpFreakaZone() {}

void wpFreakaZone::init(String deviceName) {
	configTime(TZ, NTP_SERVER);
	calcValues = true;
	restartRequired = false;

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

	publishCountCalcValues = 0;
	publishCountOnDuration = 0;
	restartRequiredLast = false;
	publishCountRestartRequired = 0;
}

//###################################################################################
// public
//###################################################################################
void wpFreakaZone::cycle() {
	OnDuration = getOnlineTime(false);
	publishValues();
}

uint16 wpFreakaZone::getVersion() {
	String SVN = "$Rev: 163 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

uint16 wpFreakaZone::getBuild(String Rev) {
	Rev.remove(0, 6);
	Rev.remove(Rev.length() - 2, 2);
	uint16 returns = Rev.toInt();
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

void wpFreakaZone::publishSettings() {
	publishSettings(false);
}
void wpFreakaZone::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicDeviceName.c_str(), DeviceName.c_str());
	wpMqtt.mqttClient.publish(mqttTopicDeviceDescription.c_str(), DeviceDescription.c_str());
	wpMqtt.mqttClient.publish(mqttTopicVersion.c_str(), Version.c_str());
	wpMqtt.mqttClient.publish(mqttTopicOnSince.c_str(), OnSince.c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetDeviceName.c_str(), String(DeviceName).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetDeviceDescription.c_str(), String(DeviceDescription).c_str());
		wpMqtt.mqttClient.publish(mqttTopicRestartRequired.c_str(), String(restartRequired).c_str());
		wpMqtt.mqttClient.publish(mqttTopicRestartDevice.c_str(), String(0).c_str());
	}
}

void wpFreakaZone::publishValues() {
	publishValues(false);
}

void wpFreakaZone::publishValues(bool force) {
	if(force) publishCountOnDuration = minute2;
	if(++publishCountOnDuration > minute2) {
		wpMqtt.mqttClient.publish(mqttTopicOnDuration.c_str(), OnDuration.c_str());
		publishCountOnDuration = 0;
	}
	if(calcValuesLast != calcValues || publishCountCalcValues > wpFZ.publishQoS) {
		calcValuesLast = calcValues;
		wpMqtt.mqttClient.publish(mqttTopicCalcValues.c_str(), String(calcValues).c_str());
		wpFZ.SendWSDebug("CalcValues", wpFZ.calcValues);
		publishCountCalcValues = 0;
	}
	if(restartRequired) {
		if(!restartRequiredLast) {
			wpMqtt.mqttClient.publish(mqttTopicRestartRequired.c_str(), String(restartRequired).c_str());
			wpMqtt.mqttClient.publish(wpFZ.mqttTopicRestartDevice.c_str(), String(0).c_str());
			SendRestartRequired("true");
			restartRequiredLast = restartRequired;
		}
		if(restartRequiredLast && ++publishCountRestartRequired > sekunde10) {
			wpMqtt.mqttClient.publish(mqttTopicRestartRequired.c_str(), String(restartRequired).c_str());
			SendRestartRequired("true");
			publishCountRestartRequired = 0;
		}
	}
}

void wpFreakaZone::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetDeviceName.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetDeviceDescription.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicRestartDevice.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicCalcValues.c_str());
}

void wpFreakaZone::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, wpFZ.mqttTopicSetDeviceName.c_str()) == 0) {
		if(wpFZ.DeviceName != msg) {
			wpFZ.DeviceName = msg;
			wpEEPROM.writeStringsToEEPROM();
			wpFZ.restartRequired = true;
			DebugcheckSubscribes(wpFZ.mqttTopicDeviceName, wpFZ.DeviceName);
		}
	}
	if(strcmp(topic, wpFZ.mqttTopicSetDeviceDescription.c_str()) == 0) {
		if(wpFZ.DeviceDescription != msg) {
			wpFZ.DeviceDescription = msg;
			wpEEPROM.writeStringsToEEPROM();
			DebugcheckSubscribes(mqttTopicDeviceDescription, wpFZ.DeviceDescription);
		}
	}
	if(strcmp(topic, wpFZ.mqttTopicRestartDevice.c_str()) == 0) {
		int readRestartDevice = msg.toInt();
		if(readRestartDevice > 0) {
			wpOnlineToggler.setMqttOffline();
			DebugcheckSubscribes(mqttTopicRestartDevice, String(readRestartDevice));
			ESP.restart();
		}
	}
	if(strcmp(topic, wpFZ.mqttTopicCalcValues.c_str()) == 0) {
		bool readCalcValues = msg.toInt();
		if(wpFZ.calcValues != readCalcValues) {
			wpFZ.calcValues = readCalcValues;
			DebugcheckSubscribes(wpFZ.mqttTopicCalcValues, String(wpFZ.calcValues));
		}
	}
}

//###################################################################################
// Debug Messages
//###################################################################################

void wpFreakaZone::DebugWS(String typ, String func, String msg) {
	String msgheader = getTime() + getOnlineTime() + typ + funcToString(func);
	String cssClass = "color-debug";
	if(typ == wpFZ.strINFO) cssClass = "color-info";
	if(typ == wpFZ.strWARN) cssClass = "color-warn";
	if(typ == wpFZ.strERRROR) cssClass = "color-error";
	String toSend = msgheader + msg;
	Serial.println(toSend);
	wpWebServer.webSocket.textAll("{\"msgheader\":\"" + msgheader + "\"," +
		"\"msgbody\":\"" + msg + "\",\"cssClass\":\"" + cssClass + "\"}");
}
// void wpFreakaZone::SendWS(String msg) {
// 	wpWebServer.webSocket.textAll(msg);
// }
void wpFreakaZone::SendWSModule(String htmlId, bool value) {
	String msg = "{\"id\":\"" + htmlId + "\",\"value\":" + (value ? "true" : "false") + "}";
	wpWebServer.webSocket.textAll("{\"cmd\":\"setModule\",\"msg\":" + msg + "}");
}
void wpFreakaZone::SendWSSendRest(String htmlId, bool value) {
	String msg = "{\"id\":\"" + htmlId + "\",\"value\":" + (value ? "true" : "false") + "}";
	wpWebServer.webSocket.textAll("{\"cmd\":\"setSendRest\",\"msg\":" + msg + "}");
}
void wpFreakaZone::SendWSDebug(String htmlId, bool value) {
	String msg = "{\"id\":\"" + htmlId + "\",\"value\":" + (value ? "true" : "false") + "}";
	wpWebServer.webSocket.textAll("{\"cmd\":\"setDebug\",\"msg\":" + msg + "}");
}
void wpFreakaZone::SendRestartRequired(String msg) {
	wpWebServer.webSocket.textAll("{\"cmd\":\"restartRequired\",\"msg\":" + msg + "}");
}
void wpFreakaZone::SendRemainPumpInPause(String readableTime) {
	wpWebServer.webSocket.textAll("{\"cmd\":\"remainPumpInPause\",\"msg\":\"" + readableTime + "\"}");
}
void wpFreakaZone::SendPumpStatus(String pumpStatus) {
	wpWebServer.webSocket.textAll("{\"cmd\":\"pumpStatus\",\"msg\":{" + pumpStatus + "}}");
}
void wpFreakaZone::pumpCycleFinished() {
	wpWebServer.webSocket.textAll("{\"cmd\":\"pumpCycleFinished\"}");
}
void wpFreakaZone::updateProgress(int percent) {
	wpWebServer.webSocket.textAll("{\"cmd\":\"updateProgress\",\"percent\":\"" + String(percent) + " %\"}");
}
void wpFreakaZone::DebugcheckSubscribes(String topic, String value) {
	String logmessage =  "Setting change found on topic: '" + topic + "': " + value;
	wpFZ.DebugWS(wpFZ.strINFO, "checkSubscripes", logmessage);
	wpFZ.blink();
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
	Serial.println(Version);
}
//###################################################################################
// private
//###################################################################################
