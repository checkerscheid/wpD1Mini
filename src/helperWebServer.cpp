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
//# File-ID      : $Id:: wpFreakaZone.cpp 117 2024-05-29 01:28:02Z                $ #
//#                                                                                 #
//###################################################################################
#include <helperWebServer.h>

helperWebServer wpWebServer();

helperWebServer::helperWebServer() {
	doWebServerCommand = WebServerCommanddoNothing;
	doWebServerBlink = WebServerCommanddoNothing;
	setupWebServer();
}

//###################################################################################
// public
//###################################################################################
void helperWebServer::loop() {
}

uint16_t helperWebServer::getVersion() {
	String SVN = "$Rev: 118 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperWebServer::setupWebServer() {
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
			if(request->getParam("cmd")->value() == "ScanWiFi") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found ScanWiFi");
				setWebServerCommand(WebServerCommandscanWiFi);
			}
			if(request->getParam("cmd")->value() == "calcValues") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found cmd calcValues");
				wpFZ.calcValues = !wpFZ.calcValues;
			}
			if(request->getParam("cmd")->value() == "Blink") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found Blink");
			}
			if(request->getParam("cmd")->value() == "waterEmpty") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found cmd waterEmpty");
				wpFZ.waterEmpty = !wpFZ.waterEmpty;
			}
		}
		request->send(200);
		setWebServerBlink();
	});
	wpFZ.server.begin();
}

void helperWebServer::setWebServerCommand(int8_t command) {
	doWebServerCommand = command;
}

void helperWebServer::setWebServerBlink() {
	doWebServerBlink = WebServerCommandblink;
}

void helperWebServer::doTheWebServerCommand() {
	if(doWebServerCommand > 0) {
		switch(doWebServerCommand) {
			case WebServerCommandpublishSettings:
				wpMqtt.publishSettings(true);
				break;
			case WebServerCommandupdateFW:
				if(wpFZ.setupOta()) {
					wpFZ.UpdateFW = true;
				}
				break;
			case WebServerCommandrestartESP:
				wpMqtt.setMqttOffline();
				ESP.restart();
				break;
			case WebServerCommandscanWiFi:
				wpWiFi.scanWiFi();
				break;
		}
		doWebServerCommand = WebServerCommanddoNothing;
	}
}
void helperWebServer::doTheWebserverBlink() {
	if(doWebServerBlink > 0) {
		wpFZ.blink();
		doWebServerBlink = WebServerCommanddoNothing;
	}
}

//###################################################################################
// private
//###################################################################################

