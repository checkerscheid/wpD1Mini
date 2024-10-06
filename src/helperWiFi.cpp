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
//# Revision     : $Rev:: 206                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWiFi.cpp 206 2024-10-06 19:10:09Z                  $ #
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
	mqttTopicSendRest = wpFZ.DeviceName + "/settings/SendRest/WiFi";
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

uint16 helperWiFi::getVersion() {
	String SVN = "$Rev: 206 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperWiFi::changeSendRest() {
	sendRest = !sendRest;
	bitWrite(wpEEPROM.bitsSendRestBasis0, wpEEPROM.bitSendRestRssi, sendRest);
	EEPROM.write(wpEEPROM.addrBitsSendRestBasis0, wpEEPROM.bitsSendRestBasis0);
	EEPROM.commit();
	wpFZ.SendWSSendRest("sendRestWiFi", sendRest);
	wpFZ.DebugWS(wpFZ.strINFO, "WiFi::changeSendRest", "WiFi sendRest: " + sendRest ? "True" : "False");
	wpFZ.blink();
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
	
	WiFi.disconnect();
	WiFi.setHostname(wpFZ.DeviceName.c_str());
	WiFi.begin(wpFZ.ssid, wpFZ.password);

	Serial.print(wpFZ.getTime());
	Serial.print(wpFZ.getOnlineTime());
	Serial.print(wpFZ.strINFO);
	Serial.print(wpFZ.funcToString("setupWiFi"));
	while(WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		wpFZ.blink();
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

		wpFZ.DebugWS(wpFZ.strINFO, "scanWiFi", "own WiFi network");
		wpFZ.DebugWS(wpFZ.strINFO, "scanWiFi",
			WiFi.SSID() + ", "
			"Channel: " + String(WiFi.channel()) + ", "
			"BSSID: " + WiFi.BSSIDstr() + ", "
			"Signal: " + String(WiFi.RSSI()) + " dBm");
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
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperWiFi::publishValues() {
	publishValues(false);
}
void helperWiFi::publishValues(bool force) {
	if(force) {
		publishSendRestLast = 0;
		publishDebugLast = 0;
	}
	if(sendRestLast != sendRest || wpFZ.CheckQoS(publishSendRestLast)) {
		sendRestLast = sendRest;
		wpMqtt.mqttClient.publish(mqttTopicSendRest.c_str(), String(sendRest).c_str());
		wpFZ.SendWSSendRest("SendRestWiFi", sendRest);
		publishSendRestLast = wpFZ.loopStartedAt;
	}
	if(DebugLast != Debug || wpFZ.CheckQoS(publishDebugLast)) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishDebugLast = wpFZ.loopStartedAt;
	}
	if(wpFZ.loopStartedAt > publishRssiLast + wpFZ.minute2) {
		wpMqtt.mqttClient.publish(mqttTopicRssi.c_str(), String(WiFi.RSSI()).c_str());
		if(sendRest) {
			wpRest.error = wpRest.error | !wpRest.sendRest("rssi", String(WiFi.RSSI()));
			wpRest.trySend = true;
		}
		publishRssiLast = wpFZ.loopStartedAt;
	}
}

void helperWiFi::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSendRest.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void helperWiFi::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSendRest.c_str()) == 0) {
		bool readSendRest = msg.toInt();
		if(sendRest != readSendRest) {
			sendRest = readSendRest;
			bitWrite(wpEEPROM.bitsSendRestBasis0, wpEEPROM.bitSendRestRssi, sendRest);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsSendRestBasis0);
			EEPROM.commit();
			wpFZ.DebugcheckSubscribes(mqttTopicSendRest, String(sendRest));
		}
	}
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

void helperWiFi::checkDns() {
	int returns = 0;
	IPAddress r;
	returns = WiFi.hostByName(wpFZ.mqttServer, r);
	if(returns == 1) {
		wpFZ.DebugWS(wpFZ.strINFO, "checkDNS", "IP Address for " + String(wpFZ.mqttServer) + " is " + r.toString());
	} else {
		wpFZ.DebugWS(wpFZ.strERRROR, "checkDNS", "IP Address for " + String(wpFZ.mqttServer) + ": " + String(returns));
	}
	returns = WiFi.hostByName(wpFZ.restServer, r);
	if(returns == 1) {
		wpFZ.DebugWS(wpFZ.strINFO, "checkDNS", "IP Address for " + String(wpFZ.restServer) + " is " + r.toString());
	} else {
		wpFZ.DebugWS(wpFZ.strERRROR, "checkDNS", "IP Address for " + String(wpFZ.restServer) + ": " + String(returns));
	}
	returns = WiFi.hostByName(wpFZ.updateServer, r);
	if(returns == 1) {
		wpFZ.DebugWS(wpFZ.strINFO, "checkDNS", "IP Address for " + String(wpFZ.updateServer) + " is " + r.toString());
	} else {
		wpFZ.DebugWS(wpFZ.strERRROR, "checkDNS", "IP Address for " + String(wpFZ.updateServer) + ": " + String(returns));
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
