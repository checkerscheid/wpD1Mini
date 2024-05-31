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
//# Revision     : $Rev:: 119                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 119 2024-05-31 03:31:43Z                          $ #
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
String getStringVersion();
uint16_t getGlobalBuild();

//void publishValuesSystem();

String SVNh = "$Rev: 119 $";
// counter
uint16_t publishCountRssi = 0;

// value stores for Com
int rssi = 0;
#endif
