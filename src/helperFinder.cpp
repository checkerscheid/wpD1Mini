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
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.cpp 117 2024-05-29 01:28:02Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperFinder.h>

helperFinder wpFinder();

helperFinder::helperFinder() {
	// commands
	mqttTopicDebugFinder = wpFZ.DeviceName + "/settings/Debug/Finder";
}

//###################################################################################
// public
//###################################################################################
void helperFinder::loop() {
}

uint16_t helperFinder::getVersion() {
	String SVN = "$Rev: 118 $";
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
