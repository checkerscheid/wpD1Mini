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
//# File-ID      : $Id:: main.h 121 2024-06-01 05:13:59Z                          $ #
//#                                                                                 #
//###################################################################################
#ifndef BasisEmpty_h
#define BasisEmpty_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <helperWiFi.h>
#include <helperMqtt.h>
#include <helperOnlineToggler.h>
#include <helperFinder.h>
#include <helperRest.h>
#include <helperWebServer.h>
#include <helperUpdate.h>
#include <moduleDHT.h>

uint16_t getVersion();
uint16_t getBuild();
String getStringVersion();
uint16_t getGlobalBuild();

//void publishValuesSystem();

String SVNh = "$Rev: 121 $";
// counter
uint16_t publishCountRssi = 0;

// value stores for Com
int rssi = 0;
#endif
