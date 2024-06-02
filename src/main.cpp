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
//# Revision     : $Rev:: 123                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.cpp 123 2024-06-02 04:37:07Z                        $ #
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
	wpModules.init();
	wpOnlineToggler.init();
	wpFinder.init();
	wpRest.init();
	wpUpdate.init();
	wpWebServer.init();

	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
		wpDHT.init();
	}
	if(wpModules.useModuleLight) {
		wpLight.init();
	}
	wpModules.publishAllValues();
	wpModules.publishAllSettings();
	wpModules.setAllSubscribes();
}

//###################################################################################
// loop
//###################################################################################
void loop() {
	wpFZ.cycle();
	wpEEPROM.cycle();
	wpWiFi.cycle();
	wpMqtt.cycle();
	wpModules.cycle();
	wpOnlineToggler.cycle();
	wpFinder.cycle();
	wpRest.cycle();
	wpUpdate.cycle();
	wpWebServer.cycle();

	if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
		wpDHT.cycle();
	}
	if(wpModules.useModuleLight) {
		wpLight.cycle();
	}
	
	delay(wpFZ.loopTime);
}

//###################################################################################
// Allgemein
//###################################################################################
uint16_t getVersion() {
	String SVN = "$Rev: 123 $";
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
	check = wpModules.getVersion();
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
