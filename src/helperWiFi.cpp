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
//# Revision     : $Rev:: 131                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWiFi.cpp 131 2024-06-05 03:01:06Z                  $ #
//#                                                                                 #
//###################################################################################
#include <helperWiFi.h>

helperWiFi wpWiFi;

helperWiFi::helperWiFi() {}
void helperWiFi::init() {
	// values
	mqttTopicRssi = wpFZ.DeviceName + "/info/WiFi/RSSI";
	mqttTopicWiFiSince = wpFZ.DeviceName + "/info/WiFi/Since";
	// settings
	mqttTopicSsid = wpFZ.DeviceName + "/info/WiFi/SSID";
	mqttTopicIp = wpFZ.DeviceName + "/info/WiFi/Ip";
	mqttTopicMac = wpFZ.DeviceName + "/info/WiFi/Mac";
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/WiFi";

	setupWiFi();
	// because DateTime Only works with NTP Server
	wpFZ.OnSince = wpFZ.getDateTime();
}

//###################################################################################
// public
//###################################################################################
void helperWiFi::cycle() {
	if(WiFi.status() == WL_CONNECTED) {
		digitalWrite(LED_BUILTIN, LOW);
	} else {
		digitalWrite(LED_BUILTIN, HIGH);
		setupWiFi();
	}
	publishValues();
}

uint16_t helperWiFi::getVersion() {
	String SVN = "$Rev: 131 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperWiFi::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis1, wpEEPROM.bitDebugWiFi, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis1, wpEEPROM.bitsDebugBasis1);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugWiFi", Debug);
	wpFZ.blink();
}

void helperWiFi::setupWiFi() {
	Serial.print(wpFZ.getTime());
	Serial.print(wpFZ.getOnlineTime());
	Serial.print(wpFZ.strINFO);
	Serial.print(wpFZ.funcToString("setupWiFi"));
	Serial.print("Connecting to ");
	Serial.println(wpFZ.ssid);

	WiFi.setHostname(wpFZ.DeviceName.c_str());
	WiFi.begin(wpFZ.ssid, wpFZ.password);

	Serial.print(wpFZ.getTime());
	Serial.print(wpFZ.getOnlineTime());
	Serial.print(wpFZ.strINFO);
	Serial.print(wpFZ.funcToString("setupWiFi"));
	while(WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println();

	Serial.print(wpFZ.getTime());
	Serial.print(wpFZ.getOnlineTime());
	Serial.print(wpFZ.strINFO);
	Serial.print(wpFZ.funcToString("setupWiFi"));
	Serial.print("WiFi Connected: ");
	Serial.println(WiFi.localIP());

	WiFiSince = wpFZ.getDateTime();
}

void helperWiFi::scanWiFi() {
	// scan for nearby networks:
	wpFZ.DebugWS(wpFZ.strWARN, "scanWiFi", "Start scan WiFi networks");
	int numSsid = WiFi.scanNetworks();
	if (numSsid == -1) {
		wpFZ.DebugWS(wpFZ.strWARN, "scanWiFi", "Couldn't get a WiFi connection");
	} else {
		// print the list of networks seen:
		wpFZ.DebugWS(wpFZ.strINFO, "scanWiFi", "number of available WiFi networks:" + String(numSsid));

		// print the network number and name for each network found:
		for (int thisNet = 0; thisNet < numSsid; thisNet++) {
			wpFZ.DebugWS(wpFZ.strINFO, "scanWiFi",
				String(thisNet + 1) + ": " + WiFi.SSID(thisNet) + ", " +
				"Channel: " + String(WiFi.channel(thisNet)) + ", " +
				"BSSID: " + WiFi.BSSIDstr(thisNet) + ", " +
				"Signal: " + String(WiFi.RSSI(thisNet)) + " dBm, " +
				"Encryption: " + printEncryptionType(WiFi.encryptionType(thisNet)));
		}
	}
	wpFZ.DebugWS(wpFZ.strWARN, "scanWiFi", "finished scan WiFi networks");
}

void helperWiFi::publishSettings() {
	publishSettings(false);
}
void helperWiFi::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicSsid.c_str(), wpFZ.ssid);
	wpMqtt.mqttClient.publish(mqttTopicIp.c_str(), WiFi.localIP().toString().c_str());
	wpMqtt.mqttClient.publish(mqttTopicMac.c_str(), WiFi.macAddress().c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperWiFi::publishValues() {
	publishValues(false);
}
void helperWiFi::publishValues(bool force) {
	if(force) {
		publishCountDebug = wpFZ.publishQoS;
		publishCountRssi = wpFZ.minute2;
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
	if(++publishCountRssi > wpFZ.minute2) {
		wpMqtt.mqttClient.publish(mqttTopicRssi.c_str(), String(WiFi.RSSI()).c_str());
		publishCountRssi = 0;
	}
}

void helperWiFi::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void helperWiFi::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugBasis1, wpEEPROM.bitDebugWiFi, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis1, wpEEPROM.bitsDebugBasis1);
			EEPROM.commit();
			wpFZ.SendWSDebug("DebugWiFi", Debug);
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}

//###################################################################################
// private
//###################################################################################
String helperWiFi::printEncryptionType(int thisType) {
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
