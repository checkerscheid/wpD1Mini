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
//# Revision     : $Rev:: 149                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWebServer.cpp 149 2024-06-20 23:26:18Z             $ #
//#                                                                                 #
//###################################################################################
#include <helperWebServer.h>

helperWebServer wpWebServer;

helperWebServer::helperWebServer() {}
void helperWebServer::init() {
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/WebServer";
	doCommand = cmdDoNothing;
	doModuleChange = cmdDoNothing;
	doSendRestChange = cmdDoNothing;
	doDebugChange = cmdDoNothing;
	doBlink = cmdDoNothing;
	setupWebServer();
}

//###################################################################################
// public
//###################################################################################
void helperWebServer::cycle() {
	doTheCommand();
	doTheModuleChange();
	doTheSendRestChange();
	doTheDebugChange();
	doTheBlink();
	publishValues();
}

uint16_t helperWebServer::getVersion() {
	String SVN = "$Rev: 149 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperWebServer::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugWebServer, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugWebServer", Debug);
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
			bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugWebServer, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
			EEPROM.commit();
			wpFZ.SendWSDebug("DebugWebServer", Debug);
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
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			message += "\"DHT\":{";
			message += wpFZ.JsonKeyValue("MaxCycleDHT", String(wpDHT.MaxCycle())) + ",";
			message += wpFZ.JsonKeyValue("TemperatureCorrection", String(float(wpDHT.temperatureCorrection / 10.0))) + ",";
			message += wpFZ.JsonKeyValue("HumidityCorrection", String(float(wpDHT.humidityCorrection / 10.0)));
			message += "},";
		}
		if(wpModules.useModuleLDR) {
			message += "\"LDR\":{";
			message += wpFZ.JsonKeyValue("MaxCycleLDR", String(wpLDR.MaxCycle())) + ",";
			message += wpFZ.JsonKeyValue("useLDRAvg", wpLDR.UseAvg() ? "true" : "false") + ",";
			message += wpFZ.JsonKeyValue("LDRCorrection", String(wpLDR.correction));
			message += "},";
		}
		if(wpModules.useModuleLight) {
			message += "\"Light\":{";
			message += wpFZ.JsonKeyValue("MaxCycleLight", String(wpLight.MaxCycle())) + ",";
			message += wpFZ.JsonKeyValue("useLightAvg", wpLight.UseAvg() ? "true" : "false") + ",";
			message += wpFZ.JsonKeyValue("LightCorrection", String(wpLight.correction));
			message += "},";
		}
		if(wpModules.useModuleBM) {
			message += "\"BM\":{";
			if(wpModules.useModuleLDR) {
				message += "\"LDR\":{";
				message += wpFZ.JsonKeyValue("Threshold", String(wpBM.threshold)) + ",";
				message += wpFZ.JsonKeyString("LightToTurnOn", wpBM.lightToTurnOn);
				message += "}";
			}
			message += "},";
		}
		if(wpModules.useModuleFK) {
			message += "\"FK\":{";
			if(wpModules.useModuleLDR) {
				message += "\"LDR\":{";
				message += wpFZ.JsonKeyValue("Threshold", String(wpFK.threshold)) + ",";
				message += wpFZ.JsonKeyString("LightToTurnOn", wpFK.lightToTurnOn);
				message += "}";
			}
			message += "},";
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			message += "\"Relais\":{";
			message += wpFZ.JsonKeyValue("Hand", wpRelais.handError ? "true" : "false") + ",";
			message += wpFZ.JsonKeyValue("HandValue", wpRelais.handValue ? "true" : "false");
			if(wpModules.useModuleMoisture) {
				message += ",\"Moisture\":{";
				message += wpFZ.JsonKeyValue("waterEmpty", wpRelais.waterEmptySet ? "true" : "false") + ",";
				message += wpFZ.JsonKeyValue("pumpActive", String(wpRelais.pumpActive)) + ",";
				message += wpFZ.JsonKeyValue("pumpPause", String(wpRelais.pumpPause));
				message += "}";
			}
			message += "},";
		}
		if(wpModules.useModuleRain) {
			message += "\"Rain\":{";
			message += wpFZ.JsonKeyValue("MaxCycleRain", String(wpRain.MaxCycle())) + ",";
			message += wpFZ.JsonKeyValue("useRainAvg", wpRain.UseAvg() ? "true" : "false") + ",";
			message += wpFZ.JsonKeyValue("RainCorrection", String(wpRain.correction));
			message += "},";
		}
		if(wpModules.useModuleMoisture) {
			message += "\"Moisture\":{";
			message += wpFZ.JsonKeyValue("MaxCycleMoisture", String(wpMoisture.MaxCycle())) + ",";
			message += wpFZ.JsonKeyValue("useMoistureAvg", wpMoisture.UseAvg() ? "true" : "false") + ",";
			message += wpFZ.JsonKeyValue("MoistureMin", String(wpMoisture.minValue)) + ",";
			message += wpFZ.JsonKeyValue("MoistureDry", String(wpMoisture.dry)) + ",";
			message += wpFZ.JsonKeyValue("MoistureWet", String(wpMoisture.wet));
			message += "},";
		}
		if(wpModules.useModuleDistance) {
			message += "\"Distance\":{";
			message += wpFZ.JsonKeyValue("MaxCycleDistance", String(wpDistance.MaxCycle())) + ",";
			message += wpFZ.JsonKeyValue("distanceCorrection", String(wpDistance.correction)) + ",";
			message += wpFZ.JsonKeyValue("maxVolume", String(wpDistance.maxVolume)) + ",";
			message += wpFZ.JsonKeyValue("height", String(wpDistance.height));
			message += "},";
		}
		message += "\"Debug\":{";
		message += wpFZ.JsonKeyValue("EEPROM", wpEEPROM.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Finder", wpFinder.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Modules", wpModules.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("MQTT", wpMqtt.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("OnlineToggler", wpOnlineToggler.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Rest", wpRest.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Update", wpUpdate.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("WebServer", wpWebServer.Debug ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("WiFi", wpWiFi.Debug ? "true" : "false");
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			message += "," + wpFZ.JsonKeyValue("DHT", wpDHT.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleLDR) {
			message += "," + wpFZ.JsonKeyValue("LDR", wpLDR.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleLight) {
			message += "," + wpFZ.JsonKeyValue("Light", wpLight.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleBM) {
			message += "," + wpFZ.JsonKeyValue("BM", wpBM.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleFK) {
			message += "," + wpFZ.JsonKeyValue("FK", wpFK.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			message += "," + wpFZ.JsonKeyValue("Relais", wpRelais.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleRain) {
			message += "," + wpFZ.JsonKeyValue("Rain", wpRain.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleMoisture) {
			message += "," + wpFZ.JsonKeyValue("Moisture", wpMoisture.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleDistance) {
			message += "," + wpFZ.JsonKeyValue("Distance", wpDistance.Debug() ? "true" : "false");
		}
		message += "},\"SendRest\":{";
		message += wpFZ.JsonKeyValue("WiFi", wpWiFi.sendRest ? "true" : "false");
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			message += "," + wpFZ.JsonKeyValue("DHT", wpDHT.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleLDR) {
			message += "," + wpFZ.JsonKeyValue("LDR", wpLDR.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleLight) {
			message += "," + wpFZ.JsonKeyValue("Light", wpLight.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleBM) {
			message += "," + wpFZ.JsonKeyValue("BM", wpBM.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleFK) {
			message += "," + wpFZ.JsonKeyValue("FK", wpFK.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			message += "," + wpFZ.JsonKeyValue("Relais", wpRelais.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleRain) {
			message += "," + wpFZ.JsonKeyValue("Rain", wpRain.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleMoisture) {
			message += "," + wpFZ.JsonKeyValue("Moisture", wpMoisture.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleDistance) {
			message += "," + wpFZ.JsonKeyValue("Distance", wpDistance.SendRest() ? "true" : "false");
		}
		message += "},\"useModul\":{";
		message += wpFZ.JsonKeyValue("DHT11", wpModules.useModuleDHT11 ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("DHT22", wpModules.useModuleDHT22 ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("LDR", wpModules.useModuleLDR ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Light", wpModules.useModuleLight ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("BM", wpModules.useModuleBM ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("FK", wpModules.useModuleFK ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Relais", wpModules.useModuleRelais ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("RelaisShield", wpModules.useModuleRelaisShield ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Rain", wpModules.useModuleRain ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Moisture", wpModules.useModuleMoisture ? "true" : "false") + ",";
		message += wpFZ.JsonKeyValue("Distance", wpModules.useModuleDistance ? "true" : "false");
		message += "}}}";
		request->send(200, "application/json", message.c_str());
	});

// commands

	webServer.on("/setModule", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setModule");
		if(request->hasParam("Module")) {
			if(request->getParam("Module")->value() == "useDHT11") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useDHT11");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleDHT11);
			}
			if(request->getParam("Module")->value() == "useDHT22") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useDHT22");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleDHT22);
			}
			if(request->getParam("Module")->value() == "useLDR") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useLDR");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleLDR);
			}
			if(request->getParam("Module")->value() == "useLight") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useLight");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleLight);
			}
			if(request->getParam("Module")->value() == "useBM") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useBM");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleBM);
			}
			if(request->getParam("Module")->value() == "useFK") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useFK");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleFK);
			}
			if(request->getParam("Module")->value() == "useRelais") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useRelais");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRelais);
			}
			if(request->getParam("Module")->value() == "useRelaisShield") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useRelaisShield");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRelaisShield);
			}
			if(request->getParam("Module")->value() == "useRain") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useRain");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRain);
			}
			if(request->getParam("Module")->value() == "useMoisture") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useMoisture");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleMoisture);
			}
			if(request->getParam("Module")->value() == "useDistance") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useDistance");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleDistance);
			}
		}
		request->send(200, "application/json", "{\"erg\":\"S_OK\"}");
		wpWebServer.setBlink();
	});

	webServer.on("/setSendRest", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setSendRest");
		if(request->hasParam("sendRest")) {
			if(request->getParam("sendRest")->value() == "sendRestWiFi") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestWiFi");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestWiFi);
			}
			if(request->getParam("sendRest")->value() == "sendRestDHT") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestDHT");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestDHT);
			}
			if(request->getParam("sendRest")->value() == "sendRestLDR") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestLDR");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestLDR);
			}
			if(request->getParam("sendRest")->value() == "sendRestLight") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestLight");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestLight);
			}
			if(request->getParam("sendRest")->value() == "sendRestBM") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestBM");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestBM);
			}
			if(request->getParam("sendRest")->value() == "sendRestFK") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestFK");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestFK);
			}
			if(request->getParam("sendRest")->value() == "sendRestRelais") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestRelais");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestRelais);
			}
			if(request->getParam("sendRest")->value() == "sendRestRain") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestRain");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestRain);
			}
			if(request->getParam("sendRest")->value() == "sendRestMoisture") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestMoisture");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestMoisture);
			}
			if(request->getParam("sendRest")->value() == "sendRestDistance") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestDistance");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestDistance);
			}
		}
		request->send(200, "application/json", "{\"erg\":\"S_OK\"}");
		wpWebServer.setBlink();
	});

	webServer.on("/setDebug", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setDebug");
		if(request->hasParam("Debug")) {
			if(request->getParam("Debug")->value() == "DebugEEPROM") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugEEPROM");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugEEPROM);
			}
			if(request->getParam("Debug")->value() == "DebugFinder") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugFinder");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugFinder);
			}
			if(request->getParam("Debug")->value() == "DebugModules") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugModules");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugModules);
			}
			if(request->getParam("Debug")->value() == "DebugMqtt") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugMqtt");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugMqtt);
			}
			if(request->getParam("Debug")->value() == "DebugOnlineToggler") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugOnlineToggler");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugOnlineToggler);
			}
			if(request->getParam("Debug")->value() == "DebugRest") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRest");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugRest);
			}
			if(request->getParam("Debug")->value() == "DebugUpdate") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugUpdate");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUpdate);
			}
			if(request->getParam("Debug")->value() == "DebugWebServer") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugWebServer");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugWebServer);
			}
			if(request->getParam("Debug")->value() == "DebugWiFi") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugWiFi");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugWiFi);
			}
			if(request->getParam("Debug")->value() == "DebugDHT") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugDHT");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugDHT);
			}
			if(request->getParam("Debug")->value() == "DebugLDR") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugLDR");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugLDR);
			}
			if(request->getParam("Debug")->value() == "DebugLight") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugLight");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugLight);
			}
			if(request->getParam("Debug")->value() == "DebugBM") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugBM");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugBM);
			}
			if(request->getParam("Debug")->value() == "DebugFK") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugFK");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugFK);
			}
			if(request->getParam("Debug")->value() == "DebugRelais") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRelais");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugRelais);
			}
			if(request->getParam("Debug")->value() == "DebugRain") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRain");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugRain);
			}
			if(request->getParam("Debug")->value() == "DebugMoisture") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugMoisture");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugMoisture);
			}
			if(request->getParam("Debug")->value() == "DebugDistance") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugDistance");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugDistance);
			}
		}
		request->send(200, "application/json", "{\"erg\":\"S_OK\"}");
		wpWebServer.setBlink();
	});

	webServer.on("/setCmd", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCmd");
		if(request->hasParam("cmd")) {
			if(request->getParam("cmd")->value() == "ForceMqttUpdate") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found ForceMqttUpdate");
				wpWebServer.setCommand(wpWebServer.cmdForceMqttUpdate);
			}
			if(request->getParam("cmd")->value() == "ForceRenewValue") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found ForceRenewValue");
				wpWebServer.setCommand(wpWebServer.cmdForceRenewValue);
			}
			if(request->getParam("cmd")->value() == "UpdateFW") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found UpdateFW");
				wpWebServer.setCommand(wpWebServer.cmdUpdateFW);
			}
			if(request->getParam("cmd")->value() == "UpdateCheck") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found UpdateCheck");
				wpWebServer.setCommand(wpWebServer.cmdUpdateCheck);
			}
			if(request->getParam("cmd")->value() == "UpdateHTTP") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found UpdateHTTP");
				wpWebServer.setCommand(wpWebServer.cmdUpdateHTTP);
			}
			if(request->getParam("cmd")->value() == "RestartDevice") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found RestartDevice");
				wpWebServer.setCommand(wpWebServer.cmdRestartESP);
			}
			if(request->getParam("cmd")->value() == "ScanWiFi") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found ScanWiFi");
				wpWebServer.setCommand(wpWebServer.cmdScanWiFi);
			}
			if(request->getParam("cmd")->value() == "CheckDns") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found CheckDns");
				wpWebServer.setCommand(wpWebServer.cmdCheckDns);
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
				wpRelais.waterEmptySet = !wpRelais.waterEmptySet;
			}
		}
		request->send(200, "application/json", "{\"erg\":\"S_OK\"}");
		wpWebServer.setBlink();
	});
	webServer.begin();
}

//###################################################################################
// do the commands
//###################################################################################
void helperWebServer::setCommand(int8_t command) {
	doCommand = command;
}
void helperWebServer::setModuleChange(int8_t module) {
	doModuleChange = module;
}
void helperWebServer::setSendRestChange(int8_t sendRest) {
	doSendRestChange = sendRest;
}
void helperWebServer::setDebugChange(int8_t debug) {
	doDebugChange = debug;
}
void helperWebServer::setBlink() {
	doBlink = cmdBlink;
}

//###################################################################################
// private
//###################################################################################

void helperWebServer::doTheCommand() {
	if(doCommand > 0) {
		if(doCommand == cmdForceMqttUpdate) {
			wpModules.publishAllSettings(true);
			wpModules.publishAllValues(true);
		}
		if(doCommand == cmdForceRenewValue) {
			wpModules.publishAllValues(true);
		}
		if(doCommand == cmdUpdateFW) {
			if(wpUpdate.setupOta()) {
				wpUpdate.UpdateFW = true;
			}
		}
		if(doCommand == cmdUpdateCheck) {
			wpUpdate.check();
		}
		if(doCommand == cmdUpdateHTTP) {
			wpUpdate.start();
		}
		if(doCommand == cmdRestartESP) {
			wpOnlineToggler.setMqttOffline();
			ESP.restart();
		}
		if(doCommand == cmdScanWiFi) {
			wpWiFi.scanWiFi();
		}
		if(doCommand == cmdCheckDns) {
			wpWiFi.checkDns();
		}
		doCommand = cmdDoNothing;
	}
}

void helperWebServer::doTheModuleChange() {
	if(doModuleChange > 0) {
		if(doModuleChange == cmdModuleDHT11) wpModules.changeModuleDHT11(!wpModules.useModuleDHT11);
		if(doModuleChange == cmdModuleDHT22) wpModules.changeModuleDHT22(!wpModules.useModuleDHT22);
		if(doModuleChange == cmdModuleLDR) wpModules.changeModuleLDR(!wpModules.useModuleLDR);
		if(doModuleChange == cmdModuleLight) wpModules.changeModuleLight(!wpModules.useModuleLight);
		if(doModuleChange == cmdModuleBM) wpModules.changeModuleBM(!wpModules.useModuleBM);
		if(doModuleChange == cmdModuleFK) wpModules.changeModuleFK(!wpModules.useModuleFK);
		if(doModuleChange == cmdModuleRelais) wpModules.changeModuleRelais(!wpModules.useModuleRelais);
		if(doModuleChange == cmdModuleRelaisShield) wpModules.changeModuleRelaisShield(!wpModules.useModuleRelaisShield);
		if(doModuleChange == cmdModuleRain) wpModules.changeModuleRain(!wpModules.useModuleRain);
		if(doModuleChange == cmdModuleMoisture) wpModules.changeModuleMoisture(!wpModules.useModuleMoisture);
		if(doModuleChange == cmdModuleDistance) wpModules.changeModuleDistance(!wpModules.useModuleDistance);
		doModuleChange = cmdDoNothing;
	}
}

void helperWebServer::doTheSendRestChange() {
	if(doSendRestChange > 0) {
		if(doSendRestChange == cmdSendRestWiFi) wpWiFi.changeSendRest();
		if(doSendRestChange == cmdSendRestDHT) wpDHT.changeSendRest();
		if(doSendRestChange == cmdSendRestLDR) wpLDR.changeSendRest();
		if(doSendRestChange == cmdSendRestLight) wpLight.changeSendRest();
		if(doSendRestChange == cmdSendRestBM) wpBM.changeSendRest();
		if(doSendRestChange == cmdSendRestFK) wpFK.changeSendRest();
		if(doSendRestChange == cmdSendRestRelais) wpRelais.changeSendRest();
		if(doSendRestChange == cmdSendRestRain) wpRain.changeSendRest();
		if(doSendRestChange == cmdSendRestMoisture) wpMoisture.changeSendRest();
		if(doSendRestChange == cmdSendRestDistance) wpDistance.changeSendRest();
		doSendRestChange = cmdDoNothing;
	}
}

void helperWebServer::doTheDebugChange() {
	if(doDebugChange > 0) {
		if(doDebugChange == cmdDebugEEPROM) wpEEPROM.changeDebug();
		if(doDebugChange == cmdDebugFinder) wpFinder.changeDebug();
		if(doDebugChange == cmdDebugModules) wpModules.changeDebug();
		if(doDebugChange == cmdDebugMqtt) wpMqtt.changeDebug();
		if(doDebugChange == cmdDebugOnlineToggler) wpOnlineToggler.changeDebug();
		if(doDebugChange == cmdDebugRest) wpRest.changeDebug();
		if(doDebugChange == cmdDebugUpdate) wpUpdate.changeDebug();
		if(doDebugChange == cmdDebugWebServer) wpWebServer.changeDebug();
		if(doDebugChange == cmdDebugWiFi) wpWiFi.changeDebug();
		if(doDebugChange == cmdDebugDHT) wpDHT.changeDebug();
		if(doDebugChange == cmdDebugLDR) wpLDR.changeDebug();
		if(doDebugChange == cmdDebugLight) wpLight.changeDebug();
		if(doDebugChange == cmdDebugBM) wpBM.changeDebug();
		if(doDebugChange == cmdDebugFK) wpFK.changeDebug();
		if(doDebugChange == cmdDebugRelais) wpRelais.changeDebug();
		if(doDebugChange == cmdDebugRain) wpRain.changeDebug();
		if(doDebugChange == cmdDebugMoisture) wpMoisture.changeDebug();
		if(doDebugChange == cmdDebugDistance) wpDistance.changeDebug();
		doDebugChange = cmdDoNothing;
	}
}

void helperWebServer::doTheBlink() {
	if(doBlink > 0) {
		wpFZ.blink();
		doBlink = cmdDoNothing;
	}
}

//###################################################################################
// stuff
//###################################################################################

String processor(const String& var) {
	String returns;
//###################################################################################
	if(var == "IPADRESS")
		return WiFi.localIP().toString();
//###################################################################################
	if(var == "DeviceName")
		return wpFZ.DeviceName;
//###################################################################################
	if(var == "DeviceDescription")
		return wpFZ.DeviceDescription;
//###################################################################################
	if(var == "Version")
		return wpFZ.Version;
//###################################################################################
	if(var == "CompiledWith") {
		returns = "<ul><li><span class='bold'>Modules:</span></li><li><hr /></li>"
		"<li><input id='useDHT11' type='checkbox'" + String(wpModules.useModuleDHT11 ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useDHT11'>wpDHT11</label></li>" +
		"<li><input id='useDHT22' type='checkbox'" + String(wpModules.useModuleDHT22 ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useDHT22'>wpDHT22</label></li>" +
		"<li><input id='useLDR' type='checkbox'" + String(wpModules.useModuleLDR ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useLDR'>wpLDR</label></li>" +
		"<li><input id='useLight' type='checkbox'" + String(wpModules.useModuleLight ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useLight'>wpLight</label></li>" +
		"<li><input id='useBM' type='checkbox'" + String(wpModules.useModuleBM ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useBM'>wpBM</label></li>" +
		"<li><input id='useFK' type='checkbox'" + String(wpModules.useModuleFK ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useFK'>wpFK</label></li>" +
		"<li><input id='useRelais' type='checkbox'" + String(wpModules.useModuleRelais ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useRelais'>wpRelais</label></li>" +
		"<li><input id='useRelaisShield' type='checkbox'" + String(wpModules.useModuleRelaisShield ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useRelaisShield'>wpRelaisShield</label></li>" +
		"<li><input id='useRain' type='checkbox'" + String(wpModules.useModuleRain ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useRain'>wpRain</label></li>" +
		"<li><input id='useMoisture' type='checkbox'" + String(wpModules.useModuleMoisture ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useMoisture'>wpMoisture</label></li>" +
		"<li><input id='useDistance' type='checkbox'" + String(wpModules.useModuleDistance ? " checked" : "") +
			" onchange='changeModule(event)' /><label for='useDistance'>wpDistance</label></li>";
		return returns += "</ul>";
	}
//###################################################################################
	if(var == "Debug") {
		returns = "<ul><li><span class='bold'>Cmds:</span></li><li><hr /></li>"
			"<li><input id='calcValues' type='checkbox'" + String(wpFZ.calcValues ? " checked" : "") +
			" onchange='cmdHandle(event)' /><label for='calcValues'>calc Values</label></li>";
		if((wpModules.useModuleRelais || wpModules.useModuleRelaisShield) && wpModules.useModuleMoisture) {
			returns += "<li><input id='waterEmpty' type='checkbox'" + String(wpRelais.waterEmptySet ? " checked" : "") +
				" onchange='cmdHandle(event)' /><label for='waterEmpty'>waterEmpty</label></li>";
		}
		returns += "<li><span class='bold'>Debug:</span></li><li><hr /></li>"
			"<li><input id='DebugEEPROM' type='checkbox'" + String(wpEEPROM.Debug ? " checked" : "") + 
				" onchange='changeDebug(event)' /><label for='DebugEEPROM'>Eprom</label></li>" +
			"<li><input id='DebugFinder' type='checkbox'" + String(wpFinder.Debug ? " checked" : "") + 
				" onchange='changeDebug(event)' /><label for='DebugFinder'>Finder</label></li>" +
			"<li><input id='DebugModules' type='checkbox'" + String(wpModules.Debug ? " checked" : "") + 
				" onchange='changeDebug(event)' /><label for='DebugModules'>Modules</label></li>" +
			"<li><input id='DebugMqtt' type='checkbox'" + String(wpMqtt.Debug ? " checked" : "") + 
				" onchange='changeDebug(event)' /><label for='DebugMqtt'>Mqtt</label></li>" +
			"<li><input id='DebugOnlineToggler' type='checkbox'" + String(wpOnlineToggler.Debug ? " checked" : "") + 
				" onchange='changeDebug(event)' /><label for='DebugOnlineToggler'>OnlineToggler</label></li>" +
			"<li><input id='DebugRest' type='checkbox'" + String(wpRest.Debug ? " checked" : "") + 
				" onchange='changeDebug(event)' /><label for='DebugRest'>Rest</label></li>" +
			"<li><input id='DebugUpdate' type='checkbox'" + String(wpUpdate.Debug ? " checked" : "") + 
				" onchange='changeDebug(event)' /><label for='DebugUpdate'>Update</label></li>" +
			"<li><input id='DebugWebServer' type='checkbox'" + String(wpWebServer.Debug ? " checked" : "") + 
				" onchange='changeDebug(event)' /><label for='DebugWebServer'>WebServer</label></li>" +
			"<li><input id='DebugWiFi' type='checkbox'" + String(wpWiFi.Debug ? " checked" : "") + 
				" onchange='changeDebug(event)' /><label for='DebugWiFi'>WiFi</label></li>" +
			"<li><hr /></li>";
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			returns += "<li><input id='DebugDHT' type='checkbox'" + String(wpDHT.Debug() ? " checked" : "") +
				" onchange='changeDebug(event)' /><label for='DebugDHT'>DHT</label></li>";
		}
		if(wpModules.useModuleLDR) {
			returns += "<li><input id='DebugLDR' type='checkbox'" + String(wpLDR.Debug() ? " checked" : "") +
				" onchange='changeDebug(event)' /><label for='DebugLDR'>LDR</label></li>";
		}
		if(wpModules.useModuleLight) {
			returns += "<li><input id='DebugLight' type='checkbox'" + String(wpLight.Debug() ? " checked" : "") +
				" onchange='changeDebug(event)' /><label for='DebugLight'>Light</label></li>";
		}
		if(wpModules.useModuleBM) {
			returns += "<li><input id='DebugBM' type='checkbox'" + String(wpBM.Debug() ? " checked" : "") +
				" onchange='changeDebug(event)' /><label for='DebugBM'>BM</label></li>";
		}
		if(wpModules.useModuleFK) {
			returns += "<li><input id='DebugFK' type='checkbox'" + String(wpFK.Debug() ? " checked" : "") +
				" onchange='changeDebug(event)' /><label for='DebugFK'>FK</label></li>";
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			returns += "<li><input id='DebugRelais' type='checkbox'" + String(wpRelais.Debug() ? " checked" : "") +
				" onchange='changeDebug(event)' /><label for='DebugRelais'>Relais</label></li>";
		}
		if(wpModules.useModuleRain) {
			returns += "<li><input id='DebugRain' type='checkbox'" + String(wpRain.Debug() ? " checked" : "") +
				" onchange='changeDebug(event)' /><label for='DebugRain'>Rain</label></li>";
		}
		if(wpModules.useModuleMoisture) {
			returns += "<li><input id='DebugMoisture' type='checkbox'" + String(wpMoisture.Debug() ? " checked" : "") +
				" onchange='changeDebug(event)' /><label for='DebugMoisture'>Moisture</label></li>";
		}
		if(wpModules.useModuleDistance) {
			returns += "<li><input id='DebugDistance' type='checkbox'" + String(wpDistance.Debug() ? " checked" : "") +
				" onchange='changeDebug(event)' /><label for='DebugDistance'>Distance</label></li>";
		}
		return returns += "</ul>";
	}
//###################################################################################
	if(var == "SendRest") {
		returns = "<ul><li><span class='bold'>SendRest:</span></li><li><hr /></li>"
			"<li><input id='sendRestWiFi' type='checkbox'" + String(wpWiFi.sendRest ? " checked" : "") + 
				" onchange='changeSendRest(event)' /><label for='sendRestWiFi'>WiFi</label></li>" +
			"<li><hr /></li>";
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			returns += "<li><input id='sendRestDHT' type='checkbox'" + String(wpDHT.SendRest() ? " checked" : "") +
				" onchange='changeSendRest(event)' /><label for='sendRestDHT'>DHT</label></li>";
		}
		if(wpModules.useModuleLDR) {
			returns += "<li><input id='sendRestLDR' type='checkbox'" + String(wpLDR.SendRest() ? " checked" : "") +
				" onchange='changeSendRest(event)' /><label for='sendRestLDR'>LDR</label></li>";
		}
		if(wpModules.useModuleLight) {
			returns += "<li><input id='sendRestLight' type='checkbox'" + String(wpLight.SendRest() ? " checked" : "") +
				" onchange='changeSendRest(event)' /><label for='sendRestLight'>Light</label></li>";
		}
		if(wpModules.useModuleBM) {
			returns += "<li><input id='sendRestBM' type='checkbox'" + String(wpBM.SendRest() ? " checked" : "") +
				" onchange='changeSendRest(event)' /><label for='sendRestBM'>BM</label></li>";
		}
		if(wpModules.useModuleFK) {
			returns += "<li><input id='sendRestFK' type='checkbox'" + String(wpFK.SendRest() ? " checked" : "") +
				" onchange='changeSendRest(event)' /><label for='sendRestFK'>FK</label></li>";
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			returns += "<li><input id='sendRestRelais' type='checkbox'" + String(wpRelais.SendRest() ? " checked" : "") +
				" onchange='changeSendRest(event)' /><label for='sendRestRelais'>Relais</label></li>";
		}
		if(wpModules.useModuleRain) {
			returns += "<li><input id='sendRestRain' type='checkbox'" + String(wpRain.SendRest() ? " checked" : "") +
				" onchange='changeSendRest(event)' /><label for='sendRestRain'>Rain</label></li>";
		}
		if(wpModules.useModuleMoisture) {
			returns += "<li><input id='sendRestMoisture' type='checkbox'" + String(wpMoisture.SendRest() ? " checked" : "") +
				" onchange='changeSendRest(event)' /><label for='sendRestMoisture'>Moisture</label></li>";
		}
		if(wpModules.useModuleDistance) {
			returns += "<li><input id='sendRestDistance' type='checkbox'" + String(wpDistance.SendRest() ? " checked" : "") +
				" onchange='changeSendRest(event)' /><label for='sendRestDistance'>Distance</label></li>";
		}
		return returns += "</ul>";
	}
//###################################################################################
	if(var == "debugWebServer") {
		String returns = "";
		if(wpWebServer.Debug) {
			returns = "console.log(event)";
		}
		return returns;
	}
	return String();
}
