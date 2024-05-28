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
//# Revision     : $Rev:: 116                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.cpp 116 2024-05-26 02:25:10Z                        $ #
//#                                                                                 #
//###################################################################################
#include <main.h>
#include <wpFreakaZone.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define SHIELD 1

uint loopTime = 200;
//###################################################################################
//  Value Defines
//###################################################################################
#ifdef wpHT
	#include <DHT.h>
	#define DHTPin D7
	uint cycleHT = 0;
	bool errorHT = false;
	bool errorHTLast = false;
	uint16_t publishCountErrorHT = 0;
	float temperature = 0.0;
	int temperatureLast = 0;
	uint16_t publishCountTemperature = 0;
	float humidity = 0.0;
	int humidityLast = 0;
	uint16_t publishCountHumidity = 0;
#endif
#ifdef wpLDR
	#define LDRPin A0
	uint cycleLDR = 0;
	bool errorLDR = false;
	bool errorLDRLast = false;
	uint16_t publishCountErrorLDR = 0;
	int16_t ldr = 0;
	int16_t ldrLast = 0;
	uint16_t publishCountLDR = 0;
#endif
// uses PIN D1 (SCL) & D2 (SDA) for I2C Bus
#ifdef wpLight
	#include <Wire.h>
	#include <AS_BH1750.h>
	uint cycleLight = 0;
	bool errorLight = false;
	bool errorLightLast = false;
	uint16_t publishCountErrorLight = 0;
	uint16_t light = 0;
	uint16_t lightLast = 0;
	uint16_t publishCountLight = 0;
#endif
#ifdef wpBM
	#define BMPin D5
	//bool errorBM = false;
	bool bm = false;
	bool bmLast = false;
	uint16_t publishCountBM = 0;
#endif
#ifdef wpRelais
	/// @todo need a better solution for switching Pins
	//if(wpRelais == SHIELD) {
		#define RelaisPin D1
	//} else {
	//	#define RelaisPin D6
	//}

	//onshield Relais

	bool relaisOut = false;
	bool relaisOutLast = false;
	uint16_t publishCountRelaisOut = 0;
	bool relaisAuto = false;
	bool relaisAutoLast = false;
	uint16_t publishCountRelaisAuto = 0;
	bool relaisHand = false;
	bool relaisHandLast = false;
	uint16_t publishCountRelaisHand = 0;
	bool setRelaisHand = false;
	bool errorRelaisHand = false;
	bool relaisHandValue = false;
	bool relaisHandValueLast = false;
	uint16_t publishCountRelaisHandValue = 0;
	bool setRelaisHandValue = false;
	#ifdef wpMoisture
	bool pumpCycleActive = false;
	bool pumpStart = false;
	bool pumpPause = false;
	unsigned long timePumpStart = 0;
	unsigned long timePumpPause = 0;
	#endif
#endif
#ifdef wpRain
	#define RainPin A0
	uint cycleRain = 0;
	bool errorRain = false;
	bool errorRainLast = false;
	uint16_t publishCountErrorRain = 0;
	double rain = 0.0;
	double rainLast = 0.0;
	uint16_t publishCountRain = 0;
#endif
#ifdef wpMoisture
	#define MoisturePin A0
	uint cycleMoisture = 0;
	bool errorMoisture = false;
	bool errorMoistureLast = false;
	uint16_t publishCountErrorMoisture = 0;
	bool errorMoistureMin = false;
	bool errorMoistureMinLast = false;
	uint16_t publishCountErrorMoistureMin = 0;
	uint16_t moisture = 0;
	uint16_t moistureLast = 0;
	uint16_t publishCountMoisture = 0;
#endif
#ifdef wpDistance
	#define trigPin D1
	#define echoPin D2
	uint cycleDistance = 0;
	bool errorDistance = false;
	bool errorDistanceLast = false;
	uint16_t publishCountErrorDistance = 0;
	uint16_t volume = 0;
	uint16_t volumeLast = 0;
	uint8_t distanceRaw = 0;
	uint8_t distanceRawLast = 0;
	uint8_t distanceAvg = 0;
	uint8_t distanceAvgLast = 0;
	uint16_t publishCountVolume = 0;
	uint16_t publishCountDistanceRaw = 0;
	uint16_t publishCountDistanceAvg = 0;
#endif
bool errorRestLast = false;
bool trySendRest = false;
uint16_t publishCountErrorRest = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
#ifdef wpHT
	DHT dht(DHTPin, wpHT);
#endif
#ifdef wpLight
	AS_BH1750 lightMeter; // uses PIN D1 & D2 for I2C Bus
#endif

//###################################################################################
//  setup
//###################################################################################
void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	Serial.begin(9600);
	while(!Serial) {}
	EEPROM.begin(4095);
	//writeStringsToEEPROM(); // Fill EEPROM - only first time!!
	wpFZ.printStart();
	getVars();
	wpFZ.printRestored();
	wpFZ.setMainVersion(getVersion());
	wpFZ.setupWiFi();
	wpFZ.OnSince = wpFZ.getDateTime();
	wpFZ.OnDuration = wpFZ.getOnlineTime(false);
	mqttClient.setServer(wpFZ.mqttServer, wpFZ.mqttServerPort);
	mqttClient.setCallback(callbackMqtt);
	connectMqtt();
	setupWebServer();
	wpFZ.setupFinder();
#ifdef wpHT
	dht.begin();
#endif
#ifdef wpLight
	lightMeter.begin();
#endif
#ifdef wpBM
	pinMode(BMPin, INPUT);
#endif
#ifdef wpRelais
	pinMode(RelaisPin, OUTPUT);
#endif
#ifdef wpDistance
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	digitalWrite(trigPin, LOW);
#endif
}

//###################################################################################
// loop
//###################################################################################
void loop() {
	doTheWebServerCommand();
	doTheWebserverBlink();
	checkOfflineTrigger();
	if(wpFZ.UpdateFW) ArduinoOTA.handle();
	if(WiFi.status() == WL_CONNECTED) {
		digitalWrite(LED_BUILTIN, LOW);
	} else {
		digitalWrite(LED_BUILTIN, HIGH);
		wpFZ.setupWiFi();
	}
	if(!mqttClient.connected()) {
		connectMqtt();
	}
	if(wpFZ.calcValues) {
#ifdef wpHT
		if(++cycleHT >= wpFZ.maxCycleHT) {
			cycleHT = 0;
			calcHT();
		}
#endif
#ifdef wpLDR
		if(++cycleLDR >= wpFZ.maxCycleLDR) {
			cycleLDR = 0;
			calcLDR();
		}
#endif
#ifdef wpLight
		if(++cycleLight > wpFZ.maxCycleLight) {
			cycleLight = 0;
			calcLight();
		}
#endif
#ifdef wpBM
		calcBM();
#endif
#ifdef wpRelais
		calcRelaisOut();
#ifdef wpMoisture
		calcRelais();
#endif
#endif
#ifdef wpRain
		if(++cycleRain >= wpFZ.maxCycleRain) {
			cycleRain = 0;
			calcRain();
		}
#endif
#ifdef wpMoisture
		if(++cycleMoisture >= wpFZ.maxCycleMoisture) {
			cycleMoisture = 0;
			calcMoisture();
		}
#endif
#ifdef wpDistance
		if(++cycleDistance >= (wpFZ.maxCycleDistance)) {
			cycleDistance = 0;
			calcDistance();
		} else {
			delay(loopTime);
		}
#endif
		publishInfo();
	}
	wpFZ.loop();
	mqttClient.loop();
#ifndef wpDistance
	delay(loopTime);
#endif
}

//###################################################################################
//  EEPROM
//###################################################################################
// strings
byte byteStartForString = 50;
void getVars() {
	wpFZ.UpdateFW = false;
	wpFZ.settingsBool1 = EEPROM.read(wpFZ.addrSettingsBool1);
	wpFZ.DebugEprom = bitRead(wpFZ.settingsBool1, wpFZ.bitDebugEprom);
	wpFZ.DebugWiFi = bitRead(wpFZ.settingsBool1, wpFZ.bitDebugWiFi);
	wpFZ.DebugMqtt = bitRead(wpFZ.settingsBool1, wpFZ.bitDebugMqtt);
	wpFZ.DebugFinder = bitRead(wpFZ.settingsBool1, wpFZ.bitDebugFinder);
	wpFZ.DebugRest = bitRead(wpFZ.settingsBool1, wpFZ.bitDebugRest);
	wpFZ.settingsBool2 = EEPROM.read(wpFZ.addrSettingsBool2);
#ifdef wpHT
	wpFZ.DebugHT = bitRead(wpFZ.settingsBool2, wpFZ.bitDebugHT);
	wpFZ.maxCycleHT = EEPROM.read(wpFZ.addrMaxCycleHT);
	EEPROM.get(wpFZ.addrTemperatureCorrection, wpFZ.temperatureCorrection);
	EEPROM.get(wpFZ.addrHumidityCorrection, wpFZ.humidityCorrection);
#endif
#ifdef wpLDR
	wpFZ.DebugLDR = bitRead(wpFZ.settingsBool2, wpFZ.bitDebugLDR);
	wpFZ.maxCycleLDR = EEPROM.read(wpFZ.addrMaxCycleLDR);
	EEPROM.get(wpFZ.addrLdrCorrection, wpFZ.ldrCorrection);
#endif
#ifdef wpLight
	wpFZ.DebugLight = bitRead(wpFZ.settingsBool2, wpFZ.bitDebugLight);
	wpFZ.maxCycleLight = EEPROM.read(wpFZ.addrMaxCycleLight);
	EEPROM.get(wpFZ.addrLightCorrection, wpFZ.lightCorrection);
#endif
#ifdef wpBM
	wpFZ.DebugBM = bitRead(wpFZ.settingsBool2, wpFZ.bitDebugBM);
#ifdef wpLDR
	EEPROM.get(wpFZ.addrThreshold, wpFZ.threshold);
#endif
#endif
#ifdef wpRelais
	wpFZ.DebugRelais = bitRead(wpFZ.settingsBool2, wpFZ.bitDebugRelais);
	wpFZ.relaisHand = bitRead(wpFZ.settingsBool3, wpFZ.bitRelaisHand);
	wpFZ.relaisHandValue = bitRead(wpFZ.settingsBool3, wpFZ.bitRelaisHandValue);
	#ifdef wpMoisture
	EEPROM.get(wpFZ.addrPumpActive, wpFZ.pumpActive);
	EEPROM.get(wpFZ.addrPumpPause, wpFZ.pumpPause);
#endif
#endif
#ifdef wpRain
	wpFZ.DebugRain = bitRead(wpFZ.settingsBool2, wpFZ.bitDebugRain);
	wpFZ.maxCycleRain = EEPROM.read(wpFZ.addrMaxCycleRain);
	EEPROM.get(wpFZ.addrRainCorrection, wpFZ.rainCorrection);
#endif
#ifdef wpDistance
	wpFZ.DebugDistance = bitRead(wpFZ.settingsBool2, wpFZ.bitDebugDistance);
	wpFZ.maxCycleDistance = EEPROM.read(wpFZ.addrMaxCycleDistance);
	wpFZ.distanceCorrection = EEPROM.read(wpFZ.addrDistanceCorrection);
	wpFZ.height = EEPROM.read(wpFZ.addrHeight);
	EEPROM.get(wpFZ.addrMaxVolume, wpFZ.maxVolume);
#endif
#ifdef wpMoisture
	wpFZ.DebugMoisture = bitRead(wpFZ.settingsBool2, wpFZ.bitDebugMoisture);
	wpFZ.maxCycleMoisture = EEPROM.read(wpFZ.addrMaxCycleMoisture);
	wpFZ.moistureMin = EEPROM.read(wpFZ.addrMoistureMin);
	EEPROM.get(wpFZ.addrMoistureDry, wpFZ.moistureDry);
	EEPROM.get(wpFZ.addrMoistureWet, wpFZ.moistureWet);
#endif
	readStringsFromEEPROM();

	// values
	mqttTopicUpdateMode = wpFZ.DeviceName + "/UpdateMode";
	mqttTopicRestartRequired = wpFZ.DeviceName + "/RestartRequired";
	mqttTopicErrorOnline = wpFZ.DeviceName + "/ERROR/Online";
	// settings
	mqttTopicDeviceName = wpFZ.DeviceName + "/info/DeviceName";
	mqttTopicDeviceDescription = wpFZ.DeviceName + "/info/DeviceDescription";
	mqttTopicOnlineToggler = wpFZ.DeviceName + "/info/Online";
	mqttTopicVersion = wpFZ.DeviceName + "/info/Version";
	mqttTopicwpFreakaZoneVersion = wpFZ.DeviceName + "/info/wpFreakaZone";
	mqttTopicOnSince = wpFZ.DeviceName + "/info/OnSince";
	mqttTopicOnDuration = wpFZ.DeviceName + "/info/OnDuration";
	mqttTopicSsid = wpFZ.DeviceName + "/info/WiFi/SSID";
	mqttTopicIp = wpFZ.DeviceName + "/info/WiFi/Ip";
	mqttTopicMac = wpFZ.DeviceName + "/info/WiFi/Mac";
	mqttTopicWiFiSince = wpFZ.DeviceName + "/info/WiFi/Since";
	mqttTopicRssi = wpFZ.DeviceName + "/info/WiFi/RSSI";
	mqttTopicMqttServer = wpFZ.DeviceName + "/info/MQTT/Server";
	mqttTopicMqttSince = wpFZ.DeviceName + "/info/MQTT/Since";
	mqttTopicRestServer = wpFZ.DeviceName + "/info/Rest/Server";
	// commands
	mqttTopicSetDeviceName = wpFZ.DeviceName + "/settings/DeviceName";
	mqttTopicSetDeviceDescription = wpFZ.DeviceName + "/settings/DeviceDescription";
	mqttTopicRestartDevice = wpFZ.DeviceName + "/RestartDevice";
	mqttTopicUpdateFW = wpFZ.DeviceName + "/UpdateFW";
	mqttTopicForceMqttUpdate = wpFZ.DeviceName + "/ForceMqttUpdate";
	mqttTopicForceRenewValue = wpFZ.DeviceName + "/ForceRenewValue";
	mqttTopicCalcValues = wpFZ.DeviceName + "/settings/calcValues";
	mqttTopicDebugEprom = wpFZ.DeviceName + "/settings/Debug/Eprom";
	mqttTopicDebugWiFi = wpFZ.DeviceName + "/settings/Debug/WiFi";
	mqttTopicDebugMqtt = wpFZ.DeviceName + "/settings/Debug/MQTT";
	mqttTopicDebugFinder = wpFZ.DeviceName + "/settings/Debug/Finder";
	mqttTopicDebugRest = wpFZ.DeviceName + "/settings/Debug/Rest";
	mqttTopicErrorRest = wpFZ.DeviceName + "/ERROR/Rest";
#ifdef wpHT
	// values
	mqttTopicTemperature = wpFZ.DeviceName + "/Temperature";
	mqttTopicHumidity = wpFZ.DeviceName + "/Humidity";
	mqttTopicErrorHT = wpFZ.DeviceName + "/ERROR/HT";
	// settings
	mqttTopicMaxCycleHT = wpFZ.DeviceName + "/settings/HT/maxCycle";
	mqttTopicTemperatureCorrection = wpFZ.DeviceName + "/settings/HT/Correction/Temperature";
	mqttTopicHumidityCorrection = wpFZ.DeviceName + "/settings/HT/Correction/Humidity";
	// commands
	mqttTopicDebugHT = wpFZ.DeviceName + "/settings/Debug/HT";
#endif
#ifdef wpLDR
	// values
	mqttTopicLDR = wpFZ.DeviceName + "/LDR";
	mqttTopicErrorLDR = wpFZ.DeviceName + "/ERROR/LDR";
	// settings
	mqttTopicMaxCycleLDR = wpFZ.DeviceName + "/settings/LDR/maxCycle";
	mqttTopicUseLdrAvg = wpFZ.DeviceName + "/settings/LDR/useAvg";
	mqttTopicLdrCorrection = wpFZ.DeviceName + "/settings/LDR/Correction";
	// commands
	mqttTopicDebugLDR = wpFZ.DeviceName + "/settings/Debug/LDR";
#endif
#ifdef wpLight
	// values
	mqttTopicLight = wpFZ.DeviceName + "/Light";
	mqttTopicErrorLight = wpFZ.DeviceName + "/ERROR/Light";
	// settings
	mqttTopicMaxCycleLight = wpFZ.DeviceName + "/settings/Light/maxCycle";
	mqttTopicUseLightAvg = wpFZ.DeviceName + "/settings/Light/useAvg";
	mqttTopicLightCorrection = wpFZ.DeviceName + "/settings/Light/Correction";
	// commands
	mqttTopicDebugLight = wpFZ.DeviceName + "/settings/Debug/Light";
#endif
#ifdef wpBM
	// values
	mqttTopicBM = wpFZ.DeviceName + "/BM";
	// settings
#ifdef wpLDR
	mqttTopicThreshold = wpFZ.DeviceName + "/settings/BM/Threshold";
	mqttTopicLightToTurnOn = wpFZ.DeviceName + "/settings/BM/LightToTurnOn";
#endif
	// commands
	mqttTopicDebugBM = wpFZ.DeviceName + "/settings/Debug/BM";
#endif
#ifdef wpRelais
	// values
	mqttTopicRelaisOut = wpFZ.DeviceName + "/Relais/Out";
	mqttTopicRelaisAuto = wpFZ.DeviceName + "/Relais/Auto";
	mqttTopicRelaisHand = wpFZ.DeviceName + "/Relais/Hand";
	mqttTopicSetRelaisHand = wpFZ.DeviceName + "/Relais/setHand";
	/// @note duplicate Hand?
	mqttTopicErrorRelaisHand = wpFZ.DeviceName + "/ERROR/RelaisHand";
	mqttTopicRelaisHandValue = wpFZ.DeviceName + "/Relais/HandValue";
	mqttTopicSetRelaisHandValue = wpFZ.DeviceName + "/Relais/setHandValue";
	// settings
#ifdef wpMoisture
	mqttTopicWaterEmpty = wpFZ.DeviceName + "/settings/Relais/waterEmpty";
	mqttTopicPumpActive = wpFZ.DeviceName + "/settings/Relais/pumpActive";
	mqttTopicPumpPause = wpFZ.DeviceName + "/settings/Relais/pumpPause";
#endif
	// commands
	mqttTopicDebugRelais = wpFZ.DeviceName + "/settings/Debug/Relais";
#endif
#ifdef wpRain
	// values
	mqttTopicRain = wpFZ.DeviceName + "/Rain";
	mqttTopicErrorRain = wpFZ.DeviceName + "/ERROR/Rain";
	// settings
	mqttTopicMaxCycleRain = wpFZ.DeviceName + "/settings/Rain/maxCycle";
	mqttTopicUseRainAvg = wpFZ.DeviceName + "/settings/Rain/useAvg";
	mqttTopicRainCorrection = wpFZ.DeviceName + "/settings/Rain/Correction";
	// commands
	mqttTopicDebugRain = wpFZ.DeviceName + "/settings/Debug/Rain";
#endif
#ifdef wpMoisture
	// values
	mqttTopicMoisture = wpFZ.DeviceName + "/Moisture";
	mqttTopicErrorMoisture = wpFZ.DeviceName + "/ERROR/Moisture";
	mqttTopicErrorMoistureMin = wpFZ.DeviceName + "/ERROR/MoistureMin";
	// settings
	mqttTopicMaxCycleMoisture = wpFZ.DeviceName + "/settings/Moisture/maxCycle";
	mqttTopicUseMoistureAvg = wpFZ.DeviceName + "/settings/Moisture/useAvg";
	mqttTopicMoistureMin = wpFZ.DeviceName + "/settings/Moisture/Min";
	mqttTopicMoistureDry = wpFZ.DeviceName + "/settings/Moisture/Dry";
	mqttTopicMoistureWet = wpFZ.DeviceName + "/settings/Moisture/Wet";
	// commands
	mqttTopicDebugMoisture = wpFZ.DeviceName + "/settings/Debug/Moisture";
#endif
#ifdef wpDistance
	// values
	mqttTopicVolume = wpFZ.DeviceName + "/Volume";
	mqttTopicDistanceRaw = wpFZ.DeviceName + "/Distance/raw";
	mqttTopicDistanceAvg = wpFZ.DeviceName + "/Distance/avg";
	mqttTopicErrorDistance = wpFZ.DeviceName + "/ERROR/Distance";
	// settings
	mqttTopicMaxCycleDistance = wpFZ.DeviceName + "/settings/Distance/maxCycle";
	mqttTopicDistanceCorrection = wpFZ.DeviceName + "/settings/Distance/Correction";
	mqttTopicMaxVolume = wpFZ.DeviceName + "/settings/Distance/maxVolume";
	mqttTopicHeight = wpFZ.DeviceName + "/settings/Distance/height";
	// commands
	mqttTopicDebugDistance = wpFZ.DeviceName + "/settings/Debug/Distance";
#endif
}
void readStringsFromEEPROM() {
	byteStartForString = 50; // reset
	wpFZ.DeviceName = wpFZ.readStringFromEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = byteStartForString + 1 + wpFZ.DeviceName.length();
	wpFZ.DeviceDescription = wpFZ.readStringFromEEPROM(byteStartForString, wpFZ.DeviceDescription);
#ifdef wpBM
#ifdef wpLDR
	byteStartForString = byteStartForString + 1 + wpFZ.DeviceDescription.length();
	wpFZ.lightToTurnOn = wpFZ.readStringFromEEPROM(byteStartForString, wpFZ.lightToTurnOn);
#endif
#endif
}
void writeStringsToEEPROM() {
	byteStartForString = 50; // reset
	byteStartForString = wpFZ.writeStringToEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = wpFZ.writeStringToEEPROM(byteStartForString, wpFZ.DeviceDescription);
#ifdef wpBM
#ifdef wpLDR
	byteStartForString = wpFZ.writeStringToEEPROM(byteStartForString, wpFZ.lightToTurnOn);
#endif
#endif
}

//###################################################################################
// Allgemein
//###################################################################################
String getVersion() {
	Rev = "$Rev: 116 $";
	Rev.remove(0, 6);
	Rev.remove(Rev.length() - 2, 2);
	Build = Rev.toInt();
	Revh = SVNh;
	Revh.remove(0, 6);
	Revh.remove(Revh.length() - 2, 2);
	Buildh = Revh.toInt();
	String returns = "Build " +
		String(Buildh) + "." + String(Build);
	return returns;
}
void connectMqtt() {
	String logmessage = "Connecting MQTT Server: " + String(wpFZ.mqttServer) + ":" + String(wpFZ.mqttServerPort) + " as " + wpFZ.DeviceName;
	wpFZ.DebugWS(wpFZ.strINFO, "connectMqtt", logmessage);
	while(!mqttClient.connected()) {
		if(mqttClient.connect(wpFZ.DeviceName.c_str())) {
			wpFZ.MqttSince = wpFZ.getDateTime();
			String logmessage = "MQTT Connected";
			wpFZ.DebugWS(wpFZ.strINFO, "connectMqtt", logmessage);
			publishSettings();
			// subscribes
			mqttClient.subscribe(mqttTopicSetDeviceName.c_str());
			mqttClient.subscribe(mqttTopicSetDeviceDescription.c_str());
			mqttClient.subscribe(mqttTopicOnlineToggler.c_str());
			mqttClient.subscribe(mqttTopicRestartDevice.c_str());
			mqttClient.subscribe(mqttTopicUpdateFW.c_str());
			mqttClient.subscribe(mqttTopicForceMqttUpdate.c_str());
			mqttClient.subscribe(mqttTopicForceRenewValue.c_str());
			mqttClient.subscribe(mqttTopicDebugEprom.c_str());
			mqttClient.subscribe(mqttTopicDebugWiFi.c_str());
			mqttClient.subscribe(mqttTopicDebugMqtt.c_str());
			mqttClient.subscribe(mqttTopicDebugFinder.c_str());
			mqttClient.subscribe(mqttTopicDebugRest.c_str());
#ifdef wpHT
			mqttClient.subscribe(mqttTopicMaxCycleHT.c_str());
			mqttClient.subscribe(mqttTopicTemperatureCorrection.c_str());
			mqttClient.subscribe(mqttTopicHumidityCorrection.c_str());
			mqttClient.subscribe(mqttTopicDebugHT.c_str());
#endif
#ifdef wpLDR
			mqttClient.subscribe(mqttTopicMaxCycleLDR.c_str());
			mqttClient.subscribe(mqttTopicUseLdrAvg.c_str());
			mqttClient.subscribe(mqttTopicLdrCorrection.c_str());
			mqttClient.subscribe(mqttTopicDebugLDR.c_str());
#endif
#ifdef wpLight
			mqttClient.subscribe(mqttTopicMaxCycleLight.c_str());
			mqttClient.subscribe(mqttTopicUseLightAvg.c_str());
			mqttClient.subscribe(mqttTopicLightCorrection.c_str());
			mqttClient.subscribe(mqttTopicDebugLight.c_str());
#endif
#ifdef wpBM
	#ifdef wpLDR
			mqttClient.subscribe(mqttTopicThreshold.c_str());
			mqttClient.subscribe(mqttTopicLightToTurnOn.c_str());
	#endif
			mqttClient.subscribe(mqttTopicDebugBM.c_str());
#endif
#ifdef wpRelais
			mqttClient.subscribe(mqttTopicSetRelaisHand.c_str());
			mqttClient.subscribe(mqttTopicSetRelaisHandValue.c_str());
	#ifdef wpMoisture
			mqttClient.subscribe(mqttTopicPumpActive.c_str());
			mqttClient.subscribe(mqttTopicPumpPause.c_str());
	#endif
			mqttClient.subscribe(mqttTopicDebugRelais.c_str());
#endif
#ifdef wpRain
			mqttClient.subscribe(mqttTopicMaxCycleRain.c_str());
			mqttClient.subscribe(mqttTopicUseRainAvg.c_str());
			mqttClient.subscribe(mqttTopicRainCorrection.c_str());
			mqttClient.subscribe(mqttTopicDebugRain.c_str());
#endif
#ifdef wpMoisture
			mqttClient.subscribe(mqttTopicMaxCycleMoisture.c_str());
			mqttClient.subscribe(mqttTopicUseMoistureAvg.c_str());
			mqttClient.subscribe(mqttTopicMoistureMin.c_str());
			mqttClient.subscribe(mqttTopicMoistureDry.c_str());
			mqttClient.subscribe(mqttTopicMoistureWet.c_str());
			mqttClient.subscribe(mqttTopicDebugMoisture.c_str());
#endif
#ifdef wpDistance
			mqttClient.subscribe(mqttTopicMaxCycleDistance.c_str());
			mqttClient.subscribe(mqttTopicDistanceCorrection.c_str());
			mqttClient.subscribe(mqttTopicMaxVolume.c_str());
			mqttClient.subscribe(mqttTopicHeight.c_str());
			mqttClient.subscribe(mqttTopicDebugDistance.c_str());
#endif
		} else {
			String logmessage =  "failed, rc= " + String(mqttClient.state()) + ",  will try again in 5 seconds";
			wpFZ.DebugWS(wpFZ.strERRROR, "connectMqtt", logmessage);
			delay(5000);
		}
	}
}

void setupWebServer() {
	wpFZ.setupWebServer();

	wpFZ.server.on("/setCmd", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCmd");
		if(request->hasParam("cmd")) {
			if(request->getParam("cmd")->value() == "ForceMqttUpdate") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found ForceMqttUpdate");
				setWebServerCommand(WebServerCommandpublishSettings);
			}
			if(request->getParam("cmd")->value() == "UpdateFW") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found UpdateFW");
				setWebServerCommand(WebServerCommandupdateFW);
			}
			if(request->getParam("cmd")->value() == "RestartDevice") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found RestartDevice");
				setWebServerCommand(WebServerCommandrestartESP);
			}
			if(request->getParam("cmd")->value() == "calcValues") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found cmd calcValues");
				wpFZ.calcValues = !wpFZ.calcValues;
			}
			if(request->getParam("cmd")->value() == "Blink") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found Blink");
			}
#ifdef wpMoisture
#ifdef wpRelais
			if(request->getParam("cmd")->value() == "waterEmpty") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found cmd waterEmpty");
				wpFZ.waterEmpty = !wpFZ.waterEmpty;
			}
#endif
#endif
		}
		request->send(200);
		setWebServerBlink();
	});
	wpFZ.server.begin();
}
void setWebServerCommand(int8_t command) {
	doWebServerCommand = command;
}
void setWebServerBlink() {
	doWebServerBlink = WebServerCommandblink;
}
void doTheWebServerCommand() {
	if(doWebServerCommand > 0) {
		switch(doWebServerCommand) {
			case WebServerCommandpublishSettings:
				publishSettings(true);
				break;
			case WebServerCommandupdateFW:
				if(wpFZ.setupOta()) {
					wpFZ.UpdateFW = true;
				}
				break;
			case WebServerCommandrestartESP:
				setMqttOffline();
				ESP.restart();
				break;
		}
		doWebServerCommand = WebServerCommanddoNothing;
	}
}
void doTheWebserverBlink() {
	if(doWebServerBlink > 0) {
		wpFZ.blink();
		doWebServerBlink = WebServerCommanddoNothing;
	}
}
void checkOfflineTrigger() {
	if(wpFZ.OfflineTrigger) {
		// set offline for reboot
		setMqttOffline();
		wpFZ.OfflineTrigger = false;
	}
}
void setMqttOffline() {
	mqttClient.publish(mqttTopicErrorOnline.c_str(), String(1).c_str());
}

//###################################################################################
// publish settings
//###################################################################################
void publishSettings() {
	publishSettings(false);
}
void publishSettings(bool force) {
	mqttClient.publish(mqttTopicOnlineToggler.c_str(), String(1).c_str());
	// values
	mqttClient.publish(mqttTopicDeviceName.c_str(), wpFZ.DeviceName.c_str(), true);
	mqttClient.publish(mqttTopicDeviceDescription.c_str(), wpFZ.DeviceDescription.c_str(), true);
	mqttClient.publish(mqttTopicErrorOnline.c_str(), String(0).c_str());
	mqttClient.publish(mqttTopicVersion.c_str(), getVersion().c_str(), true);
	mqttClient.publish(mqttTopicwpFreakaZoneVersion.c_str(), wpFZ.getVersion().c_str(), true);
	mqttClient.publish(mqttTopicOnSince.c_str(), wpFZ.OnSince.c_str());
	mqttClient.publish(mqttTopicOnDuration.c_str(), wpFZ.OnDuration.c_str());
	mqttClient.publish(mqttTopicSsid.c_str(), wpFZ.ssid, true);
	mqttClient.publish(mqttTopicRssi.c_str(), String(WiFi.RSSI()).c_str());
	mqttClient.publish(mqttTopicIp.c_str(), WiFi.localIP().toString().c_str(), true);
	mqttClient.publish(mqttTopicMac.c_str(), WiFi.macAddress().c_str(), true);
	mqttClient.publish(mqttTopicWiFiSince.c_str(), wpFZ.WiFiSince.c_str());
	mqttClient.publish(mqttTopicMqttServer.c_str(), (String(wpFZ.mqttServer) + ":" + String(wpFZ.mqttServerPort)).c_str(), true);
	mqttClient.publish(mqttTopicMqttSince.c_str(), wpFZ.MqttSince.c_str());
	mqttClient.publish(mqttTopicRestServer.c_str(), (String(wpFZ.restServer) + ":" + String(wpFZ.restServerPort)).c_str(), true);
	// settings
	mqttClient.publish(mqttTopicSetDeviceName.c_str(), wpFZ.DeviceName.c_str());
	mqttClient.publish(mqttTopicSetDeviceDescription.c_str(), wpFZ.DeviceDescription.c_str());
	mqttClient.publish(mqttTopicCalcValues.c_str(), String(wpFZ.calcValues).c_str());
	mqttClient.publish(mqttTopicDebugEprom.c_str(), String(wpFZ.DebugEprom).c_str());
	mqttClient.publish(mqttTopicDebugWiFi.c_str(), String(wpFZ.DebugWiFi).c_str());
	mqttClient.publish(mqttTopicDebugMqtt.c_str(), String(wpFZ.DebugMqtt).c_str());
	mqttClient.publish(mqttTopicDebugFinder.c_str(), String(wpFZ.DebugFinder).c_str());
	mqttClient.publish(mqttTopicDebugRest.c_str(), String(wpFZ.DebugRest).c_str());
	mqttClient.publish(mqttTopicErrorRest.c_str(), String(wpFZ.ErrorRest).c_str());
#ifdef wpHT
	mqttClient.publish(mqttTopicMaxCycleHT.c_str(), String(wpFZ.maxCycleHT).c_str(), true);
	mqttClient.publish(mqttTopicTemperatureCorrection.c_str(), String(wpFZ.temperatureCorrection).c_str(), true);
	mqttClient.publish(mqttTopicHumidityCorrection.c_str(), String(wpFZ.humidityCorrection).c_str(), true);
	mqttClient.publish(mqttTopicDebugHT.c_str(), String(wpFZ.DebugHT).c_str());
	mqttClient.publish(mqttTopicErrorHT.c_str(), String(errorHT).c_str());
#endif
#ifdef wpLDR
	mqttClient.publish(mqttTopicMaxCycleLDR.c_str(), String(wpFZ.maxCycleLDR).c_str(), true);
	mqttClient.publish(mqttTopicUseLdrAvg.c_str(), String(wpFZ.useLdrAvg).c_str(), true);
	mqttClient.publish(mqttTopicLdrCorrection.c_str(), String(wpFZ.ldrCorrection).c_str(), true);
	mqttClient.publish(mqttTopicDebugLDR.c_str(), String(wpFZ.DebugLDR).c_str());
	mqttClient.publish(mqttTopicErrorLDR.c_str(), String(errorLDR).c_str());
#endif
#ifdef wpLight
	mqttClient.publish(mqttTopicMaxCycleLight.c_str(), String(wpFZ.maxCycleLight).c_str(), true);
	mqttClient.publish(mqttTopicUseLightAvg.c_str(), String(wpFZ.useLightAvg).c_str(), true);
	mqttClient.publish(mqttTopicLightCorrection.c_str(), String(wpFZ.lightCorrection).c_str(), true);
	mqttClient.publish(mqttTopicDebugLight.c_str(), String(wpFZ.DebugLight).c_str());
	mqttClient.publish(mqttTopicErrorLight.c_str(), String(errorLight).c_str());
#endif
#ifdef wpBM
	#ifdef wpLDR
	mqttClient.publish(mqttTopicThreshold.c_str(), String(wpFZ.threshold).c_str(), true);
	mqttClient.publish(mqttTopicLightToTurnOn.c_str(), wpFZ.lightToTurnOn.c_str(), true);
	#endif
	mqttClient.publish(mqttTopicDebugBM.c_str(), String(wpFZ.DebugBM).c_str());
#endif
#ifdef wpRelais
	mqttClient.publish(mqttTopicRelaisOut.c_str(), String(relaisOut).c_str());
	mqttClient.publish(mqttTopicRelaisAuto.c_str(), String(relaisAuto).c_str());
	mqttClient.publish(mqttTopicRelaisHand.c_str(), String(relaisHand).c_str());
	mqttClient.publish(mqttTopicErrorRelaisHand.c_str(), String(errorRelaisHand).c_str());
	mqttClient.publish(mqttTopicRelaisHandValue.c_str(), String(relaisHandValue).c_str());
	#ifdef wpMoisture
	mqttClient.publish(mqttTopicWaterEmpty.c_str(), String(wpFZ.waterEmpty).c_str());
	mqttClient.publish(mqttTopicPumpActive.c_str(), String(wpFZ.pumpActive).c_str());
	mqttClient.publish(mqttTopicPumpPause.c_str(), String(wpFZ.pumpPause).c_str());
	#endif
	mqttClient.publish(mqttTopicDebugRelais.c_str(), String(wpFZ.DebugRelais).c_str());
#endif
#ifdef wpRain
	mqttClient.publish(mqttTopicMaxCycleRain.c_str(), String(wpFZ.maxCycleRain).c_str(), true);
	mqttClient.publish(mqttTopicUseRainAvg.c_str(), String(wpFZ.useRainAvg).c_str(), true);
	mqttClient.publish(mqttTopicRainCorrection.c_str(), String(wpFZ.rainCorrection).c_str(), true);
	mqttClient.publish(mqttTopicDebugRain.c_str(), String(wpFZ.DebugRain).c_str());
	mqttClient.publish(mqttTopicErrorRain.c_str(), String(errorRain).c_str());
#endif
#ifdef wpMoisture
	mqttClient.publish(mqttTopicMaxCycleMoisture.c_str(), String(wpFZ.maxCycleMoisture).c_str(), true);
	mqttClient.publish(mqttTopicUseMoistureAvg.c_str(), String(wpFZ.useMoistureAvg).c_str(), true);
	mqttClient.publish(mqttTopicMoistureMin.c_str(), String(wpFZ.moistureMin).c_str(), true);
	mqttClient.publish(mqttTopicMoistureDry.c_str(), String(wpFZ.moistureDry).c_str(), true);
	mqttClient.publish(mqttTopicMoistureWet.c_str(), String(wpFZ.moistureWet).c_str(), true);
	mqttClient.publish(mqttTopicDebugMoisture.c_str(), String(wpFZ.DebugMoisture).c_str());
	mqttClient.publish(mqttTopicErrorMoisture.c_str(), String(errorMoisture).c_str());
	mqttClient.publish(mqttTopicErrorMoistureMin.c_str(), String(errorMoistureMin).c_str());
#endif
#ifdef wpDistance
	mqttClient.publish(mqttTopicMaxCycleDistance.c_str(), String(wpFZ.maxCycleDistance).c_str(), true);
	mqttClient.publish(mqttTopicDistanceCorrection.c_str(), String(wpFZ.distanceCorrection).c_str(), true);
	mqttClient.publish(mqttTopicMaxVolume.c_str(), String(wpFZ.maxVolume).c_str(), true);
	mqttClient.publish(mqttTopicHeight.c_str(), String(wpFZ.height).c_str(), true);
	mqttClient.publish(mqttTopicDebugDistance.c_str(), String(wpFZ.DebugDistance).c_str());
	mqttClient.publish(mqttTopicErrorDistance.c_str(), String(errorDistance).c_str());
#endif
	if(force) {
		mqttClient.publish(mqttTopicUpdateMode.c_str(), wpFZ.UpdateFW ? "On" : "Off");
		mqttClient.publish(mqttTopicForceMqttUpdate.c_str(), "0");
		mqttClient.publish(mqttTopicForceRenewValue.c_str(), "0");
	}
}

//###################################################################################
//  publish values Helper
//###################################################################################
#ifdef wpHT
void publishValueTemp(int equalVal) {
	mqttClient.publish(mqttTopicTemperature.c_str(), String(temperature).c_str());
	wpFZ.ErrorRest = wpFZ.ErrorRest | !wpFZ.sendRest("temp", String(temperature));
	trySendRest = true;
	temperatureLast = equalVal;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("Temperature", String(temperature), String(publishCountTemperature));
	}
	publishCountTemperature = 0;
}
void publishValueHum(int equalVal) {
	mqttClient.publish(mqttTopicHumidity.c_str(), String(humidity).c_str());
	wpFZ.ErrorRest = wpFZ.ErrorRest | !wpFZ.sendRest("hum", String(humidity));
	trySendRest = true;
	humidityLast = equalVal;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("Humidity", String(humidity), String(publishCountHumidity));
	}
	publishCountHumidity = 0;
}
void publishErrorHT() {
	mqttClient.publish(mqttTopicErrorHT.c_str(), String(errorHT).c_str());
	errorHTLast = errorHT;
	publishCountErrorHT = 0;
}
#endif
#ifdef wpLDR
void publishValueLDR() {
	mqttClient.publish(mqttTopicLDR.c_str(), String(ldr).c_str());
	wpFZ.ErrorRest = wpFZ.ErrorRest | !wpFZ.sendRest("ldr", String(ldr));
	trySendRest = true;
	ldrLast = ldr;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("LDR", String(ldr), String(publishCountLDR));
	}
	publishCountLDR = 0;
}
void publishErrorLDR() {
	mqttClient.publish(mqttTopicErrorLDR.c_str(), String(errorLDR).c_str());
	errorLDRLast = errorLDR;
	publishCountErrorLDR = 0;
}
#endif
#ifdef wpLight
void publishValueLight() {
	mqttClient.publish(mqttTopicLight.c_str(), String(light).c_str());
	wpFZ.ErrorRest = wpFZ.ErrorRest | !wpFZ.sendRest("light", String(light));
	trySendRest = true;
	lightLast = light;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("Light", String(light), String(publishCountLight));
	}
	publishCountLight = 0;
}
void publishErrorLight() {
	mqttClient.publish(mqttTopicErrorLight.c_str(), String(errorLight).c_str());
	errorLightLast = errorLight;
	publishCountErrorLight = 0;
}
#endif
#ifdef wpBM
void publishValueBM() {
	mqttClient.publish(mqttTopicBM.c_str(), String(bm).c_str());
	bmLast = bm;
#ifdef wpLDR
	if(bm && ldr <= wpFZ.threshold) {
		String lm = "MQTT Set Light (" + String(ldr) + " <= " + String(wpFZ.threshold) + ")";
		if(!wpFZ.lightToTurnOn.startsWith("_")) {
			if(wpFZ.lightToTurnOn.startsWith("http://")) {
				wpFZ.ErrorRest = wpFZ.ErrorRest | !wpFZ.sendRawRest(wpFZ.lightToTurnOn);
				trySendRest = true;
				lm += ", send REST '" + wpFZ.lightToTurnOn + "': 'On'";
			} else {
				mqttClient.publish(wpFZ.lightToTurnOn.c_str(), String("on").c_str());
				lm += ", send MQTT '" + wpFZ.lightToTurnOn + "': 'On'";
			}
		}
		wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", lm);
	}
#endif
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("BM", String(bm), String(publishCountBM));
	}
	publishCountBM = 0;
}
#endif
#ifdef wpRelais
void publishValueRelaisOut() {
	mqttClient.publish(mqttTopicRelaisOut.c_str(), String(relaisOut).c_str());
	wpFZ.ErrorRest = wpFZ.ErrorRest | !wpFZ.sendRest("RelaisOut", String(relaisOut));
	trySendRest = true;
	relaisOutLast = relaisOut;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("RelaisOut", String(relaisOut), String(publishCountRelaisOut));
	}
	publishCountRelaisOut = 0;
}
void publishValueRelaisAuto() {
	mqttClient.publish(mqttTopicRelaisAuto.c_str(), String(relaisAuto).c_str());
	relaisAutoLast = relaisAuto;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("RelaisAuto", String(relaisAuto), String(publishCountRelaisAuto));
	}
	publishCountRelaisAuto = 0;
}
#endif
#ifdef wpRain
void publishValueRain() {
	mqttClient.publish(mqttTopicRain.c_str(), String(rain).c_str());
	wpFZ.ErrorRest = wpFZ.ErrorRest | !wpFZ.sendRest("rain", String(rain));
	trySendRest = true;
	rainLast = rain;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("Rain", String(rain), String(publishCountRain));
	}
	publishCountRain = 0;
}
void publishErrorRain() {
	mqttClient.publish(mqttTopicErrorRain.c_str(), String(errorRain).c_str());
	errorRainLast = errorRain;
	publishCountErrorRain = 0;
}
#endif
#ifdef wpMoisture
void publishValueMoisture() {
	mqttClient.publish(mqttTopicMoisture.c_str(), String(moisture).c_str());
	wpFZ.sendRest("moisture", String(moisture));
	moistureLast = moisture;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("Moisture", String(moisture), String(publishCountMoisture));
	}
	publishCountMoisture = 0;
}
void publishErrorMoisture() {
	mqttClient.publish(mqttTopicErrorMoisture.c_str(), String(errorMoisture).c_str());
	errorMoistureLast = errorMoisture;
	publishCountErrorMoisture = 0;
}
void publishErrorMoistureMin() {
	mqttClient.publish(mqttTopicErrorMoistureMin.c_str(), String(errorMoistureMin).c_str());
	errorMoistureMinLast = errorMoistureMin;
	publishCountErrorMoistureMin = 0;
}
#endif
#ifdef wpDistance
void publishValueDistanceRaw() {
	mqttClient.publish(mqttTopicDistanceRaw.c_str(), String(distanceRaw).c_str());
	distanceRawLast = distanceRaw;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("distanceRaw", String(distanceRaw), String(publishCountDistanceRaw));
	}
	publishCountDistanceRaw = 0;
}
void publishValueDistanceAvg() {
	mqttClient.publish(mqttTopicDistanceAvg.c_str(), String(distanceAvg).c_str());
	distanceAvgLast = distanceAvg;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("distanceAvg", String(distanceAvg), String(publishCountDistanceAvg));
	}
	publishCountDistanceAvg = 0;
}
void publishValueVolume() {
	mqttClient.publish(mqttTopicVolume.c_str(), String(volume).c_str());
	wpFZ.ErrorRest = wpFZ.ErrorRest | !wpFZ.sendRest("vol", String(volume));
	trySendRest = true;
	volumeLast = volume;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("Volume", String(volume), String(publishCountVolume));
	}
	publishCountVolume = 0;
}
void publishErrorDistance() {
	mqttClient.publish(mqttTopicErrorDistance.c_str(), String(errorDistance).c_str());
	errorDistanceLast = errorDistance;
	publishCountErrorDistance = 0;
}
#endif
void publishValuesSystem() {
	mqttClient.publish(mqttTopicRssi.c_str(), String(WiFi.RSSI()).c_str());
	mqttClient.publish(mqttTopicOnSince.c_str(), wpFZ.OnSince.c_str());
	mqttClient.publish(mqttTopicOnDuration.c_str(), wpFZ.OnDuration.c_str());
	mqttClient.publish(mqttTopicWiFiSince.c_str(), wpFZ.WiFiSince.c_str());
	mqttClient.publish(mqttTopicMqttSince.c_str(), wpFZ.MqttSince.c_str());
	rssi = WiFi.RSSI();
	wpFZ.ErrorRest = wpFZ.ErrorRest | !wpFZ.sendRest("rssi", String(rssi));
	trySendRest = true;
	if(wpFZ.DebugMqtt) {
		publishInfoDebug("RSSI", String(rssi), String(publishCountRssi));
		publishInfoDebug("OnSince", String(wpFZ.OnSince), String(publishCountRssi));
		publishInfoDebug("OnDuration", String(wpFZ.OnDuration), String(publishCountRssi));
		publishInfoDebug("WiFiSince", String(wpFZ.WiFiSince), String(publishCountRssi));
		publishInfoDebug("MqttSince", String(wpFZ.MqttSince), String(publishCountRssi));
	}
	publishCountRssi = 0;
}
void publishErrorRest() {
	mqttClient.publish(mqttTopicErrorRest.c_str(), String(wpFZ.ErrorRest).c_str());
	errorRestLast = wpFZ.ErrorRest;
	trySendRest = false;
	if(wpFZ.DebugRest)
		publishInfoDebug("ErrorRest", String(wpFZ.ErrorRest), String(publishCountErrorRest));
	publishCountErrorRest = 0;
}

//###################################################################################
//  publish values
//###################################################################################
void publishValues() {
	wpFZ.ErrorRest = false;
	trySendRest = false;
#ifdef wpHT
	publishValueTemp(temperature);
	publishValueHum(humidity);
	publishErrorHT();
#endif
#ifdef wpLDR
	publishValueLDR();
	publishErrorLDR();
#endif
#ifdef wpLight
	publishValueLight();
	publishErrorLight();
#endif
#ifdef wpBM
	publishValueBM();
#endif
#ifdef wpRelais
	publishValueRelaisOut();
	publishValueRelaisAuto();
#endif
#ifdef wpRain
	publishValueRain();
	publishErrorRain();
#endif
#ifdef wpMoisture
	publishValueMoisture();
	publishErrorMoisture();
	publishErrorMoistureMin();
#endif
#ifdef wpDistance
	publishValueDistanceRaw();
	publishValueDistanceAvg();
	publishValueVolume();
	publishErrorDistance();
#endif
	publishValuesSystem();
	publishErrorRest();
}

void publishInfo() {
	wpFZ.ErrorRest = false;
	trySendRest = false;
#ifdef wpHT
	int tempEqual = (int) temperature * 10;
	if(temperatureLast != tempEqual || ++publishCountTemperature > wpFZ.publishQoS) {
		publishValueTemp(tempEqual);
	}
	int humEqual = (int) humidity * 10;
	if(humidityLast != humEqual || ++publishCountHumidity > wpFZ.publishQoS) {
		publishValueHum(humEqual);
	}
	if(errorHTLast != errorHT || ++publishCountErrorHT > wpFZ.publishQoS) {
		publishErrorHT();
	}
#endif
#ifdef wpLDR
	if(ldrLast != ldr || ++publishCountLDR > wpFZ.publishQoS) {
		publishValueLDR();
	}
	if(errorLDRLast != errorLDR || ++publishCountErrorLDR > wpFZ.publishQoS) {
		publishErrorLDR();
	}
#endif
#ifdef wpLight
	if(lightLast != light || ++publishCountLight > wpFZ.publishQoS) {
		publishValueLight();
	}
	if(errorLightLast != errorLight || ++publishCountErrorLight > wpFZ.publishQoS) {
		publishErrorLight();
	}
#endif
#ifdef wpBM
	if(bmLast != bm || ++publishCountBM > wpFZ.publishQoS) {
		publishValueBM();
	}
#endif
#ifdef wpRelais
	if(relaisOutLast != relaisOut || ++publishCountRelaisOut > wpFZ.publishQoS) {
		publishValueRelaisOut();
	}
	if(relaisAutoLast != relaisAuto || ++publishCountRelaisAuto > wpFZ.publishQoS) {
		publishValueRelaisAuto();
	}
#endif
#ifdef wpRain
	if(rainLast != rain || ++publishCountRain > wpFZ.publishQoS) {
		publishValueRain();
	}
	if(errorRainLast != errorRain || ++publishCountErrorRain > wpFZ.publishQoS) {
		publishErrorRain();
	}
#endif
#ifdef wpMoisture
	if(moistureLast != moisture || ++publishCountMoisture > wpFZ.publishQoS) {
		publishValueMoisture();
	}
	if(errorMoistureLast != errorMoisture || ++publishCountErrorMoisture > wpFZ.publishQoS) {
		publishErrorMoisture();
	}
	if(errorMoistureMinLast != errorMoistureMin || ++publishCountErrorMoistureMin > wpFZ.publishQoS) {
		publishErrorMoistureMin();
	}
#endif
#ifdef wpDistance
	if(distanceRawLast != distanceRaw || ++publishCountDistanceRaw > wpFZ.publishQoS) {
		publishValueDistanceRaw();
	}
	if(distanceAvgLast != distanceAvg || ++publishCountDistanceAvg > wpFZ.publishQoS) {
		publishValueDistanceAvg();
	}
	if(volumeLast != volume || ++publishCountVolume > wpFZ.publishQoS) {
		publishValueVolume();
	}
	if(errorDistanceLast != errorDistance || ++publishCountErrorDistance > wpFZ.publishQoS) {
		publishErrorDistance();
	}
#endif
	if(++publishCountRssi > 4 * 120) {
		publishValuesSystem();
	}
	if((errorRestLast != wpFZ.ErrorRest && trySendRest) || ++publishCountErrorRest > wpFZ.publishQoS) {
		publishErrorRest();
	}
}
void publishInfoDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
}

//###################################################################################
// set settings
//###################################################################################
void callbackMqtt(char* topic, byte* payload, unsigned int length) {
	String msg = "";
	for (unsigned int i = 0; i < length; i++) {
		msg += (char)payload[i];
	}
	if(wpFZ.DebugMqtt) {
		wpFZ.DebugWS(wpFZ.strDEBUG, "callbackMqtt", "Message arrived on topic: '" + String(topic) + "': " + msg);
	}
	if(msg == "") {
		wpFZ.DebugWS(wpFZ.strWARN, "callbackMqtt", "msg is empty, '" + String(topic) + "'");
	} else {
		if(strcmp(topic, mqttTopicSetDeviceName.c_str()) == 0) {
			if(strcmp(msg.c_str(), wpFZ.DeviceName.c_str()) != 0) {
				wpFZ.DeviceName = msg;
				writeStringsToEEPROM();
				mqttClient.publish(mqttTopicRestartRequired.c_str(), String(true).c_str());
				mqttClient.publish(mqttTopicRestartDevice.c_str(), String(false).c_str());
				callbackMqttDebug(mqttTopicDeviceName, wpFZ.DeviceName);
			}
		}
		if(strcmp(topic, mqttTopicSetDeviceDescription.c_str()) == 0) {
			if(strcmp(msg.c_str(), wpFZ.DeviceDescription.c_str()) != 0) {
				wpFZ.DeviceDescription = msg;
				writeStringsToEEPROM();
				callbackMqttDebug(mqttTopicDeviceDescription, wpFZ.DeviceDescription);
			}
		}
		if(strcmp(topic, mqttTopicOnlineToggler.c_str()) == 0) {
			int readOnline = msg.toInt();
			if(readOnline != 1) {
				//reset
				mqttClient.publish(mqttTopicOnlineToggler.c_str(), String(1).c_str());
			}
		}
		if(strcmp(topic, mqttTopicCalcValues.c_str()) == 0) {
			bool readCalcValues = msg.toInt();
			if(wpFZ.calcValues != readCalcValues) {
				wpFZ.calcValues = readCalcValues;
				callbackMqttDebug(mqttTopicCalcValues, String(wpFZ.calcValues));
			}
		}
		if(strcmp(topic, mqttTopicRestartDevice.c_str()) == 0) {
			int readRestartDevice = msg.toInt();
			if(readRestartDevice > 0) {
				setMqttOffline();
				callbackMqttDebug(mqttTopicRestartDevice, String(readRestartDevice));
				ESP.restart();
			}
		}
		if(strcmp(topic, mqttTopicUpdateFW.c_str()) == 0) {
			int readUpdateFW = msg.toInt();
			if(readUpdateFW > 0) {
				if(wpFZ.setupOta()) {
					wpFZ.UpdateFW = true;
					mqttClient.publish(mqttTopicUpdateMode.c_str(), "On");
				}
				callbackMqttDebug(mqttTopicUpdateFW, String(readUpdateFW));
			}
		}
		if(strcmp(topic, mqttTopicForceMqttUpdate.c_str()) == 0) {
			int readForceMqttUpdate = msg.toInt();
			if(readForceMqttUpdate > 0) {
				publishSettings(true);
				publishValues();
				//reset
				mqttClient.publish(mqttTopicForceMqttUpdate.c_str(), String(false).c_str());
				mqttClient.publish(mqttTopicForceRenewValue.c_str(), String(false).c_str());
				callbackMqttDebug(mqttTopicForceMqttUpdate, String(readForceMqttUpdate));
			}
		}
		if(strcmp(topic, mqttTopicForceRenewValue.c_str()) == 0) {
			int readForceRenewValue = msg.toInt();
			if(readForceRenewValue > 0) {
				publishValues();
				//reset
				mqttClient.publish(mqttTopicForceRenewValue.c_str(), String(false).c_str());
				callbackMqttDebug(mqttTopicForceRenewValue, String(readForceRenewValue));
			}
		}
		if(strcmp(topic, mqttTopicDebugEprom.c_str()) == 0) {
			bool readDebugEprom = msg.toInt();
			if(wpFZ.DebugEprom != readDebugEprom) {
				wpFZ.DebugEprom = readDebugEprom;
				bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugEprom, wpFZ.DebugEprom);
				EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugEprom, String(wpFZ.DebugEprom));
				wpFZ.SendWS("{\"id\":\"DebugEprom\",\"value\":" + String(wpFZ.DebugEprom ? "true" : "false") + "}");
			}
		}
		if(strcmp(topic, mqttTopicDebugWiFi.c_str()) == 0) {
			bool readDebugWiFi = msg.toInt();
			if(wpFZ.DebugWiFi != readDebugWiFi) {
				wpFZ.DebugWiFi = readDebugWiFi;
				bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugWiFi, wpFZ.DebugWiFi);
				EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugWiFi, String(wpFZ.DebugWiFi));
				wpFZ.SendWS("{\"id\":\"DebugWiFi\",\"value\":" + String(wpFZ.DebugWiFi ? "true" : "false") + "}");
			}
		}
		if(strcmp(topic, mqttTopicDebugMqtt.c_str()) == 0) {
			bool readDebugMqtt = msg.toInt();
			if(wpFZ.DebugMqtt != readDebugMqtt) {
				wpFZ.DebugMqtt = readDebugMqtt;
				bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugMqtt, wpFZ.DebugMqtt);
				EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugMqtt, String(wpFZ.DebugMqtt));
				wpFZ.SendWS("{\"id\":\"DebugMqtt\",\"value\":" + String(wpFZ.DebugMqtt ? "true" : "false") + "}");
			}
		}
		if(strcmp(topic, mqttTopicDebugFinder.c_str()) == 0) {
			bool readDebugFinder = msg.toInt();
			if(wpFZ.DebugFinder != readDebugFinder) {
				wpFZ.DebugFinder = readDebugFinder;
				bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugFinder, wpFZ.DebugFinder);
				EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugFinder, String(wpFZ.DebugFinder));
				wpFZ.SendWS("{\"id\":\"DebugFinder\",\"value\":" + String(wpFZ.DebugFinder ? "true" : "false") + "}");
			}
		}
		if(strcmp(topic, mqttTopicDebugRest.c_str()) == 0) {
			bool readDebugRest = msg.toInt();
			if(wpFZ.DebugRest != readDebugRest) {
				wpFZ.DebugRest = readDebugRest;
				bitWrite(wpFZ.settingsBool1, wpFZ.bitDebugRest, wpFZ.DebugRest);
				EEPROM.write(wpFZ.addrSettingsBool1, wpFZ.settingsBool1);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugRest, String(wpFZ.DebugRest));
				wpFZ.SendWS("{\"id\":\"DebugRest\",\"value\":" + String(wpFZ.DebugRest ? "true" : "false") + "}");
			}
		}
#ifdef wpHT
		if(strcmp(topic, mqttTopicMaxCycleHT.c_str()) == 0) {
			byte readMaxCycleHT = msg.toInt();
			if(readMaxCycleHT <= 0) readMaxCycleHT = 1;
			if(wpFZ.maxCycleHT != readMaxCycleHT) {
				wpFZ.maxCycleHT = readMaxCycleHT;
				EEPROM.write(wpFZ.addrMaxCycleHT, wpFZ.maxCycleHT);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMaxCycleHT, String(wpFZ.maxCycleHT));
			}
		}
		if(strcmp(topic, mqttTopicTemperatureCorrection.c_str()) == 0) {
			float readTemperatureCorrection = msg.toFloat();
			if(wpFZ.temperatureCorrection != readTemperatureCorrection) {
				wpFZ.temperatureCorrection = readTemperatureCorrection;
				EEPROM.put(wpFZ.addrTemperatureCorrection, wpFZ.temperatureCorrection);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicTemperatureCorrection, String(wpFZ.temperatureCorrection));
			}
		}
		if(strcmp(topic, mqttTopicHumidityCorrection.c_str()) == 0) {
			float readHumidityCorrection = msg.toFloat();
			if(wpFZ.humidityCorrection != readHumidityCorrection) {
				wpFZ.humidityCorrection = readHumidityCorrection;
				EEPROM.put(wpFZ.addrHumidityCorrection, wpFZ.humidityCorrection);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicHumidityCorrection, String(wpFZ.humidityCorrection));
			}
		}
		if(strcmp(topic, mqttTopicDebugHT.c_str()) == 0) {
			bool readDebugHT = msg.toInt();
			if(wpFZ.DebugHT != readDebugHT) {
				wpFZ.DebugHT = readDebugHT;
				bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugHT, wpFZ.DebugHT);
				EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugHT, String(wpFZ.DebugHT));
				wpFZ.SendWS("{\"id\":\"DebugHT\",\"value\":" + String(wpFZ.DebugHT ? "true" : "false") + "}");
			}
		}
#endif
#ifdef wpLDR
		if(strcmp(topic, mqttTopicMaxCycleLDR.c_str()) == 0) {
			byte readMaxCycleLDR = msg.toInt();
			if(readMaxCycleLDR <= 0) readMaxCycleLDR = 1;
			if(wpFZ.maxCycleLDR != readMaxCycleLDR) {
				wpFZ.maxCycleLDR = readMaxCycleLDR;
				EEPROM.write(wpFZ.addrMaxCycleLDR, wpFZ.maxCycleLDR);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMaxCycleLDR, String(wpFZ.maxCycleLDR));
			}
		}
		if(strcmp(topic, mqttTopicUseLdrAvg.c_str()) == 0) {
			bool readUseLdrAvg = msg.toInt();
			if(wpFZ.useLdrAvg != readUseLdrAvg) {
				wpFZ.useLdrAvg = readUseLdrAvg;
				bitWrite(wpFZ.settingsBool3, wpFZ.bitUseLdrAvg, wpFZ.useLdrAvg);
				EEPROM.write(wpFZ.addrSettingsBool3, wpFZ.settingsBool3);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicUseLdrAvg, String(wpFZ.useLdrAvg));
			}
		}
		if(strcmp(topic, mqttTopicLdrCorrection.c_str()) == 0) {
			int readLdrCorrection = msg.toInt();
			if(wpFZ.ldrCorrection != readLdrCorrection) {
				wpFZ.ldrCorrection = readLdrCorrection;
				EEPROM.put(wpFZ.addrLdrCorrection, wpFZ.ldrCorrection);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicLdrCorrection, String(wpFZ.ldrCorrection));
			}
		}
		if(strcmp(topic, mqttTopicDebugLDR.c_str()) == 0) {
			bool readDebugLDR = msg.toInt();
			if(wpFZ.DebugLDR != readDebugLDR) {
				wpFZ.DebugLDR = readDebugLDR;
				bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugLDR, wpFZ.DebugLDR);
				EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugLDR, String(wpFZ.DebugLDR));
				wpFZ.SendWS("{\"id\":\"DebugLDR\",\"value\":" + String(wpFZ.DebugLDR ? "true" : "false") + "}");
			}
		}
#endif
#ifdef wpLight
		if(strcmp(topic, mqttTopicMaxCycleLight.c_str()) == 0) {
			byte readMaxCycleLight = msg.toInt();
			if(readMaxCycleLight <= 0) readMaxCycleLight = 1;
			if(wpFZ.maxCycleLight != readMaxCycleLight) {
				wpFZ.maxCycleLight = readMaxCycleLight;
				EEPROM.write(wpFZ.addrMaxCycleLight, wpFZ.maxCycleLight);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMaxCycleLight, String(wpFZ.maxCycleLight));
			}
		}
		if(strcmp(topic, mqttTopicUseLightAvg.c_str()) == 0) {
			bool readUseLightAvg = msg.toInt();
			if(wpFZ.useLightAvg != readUseLightAvg) {
				wpFZ.useLightAvg = readUseLightAvg;
				bitWrite(wpFZ.settingsBool3, wpFZ.bitUseLightAvg, wpFZ.useLightAvg);
				EEPROM.write(wpFZ.addrSettingsBool3, wpFZ.settingsBool3);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicUseLightAvg, String(wpFZ.useLightAvg));
			}
		}
		if(strcmp(topic, mqttTopicLightCorrection.c_str()) == 0) {
			int readLightCorrection = msg.toInt();
			if(wpFZ.lightCorrection != readLightCorrection) {
				wpFZ.lightCorrection = readLightCorrection;
				EEPROM.put(wpFZ.addrLightCorrection, wpFZ.lightCorrection);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicLightCorrection, String(wpFZ.lightCorrection));
			}
		}
		if(strcmp(topic, mqttTopicDebugLight.c_str()) == 0) {
			bool readDebugLight = msg.toInt();
			if(wpFZ.DebugLight != readDebugLight) {
				wpFZ.DebugLight = readDebugLight;
				bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugLight, wpFZ.DebugLight);
				EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugLight, String(wpFZ.DebugLight));
				wpFZ.SendWS("{\"id\":\"DebugLight\",\"value\":" + String(wpFZ.DebugLight ? "true" : "false") + "}");
			}
		}
#endif
#ifdef wpBM
		if(strcmp(topic, mqttTopicDebugBM.c_str()) == 0) {
			bool readDebugBM = msg.toInt();
			if(wpFZ.DebugBM != readDebugBM) {
				wpFZ.DebugBM = readDebugBM;
				bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugBM, wpFZ.DebugBM);
				EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugBM, String(wpFZ.DebugBM));
				wpFZ.SendWS("{\"id\":\"DebugBM\",\"value\":" + String(wpFZ.DebugBM ? "true" : "false") + "}");
			}
		}
#ifdef wpLDR
		if(strcmp(topic, mqttTopicThreshold.c_str()) == 0) {
			uint16_t readThreshold = msg.toInt();
			if(wpFZ.threshold != readThreshold) {
				wpFZ.threshold = readThreshold;
				EEPROM.put(wpFZ.addrThreshold, wpFZ.threshold);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicThreshold, String(wpFZ.threshold));
			}
		}
		if(strcmp(topic, mqttTopicLightToTurnOn.c_str()) == 0) {
			if(strcmp(msg.c_str(), wpFZ.lightToTurnOn.c_str()) != 0) {
				wpFZ.lightToTurnOn = msg;
				writeStringsToEEPROM();
				callbackMqttDebug(mqttTopicLightToTurnOn, wpFZ.lightToTurnOn);
			}
		}
#endif
#endif
#ifdef wpRelais
		if(strcmp(topic, mqttTopicRelaisHand.c_str()) == 0) {
			bool readRelaisHand = msg.toInt();
			if(relaisHand != readRelaisHand) {
				relaisHand = readRelaisHand;
				callbackMqttDebug(mqttTopicRelaisHand, String(relaisHand));
			}
		}
		if(strcmp(topic, mqttTopicRelaisHandValue.c_str()) == 0) {
			bool readRelaisHandValue = msg.toInt();
			if(relaisHandValue != readRelaisHandValue) {
				relaisHandValue = readRelaisHandValue;
				callbackMqttDebug(mqttTopicRelaisHandValue, String(relaisHandValue));
			}
		}
#ifdef wpMoisture
		if(strcmp(topic, mqttTopicWaterEmpty.c_str()) == 0) {
			bool readWaterEmpty = msg.toInt();
			if(wpFZ.waterEmpty != readWaterEmpty) {
				wpFZ.waterEmpty = readWaterEmpty;
				callbackMqttDebug(mqttTopicWaterEmpty, String(wpFZ.waterEmpty));
			}
		}
		if(strcmp(topic, mqttTopicPumpActive.c_str()) == 0) {
			uint16_t readPumpActive = msg.toInt();
			if(wpFZ.pumpActive != readPumpActive) {
				wpFZ.pumpActive = readPumpActive;
				EEPROM.put(wpFZ.addrPumpActive, wpFZ.pumpActive);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicPumpActive, String(wpFZ.pumpActive));
			}
		}
		if(strcmp(topic, mqttTopicPumpPause.c_str()) == 0) {
			uint16_t readPumpPause = msg.toInt();
			if(wpFZ.pumpPause != readPumpPause) {
				wpFZ.pumpPause = readPumpPause;
				EEPROM.put(wpFZ.addrPumpPause, wpFZ.pumpPause);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicPumpPause, String(wpFZ.pumpPause));
			}
		}
#endif
	if(strcmp(topic, mqttTopicDebugRelais.c_str()) == 0) {
		bool readDebugRelais = msg.toInt();
		if(wpFZ.DebugRelais != readDebugRelais) {
			wpFZ.DebugRelais = readDebugRelais;
			bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugRelais, wpFZ.DebugRelais);
			EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
			EEPROM.commit();
			callbackMqttDebug(mqttTopicDebugRelais, String(wpFZ.DebugRelais));
			wpFZ.SendWS("{\"id\":\"DebugRelais\",\"value\":" + String(wpFZ.DebugRelais ? "true" : "false") + "}");
		}
	}
#endif
#ifdef wpRain
		if(strcmp(topic, mqttTopicMaxCycleRain.c_str()) == 0) {
			byte readMaxCycleRain = msg.toInt();
			if(readMaxCycleRain <= 0) readMaxCycleRain = 1;
			if(wpFZ.maxCycleRain != readMaxCycleRain) {
				wpFZ.maxCycleRain = readMaxCycleRain;
				EEPROM.write(wpFZ.addrMaxCycleRain, wpFZ.maxCycleRain);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMaxCycleRain, String(wpFZ.maxCycleRain));
			}
		}
		if(strcmp(topic, mqttTopicUseRainAvg.c_str()) == 0) {
			bool readUseRainAvg = msg.toInt();
			if(wpFZ.useRainAvg != readUseRainAvg) {
				wpFZ.useRainAvg = readUseRainAvg;
				bitWrite(wpFZ.settingsBool3, wpFZ.bitUseRainAvg, wpFZ.useRainAvg);
				EEPROM.write(wpFZ.addrSettingsBool3, wpFZ.settingsBool3);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicUseRainAvg, String(wpFZ.useRainAvg));
			}
		}
		if(strcmp(topic, mqttTopicRainCorrection.c_str()) == 0) {
			int readRainCorrection = msg.toInt();
			if(wpFZ.rainCorrection != readRainCorrection) {
				wpFZ.rainCorrection = readRainCorrection;
				EEPROM.put(wpFZ.addrRainCorrection, wpFZ.rainCorrection);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicRainCorrection, String(wpFZ.rainCorrection));
			}
		}
		if(strcmp(topic, mqttTopicDebugRain.c_str()) == 0) {
			bool readDebugRain = msg.toInt();
			if(wpFZ.DebugRain != readDebugRain) {
				wpFZ.DebugRain = readDebugRain;
				bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugRain, wpFZ.DebugRain);
				EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugRain, String(wpFZ.DebugRain));
				wpFZ.SendWS("{\"id\":\"DebugRain\",\"value\":" + String(wpFZ.DebugRain ? "true" : "false") + "}");
			}
		}
#endif
#ifdef wpMoisture
		if(strcmp(topic, mqttTopicMaxCycleMoisture.c_str()) == 0) {
			byte readMaxCycleMoisture = msg.toInt();
			if(readMaxCycleMoisture <= 0) readMaxCycleMoisture = 1;
			if(wpFZ.maxCycleMoisture != readMaxCycleMoisture) {
				wpFZ.maxCycleMoisture = readMaxCycleMoisture;
				EEPROM.write(wpFZ.addrMaxCycleMoisture, wpFZ.maxCycleMoisture);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMaxCycleMoisture, String(wpFZ.maxCycleMoisture));
			}
		}
		if(strcmp(topic, mqttTopicUseMoistureAvg.c_str()) == 0) {
			bool readUseMoistureAvg = msg.toInt();
			if(wpFZ.useMoistureAvg != readUseMoistureAvg) {
				wpFZ.useMoistureAvg = readUseMoistureAvg;
				bitWrite(wpFZ.settingsBool3, wpFZ.bitUseMoistureAvg, wpFZ.useMoistureAvg);
				EEPROM.write(wpFZ.addrSettingsBool3, wpFZ.settingsBool3);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicUseMoistureAvg, String(wpFZ.useMoistureAvg));
			}
		}
		if(strcmp(topic, mqttTopicMoistureMin.c_str()) == 0) {
			int readMoistureMin = msg.toInt();
			if(wpFZ.moistureMin != readMoistureMin) {
				wpFZ.moistureMin = readMoistureMin;
				EEPROM.write(wpFZ.addrMoistureMin, wpFZ.moistureMin);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMoistureMin, String(wpFZ.moistureMin));
			}
		}
		if(strcmp(topic, mqttTopicMoistureDry.c_str()) == 0) {
			int readMoistureDry = msg.toInt();
			if(wpFZ.moistureDry != readMoistureDry) {
				wpFZ.moistureDry = readMoistureDry;
				EEPROM.put(wpFZ.addrMoistureDry, wpFZ.moistureDry);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMoistureDry, String(wpFZ.moistureDry));
			}
		}
		if(strcmp(topic, mqttTopicMoistureWet.c_str()) == 0) {
			int readMoistureWet = msg.toInt();
			if(wpFZ.moistureWet != readMoistureWet) {
				wpFZ.moistureWet = readMoistureWet;
				EEPROM.put(wpFZ.addrMoistureWet, wpFZ.moistureWet);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMoistureWet, String(wpFZ.moistureWet));
			}
		}
		if(strcmp(topic, mqttTopicDebugMoisture.c_str()) == 0) {
			bool readDebugMoisture = msg.toInt();
			if(wpFZ.DebugMoisture != readDebugMoisture) {
				wpFZ.DebugMoisture = readDebugMoisture;
				bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugMoisture, wpFZ.DebugMoisture);
				EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugMoisture, String(wpFZ.DebugMoisture));
				wpFZ.SendWS("{\"id\":\"DebugMoisture\",\"value\":" + String(wpFZ.DebugMoisture ? "true" : "false") + "}");
			}
		}
#endif
#ifdef wpDistance
		if(strcmp(topic, mqttTopicMaxCycleDistance.c_str()) == 0) {
			byte readMaxCycleDistance = msg.toInt();
			if(readMaxCycleDistance <= 0) readMaxCycleDistance = 1;
			if(wpFZ.maxCycleDistance != readMaxCycleDistance) {
				wpFZ.maxCycleDistance = readMaxCycleDistance;
				EEPROM.write(wpFZ.addrMaxCycleDistance, wpFZ.maxCycleDistance);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMaxCycleDistance, String(wpFZ.maxCycleDistance));
			}
		}
		if(strcmp(topic, mqttTopicDistanceCorrection.c_str()) == 0) {
			int8_t readDistanceCorrection = msg.toInt();
			if(wpFZ.distanceCorrection != readDistanceCorrection) {
				wpFZ.distanceCorrection = readDistanceCorrection;
				EEPROM.write(wpFZ.addrDistanceCorrection, wpFZ.distanceCorrection);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDistanceCorrection, String(wpFZ.distanceCorrection));
			}
		}
		if(strcmp(topic, mqttTopicMaxVolume.c_str()) == 0) {
			uint16_t readMaxVolume = msg.toInt();
			if(wpFZ.maxVolume != readMaxVolume) {
				wpFZ.maxVolume = readMaxVolume;
				EEPROM.put(wpFZ.addrMaxVolume, wpFZ.maxVolume);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicMaxVolume, String(wpFZ.maxVolume));
			}
		}
		if(strcmp(topic, mqttTopicHeight.c_str()) == 0) {
			uint8_t readHeight = msg.toInt();
			if(wpFZ.height != readHeight) {
				wpFZ.height = readHeight;
				EEPROM.write(wpFZ.addrHeight, wpFZ.height);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicHeight, String(wpFZ.height));
			}
		}
		if(strcmp(topic, mqttTopicDebugDistance.c_str()) == 0) {
			bool readDebugDistance = msg.toInt();
			if(wpFZ.DebugDistance != readDebugDistance) {
				wpFZ.DebugDistance = readDebugDistance;
				bitWrite(wpFZ.settingsBool2, wpFZ.bitDebugDistance, wpFZ.DebugDistance);
				EEPROM.write(wpFZ.addrSettingsBool2, wpFZ.settingsBool2);
				EEPROM.commit();
				callbackMqttDebug(mqttTopicDebugDistance, String(wpFZ.DebugDistance));
				wpFZ.SendWS("{\"id\":\"DebugDistance\",\"value\":" + String(wpFZ.DebugDistance ? "true" : "false") + "}");
			}
		}
#endif
	}
}
void callbackMqttDebug(String topic, String value) {
	String logmessage =  "Setting change found on topic: '" + topic + "': " + value;
	wpFZ.DebugWS(wpFZ.strINFO, "callbackMqtt", logmessage);
	wpFZ.blink();
}


//###################################################################################
// calc values
//###################################################################################
#ifdef wpHT
	void calcHT() {
		bool e = false;
		float newT = dht.readTemperature();
		float newH = dht.readHumidity();
		if(!isnan(newT)) {
			temperature = newT + wpFZ.temperatureCorrection;
			e = e | false;
			if(wpFZ.DebugHT) {
				calcHTDebug("Temperature", temperature, newT);
			}
		} else {
			e = e | true;
			calcHTError("Temperature");
		}
		if(!isnan(newH)) {
			humidity = newH + wpFZ.humidityCorrection;
			if(wpFZ.DebugHT) {
				e = e | false;
				calcHTDebug("Humidity", humidity, newH);
			}
		} else {
			e = e | true;
			calcHTError("Humidity");
		}
		errorHT = e;
	}
	void calcHTDebug(String name, float value, float raw) {
		String logmessage = name + ": " + String(value) + " (" + String(raw) + ")";
		wpFZ.DebugWS(wpFZ.strDEBUG, "calcHT", logmessage);
	}
	void calcHTError(String name) {
		wpFZ.blink();
		String logmessage = name + ": Sensor Failure";
		wpFZ.DebugWS(wpFZ.strERRROR, "calcHT", logmessage);
	}
#endif
#ifdef wpLDR
	void calcLDR() {
		int ar = analogRead(LDRPin);
		uint16_t newLdr = (uint16_t)ar;
		if(!isnan(newLdr) && ar > 0) {
			if(newLdr > 1023) newLdr = 1023;
			if(newLdr < 0) newLdr = 0;
			if(wpFZ.useLdrAvg) {
				newLdr = calcLdrAvg(newLdr);
			}
			ldr = (1023 - newLdr) + wpFZ.ldrCorrection;
			errorLDR = false;
			if(wpFZ.DebugLDR) {
				String logmessage = "LDR: " + String(ldr) + " (" + String(newLdr) + ")";
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcLDR", logmessage);
			}
		} else {
			errorLDR = true;
			String logmessage = "Sensor Failure";
			wpFZ.DebugWS(wpFZ.strERRROR, "calcLDR", logmessage);
		}
	}
	const uint8_t avgLdrLength = 128;
	int LdrValues[avgLdrLength];
	uint16_t calcLdrAvg(uint16_t raw) {
		int avg = 0;
		int avgCount = avgLdrLength;
		LdrValues[avgLdrLength - 1] = raw;
		for(int i = 0; i < avgLdrLength - 1; i++) {
			if(LdrValues[i + 1] > 0) {
				LdrValues[i] = LdrValues[i + 1];
				avg += LdrValues[i] * (i + 1);
				avgCount += (i + 1);
			}
		}
		avg += raw * avgLdrLength;
		return round(avg / avgCount);
	}
#endif
#ifdef wpLight
	void calcLight() {
		float ar = lightMeter.readLightLevel();
		uint16_t newLight = (uint16_t)ar;
		if(!isnan(newLight) || ar < 0) {
			if(wpFZ.useLightAvg) {
				newLight = calcLightAvg(newLight);
			}
			light = newLight + wpFZ.lightCorrection;
			errorLight = false;
			if(wpFZ.DebugLight) {
				String logmessage = "Light: " + String(light) + " (" + String(newLight) + ")";
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcLight", logmessage);
			}
		} else {
			errorLight = true;
			String logmessage = "Sensor Failure";
			wpFZ.DebugWS(wpFZ.strERRROR, "calcLight", logmessage);
		}
	}
	const uint8_t avgLightLength = 128;
	int lightValues[avgLightLength];
	uint16_t calcLightAvg(uint16_t raw) {
		long avg = 0;
		long avgCount = avgLightLength;
		lightValues[avgLightLength - 1] = raw;
		for(int i = 0; i < avgLightLength - 1; i++) {
			if(lightValues[i + 1] > 0) {
				lightValues[i] = lightValues[i + 1];
				avg += lightValues[i] * (i + 1);
				avgCount += (i + 1);
			}
		}
		avg += raw * avgLightLength;
		return round(avg / avgCount);
	}
#endif
#ifdef wpBM
	void calcBM() {
		if(digitalRead(BMPin) == HIGH) {
			if(bm == false) {
				bm = true;
				wpFZ.blink();
				if(wpFZ.DebugBM) {
					wpFZ.DebugWS(wpFZ.strDEBUG, "calcBM", "Bewegung erkannt");
				}
			}
		} else {
			bm = false;
		}
	}
#endif
#ifdef wpRelais
	void calcRelaisOut() {
		if(relaisHand) {
			relaisOut = relaisHandValue;
		} else {
			relaisOut = relaisAuto;
		}
		if(relaisOut && digitalRead(RelaisPin) == LOW) {
			digitalWrite(RelaisPin, HIGH);
			if(wpFZ.DebugRelais) {
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcRelais", "Relais turns on");
			}
		}
		if(!relaisOut && digitalRead(RelaisPin) == HIGH) {
			digitalWrite(RelaisPin, LOW);
			if(wpFZ.DebugRelais) {
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcRelais", "Relais turns off");
			}
		}
	}
#ifdef wpMoisture
	void calcRelais() {
		if(!wpFZ.waterEmpty) {
			unsigned long m = millis();
			// detect to dry soil
			if(errorMoistureMin && !pumpCycleActive) {
				pumpCycleActive = true;
				pumpStart = false;
				pumpPause = false;
				if(wpFZ.DebugRelais) {
					wpFZ.DebugWS(wpFZ.strDEBUG, "calcRelais", "Detect 'toDry', start Pump Cycle");
				}
			}
			// start pump and store timestart
			if(pumpCycleActive && !pumpStart && !pumpPause) {
				relaisAuto = true;
				//digitalWrite(RelaisPin, HIGH);
				pumpStart = true;
				timePumpStart = m;
				if(wpFZ.DebugRelais) {
					wpFZ.DebugWS(wpFZ.strDEBUG, "calcRelais", "start 'pump' (" + String(timePumpStart) + ")");
				}
			}
			// stop pump and start pause
			if(pumpCycleActive && pumpStart && !pumpPause) {
				if(m > (timePumpStart + (wpFZ.pumpActive * 1000))) {
					relaisAuto = false;
					//digitalWrite(RelaisPin, LOW);
					pumpPause = true;
					timePumpPause = m;
					if(wpFZ.DebugRelais) {
						wpFZ.DebugWS(wpFZ.strDEBUG, "calcRelais", "stopped 'pump', start 'pumpPause' (" + String(timePumpPause) + ")");
					}
				}
			}
			// finished pause, reset all
			if(pumpCycleActive && pumpStart && pumpPause) {
				if(m > (timePumpPause + (wpFZ.pumpPause * 1000))) {
					pumpCycleActive = false;
					pumpStart = false;
					pumpPause = false;
					if(wpFZ.DebugRelais) {
						wpFZ.DebugWS(wpFZ.strDEBUG, "calcRelais", "stopped 'pumpPause' and reset");
					}
				}
			}
		}
	}
#endif
#endif
#ifdef wpRain
	void calcRain() {
		int newRain = analogRead(RainPin);
		if(!isnan(newRain)) {
			if(newRain > 1023) newRain = 1023;
			if(newRain < 0) newRain = 0;
			int calcedRain = newRain;
			if(wpFZ.useRainAvg) {
				calcedRain = calcRainAvg(newRain);
			}
			calcedRain = map(newRain, 1023, 0, 0, 500);
			rain = (float)(calcedRain / 10.0) + wpFZ.rainCorrection;
			errorRain = false;
			if(wpFZ.DebugRain) {
				String logmessage = "Rain: " + String(rain) + " (" + String(newRain) + ")";
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcRain", logmessage);
			}
		} else {
			errorRain = true;
			String logmessage = "Sensor Failure";
			wpFZ.DebugWS(wpFZ.strERRROR, "calcRain", logmessage);
		}
	}
	const uint8_t avgRainLength = 128;
	int rainValues[avgRainLength];
	uint16_t calcRainAvg(uint16_t raw) {
		long avg = 0;
		long avgCount = avgRainLength;
		rainValues[avgRainLength - 1] = raw;
		for(int i = 0; i < avgRainLength - 1; i++) {
			if(rainValues[i + 1] > 0) {
				rainValues[i] = rainValues[i + 1];
				avg += rainValues[i] * (i + 1);
				avgCount += (i + 1);
			}
		}
		avg += raw * avgRainLength;
		return round(avg / avgCount);
	}
#endif
#ifdef wpMoisture
	void calcMoisture() {
		int ar = analogRead(MoisturePin);
		double newMoisture = (double)ar;
		if(!isnan(newMoisture)) {
			if(newMoisture > 1023) newMoisture = 1023;
			if(newMoisture < 0) newMoisture = 0;
			if(wpFZ.useMoistureAvg) {
				newMoisture = calcMoistureAvg(newMoisture);
			}
			//Divission 0
			if((wpFZ.moistureWet + wpFZ.moistureDry) == 0) wpFZ.moistureDry = 1;
			moisture = map(newMoisture, wpFZ.moistureDry, wpFZ.moistureWet, 0, 100);
			if(moisture > 100) moisture = 100;
			if(moisture < 0) moisture = 0;
			if(moisture < wpFZ.moistureMin) errorMoistureMin = true;
			if(moisture > wpFZ.moistureMin) errorMoistureMin = false;
			errorMoisture = false;
			if(wpFZ.DebugMoisture) {
				String logmessage = "Moisture: " + String(moisture) + " (" + String(newMoisture) + ")";
				wpFZ.DebugWS(wpFZ.strDEBUG, "calcMoisture", logmessage);
			}
		} else {
			errorMoisture = true;
			String logmessage = "Sensor Failure";
			wpFZ.DebugWS(wpFZ.strERRROR, "calcMoisture", logmessage);
		}
	}
	const uint8_t avgMoistureLength = 128;
	int moistureValues[avgMoistureLength];
	uint16_t calcMoistureAvg(uint16_t raw) {
		long avg = 0;
		long avgCount = avgMoistureLength;
		moistureValues[avgMoistureLength - 1] = raw;
		for(int i = 0; i < avgMoistureLength - 1; i++) {
			if(moistureValues[i + 1] > 0) {
				moistureValues[i] = moistureValues[i + 1];
				avg += moistureValues[i] * (i + 1);
				avgCount += (i + 1);
			}
		}
		avg += raw * avgMoistureLength;
		return round(avg / avgCount);
	}
#endif
#ifdef wpDistance
	void calcDistance() {
		unsigned long duration;
		digitalWrite(trigPin, HIGH);
		delayMicroseconds(20);
		digitalWrite(trigPin, LOW);
		duration = pulseIn(echoPin, HIGH, loopTime * 1000);
		if(duration > 0) {
			distanceRaw = (duration * 0.034 / 2);
			distanceAvg = calcDistanceAvg(distanceRaw) + wpFZ.distanceCorrection;
			if(wpFZ.height <= 0) wpFZ.height = 1; // durch 0
			if(distanceAvg > wpFZ.height) distanceAvg = wpFZ.height;
			volume = wpFZ.maxVolume - round(wpFZ.maxVolume * distanceAvg / wpFZ.height);
			if(volume > wpFZ.maxVolume) volume = wpFZ.maxVolume;
			errorDistance = false;
			if(wpFZ.DebugDistance) {
				calcDistanceDebug("Distance", distanceAvg, distanceRaw);
			}
			delay(loopTime);
		} else {
			errorDistance = true;
			String logmessage = "Sensor Failure";
			wpFZ.DebugWS(wpFZ.strERRROR, "calcDistance", logmessage);
		}
	}
	const uint8_t avgDistanceLength = 240;
	int distanceValues[avgDistanceLength];
	uint8_t calcDistanceAvg(uint8_t raw) {
		long avg = 0;
		long avgCount = avgDistanceLength;
		distanceValues[avgDistanceLength - 1] = raw;
		for(int i = 0; i < avgDistanceLength - 1; i++) {
			if(distanceValues[i + 1] > 0) {
				distanceValues[i] = distanceValues[i + 1];
				avg += distanceValues[i] * (i + 1);
				avgCount += (i + 1);
			}
		}
		avg += raw * avgDistanceLength;
		return round(avg / avgCount);
	}
	void calcDistanceDebug(String name, uint8_t avg, uint8_t raw) {
		String logmessage = name + ": " + String(avg) + " (" + String(raw) + ")";
		wpFZ.DebugWS(wpFZ.strDEBUG, "calcDistance", logmessage);
	}
#endif
