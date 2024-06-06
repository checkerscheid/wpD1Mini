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
//# Revision     : $Rev:: 132                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.cpp 132 2024-06-06 11:07:48Z                        $ #
//#                                                                                 #
//###################################################################################
#include <main.h>

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	Serial.begin(9600);
	while(!Serial) {}
	wpEEPROM.init();
	wpFZ.init("BasisEmpty");
	wpFZ.printStart();
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
	if(wpModules.useModuleLDR) {
		wpLDR.init();
	}
	if(wpModules.useModuleLight) {
		wpLight.init();
	}
	if(wpModules.useModuleBM) {
		wpBM.init();
	}
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.init();
	}
	if(wpModules.useModuleRain) {
		wpRain.init();
	}
	if(wpModules.useModuleMoisture) {
		wpMoisture.init();
	}
	if(wpModules.useModuleDistance) {
		wpDistance.init();
	}
	wpModules.publishAllValues();
	wpModules.publishAllSettings();
	wpModules.setAllSubscribes();
}

//###################################################################################
// loop
//###################################################################################
void loop() {
	wpRest.trySend = false;
	wpRest.error = false;
	wpFZ.cycle();
	wpEEPROM.cycle();
	wpWiFi.cycle();
	wpMqtt.cycle();
	wpModules.cycle();
	wpOnlineToggler.cycle();
	wpFinder.cycle();
	wpUpdate.cycle();
	wpWebServer.cycle();
	if(!wpFZ.restartRequired) {
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			wpDHT.cycle();
		}
		if(wpModules.useModuleLDR) {
			wpLDR.cycle();
		}
		if(wpModules.useModuleLight) {
			wpLight.cycle();
		}
		if(wpModules.useModuleBM) {
			wpBM.cycle();
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			wpRelais.cycle();
		}
		if(wpModules.useModuleRain) {
			wpRain.cycle();
		}
		if(wpModules.useModuleMoisture) {
			wpMoisture.cycle();
		}
		if(wpModules.useModuleDistance) {
			wpDistance.cycle();
		}
	}
	wpRest.cycle();
	delay(wpFZ.loopTime);
}

//###################################################################################
// Allgemein
//###################################################################################
uint16_t getVersion() {
	String SVN = "$Rev: 132 $";
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
	buildChecker(v, wpEEPROM.getVersion());
	buildChecker(v, wpFinder.getVersion());
	buildChecker(v, wpModules.getVersion());
	buildChecker(v, wpMqtt.getVersion());
	buildChecker(v, wpOnlineToggler.getVersion());
	buildChecker(v, wpRest.getVersion());
	buildChecker(v, wpUpdate.getVersion());
	buildChecker(v, wpWebServer.getVersion());
	buildChecker(v, wpWiFi.getVersion());
	buildChecker(v, wpFZ.getVersion());

	buildChecker(v, wpDHT.getVersion());
	buildChecker(v, wpLDR.getVersion());
	buildChecker(v, wpLight.getVersion());
	buildChecker(v, wpBM.getVersion());
	buildChecker(v, wpRelais.getVersion());
	buildChecker(v, wpRain.getVersion());
	buildChecker(v, wpMoisture.getVersion());
	buildChecker(v, wpDistance.getVersion());
	return v;
}
void buildChecker(uint16_t &v, uint16 moduleBuild) {
	v = v > moduleBuild ? v : moduleBuild;
}

uint16_t getBuild() {
	uint16_t globalBuild = getGlobalBuild();
	uint16_t Build = getVersion();
	return globalBuild > Build ? globalBuild : Build;
}

