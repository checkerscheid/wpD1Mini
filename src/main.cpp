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
//# Revision     : $Rev:: 119                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.cpp 119 2024-05-31 03:31:43Z                        $ #
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
	wpFZ.Version = getStringVersion();
	wpFZ.printRestored();

	wpWiFi.init();
	wpMqtt.init();
	wpOnlineToggler.init();

}

//###################################################################################
// loop
//###################################################################################
void loop() {
	wpEEPROM.cycle();
	wpWiFi.cycle();
	wpMqtt.cycle();
	wpFZ.cycle();
	wpOnlineToggler.cycle();
	if(wpFZ.calcValues) {
		//calcValues();
	}
	
	delay(wpFZ.loopTime);
}

//###################################################################################
// Allgemein
//###################################################################################
uint16_t getVersion() {
	String SVN = "$Rev: 119 $";
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
