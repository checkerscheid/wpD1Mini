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
//# Revision     : $Rev:: 118                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 118 2024-05-29 01:29:33Z                          $ #
//#                                                                                 #
//###################################################################################
#ifndef BasisEmpty_h
#define BasisEmpty_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <helperFinder.h>
#include <helperMqtt.h>
#include <helperOnlineToggler.h>
#include <helperRest.h>
#include <helperUpdate.h>
#include <helperWebServer.h>
#include <helperWiFi.h>
#include <moduleDHT.h>

uint16_t getVersion();
String getStringVersion();
uint16_t getGlobalBuild();

void publishValuesSystem();

String SVNh = "$Rev: 118 $";
// counter
uint16_t publishCountRssi = 0;

// value stores for Com
int rssi = 0;
#endif
