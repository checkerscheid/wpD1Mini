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
	MajorVersion = 2;
	MinorVersion = 5;
	EEPROM.begin(4095);
	configTime(TZ, NTP_SERVER);
	DeviceName = deviceName;
	DeviceDescription = deviceName;
	OfflineTrigger = false;
	UpdateFW = false;
	calcValues = true;
	DebugEprom = false;
	DebugWiFi = false;
	DebugMqtt = false;
	DebugFinder = false;
	DebugRest = false;
	ErrorRest = false;

#ifdef wpHT
	DebugHT = false;
	maxCycleHT = 4;
	temperatureCorrection = 0.0;
	humidityCorrection = 0.0;
#endif
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

void wpFreakaZone::loop() {
	doTheWebServerDebugChange();
	OnDuration = getOnlineTime(false);
}

String wpFreakaZone::getVersion() {
	Rev = "$Rev: 117 $";
	Rev.remove(0, 6);
	Rev.remove(Rev.length() - 2, 2);
	Build = Rev.toInt();
	Revh = SVNh;
	Revh.remove(0, 6);
	Revh.remove(Revh.length() - 2, 2);
	Buildh = Revh.toInt();

	String returns = "V " +
		String(MajorVersion) + "." + String(MinorVersion) +
		" Build " + String(Buildh) + "." + String(Build);
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
void wpFreakaZone::setMainVersion(String v) {
	MainVersion = v;
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

void wpFreakaZone::setupWiFi() {
	delay(10);
	Serial.print(getTime());
	Serial.print(getOnlineTime());
	Serial.print(strINFO);
	Serial.print(funcToString("setupWiFi"));
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.setHostname(DeviceName.c_str());
	WiFi.begin(ssid, password);
	Serial.print(getTime());
	Serial.print(getOnlineTime());
	Serial.print(strINFO);
	Serial.print(funcToString("setupWiFi"));
	while(WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println();
	Serial.print(getTime());
	Serial.print(getOnlineTime());
	Serial.print(strINFO);
	Serial.print(funcToString("setupWiFi"));
	Serial.print("WiFi Connected: ");
	Serial.println(WiFi.localIP());
	WiFiSince = getDateTime();
}

void wpFreakaZone::scanWiFi() {
	// scan for nearby networks:
	DebugWS(strINFO, "scanWiFi", "Start scan WiFi networks");
	int numSsid = WiFi.scanNetworks();
	if (numSsid == -1) {
		DebugWS(strWARN, "scanWiFi", "Couldn't get a WiFi connection");
	} else {
		// print the list of networks seen:
		DebugWS(strINFO, "scanWiFi", "number of available WiFi networks:" + String(numSsid));

		// print the network number and name for each network found:
		for (int thisNet = 0; thisNet < numSsid; thisNet++) {
			DebugWS(strINFO, "scanWiFi",
				String(thisNet + 1) + ": " + WiFi.SSID(thisNet) + ", " +
				"Channel: " + String(WiFi.channel(thisNet)) + ", " +
				"BSSID: " + WiFi.BSSIDstr(thisNet) + ", " +
				"Signal: " + String(WiFi.RSSI(thisNet)) + " dBm, " +
				"Encryption: " + printEncryptionType(WiFi.encryptionType(thisNet)));
		}
	}
	DebugWS(strINFO, "scanWiFi", "finished scan WiFi networks");
}

bool wpFreakaZone::setupOta() {
	bool returns = false;
	ArduinoOTA.onStart([]() {
		String logmessage = "OTA Start";
		wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage);
	});
	ArduinoOTA.onEnd([]() {
		wpFZ.OfflineTrigger = true;
		String logmessage = "OTA End";
		wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage);
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		String logmessage = "OTA Progress: " + String((progress / (total / 100))) + " %";
		wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage, false);
	});
	ArduinoOTA.onError([](ota_error_t error) {
		String logmessage = "Error (" + String(error) + ") ";
		if (error == OTA_AUTH_ERROR) logmessage += "Auth Failed";
		else if (error == OTA_BEGIN_ERROR) logmessage += "Begin Failed";
		else if (error == OTA_CONNECT_ERROR) logmessage += "Connect Failed";
		else if (error == OTA_RECEIVE_ERROR) logmessage += "Receive Failed";
		else if (error == OTA_END_ERROR) logmessage += "End Failed";
		wpFZ.DebugWS(wpFZ.strERRROR, "setupOta", logmessage);
	});
	ArduinoOTA.begin();
	returns = true;
	String logmessage = "OTA Ready, IP address: " + WiFi.localIP().toString();
	wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage);
	return returns;
}

AsyncUDP udp;

void wpFreakaZone::setupFinder() {
	if(udp.listen(wpFZ.findListenPort)) {
		udp.onPacket([](AsyncUDPPacket packet) {
			int val = strncmp((char *) packet.data(), "FreakaZone Member?", 2);
			if(val == 0) {
				String IP = WiFi.localIP().toString();
				packet.printf("{\"Iam\":{\"FreakaZoneClient\":\"%s\",\"IP\":\"%s\",\"MAC\":\"%s\",\"wpFreakaZoneVersion\":\"%s\",\"Version\":\"%s\"}}",
					wpFZ.DeviceName.c_str(), IP.c_str(), WiFi.macAddress().c_str(), wpFZ.getVersion().c_str(), wpFZ.MainVersion.c_str());
				String logmessage = "Found FreakaZone Member question";
				wpFZ.DebugWS(wpFZ.strDEBUG, "setupFinder", logmessage);
			}
			char* tmpStr = (char*) malloc(packet.length() + 1);
			memcpy(tmpStr, packet.data(), packet.length());
			tmpStr[packet.length()] = '\0';
			String dataString = String(tmpStr);
			free(tmpStr);
			String logmessage = "UDP Packet Type: " +
				String(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast") +
				", From: " + packet.remoteIP().toString() + ":" + String(packet.remotePort()) +
				", To: " + packet.localIP().toString() + ":" + String(packet.localPort()) +
				", Length: " + String(packet.length()) +
				", Data: " + dataString;
			wpFZ.DebugWS(wpFZ.strDEBUG, "setupFinder", logmessage);
		});
	}
}

String wpFreakaZone::readStringFromEEPROM(int addrOffset, String defaultString) {
	int newStrLen = EEPROM.read(addrOffset);
	if (newStrLen == 255) return defaultString;

	if(DebugEprom) {
		Serial.printf("newStrLen: %u\n", newStrLen);
	}
	char data[newStrLen];
	for (int i = 0; i < newStrLen; i++) {
		data[i] = EEPROM.read(addrOffset + 1 + i);
	}
	data[newStrLen] = '\0';
	return String(data);
}

int wpFreakaZone::writeStringToEEPROM(int addrOffset, String &strToWrite) {
	byte len = strToWrite.length();
	EEPROM.write(addrOffset, len);
	int returns = addrOffset + 1;
	for (int i = 0; i < len; i++) {
		EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
		returns = addrOffset + 1 + i + 1;
	}
	EEPROM.commit();
	if(DebugEprom) {
		Serial.println();
		Serial.printf("Start: %u\n", addrOffset);
		Serial.printf("Len: %u\n", len);
		Serial.printf("Start Next: %u\n", returns);
	}
	return returns;
}
// return true on success
bool wpFreakaZone::sendRest(String name, String value) {
	bool returns = false;
	WiFiClient client;
	HTTPClient http;
	String targetmac = WiFi.macAddress();
	targetmac.replace(":", "");
	targetmac.toLowerCase();
	String targetwithident = "http://" + String(wpFZ.restServer) + ":" + String(wpFZ.restServerPort) + "/?m=" + targetmac;
	String target = targetwithident + "&" + name + "=" + value;
	if(DebugRest) {
		String logmessage = "HTTP: '" + target + "'";
		wpFZ.DebugWS(wpFZ.strINFO, "sendRest", logmessage);
	}
	http.begin(client, target.c_str());
	int httpResponseCode = http.GET();
	if (httpResponseCode > 0) {
		if(DebugRest) {
			String payload = http.getString();
			payload.replace("\"", "'");
			String logmessage = "HTTP Response (" + String(httpResponseCode) + "): " + payload;
			wpFZ.DebugWS(wpFZ.strINFO, "sendRest", logmessage);
		}
		returns = true;
	} else {
		String logmessage = "HTTP Response (" + String(httpResponseCode) + ")";
		wpFZ.DebugWS(wpFZ.strERRROR, "sendRest", logmessage);
	}
	http.end();
	return returns;
}
// return true on success
bool wpFreakaZone::sendRawRest(String target) {
	bool returns = false;
	WiFiClient client;
	HTTPClient http;
	if(DebugRest) {
		String logmessage = "HTTP: '" + target + "'";
		wpFZ.DebugWS(wpFZ.strINFO, "sendRawRest", logmessage);
	}
	http.begin(client, target.c_str());
	int httpResponseCode = http.GET();
	if (httpResponseCode > 0) {
		if(DebugRest) {
			String payload = http.getString();
			payload.replace("\"", "'");
			String logmessage = "HTTP Response (" + String(httpResponseCode) + "): " + payload;
			wpFZ.DebugWS(wpFZ.strINFO, "sendRawRest", logmessage);
		}
		returns = true;
	} else {
		String logmessage = "HTTP Response (" + String(httpResponseCode) + ")";
		wpFZ.DebugWS(wpFZ.strERRROR, "sendRawRest", logmessage);
	}
	http.end();
	return returns;
}
void wpFreakaZone::setupWebServer() {
	server.addHandler(&ws);
	server.onNotFound([](AsyncWebServerRequest *request){ 
		request->send(404, "text/plain", "Link was not found!");  
	});

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "application/json", "{\"this\":{\"is\":{\"a\":{\"FreakaZone\":{\"member\":true}}}}}");
	});

	server.on("/print", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/html", index_html, processor);
	});
	server.on("/setDebug", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setDebug");
		if(request->hasParam("Debug")) {
			wpFZ.setWebServerDebugChange(request->getParam("Debug")->value());
		}
		request->send(200);
	});

	server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", favicon, sizeof(favicon));
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});

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

String wpFreakaZone::printEncryptionType(int thisType) {
	switch (thisType) {
		case ENC_TYPE_WEP:
			return "WEP";
			break;
		case ENC_TYPE_TKIP:
			return "WPA";
			break;
		case ENC_TYPE_CCMP:
			return "WPA2";
			break;
		case ENC_TYPE_NONE:
			return "None";
			break;
		case ENC_TYPE_AUTO:
			return "Auto";
			break;
	}
	return "Type not known: " + String(thisType);
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
	ws.textAll("{\"msg\":\"" + toSend + "\",\"newline\":" + (newline ? "true" : "false") + "}");
}
void wpFreakaZone::SendWS(String msg) {
	ws.textAll("{\"cmd\":\"setDebug\",\"msg\":" + msg + "}");
}

String wpFreakaZone::funcToString(String msg) {
	msg.remove(16);
	String e = "";
	for(int i = msg.length(); i < 16; i++) {
		e += ' ';
	}
	return "[" + msg + e + "] ";
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

String processor(const String& var) {
	String wpHT_s;
	if(var == "IPADRESS")
		return WiFi.localIP().toString();
	if(var == "DeviceName")
		return wpFZ.DeviceName;
	if(var == "DeviceDescription")
		return wpFZ.DeviceDescription;
	if(var == "CompiledWith") {
		wpHT_s = "<ul>";
#ifdef wpHT
	if(wpHT == DHT11)
			wpHT_s += "<li>wpHT HT11</li>";
	if(wpHT == DHT22)
			wpHT_s += "<li>wpHT HT22</li>";
#endif
#ifdef wpLDR
		wpHT_s += "<li>wpLDR</li>";
#endif
#ifdef wpLight
		wpHT_s += "<li>wpLight</li>";
#endif
#ifdef wpBM
		wpHT_s += "<li>wpBM</li>";
#endif
#ifdef wpRelais
		wpHT_s += "<li>wpRelais</li>";
#endif
#ifdef wpRain
		wpHT_s += "<li>wpRain</li>";
#endif
#ifdef wpMoisture
		wpHT_s += "<li>wpMoisture</li>";
#endif
#ifdef wpDistance
		wpHT_s += "<li>wpDistance</li>";
#endif
		return wpHT_s += "</ul>";
	}
	if(var == "Debug") {
		wpHT_s = "<ul><li><span class='bold'>Cmds:</span></li><li><hr /></li>";
		wpHT_s += "<li><input id='calcValues' type='checkbox'" + String(wpFZ.calcValues ? " checked" : "") + " onchange='cmdHandle(event)' /><label for='calcValues'>calc Values</label></li>";	
#ifdef wpMoisture
#ifdef wpRelais
		wpHT_s += "<li><input id='waterEmpty' type='checkbox'" + String(wpFZ.waterEmpty ? " checked='checked'" : "") + " onchange='cmdHandle(event)' /><label for='waterEmpty'>waterEmpty</label></li>";
#endif
#endif
		wpHT_s += "<li><span class='bold'>Debug:</span></li><li><hr /></li>";
		wpHT_s += "<li><input id='DebugEprom' type='checkbox'" + String(wpFZ.DebugEprom ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugEprom'>Eprom</label></li>";
		wpHT_s += "<li><input id='DebugWiFi' type='checkbox'" + String(wpFZ.DebugWiFi ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugWiFi'>WiFi</label></li>";
		wpHT_s += "<li><input id='DebugMqtt' type='checkbox'" + String(wpFZ.DebugMqtt ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugMqtt'>Mqtt</label></li>";
		wpHT_s += "<li><input id='DebugFinder' type='checkbox'" + String(wpFZ.DebugFinder ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugFinder'>Finder</label></li>";
		wpHT_s += "<li><input id='DebugRest' type='checkbox'" + String(wpFZ.DebugRest ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugRest'>Rest</label></li>";

#ifdef wpHT
		wpHT_s += "<li><input id='DebugHT' type='checkbox'" + String(wpFZ.DebugHT ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugHT'>HT</label></li>";
#endif
#ifdef wpLDR
		wpHT_s += "<li><input id='DebugLDR' type='checkbox'" + String(wpFZ.DebugLDR ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugLDR'>LDR</label></li>";
#endif
#ifdef wpLight
		wpHT_s += "<li><input id='DebugLight' type='checkbox'" + String(wpFZ.DebugLight ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugLight'>Light</label></li>";
#endif
#ifdef wpBM
		wpHT_s += "<li><input id='DebugBM' type='checkbox'" + String(wpFZ.DebugBM ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugBM'>BM</label></li>";
#endif
#ifdef wpRelais
		wpHT_s += "<li><input id='DebugRelais' type='checkbox'" + String(wpFZ.DebugRelais ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugRelais'>Relais</label></li>";
#endif
#ifdef wpRain
		wpHT_s += "<li><input id='DebugRain' type='checkbox'" + String(wpFZ.DebugRain ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugRain'>Rain</label></li>";
#endif
#ifdef wpMoisture
		wpHT_s += "<li><input id='DebugMoisture' type='checkbox'" + String(wpFZ.DebugMoisture ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugMoisture'>Moisture</label></li>";
#endif
#ifdef wpDistance
		wpHT_s += "<li><input id='DebugDistance' type='checkbox'" + String(wpFZ.DebugDistance ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugDistance'>Distance</label></li>";
#endif
		return wpHT_s += "</ul>";
	}
	return String();
}
