//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 29.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 118                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 118 2024-05-29 01:29:33Z                          $ #
//#                                                                                 #
//###################################################################################
#include <helperUpdate.h>

helperUpdate wpUpdate();

helperUpdate::helperUpdate() {
	// values
	mqttTopicUpdateMode = wpFZ.DeviceName + "/UpdateMode";
	mqttTopicRestartRequired = wpFZ.DeviceName + "/RestartRequired";
	// commands
	mqttTopicUpdateFW = wpFZ.DeviceName + "/UpdateFW";
}

//###################################################################################
// public
//###################################################################################

void helperUpdate::loop() {
	if(UpdateFW) ArduinoOTA.handle();
}

uint16_t helperUpdate::getVersion() {
	String SVN = "$Rev: 118 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperUpdate::check() {

}

void helperUpdate::start() {
	WiFiClient client;
	ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

	// Add optional callback notifiers
	ESPhttpUpdate.onStart(started);
	ESPhttpUpdate.onEnd(finished);
	ESPhttpUpdate.onProgress(progress);
	ESPhttpUpdate.onError(error);

	t_httpUpdate_return ret = ESPhttpUpdate.update(client, wpFZ.updateServer);
	// Or:
	// t_httpUpdate_return ret = ESPhttpUpdate.update(client, "server", 80, "file.bin");

	switch (ret) {
		case HTTP_UPDATE_FAILED:
			wpFZ.DebugWS(wpFZ.strERRROR, "wpUpdate::start", "HTTP_UPDATE_FAILD Error (" +
				String(ESPhttpUpdate.getLastError()) + "): " +
				ESPhttpUpdate.getLastErrorString());
			break;

		case HTTP_UPDATE_NO_UPDATES:
			wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::start", "HTTP_UPDATE_NO_UPDATES");
			break;

		case HTTP_UPDATE_OK:
			wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::start", "HTTP_UPDATE_OK");
			break;
	}
}

//###################################################################################
// private
//###################################################################################

bool helperUpdate::setupOta() {
	bool returns = false;
	ArduinoOTA.onStart([]() {
		String logmessage = "OTA Start";
		wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage);
	});
	ArduinoOTA.onEnd([]() {
		wpOnlineToggler.OfflineTrigger = true;
		String logmessage = "OTA End";
		wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage);
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		String logmessage = "OTA Progress: " + String((progress / (total / 100))) + " %";
		wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage, false);
	});
	ArduinoOTA.onError([](ota_error_t error) {
		String logmessage = "Error (" + String(error) + ") ";
		if (error == OTA_AUTH_ERROR) logmessage += "Auth Failed";
		else if (error == OTA_BEGIN_ERROR) logmessage += "Begin Failed";
		else if (error == OTA_CONNECT_ERROR) logmessage += "Connect Failed";
		else if (error == OTA_RECEIVE_ERROR) logmessage += "Receive Failed";
		else if (error == OTA_END_ERROR) logmessage += "End Failed";
		wpFZ.DebugWS(wpFZ.strERRROR, "setupOta", logmessage);
	});
	ArduinoOTA.begin();
	returns = true;
	String logmessage = "OTA Ready, IP address: " + WiFi.localIP().toString();
	wpFZ.DebugWS(wpFZ.strINFO, "setupOta", logmessage);
	return returns;
}

void helperUpdate::started() {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::started", "HTTP update started");
}

void helperUpdate::finished() {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::finished", "HTTP update finished");
}

void helperUpdate::progress(int cur, int total) {
	String logmessage = "HTTP update: " + String(cur) + " of " + String(total) + " bytes";
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::progress", logmessage, false);
}

void helperUpdate::error(int err) {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::error", "HTTP update fatal error, code: " + String(err));
}
