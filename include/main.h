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
//# Revision     : $Rev:: 125                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 125 2024-06-03 03:11:11Z                          $ #
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
#include <moduleRelais.h>
#include <moduleRain.h>
#include <moduleMoisture.h>
#include <moduleDistance.h>
#include <wpFreakaZone.h>

uint16_t getVersion();
void buildChecker(uint16_t &v, uint16 moduleBuild);
uint16_t getBuild();
String getStringVersion();
uint16_t getGlobalBuild();
void BuildChecker(uint16_t &v, uint16 moduleBuild);

//void publishValuesSystem();

String SVNh = "$Rev: 125 $";
// counter
uint16_t publishCountRssi = 0;

// value stores for Com
int rssi = 0;
#endif
