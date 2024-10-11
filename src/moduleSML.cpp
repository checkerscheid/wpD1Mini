//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 08.10.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRpm.cpp 207 2024-10-07 12:59:22Z                   $ #
//#                                                                                 #
//###################################################################################
#include <moduleSML.h>

moduleSML wpSML;

moduleSML::moduleSML() {
	// section to config and copy
	ModuleName = "SML";
	mb = new moduleBase(ModuleName);
}
void moduleSML::init() {
	// section for define

	// values
	// settings


	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules2, wpEEPROM.bitDebugSML);

	mb->calcLast = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleSML::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleSML::publishSettings() {
	publishSettings(false);
}
void moduleSML::publishSettings(bool force) {
	mb->publishSettings(force);
}

void moduleSML::publishValues() {
	publishValues(false);
}
void moduleSML::publishValues(bool force) {
	if(force) {
	}
	mb->publishValues(force);
}

void moduleSML::setSubscribes() {
	mb->setSubscribes();
}

void moduleSML::checkSubscribes(char* topic, String msg) {
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleSML::publishValue() {
}

void moduleSML::calc() {
	int i = 0, iHandler = 0;
	unsigned char c;
	sml_states_t s;
	for (i = 0; i < ehz_bin_len; ++i) {
		c = ehz_bin[i];
		s = smlState(c);
		if (s == SML_START) {
			/* reset local vars */
			wpSML.manuf[0] = 0;
			wpSML.T1Wh = -3;
			wpSML.SumWh = -3;
		}
		if (s == SML_LISTEND) {
			/* check handlers on last received list */
			for (iHandler = 0; OBISHandlers[iHandler].Handler != 0 &&
				!(smlOBISCheck(OBISHandlers[iHandler].OBIS)); iHandler++);
			if (OBISHandlers[iHandler].Handler != 0) {
				OBISHandlers[iHandler].Handler();
			}
		}
		if (s == SML_UNEXPECTED) {
			printf(">>> Unexpected byte! <<<\n");
		}
		if (s == SML_FINAL) {
			printf(">>> FINAL! Checksum OK\n");
			printf(">>> Manufacturer.............: %s\n", manuf);
			printf(">>> Power T1    (1-0:1.8.1)..: %.3f kWh\n", T1Wh);
			printf(">>> Power T1+T2 (1-0:1.8.0)..: %.3f kWh\n\n", SumWh);
			printf(">>> Watt        (1-0:15.7.0).: %.3f W\n\n", Watt);
		}
	}
}
void moduleSML::Manufacturer() {
	smlOBISManufacturer(manuf, MAX_STR_MANUF);
}

void moduleSML::PowerT1() {
	smlOBISWh(T1Wh);
}

void moduleSML::PowerSum() {
	smlOBISWh(SumWh);
}

void moduleSML::PowerW() {
	smlOBISW(Watt);
}


//###################################################################################
// section to copy
//###################################################################################
uint16 moduleSML::getVersion() {
	String SVN = "$Rev: 207 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleSML::changeDebug() {
	mb->changeDebug();
}
bool moduleSML::Debug() {
	return mb->debug;
}
bool moduleSML::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
