//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 30.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 120                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperFinder.cpp 120 2024-05-31 03:32:41Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperFinder.h>

helperFinder wpFinder;

helperFinder::helperFinder() {}
void helperFinder::init() {
	// commands
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Finder";
	setupFinder();
}

//###################################################################################
// public
//###################################################################################
void helperFinder::cycle() {
}

uint16_t helperFinder::getVersion() {
	String SVN = "$Rev: 120 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperFinder::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugFinder, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugFinder\",\"value\":" + String(Debug ? "true" : "false") + "}");
	wpFZ.blink();
}

void helperFinder::publishSettings() {
	publishSettings(false);
}
void helperFinder::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperFinder::publishValues() {
	publishValues(false);
}
void helperFinder::publishValues(bool force) {
	if(force) publishCountDebug = wpFZ.publishQoS;
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void helperFinder::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void helperFinder::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugFinder, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
			EEPROM.commit();
			wpFZ.SendWS("{\"id\":\"DebugFinder\",\"value\":" + String(Debug ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}

//###################################################################################
// private
//###################################################################################

AsyncUDP udp;

void helperFinder::setupFinder() {
	if(udp.listen(wpFZ.finderListenPort)) {
		udp.onPacket([](AsyncUDPPacket packet) {
			int val = strncmp((char *) packet.data(), "FreakaZone Member?", 2);
			if(val == 0) {
				String IP = WiFi.localIP().toString();
				packet.printf("{\"Iam\":{\"FreakaZoneClient\":\"%s\",\"IP\":\"%s\",\"MAC\":\"%s\",\"Version\":\"%s\"}}",
					wpFZ.DeviceName.c_str(), IP.c_str(), WiFi.macAddress().c_str(), wpFZ.Version.c_str());
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
