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
//# Revision     : $Rev:: 101                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 101 2024-05-16 23:36:36Z                          $ #
//#                                                                                 #
//###################################################################################
#ifndef BasisEmpty_h
#define BasisEmpty_h
#include <Arduino.h>

void getVars();
String getVersion();
void readStringsFromEEPROM();
int writeStringToEEPROM(int addrOffset, String &strToWrite);
void connectMqtt();
const int8_t WebServerCommanddoNothing = -1;
const int8_t WebServerCommandblink = 1;
const int8_t WebServerCommandpublishSettings = 2;
const int8_t WebServerCommandupdateFW = 3;
const int8_t WebServerCommandrestartESP = 4;
int8_t doWebServerCommand = WebServerCommanddoNothing;
int8_t doWebServerBlink = WebServerCommanddoNothing;
void setupWebServer();
void setWebServerCommand(int8_t command);
void setWebServerBlink();
void doTheWebServerCommand();
void doTheWebserverBlink();
void checkOfflineTrigger();
void setMqttOffline();
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
#ifdef wpMoisture
void calcMoisture();
#endif

String SVNh = "$Rev: 101 $";
String Revh;
String Rev;
int Buildh;
int Build;
// counter
uint16_t publishCountRssi = 0;

// value stores for Com
int rssi = 0;
#endif
