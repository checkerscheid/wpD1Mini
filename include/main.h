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
#include <helperMqtt.h>

// values
String mqttTopicOnlineToggler;
// settings
String mqttTopicDeviceName;
String mqttTopicDeviceDescription;
String mqttTopicErrorOnline; // 1 Error
String mqttTopicRestartDevice;
String mqttTopicOnSince;
String mqttTopicOnDuration;
// commands
String mqttTopicSetDeviceName;
String mqttTopicSetDeviceDescription;
String mqttTopicCalcValues;


void getVars();
String getVersion();
void checkOfflineTrigger();
void callbackMqtt(char*, byte*, unsigned int);
void callbackMqttDebug(String topic, String value);

void publishValuesSystem();

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
