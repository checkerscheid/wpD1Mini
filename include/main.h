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

// values
String mqttTopicUpdateMode;
String mqttTopicRestartRequired;
String mqttTopicOnlineToggler;
// settings
String mqttTopicDeviceName;
String mqttTopicDeviceDescription;
String mqttTopicErrorOnline; // 1 Error
String mqttTopicVersion;
String mqttTopicwpFreakaZoneVersion;
String mqttTopicOnSince;
String mqttTopicOnDuration;
String mqttTopicSsid;
String mqttTopicIp;
String mqttTopicMac;
String mqttTopicWiFiSince;
String mqttTopicRssi;
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
String mqttTopicDebugWiFi;
String mqttTopicDebugMqtt;
String mqttTopicDebugFinder;
String mqttTopicDebugRest;
String mqttTopicErrorRest;
#ifdef wpLDR
	// values
	String mqttTopicLDR;
	String mqttTopicErrorLDR;
	// settings
	String mqttTopicUseLdrAvg;
	String mqttTopicMaxCycleLDR;
	String mqttTopicLdrCorrection;
	// commands
	String mqttTopicDebugLDR;
#endif
#ifdef wpLight
	// values
	String mqttTopicLight;
	String mqttTopicErrorLight;
	// settings
	String mqttTopicUseLightAvg;
	String mqttTopicMaxCycleLight;
	String mqttTopicLightCorrection;
	// commands
	String mqttTopicDebugLight;
#endif
#ifdef wpBM
	// values
	String mqttTopicBM;
	// settings
	#ifdef wpLDR
	String mqttTopicThreshold;
	String mqttTopicLightToTurnOn;
	#endif
	// commands
	String mqttTopicDebugBM;
#endif
#ifdef wpRelais
	// values
	String mqttTopicRelaisOut;
	String mqttTopicRelaisAuto;
	String mqttTopicRelaisHand;
	String mqttTopicSetRelaisHand;
	String mqttTopicErrorRelaisHand;
	String mqttTopicRelaisHandValue;
	String mqttTopicSetRelaisHandValue;
	// settings
	#ifdef wpMoisture
	String mqttTopicWaterEmpty;
	String mqttTopicPumpActive;
	String mqttTopicPumpPause;
	#endif
	// commands
	String mqttTopicDebugRelais;
#endif
#ifdef wpRain
	// values
	String mqttTopicRain;
	String mqttTopicErrorRain;
	// settings
	String mqttTopicMaxCycleRain;
	String mqttTopicUseRainAvg;
	String mqttTopicRainCorrection;
	// commands
	String mqttTopicDebugRain;
#endif
#ifdef wpMoisture
	// values
	String mqttTopicMoisture;
	String mqttTopicErrorMoisture;
	String mqttTopicErrorMoistureMin;
	// settings
	String mqttTopicMaxCycleMoisture;
	String mqttTopicUseMoistureAvg;
	String mqttTopicMoistureMin;
	String mqttTopicMoistureDry;
	String mqttTopicMoistureWet;
	// commands
	String mqttTopicDebugMoisture;
#endif
#ifdef wpDistance
	// values
	String mqttTopicVolume;
	String mqttTopicDistanceRaw;
	String mqttTopicDistanceAvg;
	String mqttTopicErrorDistance;
	// settings
	String mqttTopicMaxCycleDistance;
	String mqttTopicDistanceCorrection;
	String mqttTopicMaxVolume;
	String mqttTopicHeight;
	// commands
	String mqttTopicDebugDistance;
#endif

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
#ifdef wpLDR
void publishValueLDR();
void publishErrorLDR();
void calcLDR();
uint16_t calcLdrAvg(uint16_t raw);
#endif
#ifdef wpLight
void publishValueLight();
void publishErrorLight();
void calcLight();
uint16_t calcLightAvg(uint16_t raw);
#endif
#ifdef wpBM
void publishValueBM();
void calcBM();
#endif
#ifdef wpRelais
void publishValueRelaisOut();
void publishValueRelaisAuto();
void calcRelaisOut();
#ifdef wpMoisture
void calcRelais();
#endif
#endif
#ifdef wpRain
void publishValueRain();
void publishErrorRain();
void calcRain();
uint16_t calcRainAvg(uint16_t raw);
#endif
#ifdef wpMoisture
void publishValueMoisture();
void publishErrorMoisture();
void calcMoisture();
uint16_t calcMoistureAvg(uint16_t raw);
#endif
#ifdef wpDistance
void publishValueDistance();
void publishErrorDistance();
void calcDistance();
uint8_t calcDistanceAvg(uint8_t raw);
void calcDistanceDebug(String name, uint8_t avg, uint8_t raw);
#endif
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
