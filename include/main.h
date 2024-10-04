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
//# Revision     : $Rev:: 203                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 203 2024-10-04 07:32:26Z                          $ #
//#                                                                                 #
//###################################################################################
#ifndef BasisEmpty_h
#define BasisEmpty_h
#include <Arduino.h>
#include <wpFreakaZone.h>

uint16 getVersion();
void buildChecker(uint16 &v, uint16 moduleBuild);
uint16 getBuild();
String getStringVersion();
uint16 getGlobalBuild();
void BuildChecker(uint16 &v, uint16 moduleBuild);

//void publishValuesSystem();

String SVNh = "$Rev: 203 $";
// counter
uint32 publishForceRssi = 0;

// value stores for Com
int rssi = 0;
#endif
