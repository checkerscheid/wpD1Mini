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

#ifdef wpLDR
	DebugLDR = false;
	useLdrAvg = false;
	maxCycleLDR = 4;
	ldrCorrection = 0;
#endif
#ifdef wpLight
	DebugLight = false;
	useLightAvg = false;
	maxCycleLight = 4;
	lightCorrection = 0;
#endif
#ifdef wpBM
	DebugBM = false;
	#ifdef wpLDR
	threshold = 600;
	lightToTurnOn = "";
	#endif
#endif
#ifdef wpRelais
	DebugRelais = false;
	relaisHand = false;
	relaisHandValue = false;
	#ifdef wpMoisture
	waterEmpty = false;
	pumpActive = 2; // sekunden
	pumpPause = 5 * 60; // 5 minutes
	#endif
#endif
#ifdef wpRain
	DebugRain = false;
	useRainAvg = false;
	maxCycleRain = 4;
	rainCorrection = 0;
#endif
#ifdef wpMoisture
	DebugMoisture = false;
	useMoistureAvg = false;
	maxCycleMoisture = 4;
	moistureMin = 30;
	moistureDry = 1023;
	moistureWet = 0;
#endif
#ifdef wpDistance
	DebugDistance = false;
	maxCycleDistance = 4;
	distanceCorrection = 0;
	maxVolume = 4000;
	height = 120;
#endif
}

//###################################################################################
// public
//###################################################################################
void wpFreakaZone::loop() {
	doTheWebServerDebugChange();
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
#ifdef oldWebserver
void wpFreakaZone::setupWebServer() {
	server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
		String message = "{\"FreakaZoneDevice\":{";
		message += JsonKeyString("DeviceName", wpFZ.DeviceName) + ",";
		message += JsonKeyString("DeviceDescription", wpFZ.DeviceDescription) + ",";
		message += JsonKeyString("wpFreakaZoneVersion", wpFZ.getVersion()) + ",";
		message += JsonKeyString("Version", wpFZ.MainVersion) + ",";
		String minimac = WiFi.macAddress();
		minimac.replace(":", "");
		minimac.toLowerCase();
		message += JsonKeyString("MAC", WiFi.macAddress()) + ",";
		message += JsonKeyString("miniMAC", minimac) + ",";
		message += JsonKeyValue("UpdateMode", wpFZ.UpdateFW ? "true" : "false") + ",";
		message += JsonKeyValue("calcValues", wpFZ.calcValues ? "true" : "false") + ",";
#ifdef wpHT
		message += "\"HT\":{";
		message += JsonKeyValue("MaxCycleHT", String(wpFZ.maxCycleHT)) + ",";
		message += JsonKeyValue("TemperatureCorrection", String(wpFZ.temperatureCorrection)) + ",";
		message += JsonKeyValue("HumidityCorrection", String(wpFZ.humidityCorrection));
		message += "},";
#endif
#ifdef wpLDR
		message += "\"LDR\":{";
		message += JsonKeyValue("MaxCycleLDR", String(wpFZ.maxCycleLDR)) + ",";
		message += JsonKeyValue("useLDRAvg", String(wpFZ.useLdrAvg)) + ",";
		message += JsonKeyValue("LDRCorrection", String(wpFZ.ldrCorrection));
		message += "},";
#endif
#ifdef wpLight
		message += "\"Light\":{";
		message += JsonKeyValue("MaxCycleLight", String(wpFZ.maxCycleLight)) + ",";
		message += JsonKeyValue("useLightAvg", String(wpFZ.useLightAvg)) + ",";
		message += JsonKeyValue("LightCorrection", String(wpFZ.lightCorrection));
		message += "},";
#endif
#ifdef wpBM
		message += "\"BM\":{";
	#ifdef wpLDR
		message += "\"LDR\":{";
		message += JsonKeyValue("Threshold", String(wpFZ.threshold)) + ",";
		message += JsonKeyString("LightToTurnOn", wpFZ.lightToTurnOn);
		message += "}";
	#endif
		message += "},";
#endif
#ifdef wpRelais
		message += "\"Relais\":{";
		message += JsonKeyValue("Hand", wpFZ.relaisHand ? "true" : "false") + ",";
		message += JsonKeyValue("HandValue", wpFZ.relaisHandValue ? "true" : "false");
	#ifdef wpMoisture
		message += ",\"Moisture\":{";
		message += JsonKeyValue("waterEmpty", wpFZ.waterEmpty ? "true" : "false") + ",";
		message += JsonKeyValue("pumpActive", String(wpFZ.pumpActive)) + ",";
		message += JsonKeyValue("pumpPause", String(wpFZ.pumpPause));
		message += "}";
	#endif
		message += "},";
#endif
#ifdef wpRain
		message += "\"Rain\":{";
		message += JsonKeyValue("MaxCycleRain", String(wpFZ.maxCycleRain)) + ",";
		message += JsonKeyValue("useRainAvg", String(wpFZ.useRainAvg)) + ",";
		message += JsonKeyValue("RainCorrection", String(wpFZ.rainCorrection));
		message += "},";
#endif
#ifdef wpMoisture
		message += "\"Moisture\":{";
		message += JsonKeyValue("MaxCycleMoisture", String(wpFZ.maxCycleMoisture)) + ",";
		message += JsonKeyValue("useMoistureAvg", String(wpFZ.useMoistureAvg)) + ",";
		message += JsonKeyValue("MoistureMin", String(wpFZ.moistureMin)) + ",";
		message += JsonKeyValue("MoistureDry", String(wpFZ.moistureDry)) + ",";
		message += JsonKeyValue("MoistureWet", String(wpFZ.moistureWet));
		message += "},";
#endif
#ifdef wpDistance
		message += "\"Distance\":{";
		message += JsonKeyValue("MaxCycleDistance", String(wpFZ.maxCycleDistance)) + ",";
		message += JsonKeyValue("distanceCorrection", String(wpFZ.distanceCorrection)) + ",";
		message += JsonKeyValue("maxVolume", String(wpFZ.maxVolume)) + ",";
		message += JsonKeyValue("height", String(wpFZ.height));
		message += "},";
#endif
		message += "\"Debug\":{";
		message += JsonKeyValue("EEPROM", wpFZ.DebugEprom ? "true" : "false") + ",";
		message += JsonKeyValue("WiFi", wpFZ.DebugWiFi ? "true" : "false") + ",";
		message += JsonKeyValue("MQTT", wpFZ.DebugMqtt ? "true" : "false") + ",";
		message += JsonKeyValue("Search", wpFZ.DebugFinder ? "true" : "false") + ",";
		message += JsonKeyValue("Rest", wpFZ.DebugRest ? "true" : "false");
#ifdef wpHT
	if(wpHT == DHT11) {
		message += "," + JsonKeyValue("HT11", wpFZ.DebugHT ? "true" : "false");
	} else if(wpHT == DHT22) {
		message += "," + JsonKeyValue("HT22", wpFZ.DebugHT ? "true" : "false");
	} else {
		message += "," + JsonKeyValue("HT", wpFZ.DebugHT ? "true" : "false");
	}
#endif
#ifdef wpLDR
		message += "," + JsonKeyValue("LDR", wpFZ.DebugLDR ? "true" : "false");
#endif
#ifdef wpLight
		message += "," + JsonKeyValue("Light", wpFZ.DebugLight ? "true" : "false");
#endif
#ifdef wpBM
		message += "," + JsonKeyValue("BM", wpFZ.DebugBM ? "true" : "false");
#endif
#ifdef wpRelais
		message += "," + JsonKeyValue("Relais", wpFZ.DebugRelais ? "true" : "false");
#endif
#ifdef wpRain
		message += "," + JsonKeyValue("Rain", wpFZ.DebugRain ? "true" : "false");
#endif
#ifdef wpMoisture
		message += "," + JsonKeyValue("Moisture", wpFZ.DebugMoisture ? "true" : "false");
#endif
#ifdef wpDistance
		message += "," + JsonKeyValue("Distance", wpFZ.DebugDistance ? "true" : "false");
#endif
		message += "}}}";
		request->send(200, "application/json", message.c_str());
	});
}

void wpFreakaZone::setWebServerDebugChange(String DebugPlugIn) {
	doWebServerDebugChange = DebugPlugIn;
}
void wpFreakaZone::doTheWebServerDebugChange() {
	if(doWebServerDebugChange != "") {
		if(doWebServerDebugChange == "DebugEprom") {
			wpFZ.DebugEprom = !wpFZ.DebugEprom;
			bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugEprom, wpFZ.DebugEprom);
			EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugEprom");
			wpFZ.blink();
		}
		if(doWebServerDebugChange == "DebugWiFi") {
			wpFZ.DebugWiFi = !wpFZ.DebugWiFi;
			bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugWiFi, wpFZ.DebugWiFi);
			EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugWiFi");
			wpFZ.blink();
		}
		if(doWebServerDebugChange == "DebugMqtt") {
			wpFZ.DebugMqtt = !wpFZ.DebugMqtt;
			bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugMqtt, wpFZ.DebugMqtt);
			EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugMqtt");
			wpFZ.blink();
		}
		if(doWebServerDebugChange == "DebugFinder") {
			wpFZ.DebugFinder = !wpFZ.DebugFinder;
			bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugFinder, wpFZ.DebugFinder);
			EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugFinder");
			wpFZ.blink();
		}
		if(doWebServerDebugChange == "DebugRest") {
			wpFZ.DebugRest = !wpFZ.DebugRest;
			bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugRest, wpFZ.DebugRest);
			EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugRest");
			wpFZ.blink();
		}
#ifdef wpHT
		if(doWebServerDebugChange == "DebugHT") {
			wpFZ.DebugHT = !wpFZ.DebugHT;
			bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugHT, wpFZ.DebugHT);
			EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugHT");
			wpFZ.blink();
		}
#endif
#ifdef wpLDR
		if(doWebServerDebugChange == "DebugLDR") {
			wpFZ.DebugLDR = !wpFZ.DebugLDR;
			bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugLDR, wpFZ.DebugLDR);
			EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugLDR");
			wpFZ.blink();
		}
#endif
#ifdef wpLight
		if(doWebServerDebugChange == "DebugLight") {
			wpFZ.DebugLight = !wpFZ.DebugLight;
			bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugLight, wpFZ.DebugLight);
			EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugLight");
			wpFZ.blink();
		}
#endif
#ifdef wpBM
		if(doWebServerDebugChange == "DebugBM") {
			wpFZ.DebugBM = !wpFZ.DebugBM;
			bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugBM, wpFZ.DebugBM);
			EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugBM");
			wpFZ.blink();
		}
#endif
#ifdef wpRelais
		if(doWebServerDebugChange == "DebugRelais") {
			wpFZ.DebugRelais = !wpFZ.DebugRelais;
			bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugRelais, wpFZ.DebugRelais);
			EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugRelais");
			wpFZ.blink();
		}
#endif
#ifdef wpRain
		if(doWebServerDebugChange == "DebugRain") {
			wpFZ.DebugRain = !wpFZ.DebugRain;
			bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugRain, wpFZ.DebugRain);
			EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugRain");
			wpFZ.blink();
		}
#endif
#ifdef wpMoisture
		if(doWebServerDebugChange == "DebugMoisture") {
			wpFZ.DebugMoisture = !wpFZ.DebugMoisture;
			bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugMoisture, wpFZ.DebugMoisture);
			EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugMoisture");
			wpFZ.blink();
		}
#endif
#ifdef wpDistance
		if(doWebServerDebugChange == "DebugDistance") {
			wpFZ.DebugDistance = !wpFZ.DebugDistance;
			bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugDistance, wpFZ.DebugDistance);
			EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
			EEPROM.commit();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setDebug DebugDistance");
			wpFZ.blink();
		}
#endif
		doWebServerDebugChange = "";
	}
}
#endif
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
