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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 269 2025-07-01 19:25:14Z                          $ #
//#                                                                                 #
//###################################################################################
#ifndef BasisEmpty_h
#define BasisEmpty_h
#include <Arduino.h>
#include <wpFreakaZone.h>

uint16_t getVersion();
void buildChecker(uint16_t &v, uint16_t moduleBuild);
uint16_t getBuild();
String getStringVersion();
uint16_t getGlobalBuild();
void BuildChecker(uint16_t &v, uint16_t moduleBuild);

//void publishValuesSystem();

String SVNh = "$Rev: 269 $";
// counter
uint32_t publishForceRssi = 0;

// value stores for Com
int rssi = 0;
#endif
