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
//# Revision     : $Rev:: 212                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.cpp 212 2024-10-16 09:30:20Z                        $ #
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
	if(wpModules.useModuleAnalogOut) {
		wpAnalogOut.init();
	}
	if(wpModules.useModuleAnalogOut2) {
		wpAnalogOut2.init();
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
	#endif
	wpModules.publishAllValues();
	wpModules.publishAllSettings();
	wpModules.setAllSubscribes();

	wpFZ.BootCount();
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
			wpWindow.cycle();
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
		if(wpModules.useModuleAnalogOut) {
			wpAnalogOut.cycle();
		}
		if(wpModules.useModuleAnalogOut2) {
			wpAnalogOut2.cycle();
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
		#endif
	}
	//delay(100);
}

//###################################################################################
// Allgemein
//###################################################################################
uint16 getVersion() {
	String SVN = "$Rev: 212 $";
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
	buildChecker(v, wpUpdate.getVersion());
	buildChecker(v, wpWebServer.getVersion());
	buildChecker(v, wpWiFi.getVersion());
	buildChecker(v, wpFZ.getVersion());

	buildChecker(v, wpDHT.getVersion());
	buildChecker(v, wpLDR.getVersion());
	buildChecker(v, wpLight.getVersion());
	buildChecker(v, wpBM.getVersion());
	buildChecker(v, wpWindow.getVersion());
	buildChecker(v, wpRelais.getVersion());
	buildChecker(v, wpRain.getVersion());
	buildChecker(v, wpMoisture.getVersion());
	buildChecker(v, wpDistance.getVersion());
	#if BUILDWITH == 1
	buildChecker(v, wpCwWw.getVersion());
	buildChecker(v, wpNeoPixel.getVersion());
	buildChecker(v, wpAnalogOut.getVersion());
	buildChecker(v, wpAnalogOut2.getVersion());
	#endif
	#if BUILDWITH == 2
	buildChecker(v, wpAnalogOut.getVersion());
	buildChecker(v, wpAnalogOut2.getVersion());
	buildChecker(v, wpRpm.getVersion());
	buildChecker(v, wpImpulseCounter.getVersion());
	#endif
	#if BUILDWITH == 3
	buildChecker(v, wpUnderfloor1.getVersion());
	buildChecker(v, wpUnderfloor2.getVersion());
	buildChecker(v, wpUnderfloor3.getVersion());
	buildChecker(v, wpUnderfloor4.getVersion());
	#endif
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

