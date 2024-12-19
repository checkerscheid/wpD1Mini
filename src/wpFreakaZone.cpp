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
//# Revision     : $Rev:: 227                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.cpp 227 2024-12-03 08:19:05Z                $ #
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

	loopStartedAt = millis();
	publishCalcValuesLast = 0;
	publishOnDurationLast = 0;
	restartRequiredLast = false;
	publishRestartRequiredLast = 0;
	blinkStatus = blinkStatusNothing;
	blinkStatsusLast = 0;
	blinkDelay = 35;
}

//###################################################################################
// public
//###################################################################################
void wpFreakaZone::cycle() {
	OnDuration = getOnlineTime(false);
	publishValues();
	doBlink();
}

uint16 wpFreakaZone::getVersion() {
	String SVN = "$Rev: 227 $";
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

void wpFreakaZone::getTime(uint8 &h, uint8 &m, uint8 &s) {
	time_t now;
	tm tm;
	time(&now);
	localtime_r(&now, &tm);
	h = tm.tm_hour;
	m = tm.tm_min;
	s = tm.tm_sec;
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
	blinkStatus = blinkStatusStart;
}
bool wpFreakaZone::blinking() {
	return blinkStatus != blinkStatusNothing;
}
long wpFreakaZone::Map(long in, long inMin, long inMax, long outMin, long outMax, bool useMin, bool useMax) {
	if(inMax - inMin == 0) {
		DebugWS(strERRROR, "Map", F("risky math operation: 'inMax - inMin == 0'"));
		return 0;
	}
	long returns = map(in, inMin, inMax, outMin, outMax);
	if(useMin && returns < outMin) returns = outMin;
	if(useMax && returns > outMax) returns = outMax;
	return returns;
}
long wpFreakaZone::Map(long in, long inMin, long inMax, long outMin, long outMax) {
	return Map(in, inMin, inMax, outMin, outMax, true, true);
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
	if(force) {
		publishOnDurationLast = 0;
		publishCalcValuesLast = 0;
	}
	if(publishOnDurationLast == 0 || loopStartedAt > publishOnDurationLast + publishQoS) {
		wpMqtt.mqttClient.publish(mqttTopicOnDuration.c_str(), OnDuration.c_str());
		publishOnDurationLast = loopStartedAt;
	}
	if(calcValuesLast != calcValues || publishCalcValuesLast == 0 ||
		loopStartedAt > publishCalcValuesLast + publishQoS) {
		calcValuesLast = calcValues;
		wpMqtt.mqttClient.publish(mqttTopicCalcValues.c_str(), String(calcValues).c_str());
		wpFZ.SendWSDebug(F("CalcValues"), wpFZ.calcValues);
		publishCalcValuesLast = loopStartedAt;
	}
	if(restartRequired) {
		if(!restartRequiredLast) {
			wpMqtt.mqttClient.publish(mqttTopicRestartRequired.c_str(), String(restartRequired).c_str());
			wpMqtt.mqttClient.publish(wpFZ.mqttTopicRestartDevice.c_str(), String(0).c_str());
			SendRestartRequired(F("true"));
			restartRequiredLast = restartRequired;
		}
		if(restartRequiredLast && loopStartedAt > publishRestartRequiredLast + sekunde10) {
			wpMqtt.mqttClient.publish(mqttTopicRestartRequired.c_str(), String(restartRequired).c_str());
			SendRestartRequired(F("true"));
			publishRestartRequiredLast = loopStartedAt;
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
			SetDeviceName(msg);
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

bool wpFreakaZone::CheckQoS(unsigned long lastSend) {
	if(lastSend == 0) return true;
	if(loopStartedAt > lastSend + publishQoS) return true;
	return false;
}

// return true on success
bool wpFreakaZone::sendRawRest(String target) {
	bool returns = false;
	WiFiClient client;
	HTTPClient http;
	String logmessage = "HTTP: '" + target + "'";
	wpFZ.DebugWS(wpFZ.strINFO, "sendRawRest", logmessage);
	http.begin(client, target.c_str());
	int httpResponseCode = http.GET();
	if (httpResponseCode > 0) {
		String payload = http.getString();
		payload.replace("\"", "'");
		String logmessage = "HTTP Response (" + String(httpResponseCode) + "): " + payload;
		wpFZ.DebugWS(wpFZ.strINFO, "sendRawRest", logmessage);
		returns = true;
	} else {
		String logmessage = "HTTP Response (" + String(httpResponseCode) + ")";
		wpFZ.DebugWS(wpFZ.strERRROR, "sendRawRest", logmessage);
	}
	http.end();
	return returns;
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
	DebugWS(strDEBUG, "sendUseModul", "send ws: id = " + htmlId + ", useModul = " + value);
}
void wpFreakaZone::SendWSDebug(String htmlId, bool value) {
	String msg = "{\"id\":\"" + htmlId + "\",\"value\":" + (value ? "true" : "false") + "}";
	wpWebServer.webSocket.textAll("{\"cmd\":\"setDebug\",\"msg\":" + msg + "}");
	DebugWS(strDEBUG, "sendDebug", "send ws: id = " + htmlId + ", debug = " + value);
}
void wpFreakaZone::SendRestartRequired(String msg) {
	wpWebServer.webSocket.textAll("{\"cmd\":\"restartRequired\",\"msg\":" + msg + "}");
}
void wpFreakaZone::SendNewVersion(bool isnew) {
	String msg = "{"
		"\"installedVersion\":\"" + wpUpdate.installedVersion + "\","
		"\"serverVersion\":\"" + wpUpdate.serverVersion + "\","
		"\"newVersion\":" + (isnew ? "true" : "false") + 
		"}";
	wpWebServer.webSocket.textAll("{\"cmd\":\"newVersion\",\"msg\":" + msg + "}");
}
void wpFreakaZone::SendRemainPumpInPause(String readableTime) {
	wpWebServer.webSocket.textAll("{\"cmd\":\"remainPumpInPause\",\"msg\":\"" + readableTime + "\"}");
}
void wpFreakaZone::SendPumpStatus(String pumpStatus) {
	wpWebServer.webSocket.textAll("{\"cmd\":\"pumpStatus\",\"msg\":{" + pumpStatus + "}}");
}
void wpFreakaZone::pumpCycleFinished() {
	wpWebServer.webSocket.textAll(F("{\"cmd\":\"pumpCycleFinished\"}"));
}
void wpFreakaZone::updateProgress(int percent) {
	wpWebServer.webSocket.textAll("{\"cmd\":\"updateProgress\",\"percent\":\"" + String(percent) + " %\"}");
}
void wpFreakaZone::DebugcheckSubscribes(String topic, String value) {
	String logmessage =  "Setting change found on topic: '" + topic + "': " + value;
	wpFZ.DebugWS(wpFZ.strINFO, F("checkSubscripes"), logmessage);
	wpFZ.blink();
}

void wpFreakaZone::SetDeviceName(String name) {
	wpFZ.DeviceName = name;
	wpEEPROM.writeStringsToEEPROM();
	wpFZ.restartRequired = true;
	DebugcheckSubscribes(wpFZ.mqttTopicDeviceName, wpFZ.DeviceName);
}
void wpFreakaZone::SetDeviceDescription(String description) {
	wpFZ.DeviceDescription = description;
	wpEEPROM.writeStringsToEEPROM();
	DebugcheckSubscribes(wpFZ.mqttTopicDeviceDescription, wpFZ.DeviceDescription);
}

void wpFreakaZone::printStart() {
	Serial.println();
	Serial.print(getTime());
	Serial.print(getOnlineTime());
	Serial.print(strINFO);
	Serial.print(funcToString("StartDevice"));
	Serial.print(F("Startet as: '"));
	Serial.print(DeviceName);
	Serial.println(F("'"));
}

void wpFreakaZone::printRestored() {
	Serial.print(getTime());
	Serial.print(getOnlineTime());
	Serial.print(strINFO);
	Serial.print(funcToString("StartDevice"));
	Serial.print(F("Restored as: '"));
	Serial.print(DeviceName);
	Serial.println(F("'"));
	Serial.print(getTime());
	Serial.print(getOnlineTime());
	Serial.print(strINFO);
	Serial.print(funcToString("StartDevice"));
	Serial.println(Version);
}
void wpFreakaZone::InitBootCounter(uint32 bc) {
	bootCounter = bc;
}
void wpFreakaZone::BootCount() {
	bootCounter++;
	WriteBootCounter();
}
uint32 wpFreakaZone::GetBootCounter() {
	return bootCounter;
}
void wpFreakaZone::ResetBootCounter() {
	bootCounter = 0;
	WriteBootCounter();
}
//###################################################################################
// private
//###################################################################################
void wpFreakaZone::WriteBootCounter() {
	EEPROM.put(wpEEPROM.addrBootCounter, bootCounter);
	DebugWS(strINFO, F("WriteBootCounter"), F("new bootCounter: ") + String(bootCounter));
}
void wpFreakaZone::doBlink() {
	if(blinkStatus != blinkStatusNothing && loopStartedAt > (blinkStatsusLast + blinkDelay)) {
		blinkStatus++;
		int led = digitalRead(LED_BUILTIN) == 0 ? 1 : 0;
		digitalWrite(LED_BUILTIN, led);
		blinkStatsusLast = loopStartedAt;
		if(blinkStatus > 10) blinkStatus = blinkStatusNothing;
	}
}
