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
	mqttTopicErrorOnline = wpFZ.DeviceName + "/ERROR/Online";
	// commands
	mqttTopicForceMqttUpdate = wpFZ.DeviceName + "/ForceMqttUpdate";
	mqttTopicForceRenewValue = wpFZ.DeviceName + "/ForceRenewValue";
	
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

void helperMqtt::setMqttOffline() {
	mqttClient.publish(mqttTopicErrorOnline.c_str(), String(1).c_str());
}

//###################################################################################
// private
//###################################################################################
void helperMqtt::callbackMqtt(char* topic, byte* payload, unsigned int length) {
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
	}
}
void helperMqtt::connectMqtt() {
	String logmessage = "Connecting MQTT Server: " + String(wpFZ.mqttServer) + ":" + String(wpFZ.mqttServerPort) + " as " + wpFZ.DeviceName;
	wpFZ.DebugWS(wpFZ.strINFO, "connectMqtt", logmessage);
	while(!mqttClient.connected()) {
		if(mqttClient.connect(wpFZ.DeviceName.c_str())) {
			wpFZ.MqttSince = wpFZ.getDateTime();
			String logmessage = "MQTT Connected";
			wpFZ.DebugWS(wpFZ.strINFO, "connectMqtt", logmessage);
		} else {
			String logmessage =  "failed, rc= " + String(mqttClient.state()) + ",  will try again in 5 seconds";
			wpFZ.DebugWS(wpFZ.strERRROR, "connectMqtt", logmessage);
			delay(5000);
		}
	}
}
