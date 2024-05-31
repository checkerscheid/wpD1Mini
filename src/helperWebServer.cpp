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

helperWebServer wpWebServer;

helperWebServer::helperWebServer() {
	doWebServerCommand = WebServerCommanddoNothing;
	doWebServerDebugChange = WebServerCommanddoNothing;
	doWebServerBlink = WebServerCommanddoNothing;
	setupWebServer();
}

//###################################################################################
// public
//###################################################################################
void helperWebServer::loop() {
	doTheWebServerCommand();
	doTheWebServerDebugChange();
	doTheWebserverBlink();
}

uint16_t helperWebServer::getVersion() {
	String SVN = "$Rev: 118 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperWebServer::setupWebServer() {
	webServer.addHandler(&webSocket);
	webServer.onNotFound([](AsyncWebServerRequest *request){ 
		request->send(404, "text/plain", "Link was not found!");  
	});

	webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", favicon, sizeof(favicon));
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});

	webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "application/json", "{\"this\":{\"is\":{\"a\":{\"FreakaZone\":{\"member\":true}}}}}");
	});

	webServer.on("/print", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/html", index_html, processor);
	});

// commands

	webServer.on("/setDebug", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setDebug");
		if(request->hasParam("Debug")) {
			if(request->getParam("Debug")->value() == "DebugEprom") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugEprom");
				wpWebServer.setWebServerDebugChange(wpEEPROM.DebugEEPROM);
			}
			if(request->getParam("Debug")->value() == "DebugWiFi") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugWiFi");
				wpWebServer.setWebServerDebugChange(wpWiFi.DebugWiFi);
			}
			if(request->getParam("Debug")->value() == "DebugMqtt") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugMqtt");
				wpWebServer.setWebServerDebugChange(wpMqtt.DebugMqtt);
			}
			if(request->getParam("Debug")->value() == "DebugFinder") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugFinder");
				wpWebServer.setWebServerDebugChange(wpFinder.DebugFinder);
			}
			if(request->getParam("Debug")->value() == "DebugRest") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRest");
				wpWebServer.setWebServerDebugChange(wpRest.DebugRest);
			}
			if(request->getParam("Debug")->value() == "DebugRest") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRest");
				wpWebServer.setWebServerDebugChange(wpRest.DebugRest);
			}
			if(request->getParam("Debug")->value() == "DebugHT") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugHT");
				wpWebServer.setWebServerDebugChange(wpDHT.DebugHT);
			}
			// if(request->getParam("Debug")->value() == "DebugLDR") {
			// 	wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugLDR");
			// 	wpWebServer.setWebServerDebugChange(wpFZ.DebugLDR);
			// }
			// if(request->getParam("Debug")->value() == "DebugLight") {
			// 	wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugLight");
			// 	wpWebServer.setWebServerDebugChange(wpFZ.DebugLight);
			// }
			// if(request->getParam("Debug")->value() == "DebugBM") {
			// 	wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugBM");
			// 	wpWebServer.setWebServerDebugChange(wpFZ.DebugBM);
			// }
			// if(request->getParam("Debug")->value() == "DebugRelais") {
			// 	wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRelais");
			// 	wpWebServer.setWebServerDebugChange(wpFZ.DebugRelais);
			// }
			// if(request->getParam("Debug")->value() == "DebugRain") {
			// 	wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRain");
			// 	wpWebServer.setWebServerDebugChange(wpFZ.DebugRain);
			// }
			// if(request->getParam("Debug")->value() == "DebugMoisture") {
			// 	wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugMoisture");
			// 	wpWebServer.setWebServerDebugChange(wpFZ.DebugMoisture);
			// }
			// if(request->getParam("Debug")->value() == "DebugDistance") {
			// 	wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugDistance");
			// 	wpWebServer.setWebServerDebugChange(wpFZ.DebugDistance);
			// }
		}
		request->send(200);
		wpWebServer.setWebServerBlink();
	});

	webServer.on("/setCmd", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCmd");
		if(request->hasParam("cmd")) {
			if(request->getParam("cmd")->value() == "ForceMqttUpdate") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found ForceMqttUpdate");
				wpWebServer.setWebServerCommand(wpWebServer.WebServerCommandpublishSettings);
			}
			if(request->getParam("cmd")->value() == "UpdateFW") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found UpdateFW");
				wpWebServer.setWebServerCommand(wpWebServer.WebServerCommandupdateFW);
			}
			if(request->getParam("cmd")->value() == "RestartDevice") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found RestartDevice");
				wpWebServer.setWebServerCommand(wpWebServer.WebServerCommandrestartESP);
			}
			if(request->getParam("cmd")->value() == "ScanWiFi") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found ScanWiFi");
				wpWebServer.setWebServerCommand(wpWebServer.WebServerCommandscanWiFi);
			}
			if(request->getParam("cmd")->value() == "calcValues") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found cmd calcValues");
				wpFZ.calcValues = !wpFZ.calcValues;
			}
			if(request->getParam("cmd")->value() == "Blink") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found Blink");
			}
			// if(request->getParam("cmd")->value() == "waterEmpty") {
			// 	wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found cmd waterEmpty");
			// 	wpFZ.waterEmpty = !wpFZ.waterEmpty;
			// }
		}
		request->send(200);
		wpWebServer.setWebServerBlink();
	});
	webServer.begin();
}

//###################################################################################
// do the commands
//###################################################################################
void helperWebServer::setWebServerCommand(int8_t command) {
	doWebServerCommand = command;
}
void helperWebServer::setWebServerDebugChange(int8_t debug) {
	doWebServerDebugChange = debug;
}
void helperWebServer::setWebServerBlink() {
	doWebServerBlink = WebServerCommandblink;
}

//###################################################################################
// private
//###################################################################################

void helperWebServer::doTheWebServerCommand() {
	if(doWebServerCommand > 0) {
		switch(doWebServerCommand) {
			case WebServerCommandpublishSettings:
				wpMqtt.publishSettings(true);
				break;
			case WebServerCommandupdateFW:
				if(wpUpdate.setupOta()) {
					wpUpdate.UpdateFW = true;
				}
				break;
			case WebServerCommandrestartESP:
				wpOnlineToggler.setMqttOffline();
				ESP.restart();
				break;
			case WebServerCommandscanWiFi:
				wpWiFi.scanWiFi();
				break;
		}
		doWebServerCommand = WebServerCommanddoNothing;
	}
}
void helperWebServer::doTheWebServerDebugChange() {
	if(doWebServerDebugChange > 0) {
		if(doWebServerDebugChange == cmdDebugEprom) {
			wpEEPROM.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugWiFi) {
			wpWiFi.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugMqtt) {
			wpMqtt.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugFinder) {
			wpFinder.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugRest) {
			wpRest.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugHT) {
			wpDHT.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugLDR) {
			// wpLDR.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugLight) {
			// wpLight.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugBM) {
			// wpBM.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugRelais) {
			// wpRelais.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugRain) {
			// wpRain.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugMoisture) {
			// wpMoisture.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugDistance) {
			// wpDistance.changeDebug();
		}
		doWebServerDebugChange = WebServerCommanddoNothing;
	}
}
void helperWebServer::doTheWebserverBlink() {
	if(doWebServerBlink > 0) {
		wpFZ.blink();
		doWebServerBlink = WebServerCommanddoNothing;
	}
}

//###################################################################################
// stuff
//###################################################################################

String processor(const String& var) {
	String wpHT_s;
	if(var == "IPADRESS")
		return WiFi.localIP().toString();
	if(var == "DeviceName")
		return wpFZ.DeviceName;
	if(var == "DeviceDescription")
		return wpFZ.DeviceDescription;
	if(var == "CompiledWith") {
		wpHT_s = "<ul>";
#ifdef wpHT
	if(wpHT == DHT11)
			wpHT_s += "<li>wpHT HT11</li>";
	if(wpHT == DHT22)
			wpHT_s += "<li>wpHT HT22</li>";
#endif
#ifdef wpLDR
		wpHT_s += "<li>wpLDR</li>";
#endif
#ifdef wpLight
		wpHT_s += "<li>wpLight</li>";
#endif
#ifdef wpBM
		wpHT_s += "<li>wpBM</li>";
#endif
#ifdef wpRelais
		wpHT_s += "<li>wpRelais</li>";
#endif
#ifdef wpRain
		wpHT_s += "<li>wpRain</li>";
#endif
#ifdef wpMoisture
		wpHT_s += "<li>wpMoisture</li>";
#endif
#ifdef wpDistance
		wpHT_s += "<li>wpDistance</li>";
#endif
		return wpHT_s += "</ul>";
	}
	if(var == "Debug") {
		wpHT_s = "<ul><li><span class='bold'>Cmds:</span></li><li><hr /></li>";
		wpHT_s += "<li><input id='calcValues' type='checkbox'" + String(wpFZ.calcValues ? " checked" : "") + " onchange='cmdHandle(event)' /><label for='calcValues'>calc Values</label></li>";	
#ifdef wpMoisture
#ifdef wpRelais
		wpHT_s += "<li><input id='waterEmpty' type='checkbox'" + String(wpFZ.waterEmpty ? " checked='checked'" : "") + " onchange='cmdHandle(event)' /><label for='waterEmpty'>waterEmpty</label></li>";
#endif
#endif
		wpHT_s += "<li><span class='bold'>Debug:</span></li><li><hr /></li>";
		wpHT_s += "<li><input id='DebugEprom' type='checkbox'" + String(wpEEPROM.DebugEEPROM ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugEprom'>Eprom</label></li>";
		wpHT_s += "<li><input id='DebugWiFi' type='checkbox'" + String(wpWiFi.DebugWiFi ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugWiFi'>WiFi</label></li>";
		wpHT_s += "<li><input id='DebugMqtt' type='checkbox'" + String(wpMqtt.DebugMqtt ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugMqtt'>Mqtt</label></li>";
		wpHT_s += "<li><input id='DebugFinder' type='checkbox'" + String(wpFinder.DebugFinder ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugFinder'>Finder</label></li>";
		wpHT_s += "<li><input id='DebugRest' type='checkbox'" + String(wpRest.DebugRest ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugRest'>Rest</label></li>";

#ifdef wpHT
		wpHT_s += "<li><input id='DebugHT' type='checkbox'" + String(wpFZ.DebugHT ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugHT'>HT</label></li>";
#endif
#ifdef wpLDR
		wpHT_s += "<li><input id='DebugLDR' type='checkbox'" + String(wpFZ.DebugLDR ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugLDR'>LDR</label></li>";
#endif
#ifdef wpLight
		wpHT_s += "<li><input id='DebugLight' type='checkbox'" + String(wpFZ.DebugLight ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugLight'>Light</label></li>";
#endif
#ifdef wpBM
		wpHT_s += "<li><input id='DebugBM' type='checkbox'" + String(wpFZ.DebugBM ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugBM'>BM</label></li>";
#endif
#ifdef wpRelais
		wpHT_s += "<li><input id='DebugRelais' type='checkbox'" + String(wpFZ.DebugRelais ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugRelais'>Relais</label></li>";
#endif
#ifdef wpRain
		wpHT_s += "<li><input id='DebugRain' type='checkbox'" + String(wpFZ.DebugRain ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugRain'>Rain</label></li>";
#endif
#ifdef wpMoisture
		wpHT_s += "<li><input id='DebugMoisture' type='checkbox'" + String(wpFZ.DebugMoisture ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugMoisture'>Moisture</label></li>";
#endif
#ifdef wpDistance
		wpHT_s += "<li><input id='DebugDistance' type='checkbox'" + String(wpFZ.DebugDistance ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugDistance'>Distance</label></li>";
#endif
		return wpHT_s += "</ul>";
	}
	return String();
}
