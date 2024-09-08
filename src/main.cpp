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
//# Revision     : $Rev:: 198                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.cpp 198 2024-09-05 12:32:25Z                        $ #
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
	if(wpModules.useModuleWindow) {
		wpWindow.init();
	}
	if(wpModules.useModuleCwWw) {
		wpCwWw.init();
	}
	if(wpModules.useModuleAnalogOut) {
		wpAnalogOut.init();
	}
	if(wpModules.useModuleAnalogOut2) {
		wpAnalogOut2.init();
	}
	if(wpModules.useModuleNeoPixel) {
		wpNeoPixel.init();
	}
	if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
		wpRelais.init();
	}
	if(wpModules.useModuleRpm) {
		wpRpm.init();
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
	if(wpModules.useModuleImpulseCounter) {
		wpImpulseCounter.init();
	}
	wpModules.publishAllValues();
	wpModules.publishAllSettings();
	wpModules.setAllSubscribes();
}

//###################################################################################
// loop
//###################################################################################
void loop() {
	wpFZ.loopStartedAt = millis();
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
		if(wpModules.useModuleWindow) {
			wpWindow.cycle();
		}
		if(wpModules.useModuleCwWw) {
			wpCwWw.cycle();
		}
		if(wpModules.useModuleAnalogOut) {
			wpAnalogOut.cycle();
		}
		if(wpModules.useModuleAnalogOut2) {
			wpAnalogOut2.cycle();
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			wpRelais.cycle();
		}
		if(wpModules.useModuleRpm) {
			wpRpm.cycle();
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
		if(wpModules.useModuleImpulseCounter) {
			wpImpulseCounter.cycle();
		}
		if(wpModules.useModuleNeoPixel) {
			wpNeoPixel.cycle();
		}
	}
	wpRest.cycle();
	//delay(100);
}

//###################################################################################
// Allgemein
//###################################################################################
uint16 getVersion() {
	String SVN = "$Rev: 198 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String getStringVersion() {
	uint16 globalBuild = getGlobalBuild();
	uint16 Build = getVersion();

	String returns = "V " +
		String(wpFZ.MajorVersion) + "." + String(wpFZ.MinorVersion) +
		" Build " + String(globalBuild > Build ? globalBuild : Build);
	return returns;
}

uint16 getGlobalBuild() {
 	uint16 v = 0;
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
	buildChecker(v, wpWindow.getVersion());
	buildChecker(v, wpCwWw.getVersion());
	buildChecker(v, wpAnalogOut.getVersion());
	buildChecker(v, wpAnalogOut2.getVersion());
	buildChecker(v, wpNeoPixel.getVersion());
	buildChecker(v, wpRelais.getVersion());
	buildChecker(v, wpRpm.getVersion());
	buildChecker(v, wpRain.getVersion());
	buildChecker(v, wpMoisture.getVersion());
	buildChecker(v, wpDistance.getVersion());
	buildChecker(v, wpImpulseCounter.getVersion());
	return v;
}
void buildChecker(uint16 &v, uint16 moduleBuild) {
	v = v > moduleBuild ? v : moduleBuild;
}

uint16 getBuild() {
	uint16 globalBuild = getGlobalBuild();
	uint16 Build = getVersion();
	return globalBuild > Build ? globalBuild : Build;
}

