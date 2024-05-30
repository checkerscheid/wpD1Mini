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
//# File-ID      : $Id:: main.cpp 118 2024-05-29 01:29:33Z                        $ #
//#                                                                                 #
//###################################################################################
#include <main.h>

uint loopTime = 200;
//###################################################################################
//  Value Defines
//###################################################################################

bool errorRestLast = false;
bool trySendRest = false;
uint16_t publishCountErrorRest = 0;

//###################################################################################
//  setup
//###################################################################################
void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	Serial.begin(9600);
	while(!Serial) {}
	EEPROM.begin(4095);
	wpFZ.printStart();
	getVars();
	wpFZ.printRestored();

	wpWiFi.setupWiFi();
	setupWebServer();
	wpFZ.setupFinder();
}

//###################################################################################
// loop
//###################################################################################
void loop() {
	checkOfflineTrigger();
	if(wpFZ.UpdateFW) ArduinoOTA.handle();
	wpWiFi.loop();
	wpMqtt.loop();
	if(wpFZ.calcValues) {
		publishInfo();
	}
	wpFZ.loop();
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
	wpFZ.settingsBool3 = EEPROM.read(wpFZ.addrSettingsBool3);
	readStringsFromEEPROM();

	// values
	mqttTopicUpdateMode = wpFZ.DeviceName + "/UpdateMode";
	mqttTopicRestartRequired = wpFZ.DeviceName + "/RestartRequired";
	// settings
	mqttTopicDeviceName = wpFZ.DeviceName + "/info/DeviceName";
	mqttTopicDeviceDescription = wpFZ.DeviceName + "/info/DeviceDescription";
	mqttTopicOnlineToggler = wpFZ.DeviceName + "/info/Online";
	mqttTopicOnSince = wpFZ.DeviceName + "/info/OnSince";
	mqttTopicOnDuration = wpFZ.DeviceName + "/info/OnDuration";
	mqttTopicRestServer = wpFZ.DeviceName + "/info/Rest/Server";
	// commands
	mqttTopicSetDeviceName = wpFZ.DeviceName + "/settings/DeviceName";
	mqttTopicSetDeviceDescription = wpFZ.DeviceName + "/settings/DeviceDescription";
	mqttTopicRestartDevice = wpFZ.DeviceName + "/RestartDevice";
	mqttTopicUpdateFW = wpFZ.DeviceName + "/UpdateFW";
	mqttTopicCalcValues = wpFZ.DeviceName + "/settings/calcValues";
	mqttTopicDebugEprom = wpFZ.DeviceName + "/settings/Debug/Eprom";
	mqttTopicDebugMqtt = wpFZ.DeviceName + "/settings/Debug/MQTT";
	mqttTopicDebugFinder = wpFZ.DeviceName + "/settings/Debug/Finder";
	mqttTopicDebugRest = wpFZ.DeviceName + "/settings/Debug/Rest";
	mqttTopicErrorRest = wpFZ.DeviceName + "/ERROR/Rest";
}
void readStringsFromEEPROM() {
	byteStartForString = 50; // reset
	wpFZ.DeviceName = wpFZ.readStringFromEEPROM(byteStartForString, wpFZ.DeviceName);
	byteStartForString = byteStartForString + 1 + wpFZ.DeviceName.length();
	wpFZ.DeviceDescription = wpFZ.readStringFromEEPROM(byteStartForString, wpFZ.DeviceDescription);
	byteStartForString = byteStartForString + 1 + wpFZ.DeviceDescription.length();
	wpFZ.lightToTurnOn = wpFZ.readStringFromEEPROM(byteStartForString, wpFZ.lightToTurnOn);
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
	Rev = "$Rev: 118 $";
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
		} else {
			String logmessage =  "failed, rc= " + String(mqttClient.state()) + ",  will try again in 5 seconds";
			wpFZ.DebugWS(wpFZ.strERRROR, "connectMqtt", logmessage);
			delay(5000);
		}
	}
}

void checkOfflineTrigger() {
	if(wpFZ.OfflineTrigger) {
		// set offline for reboot
		setMqttOffline();
		wpFZ.OfflineTrigger = false;
	}
}

//###################################################################################
// publish settings
//###################################################################################


//###################################################################################
//  publish values Helper
//###################################################################################

void publishValuesSystem() {
	mqttClient.publish(mqttTopicRssi.c_str(), String(WiFi.RSSI()).c_str());
	mqttClient.publish(mqttTopicOnSince.c_str(), wpFZ.OnSince.c_str());
	mqttClient.publish(mqttTopicOnDuration.c_str(), wpFZ.OnDuration.c_str());
	mqttClient.publish(mqttTopicWiFiSince.c_str(), wpFZ.WiFiSince.c_str());
	mqttClient.publish(mqttTopicMqttSince.c_str(), wpFZ.MqttSince.c_str());
	rssi = WiFi.RSSI();
	wpFZ.errorRest = wpFZ.errorRest | !wpFZ.sendRest("rssi", String(rssi));
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
	mqttClient.publish(mqttTopicErrorRest.c_str(), String(wpFZ.errorRest).c_str());
	errorRestLast = wpFZ.errorRest;
	trySendRest = false;
	if(wpFZ.DebugRest)
		publishInfoDebug("ErrorRest", String(wpFZ.errorRest), String(publishCountErrorRest));
	publishCountErrorRest = 0;
}

//###################################################################################
//  publish values
//###################################################################################
void publishValues() {
	wpFZ.errorRest = false;
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
	publishValueRelaisHand();
	publishErrorRelaisHand();
	publishValueRelaisHandValue();
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
	wpFZ.errorRest = false;
	trySendRest = false;
	if(++publishCountRssi > 4 * 120) {
		publishValuesSystem();
	}
	if((errorRestLast != wpFZ.errorRest && trySendRest) || ++publishCountErrorRest > wpFZ.publishQoS) {
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
	}
}
void callbackMqttDebug(String topic, String value) {
	String logmessage =  "Setting change found on topic: '" + topic + "': " + value;
	wpFZ.DebugWS(wpFZ.strINFO, "callbackMqtt", logmessage);
	wpFZ.blink();
}
#endif
