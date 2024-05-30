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
#include <helperWiFi.h>

// values
String mqttTopicUpdateMode;
String mqttTopicRestartRequired;
String mqttTopicOnlineToggler;
// settings
String mqttTopicDeviceName;
String mqttTopicDeviceDescription;
String mqttTopicErrorOnline; // 1 Error
String mqttTopicOnSince;
String mqttTopicOnDuration;
String mqttTopicMqttServer;
String mqttTopicMqttSince;
String mqttTopicRestServer;
// commands
String mqttTopicSetDeviceName;
String mqttTopicSetDeviceDescription;
String mqttTopicRestartDevice;
String mqttTopicUpdateFW;
String mqttTopicForceMqttUpdate;
String mqttTopicForceRenewValue;
String mqttTopicCalcValues;
String mqttTopicDebugEprom;
String mqttTopicDebugMqtt;
String mqttTopicDebugFinder;
String mqttTopicDebugRest;
String mqttTopicErrorRest;


void getVars();
String getVersion();
void readStringsFromEEPROM();
int writeStringToEEPROM(int addrOffset, String &strToWrite);
const int8_t WebServerCommanddoNothing = -1;
const int8_t WebServerCommandblink = 1;
const int8_t WebServerCommandpublishSettings = 2;
const int8_t WebServerCommandupdateFW = 3;
const int8_t WebServerCommandrestartESP = 4;
const int8_t WebServerCommandscanWiFi = 5;
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

void publishValuesSystem();
void publishErrorRest();

String SVNh = "$Rev: 118 $";
String Revh;
String Rev;
int Buildh;
int Build;
// counter
uint16_t publishCountRssi = 0;

// value stores for Com
int rssi = 0;
#endif
