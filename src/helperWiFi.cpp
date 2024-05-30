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
#include <helperWiFi.h>

helperWiFi wpWiFi();

helperWiFi::helperWiFi() {
	// settings
	mqttTopicSsid = wpFZ.DeviceName + "/info/WiFi/SSID";
	mqttTopicIp = wpFZ.DeviceName + "/info/WiFi/Ip";
	mqttTopicMac = wpFZ.DeviceName + "/info/WiFi/Mac";
	mqttTopicWiFiSince = wpFZ.DeviceName + "/info/WiFi/Since";
	mqttTopicRssi = wpFZ.DeviceName + "/info/WiFi/RSSI";
	// commands
	mqttTopicDebugWiFi = wpFZ.DeviceName + "/settings/Debug/WiFi";

	setupWiFi();
	// because DateTime Only works with NTP Server
	wpFZ.OnSince = wpFZ.getDateTime();
}

//###################################################################################
// public
//###################################################################################
void helperWiFi::loop() {
	if(WiFi.status() == WL_CONNECTED) {
		digitalWrite(LED_BUILTIN, LOW);
	} else {
		digitalWrite(LED_BUILTIN, HIGH);
		setupWiFi();
	}
}

uint16_t helperWiFi::getVersion() {
	String SVN = "$Rev: 118 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
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

	wpFZ.WiFiSince = wpFZ.getDateTime();
}

void helperWiFi::scanWiFi() {
	// scan for nearby networks:
	wpFZ.DebugWS(wpFZ.strINFO, "scanWiFi", "Start scan WiFi networks");
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
	wpFZ.DebugWS(wpFZ.strINFO, "scanWiFi", "finished scan WiFi networks");
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
