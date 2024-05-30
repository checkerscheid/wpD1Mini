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
//# Revision     : $Rev:: 117                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: wpFreakaZone.h 117 2024-05-29 01:28:02Z                  $ #
//#                                                                                 #
//###################################################################################
#include <helperMqtt.h>

helperMqtt wpMqtt();

WiFiClient helperMqtt::wifiClient;
PubSubClient helperMqtt::mqttClient(helperMqtt::wifiClient);

helperMqtt::helperMqtt() {
	// settings
	mqttTopicMqttServer = wpFZ.DeviceName + "/info/MQTT/Server";
	mqttTopicMqttSince = wpFZ.DeviceName + "/info/MQTT/Since";
	// commands
	mqttTopicForceMqttUpdate = wpFZ.DeviceName + "/ForceMqttUpdate";
	mqttTopicForceRenewValue = wpFZ.DeviceName + "/ForceRenewValue";
	mqttTopicDebugMqtt = wpFZ.DeviceName + "/settings/Debug/MQTT";
	
	mqttClient.setServer(wpFZ.mqttServer, wpFZ.mqttServerPort);
	mqttClient.setCallback(callbackMqtt);
	connectMqtt();
}

//###################################################################################
// public
//###################################################################################
void helperMqtt::loop() {
	if(!mqttClient.connected()) {
		connectMqtt();
	}
	mqttClient,loop();
}

uint16_t helperMqtt::getVersion() {
	String SVN = "$Rev: 118 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperMqtt::changeDebug() {
	DebugMqtt = !DebugMqtt;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugMqtt, DebugMqtt);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugMqtt\",\"value\":" + String(DebugMqtt ? "true" : "false") + "}");
	wpFZ.blink();
}

void helperMqtt::setMqttOffline() {
	mqttClient.publish(wpOnlineToggler.mqttTopicErrorOnline.c_str(), String(1).c_str());
}

void helperMqtt::publishSettings() {
	publishSettings(false);
}

void helperMqtt::publishSettings(bool force) {
	mqttClient.publish(wpOnlineToggler.mqttTopicOnlineToggler.c_str(), String(1).c_str());
	// values
	mqttClient.publish(mqttTopicDeviceName.c_str(), wpFZ.DeviceName.c_str(), true);
	mqttClient.publish(mqttTopicDeviceDescription.c_str(), wpFZ.DeviceDescription.c_str(), true);
	mqttClient.publish(mqttTopicErrorOnline.c_str(), String(0).c_str());
	mqttClient.publish(mqttTopicVersion.c_str(), getVersion().c_str(), true);
	mqttClient.publish(mqttTopicwpFreakaZoneVersion.c_str(), wpFZ.getVersion().c_str(), true);
	mqttClient.publish(mqttTopicOnSince.c_str(), wpFZ.OnSince.c_str());
	mqttClient.publish(mqttTopicOnDuration.c_str(), wpFZ.OnDuration.c_str());
	mqttClient.publish(wpWiFi.mqttTopicSsid.c_str(), wpFZ.ssid, true);
	mqttClient.publish(wpWiFi.mqttTopicRssi.c_str(), String(WiFi.RSSI()).c_str());
	mqttClient.publish(wpWiFi.mqttTopicIp.c_str(), WiFi.localIP().toString().c_str(), true);
	mqttClient.publish(wpWiFi.mqttTopicMac.c_str(), WiFi.macAddress().c_str(), true);
	mqttClient.publish(wpWiFi.mqttTopicWiFiSince.c_str(), wpWiFi.WiFiSince.c_str());
	mqttClient.publish(mqttTopicMqttServer.c_str(), (String(wpFZ.mqttServer) + ":" + String(wpFZ.mqttServerPort)).c_str(), true);
	mqttClient.publish(mqttTopicMqttSince.c_str(), MqttSince.c_str());
	mqttClient.publish(mqttTopicRestServer.c_str(), (String(wpFZ.restServer) + ":" + String(wpFZ.restServerPort)).c_str(), true);
	// settings
	mqttClient.publish(mqttTopicSetDeviceName.c_str(), wpFZ.DeviceName.c_str());
	mqttClient.publish(mqttTopicSetDeviceDescription.c_str(), wpFZ.DeviceDescription.c_str());
	mqttClient.publish(mqttTopicCalcValues.c_str(), String(wpFZ.calcValues).c_str());
	mqttClient.publish(wpEEPROM.mqttTopicDebugEprom.c_str(), String(wpEEPROM.DebugEEPROM).c_str());
	mqttClient.publish(wpWiFi.mqttTopicDebugWiFi.c_str(), String(wpWiFi.DebugWiFi).c_str());
	mqttClient.publish(mqttTopicDebugMqtt.c_str(), String(DebugMqtt).c_str());
	mqttClient.publish(mqttTopicDebugFinder.c_str(), String(wpFZ.DebugFinder).c_str());
	mqttClient.publish(mqttTopicDebugRest.c_str(), String(wpFZ.DebugRest).c_str());
	mqttClient.publish(mqttTopicErrorRest.c_str(), String(wpFZ.errorRest).c_str());
	if(force) {
		mqttClient.publish(mqttTopicUpdateMode.c_str(), wpFZ.UpdateFW ? "On" : "Off");
		mqttClient.publish(mqttTopicForceMqttUpdate.c_str(), "0");
		mqttClient.publish(mqttTopicForceRenewValue.c_str(), "0");
	}
}

//###################################################################################
// private
//###################################################################################
void helperMqtt::callbackMqtt(char* topic, byte* payload, unsigned int length) {
	String msg = "";
	for (unsigned int i = 0; i < length; i++) {
		msg += (char)payload[i];
	}
	if(DebugMqtt) {
		wpFZ.DebugWS(wpFZ.strDEBUG, "callbackMqtt", "Message arrived on topic: '" + String(topic) + "': " + msg);
	}
	if(msg == "") {
		wpFZ.DebugWS(wpFZ.strWARN, "callbackMqtt", "msg is empty, '" + String(topic) + "'");
	} else {
	}
}
void helperMqtt::connectMqtt() {
	String logmessage = "Connecting MQTT Server: " + String(wpFZ.mqttServer) + ":" + String(wpFZ.mqttServerPort) + " as " + wpFZ.DeviceName;
	wpFZ.DebugWS(wpFZ.strINFO, "connectMqtt", logmessage);
	while(!mqttClient.connected()) {
		if(mqttClient.connect(wpFZ.DeviceName.c_str())) {
			MqttSince = wpFZ.getDateTime();
			String logmessage = "MQTT Connected";
			wpFZ.DebugWS(wpFZ.strINFO, "connectMqtt", logmessage);
		} else {
			String logmessage =  "failed, rc= " + String(mqttClient.state()) + ",  will try again in 5 seconds";
			wpFZ.DebugWS(wpFZ.strERRROR, "connectMqtt", logmessage);
			delay(5000);
		}
	}
}
