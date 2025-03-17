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
//# Revision     : $Rev:: 253                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWebServer.cpp 253 2025-03-17 19:29:41Z             $ #
//#                                                                                 #
//###################################################################################
#include <helperWebServer.h>

helperWebServer wpWebServer;

helperWebServer::helperWebServer() {}
void helperWebServer::init() {
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/WebServer";
	doCommand = cmdDoNothing;
	doModuleChange = cmdDoNothing;
	doDebugChange = cmdDoNothing;
	doBlink = cmdDoNothing;
	newName = wpFZ.DeviceName;
	newDescription = wpFZ.DeviceDescription;
	setupWebServer();
}

//###################################################################################
// public
//###################################################################################
void helperWebServer::cycle() {
	doTheCommand();
	doTheModuleChange();
	doTheDebugChange();
	doTheBlink();
	publishValues();
}

uint16 helperWebServer::getVersion() {
	String SVN = "$Rev: 253 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void helperWebServer::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugBasis0, wpEEPROM.bitDebugWebServer, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitsDebugBasis0);
	wpFZ.DebugSaveBoolToEEPROM("DebugWebServer", wpEEPROM.addrBitsDebugBasis0, wpEEPROM.bitDebugWebServer, Debug);
	EEPROM.commit();
	wpFZ.DebugWS(wpFZ.strINFO, "writeEEPROM", "DebugWebServer: " + String(Debug));
	wpFZ.SendWSDebug(F("DebugWebServer"), Debug);
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
	if(force) publishDebugLast = 0;
	if(DebugLast != Debug || publishDebugLast == 0 ||
		wpFZ.loopStartedAt > publishDebugLast + wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		wpFZ.SendWSDebug(F("Debug WebServer"), Debug);
		publishDebugLast = wpFZ.loopStartedAt;
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
			wpFZ.SendWSDebug(F("DebugWebServer"), Debug);
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
}
void helperWebServer::setupWebServer() {
	webServer.addHandler(&webSocket);
	webServer.onNotFound([](AsyncWebServerRequest *request){ 
		request->send(404, F("text/plain"), F("Link was not found!"));  
	});

	webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/x-icon"), favicon, sizeof(favicon));
		response->addHeader(F("Content-Encoding"), F("gzip"));
		request->send(response);
	});

	webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, F("application/json"), F("{\"this\":{\"is\":{\"a\":{\"FreakaZone\":{\"member\":true}}}}}"));
	});

	webServer.on("/print", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, F("text/html"), index_html, processor);
	});

//###################################################################################
// JSON Status
//###################################################################################

	webServer.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
		String minimac = WiFi.macAddress();
		minimac.replace(":", "");
		minimac.toLowerCase();
		String message = F("{\"FreakaZoneDevice\":{") +
			wpFZ.JsonKeyString(F("DeviceName"), wpFZ.DeviceName) + F(",") +
			wpFZ.JsonKeyString(F("DeviceDescription"), wpFZ.DeviceDescription) + F(",") +
			wpFZ.JsonKeyString(F("Version"), wpFZ.Version) + F(",") +
			wpFZ.JsonKeyValue(F("newVersion"), wpUpdate.newVersion ? "true" : "false") + F(",") +
			wpFZ.JsonKeyString(F("UpdateChanel"), wpUpdate.GetUpdateChanel()) + F(",") +
			wpFZ.JsonKeyString(F("MAC"), WiFi.macAddress()) + F(",") +
			wpFZ.JsonKeyString(F("miniMAC"), minimac) + F(",") +
			wpFZ.JsonKeyString(F("IP"), WiFi.localIP().toString()) + F(",") +
			wpFZ.JsonKeyValue(F("UpdateMode"), wpUpdate.UpdateFW ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("calcValues"), wpFZ.calcValues ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("BootCounter"), String(wpFZ.GetBootCounter())) + F(",") +
			wpFZ.JsonKeyString(F("RestartReason"), wpFZ.getLastRestartReason()) + F(",") +
			wpFZ.JsonKeyValue(F("useMaxWorking"), wpFZ.GetMaxWorking() ? "true" : "false") + F(",") +
			wpFZ.JsonKeyString(F("OnSince"), wpFZ.OnSince) + F(",") +
			wpFZ.JsonKeyString(F("WiFiSince"), wpWiFi.WiFiSince) + F(",") +
			wpFZ.JsonKeyString(F("MQTTSince"), wpMqtt.MqttSince) + F(",") +
			wpFZ.JsonKeyString(F("OnDuration"), wpFZ.OnDuration) + F(",");
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			message += wpDHT.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleLDR) {
			message += wpLDR.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleLight) {
			message += wpLight.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleBM) {
			message += wpBM.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleWindow) {
			message += wpWindow1.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			message += wpRelais.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleRain) {
			message += wpRain.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleMoisture) {
			message += wpMoisture.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleDistance) {
			message += wpDistance.GetJsonSettings() + F(",");
		}
		#if BUILDWITH == 1
		if(wpModules.useModuleCwWw) {
			message += wpCwWw.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleNeoPixel) {
			message += wpNeoPixel.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleAnalogOut) {
			message += wpAnalogOut.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleAnalogOut2) {
			message += wpAnalogOut2.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleClock) {
			message += wpClock.GetJsonSettings() + F(",");
		}
		#endif
		#if BUILDWITH == 2
		if(wpModules.useModuleAnalogOut) {
			message += wpAnalogOut.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleAnalogOut2) {
			message += wpAnalogOut2.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleRpm) {
			message += wpRpm.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleImpulseCounter) {
			message += wpImpulseCounter.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleWindow2) {
			message += wpWindow2.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleWindow3) {
			message += wpWindow3.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleWeight) {
			message += wpWeight.GetJsonSettings() + F(",");
		}
		#endif
		#if BUILDWITH == 3
		if(wpModules.useModuleUnderfloor1) {
			message += wpUnderfloor1.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleUnderfloor2) {
			message += wpUnderfloor2.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleUnderfloor3) {
			message += wpUnderfloor3.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleUnderfloor4) {
			message += wpUnderfloor4.GetJsonSettings() + F(",");
		}
		if(wpModules.useModuleDS18B20) {
			message += wpDS18B20.GetJsonSettings() + F(",");
		}
		#endif
		#if BUILDWITH == 4
		if(wpModules.useModuleRFID) {
			message += wpRFID.GetJsonSettings() + F(",");
		}
		#endif
		message += F("\"Debug\":{") +
			wpFZ.JsonKeyValue(F("EEPROM"), wpEEPROM.Debug ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("Finder"), wpFinder.Debug ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("Modules"), wpModules.Debug ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("MQTT"), wpMqtt.Debug ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("OnlineToggler"), wpOnlineToggler.Debug ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("Update"), wpUpdate.Debug ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("WebServer"), wpWebServer.Debug ? "true" : "false") + F(",") +
			wpFZ.JsonKeyValue(F("WiFi"), wpWiFi.Debug ? "true" : "false");
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			message += F(",") + wpFZ.JsonKeyValue(F("DHT"), wpDHT.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleLDR) {
			message += F(",") + wpFZ.JsonKeyValue(F("LDR"), wpLDR.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleLight) {
			message += F(",") + wpFZ.JsonKeyValue(F("Light"), wpLight.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleBM) {
			message += F(",") + wpFZ.JsonKeyValue(F("BM"), wpBM.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleWindow) {
			message += F(",") + wpFZ.JsonKeyValue(F("Window"), wpWindow1.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			message += F(",") + wpFZ.JsonKeyValue(F("Relais"), wpRelais.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleRain) {
			message += F(",") + wpFZ.JsonKeyValue(F("Rain"), wpRain.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleMoisture) {
			message += F(",") + wpFZ.JsonKeyValue(F("Moisture"), wpMoisture.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleDistance) {
			message += F(",") + wpFZ.JsonKeyValue(F("Distance"), wpDistance.Debug() ? "true" : "false");
		}
		#if BUILDWITH == 1
		if(wpModules.useModuleCwWw) {
			message += F(",") + wpFZ.JsonKeyValue(F("CwWw"), wpCwWw.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleNeoPixel) {
			message += F(",") + wpFZ.JsonKeyValue(F("NeoPixel"), wpNeoPixel.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleAnalogOut) {
			message += F(",") + wpFZ.JsonKeyValue(F("AnalogOut"), wpAnalogOut.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleAnalogOut2) {
			message += F(",") + wpFZ.JsonKeyValue(F("AnalogOut2"), wpAnalogOut2.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleClock) {
			message += F(",") + wpFZ.JsonKeyValue(F("Clock"), wpClock.Debug() ? "true" : "false");
		}
		#endif
		#if BUILDWITH == 2
		if(wpModules.useModuleAnalogOut) {
			message += F(",") + wpFZ.JsonKeyValue(F("AnalogOut"), wpAnalogOut.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleAnalogOut2) {
			message += F(",") + wpFZ.JsonKeyValue(F("AnalogOut2"), wpAnalogOut2.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleRpm) {
			message += F(",") + wpFZ.JsonKeyValue(F("Rpm"), wpRpm.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleImpulseCounter) {
			message += F(",") + wpFZ.JsonKeyValue(F("ImpulseCounter"), wpImpulseCounter.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleWindow2) {
			message += F(",") + wpFZ.JsonKeyValue(F("Window2"), wpWindow2.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleWindow3) {
			message += F(",") + wpFZ.JsonKeyValue(F("Window3"), wpWindow3.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleWeight) {
			message += F(",") + wpFZ.JsonKeyValue(F("Weight"), wpWeight.Debug() ? "true" : "false");
		}
		#endif
		#if BUILDWITH == 3
		if(wpModules.useModuleUnderfloor1) {
			message += F(",") + wpFZ.JsonKeyValue(F("Underfloor1"), wpUnderfloor1.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleUnderfloor2) {
			message += F(",") + wpFZ.JsonKeyValue(F("Underfloor2"), wpUnderfloor2.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleUnderfloor3) {
			message += F(",") + wpFZ.JsonKeyValue(F("Underfloor3"), wpUnderfloor3.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleUnderfloor4) {
			message += F(",") + wpFZ.JsonKeyValue(F("Underfloor4"), wpUnderfloor4.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleDS18B20) {
			message += F(",") + wpFZ.JsonKeyValue(F("DS18B20"), wpDS18B20.Debug() ? "true" : "false");
		}
		#endif
		#if BUILDWITH == 4
		if(wpModules.useModuleRFID) {
			message += F(",") + wpFZ.JsonKeyValue(F("RFID"), wpRFID.Debug() ? "true" : "false");
		}
		#endif
		message += F("},\"useModul\":{") +
			wpFZ.JsonKeyValue(F("DHT11"), wpModules.useModuleDHT11 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("DHT22"), wpModules.useModuleDHT22 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("LDR"), wpModules.useModuleLDR ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Light"), wpModules.useModuleLight ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("BM"), wpModules.useModuleBM ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Window"), wpModules.useModuleWindow ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Relais"), wpModules.useModuleRelais ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("RelaisShield"), wpModules.useModuleRelaisShield ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Rain"), wpModules.useModuleRain ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Moisture"), wpModules.useModuleMoisture ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Distance"), wpModules.useModuleDistance ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("CwWw"), wpModules.useModuleCwWw ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("NeoPixel"), wpModules.useModuleNeoPixel ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("AnalogOut"), wpModules.useModuleAnalogOut ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("AnalogOut2"), wpModules.useModuleAnalogOut2 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("AnalogOut"), wpModules.useModuleAnalogOut ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("AnalogOut2"), wpModules.useModuleAnalogOut2 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Rpm"), wpModules.useModuleRpm ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("ImpulseCounter"), wpModules.useModuleImpulseCounter ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Window2"), wpModules.useModuleWindow2 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Window3"), wpModules.useModuleWindow3 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Weight"), wpModules.useModuleWeight ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Underfloor1"), wpModules.useModuleUnderfloor1 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Underfloor2"), wpModules.useModuleUnderfloor2 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Underfloor3"), wpModules.useModuleUnderfloor3 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Underfloor4"), wpModules.useModuleUnderfloor4 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("DS18B20"), wpModules.useModuleDS18B20 ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("RFID"), wpModules.useModuleRFID ? "true" : "false") +
			F(",") + wpFZ.JsonKeyValue(F("Clock"), wpModules.useModuleClock ? "true" : "false") +
			F("}}}");
		request->send(200, F("application/json"), message.c_str());
	});

//###################################################################################
// command set Module
//###################################################################################

	webServer.on("/setModule", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setModule"));
		if(request->hasParam(F("Module"))) {
			if(request->getParam(F("Module"))->value() == F("useDHT11")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useDHT11"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleDHT11);
			}
			if(request->getParam(F("Module"))->value() == F("useDHT22")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useDHT22"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleDHT22);
			}
			if(request->getParam(F("Module"))->value() == F("useLDR")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useLDR"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleLDR);
			}
			if(request->getParam(F("Module"))->value() == F("useLight")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useLight"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleLight);
			}
			if(request->getParam(F("Module"))->value() == F("useBM")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useBM"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleBM);
			}
			if(request->getParam(F("Module"))->value() == F("useWindow")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useWindow"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleWindow);
			}
			if(request->getParam(F("Module"))->value() == F("useRelais")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useRelais"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRelais);
			}
			if(request->getParam(F("Module"))->value() == F("useRelaisShield")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useRelaisShield"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRelaisShield);
			}
			if(request->getParam(F("Module"))->value() == F("useRain")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useRain"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRain);
			}
			if(request->getParam(F("Module"))->value() == F("useMoisture")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useMoisture"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleMoisture);
			}
			if(request->getParam(F("Module"))->value() == F("useDistance")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useDistance"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleDistance);
			}
			#if BUILDWITH == 1
			if(request->getParam(F("Module"))->value() == F("useCwWw")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useCwWw"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleCwWw);
			}
			if(request->getParam(F("Module"))->value() == F("useNeoPixel")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useNeoPixel"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleNeoPixel);
			}
			if(request->getParam(F("Module"))->value() == F("useAnalogOut")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useAnalogOut"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleAnalogOut);
			}
			if(request->getParam(F("Module"))->value() == F("useAnalogOut2")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useAnalogOut2"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleAnalogOut2);
			}
			if(request->getParam(F("Module"))->value() == F("useClock")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useClock"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleClock);
			}
			#endif
			#if BUILDWITH == 2
			if(request->getParam(F("Module"))->value() == F("useAnalogOut")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useAnalogOut"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleAnalogOut);
			}
			if(request->getParam(F("Module"))->value() == F("useAnalogOut2")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useAnalogOut2"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleAnalogOut2);
			}
			if(request->getParam(F("Module"))->value() == F("useRpm")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useRpm"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRpm);
			}
			if(request->getParam(F("Module"))->value() == F("useImpulseCounter")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useImpulseCounter"));
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleImpulseCounter);
			}
			if(request->getParam(F("Module"))->value() == F("useWindow2")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useWindow2"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleWindow2);
			}
			if(request->getParam(F("Module"))->value() == F("useWindow3")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useWindow3"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleWindow3);
			}
			if(request->getParam(F("Module"))->value() == F("useWeight")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useWeight"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleWeight);
			}
			#endif
			#if BUILDWITH == 3
			if(request->getParam(F("Module"))->value() == F("useUnderfloor1")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useUnderfloor1"));
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleUnderfloor1);
			}
			if(request->getParam(F("Module"))->value() == F("useUnderfloor2")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useUnderfloor2"));
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleUnderfloor2);
			}
			if(request->getParam(F("Module"))->value() == F("useUnderfloor3")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useUnderfloor3"));
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleUnderfloor3);
			}
			if(request->getParam(F("Module"))->value() == F("useUnderfloor4")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useUnderfloor4"));
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleUnderfloor4);
			}
			if(request->getParam(F("Module"))->value() == F("useDS18B20")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useDS18B20"));
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleDS18B20);
			}
			#endif
			#if BUILDWITH == 4
			if(request->getParam(F("Module"))->value() == F("useRFID")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found useRFID"));
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRFID);
			}
			#endif
		}
		request->send(200, F("application/json"), wpFZ.jsonOK);
		wpWebServer.setBlink();
	});

//###################################################################################
// command set Debug
//###################################################################################

	webServer.on("/setDebug", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setDebug"));
		if(request->hasParam(F("Debug"))) {
			if(request->getParam(F("Debug"))->value() == F("DebugEEPROM")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugEEPROM"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugEEPROM);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugFinder")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugFinder"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugFinder);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugModules")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugModules"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugModules);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugMqtt")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugMqtt"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugMqtt);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugOnlineToggler")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugOnlineToggler"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugOnlineToggler);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugUpdate")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugUpdate"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUpdate);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugWebServer")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugWebServer"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugWebServer);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugWiFi")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugWiFi"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugWiFi);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugDHT")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugDHT"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugDHT);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugLDR")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugLDR"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugLDR);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugLight")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugLight"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugLight);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugBM")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugBM"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugBM);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugWindow")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugWindow"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugWindow);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugRelais")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugRelais"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugRelais);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugRain")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugRain"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugRain);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugMoisture")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugMoisture"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugMoisture);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugDistance")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugDistance"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugDistance);
			}
			#if BUILDWITH == 1
			if(request->getParam(F("Debug"))->value() == F("DebugCwWw")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugCwWw"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugCwWw);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugNeoPixel")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugNeoPixel"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugNeoPixel);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugAnalogOut")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugAnalogOut"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugAnalogOut);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugAnalogOut2")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugAnalogOut2"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugAnalogOut2);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugClock")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugClock"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugClock);
			}
			#endif
			#if BUILDWITH == 2
			if(request->getParam(F("Debug"))->value() == F("DebugAnalogOut")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugAnalogOut"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugAnalogOut);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugAnalogOut2")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugAnalogOut2"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugAnalogOut2);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugRpm")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugRpm"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugRpm);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugImpulseCounter")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugImpulseCounter"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugImpulseCounter);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugWindow2")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugWindow2"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugWindow2);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugWindow3")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugWindow3"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugWindow3);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugWeight")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugWeight"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugWeight);
			}
			#endif
			#if BUILDWITH == 3
			if(request->getParam(F("Debug"))->value() == F("DebugUnderfloor1")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugUnderfloor1"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUnderfloor1);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugUnderfloor2")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugUnderfloor2"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUnderfloor2);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugUnderfloor3")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugUnderfloor3"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUnderfloor3);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugUnderfloor4")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugUnderfloor4"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUnderfloor4);
			}
			if(request->getParam(F("Debug"))->value() == F("DebugDS18B20")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugDS18B20"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugDS18B20);
			}
			#endif
			#if BUILDWITH == 4
			if(request->getParam(F("Debug"))->value() == F("DebugRFID")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found DebugRFID"));
				wpWebServer.setDebugChange(wpWebServer.cmdDebugRFID);
			}
			#endif
		}
		request->send(200, F("application/json"), wpFZ.jsonOK);
		wpWebServer.setBlink();
	});

//###################################################################################
// command set Cmd
//###################################################################################

	webServer.on("/setCmd", HTTP_GET, [](AsyncWebServerRequest *request) {
		wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setCmd"));
		if(request->hasParam(F("cmd"))) {
			if(request->getParam(F("cmd"))->value() == F("ForceMqttUpdate")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found ForceMqttUpdate"));
				wpWebServer.setCommand(wpWebServer.cmdForceMqttUpdate);
			}
			if(request->getParam(F("cmd"))->value() == F("ForceRenewValue")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found ForceRenewValue"));
				wpWebServer.setCommand(wpWebServer.cmdForceRenewValue);
			}
			if(request->getParam(F("cmd"))->value() == F("UpdateFW")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found UpdateFW"));
				wpWebServer.setCommand(wpWebServer.cmdUpdateFW);
			}
			if(request->getParam(F("cmd"))->value() == F("UpdateCheck")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found UpdateCheck"));
				wpWebServer.setCommand(wpWebServer.cmdUpdateCheck);
			}
			if(request->getParam(F("cmd"))->value() == F("UpdateHTTP")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found UpdateHTTP"));
				wpWebServer.setCommand(wpWebServer.cmdUpdateHTTP);
			}
			if(request->getParam(F("cmd"))->value() == F("RestartDevice")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found RestartDevice"));
				wpWebServer.setCommand(wpWebServer.cmdRestartESP);
			}
			if(request->getParam(F("cmd"))->value() == F("ScanWiFi")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found ScanWiFi"));
				wpWebServer.setCommand(wpWebServer.cmdScanWiFi);
			}
			if(request->getParam(F("cmd"))->value() == F("CheckDns")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found CheckDns"));
				wpWebServer.setCommand(wpWebServer.cmdCheckDns);
			}
			if(request->getParam(F("cmd"))->value() == F("calcValues")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found cmd calcValues"));
				wpFZ.calcValues = !wpFZ.calcValues;
			}
			if(request->getParam(F("cmd"))->value() == F("useMaxWorking")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found cmd useMaxWorking"));
				wpWebServer.setCommand(wpWebServer.cmdMaxWorking);
			}
			if(request->getParam(F("cmd"))->value() == F("Blink")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found Blink"));
			}
			if(request->getParam(F("cmd"))->value() == F("waterEmpty")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found cmd waterEmpty"));
				wpRelais.waterEmptySet = !wpRelais.waterEmptySet;
			}
			if(request->getParam(F("cmd"))->value() == F("SetDeviceName")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found cmd SetDeviceName"));
				if(request->hasParam(F("newName"))) {
					wpWebServer.newName = request->getParam(F("newName"))->value();
				}
				if(request->hasParam(F("newDescription"))) {
					wpWebServer.newDescription = request->getParam(F("newDescription"))->value();
				}
				wpWebServer.setCommand(wpWebServer.cmdSetName);
			}
			if(request->getParam(F("cmd"))->value() == F("SetUpdateChanel")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found cmd SetUpdateChanel"));
				if(request->hasParam(F("newUpdateChanel"))) {
					wpUpdate.SetUpdateChanel(request->getParam(F("newUpdateChanel"))->value().toInt());
				}
			}
			if(request->getParam(F("cmd"))->value() == F("ResetBootCounter")) {
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found cmd ResetBootCounter"));
				wpFZ.ResetBootCounter();
			}
		}
		request->send(200, F("application/json"), wpFZ.jsonOK);
		wpWebServer.setBlink();
	});
	if(wpModules.useModuleBM) {
		webServer.on("/setBM", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam("mode")) {
				// /setBM?mode=auto
				if(request->getParam("mode")->value() == "auto") {
					request->send_P(200, "application/json", wpBM.SetAuto().c_str());
				}
				// /setBM?mode=manual
				if(request->getParam("mode")->value() == "manual") {
					request->send_P(200, "application/json", wpBM.SetManual().c_str());
				}
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setBM, set mode: '" + request->getParam("mode")->value() + "'");
			}
			wpWebServer.setBlink();
		});
	}
	#if BUILDWITH == 1
	if(wpModules.useModuleCwWw) {
		webServer.on("/setCwWwAuto", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("ww")) && request->hasParam(F("cw")) && request->hasParam(F("sleep"))) {
				byte ww = (uint8)(request->getParam(F("ww"))->value().toInt() * 2.55);
				byte cw = (uint8)(request->getParam(F("cw"))->value().toInt() * 2.55);
				uint sleep = request->getParam(F("sleep"))->value().toInt();
				request->send(200, F("application/json"), wpCwWw.SetWwCwAuto(ww, cw, sleep).c_str());
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setCwWw, set WwCw: '" + String(ww) + "'");
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setCwWw, set Sleep: '" + String(sleep) + "'");
			}
			wpWebServer.setBlink();
		});
		webServer.on("/setCwWw", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("turn"))) {
				uint8 t = request->getParam(F("turn"))->value().toInt();
				if(t == 1) {
					request->send(200, F("application/json"), wpCwWw.SetOn().c_str());
				}
				if(t == 0) {
					request->send(200, F("application/json"), wpCwWw.SetOff().c_str());
				}
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), "Found setCwWw turn '" + String(t) + "'");
			}
			if(request->hasParam(F("sleep"))) {
				uint sleep = request->getParam(F("sleep"))->value().toInt();
				wpCwWw.SetSleep(sleep);
				request->send(200, F("application/json"), wpFZ.jsonOK);
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setCwWw, set Sleep: '" + String(sleep) + "'");
			}
			if(request->hasParam(F("ww")) && request->hasParam(F("cw")) && request->hasParam(F("sleep"))) {
				byte ww = (uint8)(request->getParam(F("ww"))->value().toInt() * 2.55);
				byte cw = (uint8)(request->getParam(F("cw"))->value().toInt() * 2.55);
				request->send(200, F("application/json"), wpCwWw.SetWwCw(ww, cw).c_str());
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setCwWw, set WwCw: '" + String(ww) + "'");
			} else {
				if(request->hasParam(F("ww"))) {
				byte ww = (uint8)(request->getParam(F("ww"))->value().toInt() * 2.55);
					request->send(200, F("application/json"), wpCwWw.SetWW(ww).c_str());
					wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setCwWw, set WW: '" + String(ww) + "'");
				}
				if(request->hasParam(F("cw"))) {
				byte cw = (uint8)(request->getParam(F("cw"))->value().toInt() * 2.55);
					request->send(200, F("application/json"), wpCwWw.SetCW(cw).c_str());
					wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setCwWw, set CW: '" + String(cw) + "'");
				}
			}
			wpWebServer.setBlink();
		});
		webServer.on("/setCwWwEffect", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("effect"))) {
				uint effect = request->getParam(F("effect"))->value().toInt();
				request->send(200, F("application/json"), wpCwWw.SetEffect(effect));
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found setCwWwEffect"));
				wpWebServer.setBlink();
			}
		});
		webServer.on("/setCwWwEffectSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint8 effectSpeed = 0;
			if(request->hasParam(F("effectSpeed"))) {
				effectSpeed = request->getParam(F("effectSpeed"))->value().toInt();
				wpCwWw.SetEffectSpeed(effectSpeed);
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setCwWwEffectSpeed, '" + String(effectSpeed) + "'");
			}
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
		webServer.on("/setCwWwSleep", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint seconds = 0;
			if(request->hasParam(F("sleep"))) {
				seconds = request->getParam(F("sleep"))->value().toInt();
			}
			wpCwWw.SetSleep(seconds);
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setCwWwSleep"));
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleNeoPixel) {
		webServer.on("/setNeoPixelDemo", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("demo"))) {
				wpNeoPixel.demoMode = !wpNeoPixel.demoMode;
			}
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found setNeoPixelDemo"));
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelEffect", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("effect"))) {
				uint effect = request->getParam(F("effect"))->value().toInt();
				wpNeoPixel.demoMode = false;
				wpNeoPixel.SetMode(effect);
			}
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found setNeoPixelEffect"));
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelColor", HTTP_GET, [](AsyncWebServerRequest *request) {
			byte r = 0;
			byte g = 0;
			byte b = 0;
			if(request->hasParam(F("r"))) {
				r = request->getParam(F("r"))->value().toInt();
			}
			if(request->hasParam(F("g"))) {
				g = request->getParam(F("g"))->value().toInt();
			}
			if(request->hasParam(F("b"))) {
				b = request->getParam(F("b"))->value().toInt();
			}
			wpNeoPixel.SetValueR(r);
			wpNeoPixel.SetValueG(g);
			wpNeoPixel.SetValueB(b);
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setNeoPixelColor: "
				"r: '" + String(r) + "', "
				"g: '" + String(g) + "', "
				"b: '" + String(b) + "'");
			
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelEffectSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint8 effectSpeed = 0;
			if(request->hasParam(F("effectSpeed"))) {
				effectSpeed = request->getParam(F("effectSpeed"))->value().toInt();
				wpNeoPixel.SetEffectSpeed(effectSpeed);
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setNeoPixelEffectSpeed, '" + String(effectSpeed) + "'");
			}
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelWW", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("ww"))) {
				byte ww = (uint8)(request->getParam(F("ww"))->value().toInt() * 2.55);
				request->send(200, F("application/json"), wpNeoPixel.SetWW(ww).c_str());
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setNeoPixelWW, '" + String(ww) + "'");
			}
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelCW", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("cw"))) {
				byte cw = (uint8)(request->getParam(F("cw"))->value().toInt() * 2.55);
				request->send(200, F("application/json"), wpNeoPixel.SetCW(cw).c_str());
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), "Found setNeoPixelCW, '" + String(cw) + "'");
			}
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixel", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint pixel = 0;
			byte r = 0;
			byte g = 0;
			byte b = 0;
			if(request->hasParam(F("pixel"))) {
				pixel = request->getParam(F("pixel"))->value().toInt();
			}
			if(request->hasParam(F("r"))) {
				r = request->getParam(F("r"))->value().toInt();
			}
			if(request->hasParam(F("g"))) {
				g = request->getParam(F("g"))->value().toInt();
			}
			if(request->hasParam(F("b"))) {
				b = request->getParam(F("b"))->value().toInt();
			}
			wpNeoPixel.ComplexEffect(pixel, r, g, b);
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setNeoPixel"));
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelSleep", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint seconds = 0;
			if(request->hasParam(F("sleep"))) {
				seconds = request->getParam(F("sleep"))->value().toInt();
			}
			wpNeoPixel.SetSleep(seconds);
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setNeoPixelSleep"));
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelOn", HTTP_GET, [](AsyncWebServerRequest *request) {
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setNeoPixelOn"));
			request->send(200, F("application/json"), wpNeoPixel.SetOn().c_str());
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelOff", HTTP_GET, [](AsyncWebServerRequest *request) {
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setNeoPixelOff"));
			request->send(200, F("application/json"), wpNeoPixel.SetOff().c_str());
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelOffRunner", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint8 steps = 5;
			if(request->hasParam(F("steps"))) {
				steps = request->getParam(F("steps"))->value().toInt();
			}
			wpNeoPixel.SetOffRunner(steps);
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setNeoPixelOffRunner"));
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelBorder", HTTP_GET, [](AsyncWebServerRequest *request) {
			wpNeoPixel.useBorder = !wpNeoPixel.useBorder;
			wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebserver"), F("Found setNeoPixelBorder"));
			request->send(200, F("application/json"), wpFZ.jsonOK);
			wpWebServer.setBlink();
		});
		// webServer.on("/getNeoPixel", HTTP_GET, [](AsyncWebServerRequest *request) {
		// 	request->send_P(200, "application/json", wpNeoPixel.getStripStatus().c_str());
		// 	wpWebServer.setBlink();
		// });
	}
	if(wpModules.useModuleClock) {
		webServer.on("/setClock", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("steps"))) {
				short steps = request->getParam(F("steps"))->value().toInt();
				wpClock.SetSteps(steps);
				wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found steps"));
				request->send(200, F("application/json"), wpFZ.jsonOK);
			}
			if(request->hasParam(F("cmd")) && request->getParam(F("cmd"))->value() == F("simulate")) {
				if(request->hasParam(F("h")) && request->hasParam(F("m")) && request->hasParam(F("s"))) {
					short h = request->getParam(F("h"))->value().toInt();
					short m = request->getParam(F("m"))->value().toInt();
					short s = request->getParam(F("s"))->value().toInt();
					wpClock.SimulateTime(h, m, s);
					wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found simulate Time actived, h: ") + String(h) + F(", m: ") + String(m));
				} else {
					wpClock.SimulateTime();
					wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found simulate Time deactivated"));
				}
				request->send(200, F("application/json"), wpFZ.jsonOK);
			}
			if(request->hasParam(F("cmd")) && request->getParam(F("cmd"))->value() == F("setColorH")) {
				if(request->hasParam(F("r")) && request->hasParam(F("g")) && request->hasParam(F("b"))) {
					short r = request->getParam(F("r"))->value().toInt();
					short g = request->getParam(F("g"))->value().toInt();
					short b = request->getParam(F("b"))->value().toInt();
					wpClock.SetColorH(r, g, b);
					wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found Set Color Hour, R: ") + String(r) + F(", G: ") + String(g) + F(", B: ") + String(b));
				}
				request->send(200, F("application/json"), wpFZ.jsonOK);
			}
			if(request->hasParam(F("cmd")) && request->getParam(F("cmd"))->value() == F("setColorM")) {
				if(request->hasParam(F("r")) && request->hasParam(F("g")) && request->hasParam(F("b"))) {
					short r = request->getParam(F("r"))->value().toInt();
					short g = request->getParam(F("g"))->value().toInt();
					short b = request->getParam(F("b"))->value().toInt();
					wpClock.SetColorM(r, g, b);
					wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found Set Color Minute, R: ") + String(r) + F(", G: ") + String(g) + F(", B: ") + String(b));
				}
				request->send(200, F("application/json"), wpFZ.jsonOK);
			}
			if(request->hasParam(F("cmd")) && request->getParam(F("cmd"))->value() == F("setColorS")) {
				if(request->hasParam(F("r")) && request->hasParam(F("g")) && request->hasParam(F("b"))) {
					short r = request->getParam(F("r"))->value().toInt();
					short g = request->getParam(F("g"))->value().toInt();
					short b = request->getParam(F("b"))->value().toInt();
					wpClock.SetColorS(r, g, b);
					wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found Set Color Second, R: ") + String(r) + F(", G: ") + String(g) + F(", B: ") + String(b));
				}
				request->send(200, F("application/json"), wpFZ.jsonOK);
			}
			if(request->hasParam(F("cmd")) && request->getParam(F("cmd"))->value() == F("setColorQ")) {
				if(request->hasParam(F("r")) && request->hasParam(F("g")) && request->hasParam(F("b"))) {
					short r = request->getParam(F("r"))->value().toInt();
					short g = request->getParam(F("g"))->value().toInt();
					short b = request->getParam(F("b"))->value().toInt();
					wpClock.SetColorQ(r, g, b);
					wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found Set Color Quarter, R: ") + String(r) + F(", G: ") + String(g) + F(", B: ") + String(b));
				}
				request->send(200, F("application/json"), wpFZ.jsonOK);
			}
			if(request->hasParam(F("cmd")) && request->getParam(F("cmd"))->value() == F("setColor5")) {
				if(request->hasParam(F("r")) && request->hasParam(F("g")) && request->hasParam(F("b"))) {
					short r = request->getParam(F("r"))->value().toInt();
					short g = request->getParam(F("g"))->value().toInt();
					short b = request->getParam(F("b"))->value().toInt();
					wpClock.SetColor5(r, g, b);
					wpFZ.DebugWS(wpFZ.strINFO, F("AsyncWebServer"), F("Found Set Color Five, R: ") + String(r) + F(", G: ") + String(g) + F(", B: ") + String(b));
				}
				request->send(200, F("application/json"), wpFZ.jsonOK);
			}
			wpWebServer.setBlink();
		});
	}
	#endif
	#if BUILDWITH == 2
	webServer.on("/setAnalogOut", HTTP_GET, [](AsyncWebServerRequest *request) {
		if(request->hasParam(F("PidType"))) {
			uint8 pidType = request->getParam(F("PidType"))->value().toInt();
			request->send(200, F("application/json"), wpAnalogOut.SetPidType(pidType).c_str());
		}
		if(request->hasParam(F("setPoint"))) {
			uint8 setPoint = request->getParam(F("setPoint"))->value().toDouble();
			request->send(200, F("application/json"), wpAnalogOut.SetSetPoint(setPoint).c_str());
		}
		wpWebServer.setBlink();
	});
	webServer.on("/setWeight", HTTP_GET, [](AsyncWebServerRequest *request) {
		if(request->hasParam(F("tare"))) {
			int tare = request->getParam(F("tare"))->value().toInt();
			switch(tare) {
				case 0:
					wpWeight.SetTare();
					request->send(200, F("application/json"), wpFZ.jsonOK);
					break;
				case 5:
					wpWeight.Set1kg();
					request->send(200, F("application/json"), wpFZ.jsonOK);
					break;
				default:
					request->send(200, F("application/json"), wpFZ.jsonERROR);
			}
		}
	});
	#endif
	#if BUILDWITH == 3
	if(wpModules.useModuleUnderfloor1) {
		webServer.on("/setUnderfloor1", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("hand"))) {
				uint8 hand = request->getParam(F("hand"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor1.SetHand(hand == 0 ? false : true).c_str());
			}
			if(request->hasParam(F("handValue"))) {
				uint8 handValue = request->getParam(F("handValue"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor1.SetHandValue(handValue == 0 ? false : true).c_str());
			}
			if(request->hasParam(F("setPoint"))) {
				uint8 setPoint = request->getParam(F("setPoint"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor1.SetSetPoint(setPoint).c_str());
			}
			if(request->hasParam(F("topic"))) {
				String topic = request->getParam(F("topic"))->value();
				request->send(200, F("application/json"), wpUnderfloor1.SetTopicTempUrl(topic).c_str());
			}
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleUnderfloor2) {
		webServer.on("/setUnderfloor2", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("hand"))) {
				uint8 hand = request->getParam(F("hand"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor2.SetHand(hand == 0 ? false : true).c_str());
			}
			if(request->hasParam(F("handValue"))) {
				uint8 handValue = request->getParam(F("handValue"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor2.SetHandValue(handValue == 0 ? false : true).c_str());
			}
			if(request->hasParam(F("setPoint"))) {
				uint8 setPoint = request->getParam(F("setPoint"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor2.SetSetPoint(setPoint).c_str());
			}
			if(request->hasParam(F("topic"))) {
				String topic = request->getParam(F("topic"))->value();
				request->send(200, F("application/json"), wpUnderfloor2.SetTopicTempUrl(topic).c_str());
			}
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleUnderfloor3) {
		webServer.on("/setUnderfloor3", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("hand"))) {
				uint8 hand = request->getParam(F("hand"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor3.SetHand(hand == 0 ? false : true).c_str());
			}
			if(request->hasParam(F("handValue"))) {
				uint8 handValue = request->getParam(F("handValue"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor3.SetHandValue(handValue == 0 ? false : true).c_str());
			}
			if(request->hasParam(F("setPoint"))) {
				uint8 setPoint = request->getParam(F("setPoint"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor3.SetSetPoint(setPoint).c_str());
			}
			if(request->hasParam(F("topic"))) {
				String topic = request->getParam(F("topic"))->value();
				request->send(200, F("application/json"), wpUnderfloor3.SetTopicTempUrl(topic).c_str());
			}
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleUnderfloor4) {
		webServer.on("/setUnderfloor4", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("hand"))) {
				uint8 hand = request->getParam(F("hand"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor4.SetHand(hand == 0 ? false : true).c_str());
			}
			if(request->hasParam(F("handValue"))) {
				uint8 handValue = request->getParam(F("handValue"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor4.SetHandValue(handValue == 0 ? false : true).c_str());
			}
			if(request->hasParam(F("setPoint"))) {
				uint8 setPoint = request->getParam(F("setPoint"))->value().toInt();
				request->send(200, F("application/json"), wpUnderfloor4.SetSetPoint(setPoint).c_str());
			}
			if(request->hasParam(F("topic"))) {
				String topic = request->getParam(F("topic"))->value();
				request->send(200, F("application/json"), wpUnderfloor4.SetTopicTempUrl(topic).c_str());
			}
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleDS18B20) {
		webServer.on("/setDS18B20", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam(F("scanBus"))) {
				request->send(200, F("application/json"), wpDS18B20.scanBus().c_str());
			}
			wpWebServer.setBlink();
		});
	}
	#endif
	#if BUILDWITH == 4
	// if(wpModules.useModuleRFID) {
	// 	webServer.on("/setRFID", HTTP_GET, [](AsyncWebServerRequest *request) {
	// 		if(request->hasParam(F("hand"))) {
	// 			uint8 hand = request->getParam(F("hand"))->value().toInt();
	// 			request->send(200, F("application/json"), wpUnderfloor1.SetHand(hand == 0 ? false : true).c_str());
	// 		}
	// 		wpWebServer.setBlink();
	// 	});
	// }
	#endif
	webServer.begin();
}

//###################################################################################
// do the commands
//###################################################################################
void helperWebServer::setCommand(uint8 command) {
	doCommand = command;
}
void helperWebServer::setModuleChange(uint8 module) {
	doModuleChange = module;
}
void helperWebServer::setDebugChange(uint8 debug) {
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
			wpFZ.SetRestartReason(wpFZ.restartReasonCmd);
			ESP.restart();
		}
		if(doCommand == cmdMaxWorking) {
			wpFZ.SetMaxWorking();
		}
		if(doCommand == cmdScanWiFi) {
			wpWiFi.scanWiFi();
		}
		if(doCommand == cmdCheckDns) {
			wpWiFi.checkDns();
		}
		if(doCommand == cmdSetName) {
			wpFZ.SetDeviceName(newName);
			wpFZ.SetDeviceDescription(newDescription);
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
		if(doModuleChange == cmdModuleWindow) wpModules.changeModuleWindow(!wpModules.useModuleWindow);
		if(doModuleChange == cmdModuleRelais) wpModules.changeModuleRelais(!wpModules.useModuleRelais);
		if(doModuleChange == cmdModuleRelaisShield) wpModules.changeModuleRelaisShield(!wpModules.useModuleRelaisShield);
		if(doModuleChange == cmdModuleRain) wpModules.changeModuleRain(!wpModules.useModuleRain);
		if(doModuleChange == cmdModuleMoisture) wpModules.changeModuleMoisture(!wpModules.useModuleMoisture);
		if(doModuleChange == cmdModuleDistance) wpModules.changeModuleDistance(!wpModules.useModuleDistance);
		#if BUILDWITH == 1
		if(doModuleChange == cmdModuleCwWw) wpModules.changeModuleCwWw(!wpModules.useModuleCwWw);
		if(doModuleChange == cmdModuleNeoPixel) wpModules.changeModuleNeoPixel(!wpModules.useModuleNeoPixel);
		if(doModuleChange == cmdModuleAnalogOut) wpModules.changeModuleAnalogOut(!wpModules.useModuleAnalogOut);
		if(doModuleChange == cmdModuleAnalogOut2) wpModules.changeModuleAnalogOut2(!wpModules.useModuleAnalogOut2);
		if(doModuleChange == cmdModuleClock) wpModules.changemoduleClock(!wpModules.useModuleClock);
		#endif
		#if BUILDWITH == 2
		if(doModuleChange == cmdModuleAnalogOut) wpModules.changeModuleAnalogOut(!wpModules.useModuleAnalogOut);
		if(doModuleChange == cmdModuleAnalogOut2) wpModules.changeModuleAnalogOut2(!wpModules.useModuleAnalogOut2);
		if(doModuleChange == cmdModuleRpm) wpModules.changeModuleRpm(!wpModules.useModuleRpm);
		if(doModuleChange == cmdmoduleImpulseCounter) wpModules.changemoduleImpulseCounter(!wpModules.useModuleImpulseCounter);
		if(doModuleChange == cmdModuleWindow2) wpModules.changeModuleWindow2(!wpModules.useModuleWindow2);
		if(doModuleChange == cmdModuleWindow3) wpModules.changeModuleWindow3(!wpModules.useModuleWindow3);
		if(doModuleChange == cmdModuleWeight) wpModules.changeModuleWeight(!wpModules.useModuleWeight);
		#endif
		#if BUILDWITH == 3
		if(doModuleChange == cmdmoduleUnderfloor1) wpModules.changemoduleUnderfloor1(!wpModules.useModuleUnderfloor1);
		if(doModuleChange == cmdmoduleUnderfloor2) wpModules.changemoduleUnderfloor2(!wpModules.useModuleUnderfloor2);
		if(doModuleChange == cmdmoduleUnderfloor3) wpModules.changemoduleUnderfloor3(!wpModules.useModuleUnderfloor3);
		if(doModuleChange == cmdmoduleUnderfloor4) wpModules.changemoduleUnderfloor4(!wpModules.useModuleUnderfloor4);
		if(doModuleChange == cmdmoduleDS18B20) wpModules.changemoduleDS18B20(!wpModules.useModuleDS18B20);
		#endif
		#if BUILDWITH == 4
		if(doModuleChange == cmdModuleRFID) wpModules.changemoduleRFID(!wpModules.useModuleRFID);
		#endif
		doModuleChange = cmdDoNothing;
	}
}

void helperWebServer::doTheDebugChange() {
	if(doDebugChange > 0) {
		if(doDebugChange == cmdDebugEEPROM) wpEEPROM.changeDebug();
		if(doDebugChange == cmdDebugFinder) wpFinder.changeDebug();
		if(doDebugChange == cmdDebugModules) wpModules.changeDebug();
		if(doDebugChange == cmdDebugMqtt) wpMqtt.changeDebug();
		if(doDebugChange == cmdDebugOnlineToggler) wpOnlineToggler.changeDebug();
		if(doDebugChange == cmdDebugUpdate) wpUpdate.changeDebug();
		if(doDebugChange == cmdDebugWebServer) wpWebServer.changeDebug();
		if(doDebugChange == cmdDebugWiFi) wpWiFi.changeDebug();
		if(doDebugChange == cmdDebugDHT) wpDHT.changeDebug();
		if(doDebugChange == cmdDebugLDR) wpLDR.changeDebug();
		if(doDebugChange == cmdDebugLight) wpLight.changeDebug();
		if(doDebugChange == cmdDebugBM) wpBM.changeDebug();
		if(doDebugChange == cmdDebugWindow) wpWindow1.changeDebug();
		if(doDebugChange == cmdDebugRelais) wpRelais.changeDebug();
		if(doDebugChange == cmdDebugMoisture) wpMoisture.changeDebug();
		if(doDebugChange == cmdDebugDistance) wpDistance.changeDebug();
		#if BUILDWITH == 1
		if(doDebugChange == cmdDebugCwWw) wpCwWw.changeDebug();
		if(doDebugChange == cmdDebugNeoPixel) wpNeoPixel.changeDebug();
		if(doDebugChange == cmdDebugAnalogOut) wpAnalogOut.changeDebug();
		if(doDebugChange == cmdDebugAnalogOut2) wpAnalogOut2.changeDebug();
		if(doDebugChange == cmdDebugClock) wpClock.changeDebug();
		#endif
		#if BUILDWITH == 2
		if(doDebugChange == cmdDebugAnalogOut) wpAnalogOut.changeDebug();
		if(doDebugChange == cmdDebugAnalogOut2) wpAnalogOut2.changeDebug();
		if(doDebugChange == cmdDebugRpm) wpRpm.changeDebug();
		if(doDebugChange == cmdDebugImpulseCounter) wpImpulseCounter.changeDebug();
		if(doDebugChange == cmdDebugWindow2) wpWindow2.changeDebug();
		if(doDebugChange == cmdDebugWindow3) wpWindow3.changeDebug();
		if(doDebugChange == cmdDebugWeight) wpWeight.changeDebug();
		#endif
		#if BUILDWITH == 3
		if(doDebugChange == cmdDebugUnderfloor1) wpUnderfloor1.changeDebug();
		if(doDebugChange == cmdDebugUnderfloor2) wpUnderfloor2.changeDebug();
		if(doDebugChange == cmdDebugUnderfloor3) wpUnderfloor3.changeDebug();
		if(doDebugChange == cmdDebugUnderfloor4) wpUnderfloor4.changeDebug();
		if(doDebugChange == cmdDebugDS18B20) wpDS18B20.changeDebug();
		#endif
		#if BUILDWITH == 4
		if(doDebugChange == cmdDebugRFID) wpRFID.changeDebug();
		#endif
		doDebugChange = cmdDoNothing;
	}
}

void helperWebServer::doTheBlink() {
	if(doBlink > 0) {
		wpFZ.blink();
		doBlink = cmdDoNothing;
	}
}
String helperWebServer::getchangeModule(String id, String name, bool state) {
	String returns = F("<li class='setChange' onclick='changeModule(event)'>");
	returns += F("<span id='") + id + F("' class='") + (state ? "color-ok" : "") + F("'>") + name + F("</span>");
	returns += F("</li>");
	return returns;
}
String helperWebServer::getChangeDebug(String id, String name, bool state) {
	String returns = F("<li class='setChange' onclick='changeDebug(event)'>");
	returns += F("<span id='") + id + F("' class='") + (state ? "color-ok" : "") + F("'>") + name + F("</span>");
	returns += F("</li>");
	return returns;
}
String helperWebServer::getChangeCmd(String id, String name, bool state) {
	String returns = F("<li class='setChange' onclick='cmdHandle(event)'>");
	returns += F("<span id='") + id + F("' class='") + (state ? "color-ok" : "") + F("'>") + name + F("</span>");
	returns += F("</li>");
	return returns;
}

//###################################################################################
// stuff
//###################################################################################

String processor(const String& var) {
	String returns = "";
//###################################################################################
	if(var == F("IPADRESS"))
		return WiFi.localIP().toString();
//###################################################################################
	if(var == F("DeviceName"))
		return wpFZ.DeviceName;
//###################################################################################
	if(var == F("DeviceDescription"))
		return wpFZ.DeviceDescription;
//###################################################################################
	if(var == F("Version"))
		return wpFZ.Version;
//###################################################################################
	if(var == F("newVersion")) {
		if(!wpUpdate.newVersion)
			return F(" wpHidden");
	}
//###################################################################################
	if(var == F("innernewVersion")) {
		if(wpUpdate.newVersion)
			return F("--- Update Available ---<br />installed: ") + wpUpdate.installedVersion + F("<br />update: ") + wpUpdate.serverVersion;
	}
//###################################################################################
	if(var == "CompiledWith") {
		returns = "<ul class='wpContainer'><li><span class='bold'>Modules:</span></li><li><hr /></li>" +
			wpWebServer.getchangeModule(F("useDHT11"), F("wpDHT11"), wpModules.useModuleDHT11) +
			wpWebServer.getchangeModule(F("useDHT22"), F("wpDHT22"), wpModules.useModuleDHT22) +
			wpWebServer.getchangeModule(F("useLDR"), F("wpLDR"), wpModules.useModuleLDR) +
			wpWebServer.getchangeModule(F("useLight"), F("wpLight"), wpModules.useModuleLight) +
			wpWebServer.getchangeModule(F("useBM"), F("wpBM"), wpModules.useModuleBM) +
			wpWebServer.getchangeModule(F("useWindow"), F("wpWindow"), wpModules.useModuleWindow) +
			wpWebServer.getchangeModule(F("useRelais"), F("wpRelais"), wpModules.useModuleRelais) +
			wpWebServer.getchangeModule(F("useRelaisShield"), F("wpRelaisShield"), wpModules.useModuleRelaisShield) +
			wpWebServer.getchangeModule(F("useRain"), F("wpRain"), wpModules.useModuleRain) +
			wpWebServer.getchangeModule(F("useMoisture"), F("wpMoisture"), wpModules.useModuleMoisture) +
			wpWebServer.getchangeModule(F("useDistance"), F("wpDistance"), wpModules.useModuleDistance);
		#if BUILDWITH == 1
		returns +=
			wpWebServer.getchangeModule(F("useCwWw"), F("wpCwWw"), wpModules.useModuleCwWw) +
			wpWebServer.getchangeModule(F("useNeoPixel"), F("wpNeoPixel"), wpModules.useModuleNeoPixel) +
			wpWebServer.getchangeModule(F("useAnalogOut"), F("wpAnalogOut"), wpModules.useModuleAnalogOut) +
			wpWebServer.getchangeModule(F("useAnalogOut2"), F("wpAnalogOut2"), wpModules.useModuleAnalogOut2) +
			wpWebServer.getchangeModule(F("useClock"), F("wpClock"), wpModules.useModuleClock);
		#endif
		#if BUILDWITH == 2
		returns +=
			wpWebServer.getchangeModule(F("useAnalogOut"), F("wpAnalogOut"), wpModules.useModuleAnalogOut) +
			wpWebServer.getchangeModule(F("useAnalogOut2"), F("wpAnalogOut2"), wpModules.useModuleAnalogOut2) +
			wpWebServer.getchangeModule(F("useRpm"), F("wpRpm"), wpModules.useModuleRpm) +
			wpWebServer.getchangeModule(F("useImpulseCounter"), F("wpImpulseCounter"), wpModules.useModuleImpulseCounter) +
			wpWebServer.getchangeModule(F("useWindow2"), F("wpWindow2"), wpModules.useModuleWindow2) +
			wpWebServer.getchangeModule(F("useWindow3"), F("wpWindow3"), wpModules.useModuleWindow3) +
			wpWebServer.getchangeModule(F("useWeight"), F("wpWeight"), wpModules.useModuleWeight);
		#endif
		#if BUILDWITH == 3
		returns +=
			wpWebServer.getchangeModule(F("useUnderfloor1"), F("wpUnderfloor1"), wpModules.useModuleUnderfloor1) +
			wpWebServer.getchangeModule(F("useUnderfloor2"), F("wpUnderfloor2"), wpModules.useModuleUnderfloor2) +
			wpWebServer.getchangeModule(F("useUnderfloor3"), F("wpUnderfloor3"), wpModules.useModuleUnderfloor3) +
			wpWebServer.getchangeModule(F("useUnderfloor4"), F("wpUnderfloor4"), wpModules.useModuleUnderfloor4) +
			wpWebServer.getchangeModule(F("useDS18B20"), F("wpDS18B20"), wpModules.useModuleDS18B20);
		#endif
		#if BUILDWITH == 4
		returns +=
			wpWebServer.getchangeModule(F("useRFID"), F("wpRFID"), wpModules.useModuleRFID);
		#endif
		returns +=
			"</ul>";
		return returns;
	}
//###################################################################################
	if(var == F("Debug")) {
		returns = F("<ul class='wpContainer'><li><span class='bold'>Cmds:</span></li><li><hr /></li>") +
			wpWebServer.getChangeDebug(F("calcValues"), F("calc Values"), wpFZ.calcValues) +
			wpWebServer.getChangeCmd(F("useMaxWorking"), F("Use Max Working"), wpFZ.GetMaxWorking());
		if((wpModules.useModuleRelais || wpModules.useModuleRelaisShield) && wpModules.useModuleMoisture) {
			returns += wpWebServer.getChangeDebug(F("waterEmpty"), F("waterEmpty"), wpRelais.waterEmptySet);
		}
		returns += F("<li><span class='bold'>Debug:</span></li><li><hr /></li>") +
			wpWebServer.getChangeDebug(F("DebugEEPROM"), F("Eprom"), wpEEPROM.Debug) +
			wpWebServer.getChangeDebug(F("DebugFinder"), F("Modules"), wpFinder.Debug) +
			wpWebServer.getChangeDebug(F("DebugModules"), F("Finder"), wpModules.Debug) +
			wpWebServer.getChangeDebug(F("DebugMqtt"), F("Mqtt"), wpMqtt.Debug) +
			wpWebServer.getChangeDebug(F("DebugOnlineToggler"), F("OnlineToggler"), wpOnlineToggler.Debug) +
			wpWebServer.getChangeDebug(F("DebugUpdate"), F("WebServer"), wpUpdate.Debug) +
			wpWebServer.getChangeDebug(F("DebugWebServer"), F("Update"), wpWebServer.Debug) +
			wpWebServer.getChangeDebug(F("DebugWiFi"), F("WiFi"), wpWiFi.Debug) +
			F("<li><hr /></li>");
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			returns += wpWebServer.getChangeDebug(F("DebugDHT"), F("DHT"), wpDHT.Debug());
		}
		if(wpModules.useModuleLDR) {
			returns += wpWebServer.getChangeDebug(F("DebugLDR"), F("LDR"), wpLDR.Debug());
		}
		if(wpModules.useModuleLight) {
			returns += wpWebServer.getChangeDebug(F("DebugLight"), F("Light"), wpLight.Debug());
		}
		if(wpModules.useModuleBM) {
			returns += wpWebServer.getChangeDebug(F("DebugBM"), F("BM"), wpBM.Debug());
		}
		if(wpModules.useModuleWindow) {
			returns += wpWebServer.getChangeDebug(F("DebugWindow"), F("Window"), wpWindow1.Debug());
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			returns += wpWebServer.getChangeDebug(F("DebugRelais"), F("Relais"), wpRelais.Debug());
		}
		if(wpModules.useModuleRain) {
			returns += wpWebServer.getChangeDebug(F("DebugRain"), F("Rain"), wpRain.Debug());
		}
		if(wpModules.useModuleMoisture) {
			returns += wpWebServer.getChangeDebug(F("DebugMoisture"), F("Moisture"), wpMoisture.Debug());
		}
		if(wpModules.useModuleDistance) {
			returns += wpWebServer.getChangeDebug(F("DebugDistance"), F("Distance"), wpDistance.Debug());
		}
		#if BUILDWITH == 1
		if(wpModules.useModuleCwWw) {
			returns += wpWebServer.getChangeDebug(F("DebugCwWw"), F("CwWw"), wpCwWw.Debug());
		}
		if(wpModules.useModuleNeoPixel) {
			returns += wpWebServer.getChangeDebug(F("DebugNeoPixel"), F("NeoPixel"), wpNeoPixel.Debug());
		}
		if(wpModules.useModuleAnalogOut) {
			returns += wpWebServer.getChangeDebug(F("DebugAnalogOut"), F("AnalogOut"), wpAnalogOut.Debug());
		}
		if(wpModules.useModuleAnalogOut2) {
			returns += wpWebServer.getChangeDebug(F("DebugAnalogOut2"), F("AnalogOut2"), wpAnalogOut2.Debug());
		}
		if(wpModules.useModuleClock) {
			returns += wpWebServer.getChangeDebug(F("DebugClock"), F("Clock"), wpClock.Debug());
		}
		#endif
		#if BUILDWITH == 2
		if(wpModules.useModuleAnalogOut) {
			returns += wpWebServer.getChangeDebug(F("DebugAnalogOut"), F("AnalogOut"), wpAnalogOut.Debug());
		}
		if(wpModules.useModuleAnalogOut2) {
			returns += wpWebServer.getChangeDebug(F("DebugAnalogOut2"), F("AnalogOut2"), wpAnalogOut2.Debug());
		}
		if(wpModules.useModuleRpm) {
			returns += wpWebServer.getChangeDebug(F("DebugRpm"), F("Rpm"), wpRpm.Debug());
		}
		if(wpModules.useModuleImpulseCounter) {
			returns += wpWebServer.getChangeDebug(F("DebugImpulseCounter"), F("ImpulseCounter"), wpImpulseCounter.Debug());
		}
		if(wpModules.useModuleWindow2) {
			returns += wpWebServer.getChangeDebug(F("DebugWindow2"), F("Window2"), wpWindow2.Debug());
		}
		if(wpModules.useModuleWindow3) {
			returns += wpWebServer.getChangeDebug(F("DebugWindow3"), F("Window3"), wpWindow3.Debug());
		}
		if(wpModules.useModuleWeight) {
			returns += wpWebServer.getChangeDebug(F("DebugWeight"), F("Weight"), wpWeight.Debug());
		}
		#endif
		#if BUILDWITH == 3
		if(wpModules.useModuleUnderfloor1) {
			returns += wpWebServer.getChangeDebug(F("DebugUnderfloor1"), F("Underfloor1"), wpUnderfloor1.Debug());
		}
		if(wpModules.useModuleUnderfloor2) {
			returns += wpWebServer.getChangeDebug(F("DebugUnderfloor2"), F("Underfloor2"), wpUnderfloor2.Debug());
		}
		if(wpModules.useModuleUnderfloor3) {
			returns += wpWebServer.getChangeDebug(F("DebugUnderfloor3"), F("Underfloor3"), wpUnderfloor3.Debug());
		}
		if(wpModules.useModuleUnderfloor4) {
			returns += wpWebServer.getChangeDebug(F("DebugUnderfloor4"), F("Underfloor4"), wpUnderfloor4.Debug());
		}
		if(wpModules.useModuleDS18B20) {
			returns += wpWebServer.getChangeDebug(F("DebugDS18B20"), F("DS18B20"), wpDS18B20.Debug());
		}
		#endif
		#if BUILDWITH == 4
		if(wpModules.useModuleRFID) {
			returns += wpWebServer.getChangeDebug(F("DebugRFID"), F("RFID"), wpRFID.Debug());
		}
		#endif
		return returns += F("</ul>");
	}
//###################################################################################
	if(var == F("debugWebServer")) {
		String returns = F("");
		if(wpWebServer.Debug) {
			returns = F("console.log(event)");
		}
		return returns;
	}
	return String();
}

