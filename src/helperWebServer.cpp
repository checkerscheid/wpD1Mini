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
//# Revision     : $Rev:: 122                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWebServer.cpp 122 2024-06-01 07:52:17Z             $ #
//#                                                                                 #
//###################################################################################
#include <helperWebServer.h>

helperWebServer wpWebServer;

helperWebServer::helperWebServer() {}
void helperWebServer::init() {
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/WebServer";
	doWebServerCommand = WebServerCommanddoNothing;
	doWebServerDebugChange = WebServerCommanddoNothing;
	doWebServerBlink = WebServerCommanddoNothing;
	setupWebServer();
}

//###################################################################################
// public
//###################################################################################
void helperWebServer::cycle() {
	doTheWebServerCommand();
	doTheWebServerDebugChange();
	doTheWebserverBlink();
}

uint16_t helperWebServer::getVersion() {
	String SVN = "$Rev: 122 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperWebServer::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugWebServer, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
	EEPROM.commit();
	wpFZ.SendWS("{\"id\":\"DebugWebServer\",\"value\":" + String(Debug ? "true" : "false") + "}");
	wpFZ.blink();
}

void helperWebServer::publishSettings() {
	publishSettings(false);
}
void helperWebServer::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void helperWebServer::publishValues() {
	publishValues(false);
}
void helperWebServer::publishValues(bool force) {
	if(force) publishCountDebug = wpFZ.publishQoS;
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void helperWebServer::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
}
void helperWebServer::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugBasis, wpEEPROM.bitDebugWebServer, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis, wpEEPROM.bitsDebugBasis);
			EEPROM.commit();
			wpFZ.SendWS("{\"id\":\"DebugWebServer\",\"value\":" + String(Debug ? "true" : "false") + "}");
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
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

	webServer.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
		String message = "{\"FreakaZoneDevice\":{";
		message += wpFZ.JsonKeyString("DeviceName", wpFZ.DeviceName) + ",";
		message += wpFZ.JsonKeyString("DeviceDescription", wpFZ.DeviceDescription) + ",";
		message += wpFZ.JsonKeyString("Version", wpFZ.Version) + ",";
		String minimac = WiFi.macAddress();
		minimac.replace(":", "");
		minimac.toLowerCase();
		message += wpFZ.JsonKeyString("MAC", WiFi.macAddress()) + ",";
		message += wpFZ.JsonKeyString("miniMAC", minimac) + ",";
		message += wpFZ.JsonKeyValue("UpdateMode", wpUpdate.UpdateFW ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("calcValues", wpFZ.calcValues ? "true" : "false") + ",";
		if(wpFZ.useModuleDHT11 || wpFZ.useModuleDHT22) {
			message += "\"DHT\":{";
			message += wpFZ.JsonKeyValue("MaxCycleDHT", String(wpDHT.maxCycle)) + ",";
			message += wpFZ.JsonKeyValue("TemperatureCorrection", String(float(wpDHT.temperatureCorrection / 10.0))) + ",";
			message += wpFZ.JsonKeyValue("HumidityCorrection", String(float(wpDHT.humidityCorrection / 10.0)));
			message += "},";
		}
#ifdef wpLDR
		message += "\"LDR\":{";
		message += wpFZ.JsonKeyValue("MaxCycleLDR", String(wpFZ.maxCycleLDR)) + ",";
		message += wpFZ.JsonKeyValue("useLDRAvg", String(wpFZ.useLdrAvg)) + ",";
		message += wpFZ.JsonKeyValue("LDRCorrection", String(wpFZ.ldrCorrection));
		message += "},";
#endif
#ifdef wpLight
		message += "\"Light\":{";
		message += wpFZ.JsonKeyValue("MaxCycleLight", String(wpFZ.maxCycleLight)) + ",";
		message += wpFZ.JsonKeyValue("useLightAvg", String(wpFZ.useLightAvg)) + ",";
		message += wpFZ.JsonKeyValue("LightCorrection", String(wpFZ.lightCorrection));
		message += "},";
#endif
#ifdef wpBM
		message += "\"BM\":{";
	#ifdef wpLDR
		message += "\"LDR\":{";
		message += wpFZ.JsonKeyValue("Threshold", String(wpFZ.threshold)) + ",";
		message += wpFZ.JsonKeyString("LightToTurnOn", wpFZ.lightToTurnOn);
		message += "}";
	#endif
		message += "},";
#endif
#ifdef wpRelais
		message += "\"Relais\":{";
		message += wpFZ.JsonKeyValue("Hand", wpFZ.relaisHand ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("HandValue", wpFZ.relaisHandValue ? "true" : "false");
	#ifdef wpMoisture
		message += ",\"Moisture\":{";
		message += wpFZ.JsonKeyValue("waterEmpty", wpFZ.waterEmpty ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("pumpActive", String(wpFZ.pumpActive)) + ",";
		message += wpFZ.JsonKeyValue("pumpPause", String(wpFZ.pumpPause));
		message += "}";
	#endif
		message += "},";
#endif
#ifdef wpRain
		message += "\"Rain\":{";
		message += wpFZ.JsonKeyValue("MaxCycleRain", String(wpFZ.maxCycleRain)) + ",";
		message += wpFZ.JsonKeyValue("useRainAvg", String(wpFZ.useRainAvg)) + ",";
		message += wpFZ.JsonKeyValue("RainCorrection", String(wpFZ.rainCorrection));
		message += "},";
#endif
#ifdef wpMoisture
		message += "\"Moisture\":{";
		message += wpFZ.JsonKeyValue("MaxCycleMoisture", String(wpFZ.maxCycleMoisture)) + ",";
		message += wpFZ.JsonKeyValue("useMoistureAvg", String(wpFZ.useMoistureAvg)) + ",";
		message += wpFZ.JsonKeyValue("MoistureMin", String(wpFZ.moistureMin)) + ",";
		message += wpFZ.JsonKeyValue("MoistureDry", String(wpFZ.moistureDry)) + ",";
		message += wpFZ.JsonKeyValue("MoistureWet", String(wpFZ.moistureWet));
		message += "},";
#endif
#ifdef wpDistance
		message += "\"Distance\":{";
		message += wpFZ.JsonKeyValue("MaxCycleDistance", String(wpFZ.maxCycleDistance)) + ",";
		message += wpFZ.JsonKeyValue("distanceCorrection", String(wpFZ.distanceCorrection)) + ",";
		message += wpFZ.JsonKeyValue("maxVolume", String(wpFZ.maxVolume)) + ",";
		message += wpFZ.JsonKeyValue("height", String(wpFZ.height));
		message += "},";
#endif
		message += "\"Debug\":{";
		message += wpFZ.JsonKeyValue("EEPROM", wpEEPROM.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("WiFi", wpWiFi.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("MQTT", wpMqtt.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Search", wpFinder.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Rest", wpRest.Debug ? "true" : "false");
		String wpDHTJson = wpFZ.JsonKeyValue("DHT", wpDHT.Debug ? "true" : "false");
		if(wpFZ.useModuleDHT11) {
			wpDHTJson = wpFZ.JsonKeyValue("DHT11", wpDHT.Debug ? "true" : "false");
		}
		if(wpFZ.useModuleDHT22) {
			wpDHTJson = wpFZ.JsonKeyValue("DHT22", wpDHT.Debug ? "true" : "false");
		}
		message += "," + wpDHTJson;
#ifdef wpLDR
		message += "," + wpFZ.JsonKeyValue("LDR", wpFZ.DebugLDR ? "true" : "false");
#endif
#ifdef wpLight
		message += "," + wpFZ.JsonKeyValue("Light", wpFZ.DebugLight ? "true" : "false");
#endif
#ifdef wpBM
		message += "," + wpFZ.JsonKeyValue("BM", wpFZ.DebugBM ? "true" : "false");
#endif
#ifdef wpRelais
		message += "," + wpFZ.JsonKeyValue("Relais", wpFZ.DebugRelais ? "true" : "false");
#endif
#ifdef wpRain
		message += "," + wpFZ.JsonKeyValue("Rain", wpFZ.DebugRain ? "true" : "false");
#endif
#ifdef wpMoisture
		message += "," + wpFZ.JsonKeyValue("Moisture", wpFZ.DebugMoisture ? "true" : "false");
#endif
#ifdef wpDistance
		message += "," + wpFZ.JsonKeyValue("Distance", wpFZ.DebugDistance ? "true" : "false");
#endif
		message += "}}}";
		request->send(200, "application/json", message.c_str());
	});

// commands

	webServer.on("/setDebug", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setDebug");
		if(request->hasParam("Debug")) {
			if(request->getParam("Debug")->value() == "DebugEEPROM") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugEEPROM");
				wpWebServer.setWebServerDebugChange(wpWebServer.cmdDebugEEPROM);
			}
			if(request->getParam("Debug")->value() == "DebugWiFi") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugWiFi");
				wpWebServer.setWebServerDebugChange(wpWebServer.cmdDebugWiFi);
			}
			if(request->getParam("Debug")->value() == "DebugMqtt") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugMqtt");
				wpWebServer.setWebServerDebugChange(wpWebServer.cmdDebugMqtt);
			}
			if(request->getParam("Debug")->value() == "DebugFinder") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugFinder");
				wpWebServer.setWebServerDebugChange(wpWebServer.cmdDebugFinder);
			}
			if(request->getParam("Debug")->value() == "DebugWebServer") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugWebServer");
				wpWebServer.setWebServerDebugChange(wpWebServer.cmdDebugWebServer);
			}
			if(request->getParam("Debug")->value() == "DebugRest") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRest");
				wpWebServer.setWebServerDebugChange(wpWebServer.cmdDebugRest);
			}
			if(request->getParam("Debug")->value() == "DebugDHT") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugDHT");
				wpWebServer.setWebServerDebugChange(wpWebServer.cmdDebugDHT);
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
			if(request->getParam("cmd")->value() == "UpdateCheck") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found UpdateCheck");
				wpWebServer.setWebServerCommand(wpWebServer.WebServerCommandupdateCheck);
			}
			if(request->getParam("cmd")->value() == "UpdateHTTP") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found UpdateHTTP");
				wpWebServer.setWebServerCommand(wpWebServer.WebServerCommandupdateHTTP);
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
		if(doWebServerCommand == WebServerCommandpublishSettings) {
			wpMqtt.publishSettings(true);
		}
		if(doWebServerCommand == WebServerCommandupdateFW) {
			if(wpUpdate.setupOta()) {
				wpUpdate.UpdateFW = true;
			}
		}
		if(doWebServerCommand == WebServerCommandupdateCheck) {
			wpUpdate.check();
		}
		if(doWebServerCommand == WebServerCommandupdateHTTP) {
			wpUpdate.start();
		}
		if(doWebServerCommand == WebServerCommandrestartESP) {
			wpOnlineToggler.setMqttOffline();
			ESP.restart();
		}
		if(doWebServerCommand == WebServerCommandscanWiFi) {
			wpWiFi.scanWiFi();
		}
		doWebServerCommand = WebServerCommanddoNothing;
	}
}
void helperWebServer::doTheWebServerDebugChange() {
	if(doWebServerDebugChange > 0) {
		if(doWebServerDebugChange == cmdDebugEEPROM) {
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
		if(doWebServerDebugChange == cmdDebugWebServer) {
			wpWebServer.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugRest) {
			wpRest.changeDebug();
		}
		if(doWebServerDebugChange == cmdDebugDHT) {
			wpDHT.changeDebug();
		}
		// if(doWebServerDebugChange == cmdDebugLDR) {
		// 	wpLDR.changeDebug();
		// }
		// if(doWebServerDebugChange == cmdDebugLight) {
		// 	wpLight.changeDebug();
		// }
		// if(doWebServerDebugChange == cmdDebugBM) {
		// 	wpBM.changeDebug();
		// }
		// if(doWebServerDebugChange == cmdDebugRelais) {
		// 	wpRelais.changeDebug();
		// }
		// if(doWebServerDebugChange == cmdDebugRain) {
		// 	wpRain.changeDebug();
		// }
		// if(doWebServerDebugChange == cmdDebugMoisture) {
		// 	wpMoisture.changeDebug();
		// }
		// if(doWebServerDebugChange == cmdDebugDistance) {
		// 	wpDistance.changeDebug();
		// }
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
	String returns;
	if(var == "IPADRESS")
		return WiFi.localIP().toString();
	if(var == "DeviceName")
		return wpFZ.DeviceName;
	if(var == "DeviceDescription")
		return wpFZ.DeviceDescription;
	if(var == "CompiledWith") {
		returns = "<ul>";
		if(wpFZ.useModuleDHT11) {
			returns += "<li>wpDHT11</li>";
		}
		if(wpFZ.useModuleDHT22) {
			returns += "<li>wpDHT22</li>";
		}
#ifdef wpLDR
		returns += "<li>wpLDR</li>";
#endif
#ifdef wpLight
		returns += "<li>wpLight</li>";
#endif
#ifdef wpBM
		returns += "<li>wpBM</li>";
#endif
#ifdef wpRelais
		returns += "<li>wpRelais</li>";
#endif
#ifdef wpRain
		returns += "<li>wpRain</li>";
#endif
#ifdef wpMoisture
		returns += "<li>wpMoisture</li>";
#endif
#ifdef wpDistance
		returns += "<li>wpDistance</li>";
#endif
		return returns += "</ul>";
	}
	if(var == "Debug") {
		returns = "<ul><li><span class='bold'>Cmds:</span></li><li><hr /></li>";
		returns += "<li><input id='calcValues' type='checkbox'" + String(wpFZ.calcValues ? " checked" : "") + " onchange='cmdHandle(event)' /><label for='calcValues'>calc Values</label></li>";	
#ifdef wpMoisture
#ifdef wpRelais
		returns += "<li><input id='waterEmpty' type='checkbox'" + String(wpFZ.waterEmpty ? " checked='checked'" : "") + " onchange='cmdHandle(event)' /><label for='waterEmpty'>waterEmpty</label></li>";
#endif
#endif
		returns += "<li><span class='bold'>Debug:</span></li><li><hr /></li>";
		returns += "<li><input id='DebugEEPROM' type='checkbox'" + String(wpEEPROM.Debug ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugEEPROM'>Eprom</label></li>";
		returns += "<li><input id='DebugWiFi' type='checkbox'" + String(wpWiFi.Debug ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugWiFi'>WiFi</label></li>";
		returns += "<li><input id='DebugMqtt' type='checkbox'" + String(wpMqtt.Debug ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugMqtt'>Mqtt</label></li>";
		returns += "<li><input id='DebugFinder' type='checkbox'" + String(wpFinder.Debug ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugFinder'>Finder</label></li>";
		returns += "<li><input id='DebugWebServer' type='checkbox'" + String(wpWebServer.Debug ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugWebServer'>WebServer</label></li>";
		returns += "<li><input id='DebugRest' type='checkbox'" + String(wpRest.Debug ? " checked" : "") + " onchange='changeHandle(event)' /><label for='DebugRest'>Rest</label></li>";
		if(wpFZ.useModuleDHT11 || wpFZ.useModuleDHT22) {
			returns += "<li><input id='DebugDHT' type='checkbox'" + String(wpDHT.Debug ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugDHT'>DHT</label></li>";
		}
#ifdef wpLDR
		returns += "<li><input id='DebugLDR' type='checkbox'" + String(wpFZ.DebugLDR ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugLDR'>LDR</label></li>";
#endif
#ifdef wpLight
		returns += "<li><input id='DebugLight' type='checkbox'" + String(wpFZ.DebugLight ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugLight'>Light</label></li>";
#endif
#ifdef wpBM
		returns += "<li><input id='DebugBM' type='checkbox'" + String(wpFZ.DebugBM ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugBM'>BM</label></li>";
#endif
#ifdef wpRelais
		returns += "<li><input id='DebugRelais' type='checkbox'" + String(wpFZ.DebugRelais ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugRelais'>Relais</label></li>";
#endif
#ifdef wpRain
		returns += "<li><input id='DebugRain' type='checkbox'" + String(wpFZ.DebugRain ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugRain'>Rain</label></li>";
#endif
#ifdef wpMoisture
		returns += "<li><input id='DebugMoisture' type='checkbox'" + String(wpFZ.DebugMoisture ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugMoisture'>Moisture</label></li>";
#endif
#ifdef wpDistance
		returns += "<li><input id='DebugDistance' type='checkbox'" + String(wpFZ.DebugDistance ? " checked='checked'" : "") + " onchange='changeHandle(event)' /><label for='DebugDistance'>Distance</label></li>";
#endif
		return returns += "</ul>";
	}
	return String();
}
