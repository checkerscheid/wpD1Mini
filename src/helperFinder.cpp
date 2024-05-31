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
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Init");
#endif
	// commands
	mqttTopicDebugFinder = wpFZ.DeviceName + "/settings/Debug/Finder";	
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("Inited");
#endif
}

//###################################################################################
// public
//###################################################################################
void helperFinder::cycle() {
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycle");
#endif
#ifdef DEBUG
	Serial.print(__FILE__);
	Serial.println("cycled");
#endif
}

uint16_t helperFinder::getVersion() {
	String SVN = "$Rev: 120 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperFinder::changeDebug() {
	DebugFinder = !DebugFinder;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugFinder, DebugFinder);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugFinder\",\"value\":" + String(DebugFinder ? "true" : "false") + "}");
	wpFZ.blink();
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
