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
//# Revision     : $Rev:: 190                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 190 2024-08-14 02:34:46Z                          $ #
//#                                                                                 #
//###################################################################################
#ifndef BasisEmpty_h
#define BasisEmpty_h
#include <Arduino.h>
#include <helperEEPROM.h>
#include <helperFinder.h>
#include <helperModules.h>
#include <helperMqtt.h>
#include <helperOnlineToggler.h>
#include <helperRest.h>
#include <helperUpdate.h>
#include <helperWebServer.h>
#include <helperWiFi.h>
#include <moduleBase.h>
#include <moduleDHT.h>
#include <moduleLDR.h>
#include <moduleLight.h>
#include <moduleBM.h>
#include <moduleWindow.h>
#include <moduleAnalogOut.h>
#include <moduleAnalogOut2.h>
#include <moduleNeoPixel.h>
#include <moduleRelais.h>
#include <moduleRpm.h>
#include <moduleRain.h>
#include <moduleMoisture.h>
#include <moduleDistance.h>
#include <moduleImpulseCounter.h>
#include <wpFreakaZone.h>

uint16 getVersion();
void buildChecker(uint16 &v, uint16 moduleBuild);
uint16 getBuild();
String getStringVersion();
uint16 getGlobalBuild();
void BuildChecker(uint16 &v, uint16 moduleBuild);

//void publishValuesSystem();

String SVNh = "$Rev: 190 $";
// counter
uint32 publishForceRssi = 0;

// value stores for Com
int rssi = 0;
#endif
