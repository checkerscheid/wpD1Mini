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
//# Revision     : $Rev:: 272                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.cpp 272 2025-08-13 18:45:43Z                        $ #
//#                                                                                 #
//###################################################################################
#include <main.h>

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	Serial.begin(112500);
	while(!Serial) {}
	wpEEPROM.init();
	wpFZ.init("BasisEmpty");
	wpFZ.printStart();
	wpFZ.Build = getBuild();
	wpFZ.Version = getStringVersion();
	wpFZ.printRestored();
	wpFZ.BootCount();

	wpWiFi.init();
	wpMqtt.init();
	wpModules.init();
	wpOnlineToggler.init();
	wpFinder.init();
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
		wpWindow1.init();
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
	#if BUILDWITH == 1
	if(wpModules.useModuleCwWw) {
		wpCwWw.init();
	}
	if(wpModules.useModuleNeoPixel) {
		wpNeoPixel.init();
	}
	if(wpModules.useModuleClock) {
		wpClock.init();
	}
	#endif
	#if BUILDWITH == 2
	if(wpModules.useModuleAnalogOut) {
		wpAnalogOut.init();
	}
	if(wpModules.useModuleAnalogOut2) {
		wpAnalogOut2.init();
	}
	if(wpModules.useModuleRpm) {
		wpRpm.init();
	}
	if(wpModules.useModuleImpulseCounter) {
		wpImpulseCounter.init();
	}
	if(wpModules.useModuleWindow2) {
		wpWindow2.init();
	}
	if(wpModules.useModuleWindow3) {
		wpWindow3.init();
	}
	if(wpModules.useModuleWeight) {
		wpWeight.init();
	}
	#endif
	#if BUILDWITH == 3
	if(wpModules.useModuleUnderfloor1) {
		wpUnderfloor1.init();
	}
	if(wpModules.useModuleUnderfloor2) {
		wpUnderfloor2.init();
	}
	if(wpModules.useModuleUnderfloor3) {
		wpUnderfloor3.init();
	}
	if(wpModules.useModuleUnderfloor4) {
		wpUnderfloor4.init();
	}
	if(wpModules.useModuleDS18B20) {
		wpDS18B20.init();
	}
	#endif
	#if BUILDWITH == 4
	if(wpModules.useModuleSML) {
		wpSML.init();
	}
	if(wpModules.useModuleModbus) {
		wpModbus.init();
	}
	if(wpModules.useModuleRFID) {
		wpRFID.init();
	}
	#endif
	// wpModules.publishAllValues();
	// wpModules.publishAllSettings();
	// wpModules.setAllSubscribes();

}

//###################################################################################
// loop
//###################################################################################
void loop() {
	wpFZ.loopStartedAt = millis();
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
			wpWindow1.cycle();
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
		#if BUILDWITH == 1
		if(wpModules.useModuleCwWw) {
			wpCwWw.cycle();
		}
		if(wpModules.useModuleNeoPixel) {
			wpNeoPixel.cycle();
		}
		if(wpModules.useModuleClock) {
			wpClock.cycle();
		}
		#endif
		#if BUILDWITH == 2
		if(wpModules.useModuleAnalogOut) {
			wpAnalogOut.cycle();
		}
		if(wpModules.useModuleAnalogOut2) {
			wpAnalogOut2.cycle();
		}
		if(wpModules.useModuleRpm) {
			wpRpm.cycle();
		}
		if(wpModules.useModuleImpulseCounter) {
			wpImpulseCounter.cycle();
		}
		if(wpModules.useModuleWindow2) {
			wpWindow2.cycle();
		}
		if(wpModules.useModuleWindow3) {
			wpWindow3.cycle();
		}
		if(wpModules.useModuleWeight) {
			wpWeight.cycle();
		}
		#endif
		#if BUILDWITH == 3
		if(wpModules.useModuleUnderfloor1) {
			wpUnderfloor1.cycle();
		}
		if(wpModules.useModuleUnderfloor2) {
			wpUnderfloor2.cycle();
		}
		if(wpModules.useModuleUnderfloor3) {
			wpUnderfloor3.cycle();
		}
		if(wpModules.useModuleUnderfloor4) {
			wpUnderfloor4.cycle();
		}
		if(wpModules.useModuleDS18B20) {
			wpDS18B20.cycle();
		}
		#endif
		#if BUILDWITH == 4
		if(wpModules.useModuleSML) {
			wpSML.cycle();
		}
		if(wpModules.useModuleModbus) {
			wpModbus.cycle();
		}
		if(wpModules.useModuleRFID) {
			wpRFID.cycle();
		}
		#endif
	}
	//delay(100);
}

//###################################################################################
// Allgemein
//###################################################################################
uint16_t getVersion() {
	String SVN = "$Rev: 272 $";
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
	buildChecker(v, wpUpdate.getVersion());
	buildChecker(v, wpWebServer.getVersion());
	buildChecker(v, wpWiFi.getVersion());
	buildChecker(v, wpFZ.getVersion());

	buildChecker(v, wpDHT.getVersion());
	buildChecker(v, wpLDR.getVersion());
	buildChecker(v, wpLight.getVersion());
	buildChecker(v, wpBM.getVersion());
	buildChecker(v, wpWindow1.getVersion());
	buildChecker(v, wpRelais.getVersion());
	buildChecker(v, wpRain.getVersion());
	buildChecker(v, wpMoisture.getVersion());
	buildChecker(v, wpDistance.getVersion());
	buildChecker(v, wpCwWw.getVersion());
	buildChecker(v, wpNeoPixel.getVersion());
	buildChecker(v, wpAnalogOut.getVersion());
	buildChecker(v, wpAnalogOut2.getVersion());
	buildChecker(v, wpAnalogOut.getVersion());
	buildChecker(v, wpAnalogOut2.getVersion());
	buildChecker(v, wpRpm.getVersion());
	buildChecker(v, wpImpulseCounter.getVersion());
	buildChecker(v, wpWindow2.getVersion());
	buildChecker(v, wpWindow3.getVersion());
	buildChecker(v, wpWeight.getVersion());
	buildChecker(v, wpUnderfloor1.getVersion());
	buildChecker(v, wpUnderfloor2.getVersion());
	buildChecker(v, wpUnderfloor3.getVersion());
	buildChecker(v, wpUnderfloor4.getVersion());
	buildChecker(v, wpDS18B20.getVersion());
	buildChecker(v, wpRFID.getVersion());
	buildChecker(v, wpClock.getVersion());
	buildChecker(v, wpSML.getVersion());
	buildChecker(v, wpModbus.getVersion());
	return v;
}
void buildChecker(uint16_t &v, uint16_t moduleBuild) {
	v = v > moduleBuild ? v : moduleBuild;
}

uint16_t getBuild() {
	uint16_t globalBuild = getGlobalBuild();
	uint16_t Build = getVersion();
	return globalBuild > Build ? globalBuild : Build;
}

