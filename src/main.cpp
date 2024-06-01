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
//# Revision     : $Rev:: 121                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.cpp 121 2024-06-01 05:13:59Z                        $ #
//#                                                                                 #
//###################################################################################
#include <main.h>

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	Serial.begin(9600);
	while(!Serial) {}
	wpFZ.init("BasisEmpty");
	wpFZ.printStart();
	wpEEPROM.init();
	wpFZ.Build = getBuild();
	wpFZ.Version = getStringVersion();
	wpFZ.printRestored();

	wpWiFi.init();
	wpMqtt.init();
	wpOnlineToggler.init();
	wpFinder.init();
	wpWebServer.init();
	wpRest.init();
}

//###################################################################################
// loop
//###################################################################################
void loop() {
	wpEEPROM.cycle();
	wpFZ.cycle();
	wpWiFi.cycle();
	wpMqtt.cycle();
	wpOnlineToggler.cycle();
	wpFinder.cycle();
	wpWebServer.cycle();
	wpRest.cycle();
	if(wpFZ.calcValues) {
		// calcValues();
	}
	
	delay(wpFZ.loopTime);
}

//###################################################################################
// Allgemein
//###################################################################################
uint16_t getVersion() {
	String SVN = "$Rev: 121 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String getStringVersion() {
	uint16_t globalBuild = getGlobalBuild();
	uint16_t Build = getVersion();

	String returns = "V " +
		String(wpFZ.MajorVersion) + "." + String(wpFZ.MinorVersion) +
		" Build " + String(globalBuild > Build ? globalBuild : Build);
	return returns;
}

uint16_t getGlobalBuild() {
 	uint16_t v = 0;
	uint16_t check = wpEEPROM.getVersion();
	v = v > check ? v : check;
	check = wpFinder.getVersion();
	v = v > check ? v : check;
	check = wpMqtt.getVersion();
	v = v > check ? v : check;
	check = wpOnlineToggler.getVersion();
	v = v > check ? v : check;
	check = wpRest.getVersion();
	v = v > check ? v : check;
	check = wpUpdate.getVersion();
	v = v > check ? v : check;
	check = wpWebServer.getVersion();
	v = v > check ? v : check;
	check = wpWiFi.getVersion();
	v = v > check ? v : check;
	check = wpFZ.getVersion();
	v = v > check ? v : check;
	check = wpDHT.getVersion();
	v = v > check ? v : check;
	return v;
}

uint16_t getBuild() {
	uint16_t globalBuild = getGlobalBuild();
	uint16_t Build = getVersion();
	return globalBuild > Build ? globalBuild : Build;
}
