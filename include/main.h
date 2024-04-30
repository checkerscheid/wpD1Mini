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
//# Revision     : $Rev:: 71                                                      $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 71 2024-04-29 03:21:40Z                           $ #
//#                                                                                 #
//###################################################################################
#ifndef BasisEmpty_h
#define BasisEmpty_h
#include <Arduino.h>

void getVars();
void readStringsFromEEPROM();
int writeStringToEEPROM(int addrOffset, String &strToWrite);
void connectMqtt();
void setupWebServer();
void publishSettings();
void publishSettings(bool);
void publishInfo();
void publishInfoDebug(String name, String value, String publishCount);
void callbackMqtt(char*, byte*, unsigned int);
void callbackMqttDebug(String topic, String value);
#ifdef wpHT
void calcHT();
void calcHTDebug(String name, float value, float raw);
void calcHTError(String name);
#endif
#ifdef wpLDR
void calcLDR();
//uint16_t calcLdrAvg(uint16_t raw);
#endif
#ifdef wpLight
void calcLight();
//uint16_t calcLightAvg(uint16_t raw);
#endif
#ifdef wpBM
void calcBM();
#endif
#ifdef wpRain
void calcRain();
#endif
#ifdef wpDistance
void calcDistance();
uint8_t calcDistanceAvg(uint8_t raw);
void calcDistanceDebug(String name, uint8_t avg, uint8_t raw);
#endif

// counter
uint16_t publishCountRssi = 0;

// value stores for Com
int rssi = 0;
#endif
