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
//# Revision     : $Rev:: 201                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWebServer.cpp 201 2024-09-08 22:39:09Z             $ #
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

uint16 helperWebServer::getVersion() {
	String SVN = "$Rev: 201 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
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
	if(force) publishDebugLast = 0;
	if(DebugLast != Debug || publishDebugLast == 0 ||
		wpFZ.loopStartedAt > publishDebugLast + wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		wpFZ.SendWSDebug("Debug WebServer", Debug);
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

//###################################################################################
// JSON Status
//###################################################################################

	webServer.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
		String minimac = WiFi.macAddress();
		minimac.replace(":", "");
		minimac.toLowerCase();
		String message = "{\"FreakaZoneDevice\":{" +
			wpFZ.JsonKeyString("DeviceName", wpFZ.DeviceName) + "," +
			wpFZ.JsonKeyString("DeviceDescription", wpFZ.DeviceDescription) + "," +
			wpFZ.JsonKeyString("Version", wpFZ.Version) + "," +
			wpFZ.JsonKeyValue("newVersion", wpUpdate.newVersion ? "true" : "false") + "," +
			wpFZ.JsonKeyString("MAC", WiFi.macAddress()) + "," +
			wpFZ.JsonKeyString("miniMAC", minimac) + "," +
			wpFZ.JsonKeyString("IP", WiFi.localIP().toString()) + "," +
			wpFZ.JsonKeyValue("UpdateMode", wpUpdate.UpdateFW ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("calcValues", wpFZ.calcValues ? "true" : "false") + ",";
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			message += "\"DHT\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpDHT.Pin])) + "," +
				wpFZ.JsonKeyValue("CalcCycle", String(wpDHT.CalcCycle())) + "," +
				wpFZ.JsonKeyValue("TemperatureCorrection", String(float(wpDHT.temperatureCorrection / 10.0))) + "," +
				wpFZ.JsonKeyValue("HumidityCorrection", String(float(wpDHT.humidityCorrection / 10.0))) +
				"},";
		}
		if(wpModules.useModuleLDR) {
			message += "\"LDR\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpLDR.Pin])) + "," +
				wpFZ.JsonKeyValue("CalcCycle", String(wpLDR.CalcCycle())) + "," +
				wpFZ.JsonKeyValue("useAvg", wpLDR.UseAvg() ? "true" : "false") + "," +
				wpFZ.JsonKeyValue("Correction", String(wpLDR.correction)) +
				"},";
		}
		if(wpModules.useModuleLight) {
			message += "\"Light\":{" +
				wpFZ.JsonKeyString("PinSCL", String(wpFZ.Pins[wpLight.PinSCL])) + "," +
				wpFZ.JsonKeyString("PinSDA", String(wpFZ.Pins[wpLight.PinSDA])) + "," +
				wpFZ.JsonKeyValue("CalcCycle", String(wpLight.CalcCycle())) + "," +
				wpFZ.JsonKeyValue("useAvg", wpLight.UseAvg() ? "true" : "false") + "," +
				wpFZ.JsonKeyValue("Correction", String(wpLight.correction)) +
				"},";
		}
		if(wpModules.useModuleBM) {
			message += "\"BM\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpBM.Pin]));
			if(wpModules.useModuleLDR) {
				message += ",\"LDR\":{" +
					wpFZ.JsonKeyValue("Threshold", String(wpBM.threshold)) + "," +
					wpFZ.JsonKeyString("LightToTurnOn", wpBM.lightToTurnOn) +
					"}";
			}
			message += "},";
		}
		if(wpModules.useModuleWindow) {
			message += "\"Window\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpWindow.Pin]));
			if(wpModules.useModuleLDR) {
				message += ",\"LDR\":{" +
					wpFZ.JsonKeyValue("Threshold", String(wpWindow.threshold)) + "," +
					wpFZ.JsonKeyString("LightToTurnOn", wpWindow.lightToTurnOn) +
					"}";
			}
			message += "},";
		}
		if(wpModules.useModuleCwWw) {
			message += "\"CwWw\":{},";
		}
		if(wpModules.useModuleAnalogOut) {
			message += "\"AnalogOut\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpAnalogOut.Pin])) + ",";
			if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
				message +=
					wpFZ.JsonKeyValue("CalcCycle", String(wpAnalogOut.CalcCycle())) + "," +
					wpFZ.JsonKeyValue("Kp", String(wpAnalogOut.Kp)) + "," +
					wpFZ.JsonKeyValue("Tv", String(wpAnalogOut.Tv)) + "," +
					wpFZ.JsonKeyValue("Tn", String(wpAnalogOut.Tn)) + "," +
					wpFZ.JsonKeyValue("SetPoint", String(wpAnalogOut.SetPoint)) + ",";
			}
			message +=
				wpFZ.JsonKeyValue("Hand", wpAnalogOut.handError ? "true" : "false") + "," +
				wpFZ.JsonKeyValue("HandValue", String(wpAnalogOut.handValue)) +
				"},";
		}
		if(wpModules.useModuleAnalogOut2) {
			message += "\"AnalogOut2\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpAnalogOut2.Pin])) + "," +
				wpFZ.JsonKeyValue("Hand", wpAnalogOut2.handError ? "true" : "false") + "," +
				wpFZ.JsonKeyValue("HandValue", String(wpAnalogOut2.handValue)) +
				"},";
		}
		if(wpModules.useModuleNeoPixel) {
			message += "\"NeoPixel\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpNeoPixel.Pin])) + "," +
				wpFZ.JsonKeyValue("ValueR", String(wpNeoPixel.GetValueR())) + "," +
				wpFZ.JsonKeyValue("ValueG", String(wpNeoPixel.GetValueG())) + "," +
				wpFZ.JsonKeyValue("ValueB", String(wpNeoPixel.GetValueB())) + "," +
				wpFZ.JsonKeyValue("Brightness", String(wpNeoPixel.GetBrightness())) + "," +
				wpFZ.JsonKeyValue("PixelCount", String(wpNeoPixel.GetPixelCount())) + "," +
				wpFZ.JsonKeyValue("isRGB", wpNeoPixel.GetRGB() ? "true" : "false") +
				"},";
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			message += "\"Relais\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpRelais.Pin])) + "," +
				wpFZ.JsonKeyValue("Hand", wpRelais.handError ? "true" : "false") + "," +
				wpFZ.JsonKeyValue("HandValue", wpRelais.handValue ? "true" : "false");
			if(wpModules.useModuleMoisture) {
				message += ",\"Moisture\":{" +
					wpFZ.JsonKeyValue("waterEmpty", wpRelais.waterEmptySet ? "true" : "false") + "," +
					wpFZ.JsonKeyValue("pumpActive", String(wpRelais.pumpActive)) + "," +
					wpFZ.JsonKeyValue("pumpPause", String(wpRelais.pumpPause / 60)) +
					"}";
			}
			message += "},";
		}
		if(wpModules.useModuleRpm) {
			message += "\"Rpm\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpRpm.Pin])) + "," +
				wpFZ.JsonKeyValue("CalcCycle", String(wpRpm.CalcCycle())) + "," +
				wpFZ.JsonKeyValue("useAvg", wpRain.UseAvg() ? "true" : "false") + "," +
				wpFZ.JsonKeyValue("Correction", String(wpRain.correction)) +
				"},";
		}
		if(wpModules.useModuleRain) {
			message += "\"Rain\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpRain.Pin])) + "," +
				wpFZ.JsonKeyValue("CalcCycle", String(wpRain.CalcCycle())) + "," +
				wpFZ.JsonKeyValue("useAvg", wpRain.UseAvg() ? "true" : "false") + "," +
				wpFZ.JsonKeyValue("Correction", String(wpRain.correction)) +
				"},";
		}
		if(wpModules.useModuleMoisture) {
			message += "\"Moisture\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpMoisture.Pin])) + "," +
				wpFZ.JsonKeyValue("CalcCycle", String(wpMoisture.CalcCycle())) + "," +
				wpFZ.JsonKeyValue("useAvg", wpMoisture.UseAvg() ? "true" : "false") + "," +
				wpFZ.JsonKeyValue("Min", String(wpMoisture.minValue)) + "," +
				wpFZ.JsonKeyValue("Dry", String(wpMoisture.dry)) + "," +
				wpFZ.JsonKeyValue("Wet", String(wpMoisture.wet)) +
				"},";
		}
		if(wpModules.useModuleDistance) {
			message += "\"Distance\":{" +
				wpFZ.JsonKeyString("Pin Trigger", String(wpFZ.Pins[wpDistance.PinTrig])) + "," +
				wpFZ.JsonKeyString("Pin Echo", String(wpFZ.Pins[wpDistance.PinEcho])) + "," +
				wpFZ.JsonKeyValue("CalcCycle", String(wpDistance.CalcCycle())) + "," +
				wpFZ.JsonKeyValue("Correction", String(wpDistance.correction)) + "," +
				wpFZ.JsonKeyValue("maxVolume", String(wpDistance.maxVolume)) + "," +
				wpFZ.JsonKeyValue("height", String(wpDistance.height)) +
				"},";
		}
		if(wpModules.useModuleImpulseCounter) {
			message += "\"ImpulseCounter\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpImpulseCounter.Pin])) + "," +
				wpFZ.JsonKeyValue("CalcCycle", String(wpImpulseCounter.CalcCycle())) + "," +
				wpFZ.JsonKeyValue("UpKWh", String(wpImpulseCounter.UpKWh)) + "," +
				wpFZ.JsonKeyValue("Silver", String(wpImpulseCounter.counterSilver)) + "," +
				wpFZ.JsonKeyValue("Red", String(wpImpulseCounter.counterRed)) +
				"},";
		}
		if(wpModules.useModuleUnderfloor1) {
			message += "\"Underfloor1\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpUnderfloor1.Pin])) +
				"},";
		}
		if(wpModules.useModuleUnderfloor2) {
			message += "\"Underfloor2\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpUnderfloor2.Pin])) +
				"},";
		}
		if(wpModules.useModuleUnderfloor3) {
			message += "\"Underfloor3\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpUnderfloor3.Pin])) +
				"},";
		}
		if(wpModules.useModuleUnderfloor4) {
			message += "\"Underfloor4\":{" +
				wpFZ.JsonKeyString("Pin", String(wpFZ.Pins[wpUnderfloor4.Pin])) +
				"},";
		}
		message += "\"Debug\":{" +
			wpFZ.JsonKeyValue("EEPROM", wpEEPROM.Debug ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Finder", wpFinder.Debug ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Modules", wpModules.Debug ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("MQTT", wpMqtt.Debug ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("OnlineToggler", wpOnlineToggler.Debug ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Rest", wpRest.Debug ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Update", wpUpdate.Debug ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("WebServer", wpWebServer.Debug ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("WiFi", wpWiFi.Debug ? "true" : "false");
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
		if(wpModules.useModuleWindow) {
			message += "," + wpFZ.JsonKeyValue("Window", wpWindow.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleCwWw) {
			message += "," + wpFZ.JsonKeyValue("CwWw", wpCwWw.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleAnalogOut) {
			message += "," + wpFZ.JsonKeyValue("AnalogOut", wpAnalogOut.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleAnalogOut2) {
			message += "," + wpFZ.JsonKeyValue("AnalogOut2", wpAnalogOut2.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleNeoPixel) {
			message += "," + wpFZ.JsonKeyValue("NeoPixel", wpNeoPixel.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			message += "," + wpFZ.JsonKeyValue("Relais", wpRelais.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleRpm) {
			message += "," + wpFZ.JsonKeyValue("Rpm", wpRpm.Debug() ? "true" : "false");
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
		if(wpModules.useModuleImpulseCounter) {
			message += "," + wpFZ.JsonKeyValue("ImpulseCounter", wpImpulseCounter.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleUnderfloor1) {
			message += "," + wpFZ.JsonKeyValue("Underfloor1", wpUnderfloor1.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleUnderfloor2) {
			message += "," + wpFZ.JsonKeyValue("Underfloor2", wpUnderfloor2.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleUnderfloor3) {
			message += "," + wpFZ.JsonKeyValue("Underfloor3", wpUnderfloor3.Debug() ? "true" : "false");
		}
		if(wpModules.useModuleUnderfloor4) {
			message += "," + wpFZ.JsonKeyValue("Underfloor4", wpUnderfloor4.Debug() ? "true" : "false");
		}
		message += "},\"SendRest\":{" +
			wpFZ.JsonKeyValue("WiFi", wpWiFi.sendRest ? "true" : "false");
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
		if(wpModules.useModuleWindow) {
			message += "," + wpFZ.JsonKeyValue("Window", wpWindow.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleAnalogOut) {
			message += "," + wpFZ.JsonKeyValue("AnalogOut", wpAnalogOut.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleAnalogOut2) {
			message += "," + wpFZ.JsonKeyValue("AnalogOut2", wpAnalogOut2.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleNeoPixel) {
			message += "," + wpFZ.JsonKeyValue("NeoPixel", wpAnalogOut.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			message += "," + wpFZ.JsonKeyValue("Relais", wpRelais.SendRest() ? "true" : "false");
		}
		if(wpModules.useModuleRpm) {
			message += "," + wpFZ.JsonKeyValue("Rpm", wpRpm.SendRest() ? "true" : "false");
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
		if(wpModules.useModuleImpulseCounter) {
			message += "," + wpFZ.JsonKeyValue("ImpulseCounter", wpImpulseCounter.SendRest() ? "true" : "false");
		}
		message += "},\"useModul\":{" +
			wpFZ.JsonKeyValue("DHT11", wpModules.useModuleDHT11 ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("DHT22", wpModules.useModuleDHT22 ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("LDR", wpModules.useModuleLDR ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Light", wpModules.useModuleLight ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("BM", wpModules.useModuleBM ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Window", wpModules.useModuleWindow ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("CwWw", wpModules.useModuleCwWw ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("AnalogOut", wpModules.useModuleAnalogOut ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("AnalogOut2", wpModules.useModuleAnalogOut2 ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("NeoPixel", wpModules.useModuleNeoPixel ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Relais", wpModules.useModuleRelais ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("RelaisShield", wpModules.useModuleRelaisShield ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Rpm", wpModules.useModuleRpm ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Rain", wpModules.useModuleRain ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Moisture", wpModules.useModuleMoisture ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Distance", wpModules.useModuleDistance ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("ImpulseCounter", wpModules.useModuleImpulseCounter ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Underfloor1", wpModules.useModuleUnderfloor1 ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Underfloor2", wpModules.useModuleUnderfloor2 ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Underfloor3", wpModules.useModuleUnderfloor3 ? "true" : "false") + "," +
			wpFZ.JsonKeyValue("Underfloor4", wpModules.useModuleUnderfloor4 ? "true" : "false") +
			"}}}";
		request->send(200, "application/json", message.c_str());
	});

//###################################################################################
// command set Module
//###################################################################################

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
			if(request->getParam("Module")->value() == "useWindow") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useWindow");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleWindow);
			}
			if(request->getParam("Module")->value() == "useCwWw") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useCwWw");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleCwWw);
			}
			if(request->getParam("Module")->value() == "useAnalogOut") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useAnalogOut");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleAnalogOut);
			}
			if(request->getParam("Module")->value() == "useAnalogOut2") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useAnalogOut2");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleAnalogOut2);
			}
			if(request->getParam("Module")->value() == "useNeoPixel") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useNeoPixel");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleNeoPixel);
			}
			if(request->getParam("Module")->value() == "useRelais") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useRelais");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRelais);
			}
			if(request->getParam("Module")->value() == "useRelaisShield") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useRelaisShield");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRelaisShield);
			}
			if(request->getParam("Module")->value() == "useRpm") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useRpm");
				wpWebServer.setModuleChange(wpWebServer.cmdModuleRpm);
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
			if(request->getParam("Module")->value() == "useImpulseCounter") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useImpulseCounter");
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleImpulseCounter);
			}
			if(request->getParam("Module")->value() == "useUnderfloor1") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useUnderfloor1");
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleUnderfloor1);
			}
			if(request->getParam("Module")->value() == "useUnderfloor2") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useUnderfloor2");
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleUnderfloor2);
			}
			if(request->getParam("Module")->value() == "useUnderfloor3") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useUnderfloor3");
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleUnderfloor3);
			}
			if(request->getParam("Module")->value() == "useUnderfloor4") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found useUnderfloor4");
				wpWebServer.setModuleChange(wpWebServer.cmdmoduleUnderfloor4);
			}
		}
		request->send(200, "application/json", "{\"erg\":\"S_OK\"}");
		wpWebServer.setBlink();
	});

//###################################################################################
// command set SendRest
//###################################################################################

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
			if(request->getParam("sendRest")->value() == "sendRestWindow") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestWindow");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestWindow);
			}
			if(request->getParam("sendRest")->value() == "sendRestAnalogOut") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestAnalogOut");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestAnalogOut);
			}
			if(request->getParam("sendRest")->value() == "sendRestAnalogOut2") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestAnalogOut2");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestAnalogOut2);
			}
			if(request->getParam("sendRest")->value() == "sendRestNeoPixel") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestNeoPixel");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestNeoPixel);
			}
			if(request->getParam("sendRest")->value() == "sendRestRelais") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestRelais");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestRelais);
			}
			if(request->getParam("sendRest")->value() == "sendRestRpm") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestRpm");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestRpm);
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
			if(request->getParam("sendRest")->value() == "sendRestImpulseCounter") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found sendRestImpulseCounter");
				wpWebServer.setSendRestChange(wpWebServer.cmdSendRestImpulseCounter);
			}
		}
		request->send(200, "application/json", "{\"erg\":\"S_OK\"}");
		wpWebServer.setBlink();
	});

//###################################################################################
// command set Debug
//###################################################################################

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
			if(request->getParam("Debug")->value() == "DebugWindow") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugWindow");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugWindow);
			}
			if(request->getParam("Debug")->value() == "DebugCwWw") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugCwWw");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugCwWw);
			}
			if(request->getParam("Debug")->value() == "DebugAnalogOut") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugAnalogOut");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugAnalogOut);
			}
			if(request->getParam("Debug")->value() == "DebugAnalogOut2") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugAnalogOut2");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugAnalogOut2);
			}
			if(request->getParam("Debug")->value() == "DebugNeoPixel") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugNeoPixel");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugNeoPixel);
			}
			if(request->getParam("Debug")->value() == "DebugRelais") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRelais");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugRelais);
			}
			if(request->getParam("Debug")->value() == "DebugRpm") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugRpm");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugRpm);
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
			if(request->getParam("Debug")->value() == "DebugImpulseCounter") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugImpulseCounter");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugImpulseCounter);
			}
			if(request->getParam("Debug")->value() == "DebugUnderfloor1") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugUnderfloor1");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUnderfloor1);
			}
			if(request->getParam("Debug")->value() == "DebugUnderfloor2") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugUnderfloor2");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUnderfloor2);
			}
			if(request->getParam("Debug")->value() == "DebugUnderfloor3") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugUnderfloor3");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUnderfloor3);
			}
			if(request->getParam("Debug")->value() == "DebugUnderfloor4") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found DebugUnderfloor4");
				wpWebServer.setDebugChange(wpWebServer.cmdDebugUnderfloor4);
			}
		}
		request->send(200, "application/json", "{\"erg\":\"S_OK\"}");
		wpWebServer.setBlink();
	});

//###################################################################################
// command set Cmd
//###################################################################################

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
			if(request->getParam("cmd")->value() == "SetDeviceName") {
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found cmd SetDeviceName");
				if(request->hasParam("newName")) {
					wpWebServer.newName = request->getParam("newName")->value();
					wpWebServer.setCommand(wpWebServer.cmdSetName);
				}
			}
		}
		request->send(200, "application/json", "{\"erg\":\"S_OK\"}");
		wpWebServer.setBlink();
	});
	if(wpModules.useModuleCwWw) {
		webServer.on("/setCwWwAuto", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam("ww") && request->hasParam("cw") && request->hasParam("sleep")) {
				byte ww = request->getParam("ww")->value().toInt();
				byte cw = request->getParam("cw")->value().toInt();
				uint sleep = request->getParam("sleep")->value().toInt();
				request->send_P(200, "application/json", wpCwWw.SetWwCwAuto(ww, cw, sleep).c_str());
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCwWw, set WwCw: '" + String(ww) + "'");
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCwWw, set Sleep: '" + String(sleep) + "'");
			}
			wpWebServer.setBlink();
		});
		webServer.on("/setCwWw", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam("turn")) {
				uint8 t = request->getParam("turn")->value().toInt();
				if(t == 1) {
					request->send_P(200, "application/json", wpCwWw.SetOn().c_str());
				}
				if(t == 0) {
					request->send_P(200, "application/json", wpCwWw.SetOff().c_str());
				}
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setCwWw turn '" + String(t) + "'");
			}
			if(request->hasParam("sleep")) {
				uint sleep = request->getParam("sleep")->value().toInt();
				wpCwWw.SetSleep(sleep);
				request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCwWw, set Sleep: '" + String(sleep) + "'");
			}
			if(request->hasParam("ww") && request->hasParam("cw") && request->hasParam("sleep")) {
				byte ww = request->getParam("ww")->value().toInt();
				byte cw = request->getParam("cw")->value().toInt();
				request->send_P(200, "application/json", wpCwWw.SetWwCw(ww, cw).c_str());
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCwWw, set WwCw: '" + String(ww) + "'");
			} else {
				if(request->hasParam("ww")) {
					byte ww = request->getParam("ww")->value().toInt();
					request->send_P(200, "application/json", wpCwWw.SetWW(ww).c_str());
					wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCwWw, set WW: '" + String(ww) + "'");
				}
				if(request->hasParam("cw")) {
					byte cw = request->getParam("cw")->value().toInt();
					request->send_P(200, "application/json", wpCwWw.SetCW(cw).c_str());
					wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCwWw, set CW: '" + String(cw) + "'");
				}
			}
			wpWebServer.setBlink();
		});
		webServer.on("/setCwWwEffect", HTTP_GET, [](AsyncWebServerRequest *request) {
			wpCwWw.SetSmooth();
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setCwWwEffect");
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setCwWwSleep", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint seconds = 0;
			if(request->hasParam("sleep")) {
				seconds = request->getParam("sleep")->value().toInt();
			}
			wpCwWw.SetSleep(seconds);
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setCwWwSleep");
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleNeoPixel) {
		webServer.on("/setNeoPixelDemo", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam("demo")) {
				wpNeoPixel.demoMode = !wpNeoPixel.demoMode;
			}
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setNeoPixelDemo");
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelEffect", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam("effect")) {
				uint effect = request->getParam("effect")->value().toInt();
				wpNeoPixel.demoMode = false;
				wpNeoPixel.SetMode(effect);
			}
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebServer", "Found setNeoPixelEffect");
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelColor", HTTP_GET, [](AsyncWebServerRequest *request) {
			byte r = 0;
			byte g = 0;
			byte b = 0;
			if(request->hasParam("r")) {
				r = request->getParam("r")->value().toInt();
			}
			if(request->hasParam("g")) {
				g = request->getParam("g")->value().toInt();
			}
			if(request->hasParam("b")) {
				b = request->getParam("b")->value().toInt();
			}
			wpNeoPixel.SetValueR(r);
			wpNeoPixel.SetValueG(g);
			wpNeoPixel.SetValueB(b);
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelColor: "
				"r: '" + String(r) + "', "
				"g: '" + String(g) + "', "
				"b: '" + String(b) + "'");
			
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelBrightness", HTTP_GET, [](AsyncWebServerRequest *request) {
			byte b = 0;
			if(request->hasParam("brightness")) {
				b = request->getParam("brightness")->value().toInt();
				wpNeoPixel.SetBrightness(b);
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelBrightness, '" + String(b) + "'");
			}
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelEffectSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint8 effectSpeed = 0;
			if(request->hasParam("effectSpeed")) {
				effectSpeed = request->getParam("effectSpeed")->value().toInt();
				wpNeoPixel.SetEffectSpeed(effectSpeed);
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelEffectSpeed, '" + String(effectSpeed) + "'");
			}
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelWW", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam("ww")) {
				byte ww = request->getParam("ww")->value().toInt();
				request->send_P(200, "application/json", wpNeoPixel.SetWW(ww).c_str());
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelWW, '" + String(ww) + "'");
			}
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelCW", HTTP_GET, [](AsyncWebServerRequest *request) {
			if(request->hasParam("cw")) {
				byte cw = request->getParam("cw")->value().toInt();
				request->send_P(200, "application/json", wpNeoPixel.SetCW(cw).c_str());
				wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelCW, '" + String(cw) + "'");
			}
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixel", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint pixel = 0;
			byte r = 0;
			byte g = 0;
			byte b = 0;
			if(request->hasParam("pixel")) {
				pixel = request->getParam("pixel")->value().toInt();
			}
			if(request->hasParam("r")) {
				r = request->getParam("r")->value().toInt();
			}
			if(request->hasParam("g")) {
				g = request->getParam("g")->value().toInt();
			}
			if(request->hasParam("b")) {
				b = request->getParam("b")->value().toInt();
			}
			wpNeoPixel.ComplexEffect(pixel, r, g, b);
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixel");
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelSleep", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint seconds = 0;
			if(request->hasParam("sleep")) {
				seconds = request->getParam("sleep")->value().toInt();
			}
			wpNeoPixel.SetSleep(seconds);
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelSleep");
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelOn", HTTP_GET, [](AsyncWebServerRequest *request) {
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelOn");
			request->send_P(200, "application/json", wpNeoPixel.SetOn().c_str());
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelOff", HTTP_GET, [](AsyncWebServerRequest *request) {
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelOff");
			request->send_P(200, "application/json", wpNeoPixel.SetOff().c_str());
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelOffRunner", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint8 steps = 5;
			if(request->hasParam("steps")) {
				steps = request->getParam("steps")->value().toInt();
			}
			wpNeoPixel.SetOffRunner(steps);
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelOffRunner");
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelOffBlender", HTTP_GET, [](AsyncWebServerRequest *request) {
			uint8 steps = 5;
			if(request->hasParam("steps")) {
				steps = request->getParam("steps")->value().toInt();
			}
			wpNeoPixel.SetOffBlender(steps);
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelOff");
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/setNeoPixelBorder", HTTP_GET, [](AsyncWebServerRequest *request) {
			wpNeoPixel.useBorder = !wpNeoPixel.useBorder;
			wpFZ.DebugWS(wpFZ.strINFO, "AsyncWebserver", "Found setNeoPixelBorder");
			request->send_P(200, "application/json", "{\"erg\":\"S_OK\"}");
			wpWebServer.setBlink();
		});
		webServer.on("/getNeoPixel", HTTP_GET, [](AsyncWebServerRequest *request) {
			request->send_P(200, "application/json", wpNeoPixel.getStripStatus().c_str());
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleUnderfloor1) {
		webServer.on("/setTopicTempUnderfloor1", HTTP_GET, [](AsyncWebServerRequest *request) {
			String topic = "_";
			if(request->hasParam("topic")) {
				topic = request->getParam("topic")->value();
			}
			request->send_P(200, "application/json", wpUnderfloor1.SetTopicTempUrl(topic).c_str());
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleUnderfloor2) {
		webServer.on("/setTopicTempUnderfloor2", HTTP_GET, [](AsyncWebServerRequest *request) {
			String topic = "_";
			if(request->hasParam("topic")) {
				topic = request->getParam("topic")->value();
			}
			request->send_P(200, "application/json", wpUnderfloor2.SetTopicTempUrl(topic).c_str());
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleUnderfloor3) {
		webServer.on("/setTopicTempUnderfloor3", HTTP_GET, [](AsyncWebServerRequest *request) {
			String topic = "_";
			if(request->hasParam("topic")) {
				topic = request->getParam("topic")->value();
			}
			request->send_P(200, "application/json", wpUnderfloor3.SetTopicTempUrl(topic).c_str());
			wpWebServer.setBlink();
		});
	}
	if(wpModules.useModuleUnderfloor4) {
		webServer.on("/setTopicTempUnderfloor4", HTTP_GET, [](AsyncWebServerRequest *request) {
			String topic = "_";
			if(request->hasParam("topic")) {
				topic = request->getParam("topic")->value();
			}
			request->send_P(200, "application/json", wpUnderfloor4.SetTopicTempUrl(topic).c_str());
			wpWebServer.setBlink();
		});
	}
	webServer.begin();
}

//###################################################################################
// do the commands
//###################################################################################
void helperWebServer::setCommand(int8 command) {
	doCommand = command;
}
void helperWebServer::setModuleChange(int8 module) {
	doModuleChange = module;
}
void helperWebServer::setSendRestChange(int8 sendRest) {
	doSendRestChange = sendRest;
}
void helperWebServer::setDebugChange(int8 debug) {
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
		if(doCommand == cmdSetName) {
			wpFZ.SetDeviceName(newName);
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
		if(doModuleChange == cmdModuleCwWw) wpModules.changeModuleCwWw(!wpModules.useModuleCwWw);
		if(doModuleChange == cmdModuleAnalogOut) wpModules.changeModuleAnalogOut(!wpModules.useModuleAnalogOut);
		if(doModuleChange == cmdModuleAnalogOut2) wpModules.changeModuleAnalogOut2(!wpModules.useModuleAnalogOut2);
		if(doModuleChange == cmdModuleNeoPixel) wpModules.changeModuleNeoPixel(!wpModules.useModuleNeoPixel);
		if(doModuleChange == cmdModuleRelais) wpModules.changeModuleRelais(!wpModules.useModuleRelais);
		if(doModuleChange == cmdModuleRelaisShield) wpModules.changeModuleRelaisShield(!wpModules.useModuleRelaisShield);
		if(doModuleChange == cmdModuleRpm) wpModules.changeModuleRpm(!wpModules.useModuleRpm);
		if(doModuleChange == cmdModuleRain) wpModules.changeModuleRain(!wpModules.useModuleRain);
		if(doModuleChange == cmdModuleMoisture) wpModules.changeModuleMoisture(!wpModules.useModuleMoisture);
		if(doModuleChange == cmdModuleDistance) wpModules.changeModuleDistance(!wpModules.useModuleDistance);
		if(doModuleChange == cmdmoduleImpulseCounter) wpModules.changemoduleImpulseCounter(!wpModules.useModuleImpulseCounter);
		if(doModuleChange == cmdmoduleUnderfloor1) wpModules.changemoduleUnderfloor1(!wpModules.useModuleUnderfloor1);
		if(doModuleChange == cmdmoduleUnderfloor2) wpModules.changemoduleUnderfloor2(!wpModules.useModuleUnderfloor2);
		if(doModuleChange == cmdmoduleUnderfloor3) wpModules.changemoduleUnderfloor3(!wpModules.useModuleUnderfloor3);
		if(doModuleChange == cmdmoduleUnderfloor4) wpModules.changemoduleUnderfloor4(!wpModules.useModuleUnderfloor4);
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
		if(doSendRestChange == cmdSendRestWindow) wpWindow.changeSendRest();
		if(doSendRestChange == cmdSendRestAnalogOut) wpAnalogOut.changeSendRest();
		if(doSendRestChange == cmdSendRestAnalogOut2) wpAnalogOut2.changeSendRest();
		if(doSendRestChange == cmdSendRestNeoPixel) wpNeoPixel.changeSendRest();
		if(doSendRestChange == cmdSendRestRelais) wpRelais.changeSendRest();
		if(doSendRestChange == cmdSendRestRpm) wpRpm.changeSendRest();
		if(doSendRestChange == cmdSendRestRain) wpRain.changeSendRest();
		if(doSendRestChange == cmdSendRestMoisture) wpMoisture.changeSendRest();
		if(doSendRestChange == cmdSendRestDistance) wpDistance.changeSendRest();
		if(doSendRestChange == cmdSendRestImpulseCounter) wpImpulseCounter.changeSendRest();
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
		if(doDebugChange == cmdDebugWindow) wpWindow.changeDebug();
		if(doDebugChange == cmdDebugAnalogOut) wpAnalogOut.changeDebug();
		if(doDebugChange == cmdDebugAnalogOut2) wpAnalogOut2.changeDebug();
		if(doDebugChange == cmdDebugNeoPixel) wpNeoPixel.changeDebug();
		if(doDebugChange == cmdDebugRelais) wpRelais.changeDebug();
		if(doDebugChange == cmdDebugRpm) wpRpm.changeDebug();
		if(doDebugChange == cmdDebugMoisture) wpMoisture.changeDebug();
		if(doDebugChange == cmdDebugDistance) wpDistance.changeDebug();
		if(doDebugChange == cmdDebugImpulseCounter) wpImpulseCounter.changeDebug();
		if(doDebugChange == cmdDebugUnderfloor1) wpUnderfloor1.changeDebug();
		if(doDebugChange == cmdDebugUnderfloor2) wpUnderfloor2.changeDebug();
		if(doDebugChange == cmdDebugUnderfloor3) wpUnderfloor3.changeDebug();
		if(doDebugChange == cmdDebugUnderfloor4) wpUnderfloor4.changeDebug();
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
	String returns = "<li class='setChange' onclick='changeModule(event)'>"
		"<span id='" + id + "' class='" + (state ? "color-ok" : "") + "'>" + name + "</span>"
	"</li>";
	return returns;
}
String helperWebServer::getChangeDebug(String id, String name, bool state) {
	String returns = "<li class='setChange' onclick='changeDebug(event)'>"
		"<span id='" + id + "' class='" + (state ? "color-ok" : "") + "'>" + name + "</span>"
	"</li>";
	return returns;
}
String helperWebServer::getChangeRest(String id, String name, bool state) {
	String returns = "<li class='setChange' onclick='changeSendRest(event)'>"
		"<span id='" + id + "' class='" + (state ? "color-ok" : "") + "'>" + name + "</span>"
	"</li>";
	return returns;
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
		returns = "<ul class='wpContainer'><li><span class='bold'>Modules:</span></li><li><hr /></li>" +
			wpWebServer.getchangeModule("useDHT11", "wpDHT11", wpModules.useModuleDHT11) +
			wpWebServer.getchangeModule("useDHT22", "wpDHT22", wpModules.useModuleDHT22) +
			wpWebServer.getchangeModule("useLDR", "wpLDR", wpModules.useModuleLDR) +
			wpWebServer.getchangeModule("useLight", "wpLight", wpModules.useModuleLight) +
			wpWebServer.getchangeModule("useBM", "wpBM", wpModules.useModuleBM) +
			wpWebServer.getchangeModule("useWindow", "wpWindow", wpModules.useModuleWindow) +
			wpWebServer.getchangeModule("useCwWw", "wpCwWw", wpModules.useModuleCwWw) +
			wpWebServer.getchangeModule("useAnalogOut", "wpAnalogOut", wpModules.useModuleAnalogOut) +
			wpWebServer.getchangeModule("useAnalogOut2", "wpAnalogOut2", wpModules.useModuleAnalogOut2) +
			wpWebServer.getchangeModule("useNeoPixel", "wpNeoPixel", wpModules.useModuleNeoPixel) +
			wpWebServer.getchangeModule("useRelais", "wpRelais", wpModules.useModuleRelais) +
			wpWebServer.getchangeModule("useRelaisShield", "wpRelaisShield", wpModules.useModuleRelaisShield) +
			wpWebServer.getchangeModule("useRpm", "wpRpm", wpModules.useModuleRpm) +
			wpWebServer.getchangeModule("useRain", "wpRain", wpModules.useModuleRain) +
			wpWebServer.getchangeModule("useMoisture", "wpMoisture", wpModules.useModuleMoisture) +
			wpWebServer.getchangeModule("useDistance", "wpDistance", wpModules.useModuleDistance) +
			wpWebServer.getchangeModule("useImpulseCounter", "wpImpulseCounter", wpModules.useModuleImpulseCounter) +
			wpWebServer.getchangeModule("useUnderfloor1", "wpUnderfloor1", wpModules.useModuleUnderfloor1) +
			wpWebServer.getchangeModule("useUnderfloor2", "wpUnderfloor2", wpModules.useModuleUnderfloor2) +
			wpWebServer.getchangeModule("useUnderfloor3", "wpUnderfloor3", wpModules.useModuleUnderfloor3) +
			wpWebServer.getchangeModule("useUnderfloor4", "wpUnderfloor4", wpModules.useModuleUnderfloor4) +
			"</ul>";
		return returns;
	}
//###################################################################################
	if(var == "Debug") {
		returns = "<ul class='wpContainer'><li><span class='bold'>Cmds:</span></li><li><hr /></li>" +
			wpWebServer.getChangeDebug("calcValues", "calc Values", wpFZ.calcValues);
		if((wpModules.useModuleRelais || wpModules.useModuleRelaisShield) && wpModules.useModuleMoisture) {
			returns += wpWebServer.getChangeDebug("waterEmpty", "waterEmpty", wpRelais.waterEmptySet);
		}
		returns += "<li><span class='bold'>Debug:</span></li><li><hr /></li>" +
			wpWebServer.getChangeDebug("DebugEEPROM", "Eprom", wpEEPROM.Debug) +
			wpWebServer.getChangeDebug("DebugFinder", "Modules", wpFinder.Debug) +
			wpWebServer.getChangeDebug("DebugModules", "Finder", wpModules.Debug) +
			wpWebServer.getChangeDebug("DebugMqtt", "Mqtt", wpMqtt.Debug) +
			wpWebServer.getChangeDebug("DebugOnlineToggler", "OnlineToggler", wpOnlineToggler.Debug) +
			wpWebServer.getChangeDebug("DebugRest", "Rest", wpRest.Debug) +
			wpWebServer.getChangeDebug("DebugUpdate", "WebServer", wpUpdate.Debug) +
			wpWebServer.getChangeDebug("DebugWebServer", "Update", wpWebServer.Debug) +
			wpWebServer.getChangeDebug("DebugWiFi", "WiFi", wpWiFi.Debug) +
			"<li><hr /></li>";
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			returns += wpWebServer.getChangeDebug("DebugDHT", "DHT", wpDHT.Debug());
		}
		if(wpModules.useModuleLDR) {
			returns += wpWebServer.getChangeDebug("DebugLDR", "LDR", wpLDR.Debug());
		}
		if(wpModules.useModuleLight) {
			returns += wpWebServer.getChangeDebug("DebugLight", "Light", wpLight.Debug());
		}
		if(wpModules.useModuleBM) {
			returns += wpWebServer.getChangeDebug("DebugBM", "BM", wpBM.Debug());
		}
		if(wpModules.useModuleWindow) {
			returns += wpWebServer.getChangeDebug("DebugWindow", "Window", wpWindow.Debug());
		}
		if(wpModules.useModuleCwWw) {
			returns += wpWebServer.getChangeDebug("DebugCwWw", "CwWw", wpCwWw.Debug());
		}
		if(wpModules.useModuleAnalogOut) {
			returns += wpWebServer.getChangeDebug("DebugAnalogOut", "AnalogOut", wpAnalogOut.Debug());
		}
		if(wpModules.useModuleAnalogOut2) {
			returns += wpWebServer.getChangeDebug("DebugAnalogOut2", "AnalogOut2", wpAnalogOut2.Debug());
		}
		if(wpModules.useModuleNeoPixel) {
			returns += wpWebServer.getChangeDebug("DebugNeoPixel", "NeoPixel", wpNeoPixel.Debug());
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			returns += wpWebServer.getChangeDebug("DebugRelais", "Relais", wpRelais.Debug());
		}
		if(wpModules.useModuleRpm) {
			returns += wpWebServer.getChangeDebug("DebugRpm", "Rpm", wpRpm.Debug());
		}
		if(wpModules.useModuleRain) {
			returns += wpWebServer.getChangeDebug("DebugRain", "Rain", wpRain.Debug());
		}
		if(wpModules.useModuleMoisture) {
			returns += wpWebServer.getChangeDebug("DebugMoisture", "Moisture", wpMoisture.Debug());
		}
		if(wpModules.useModuleDistance) {
			returns += wpWebServer.getChangeDebug("DebugDistance", "Distance", wpDistance.Debug());
		}
		if(wpModules.useModuleImpulseCounter) {
			returns += wpWebServer.getChangeDebug("DebugImpulseCounter", "ImpulseCounter", wpImpulseCounter.Debug());
		}
		if(wpModules.useModuleUnderfloor1) {
			returns += wpWebServer.getChangeDebug("DebugUnderfloor1", "Underfloor1", wpUnderfloor1.Debug());
		}
		if(wpModules.useModuleUnderfloor2) {
			returns += wpWebServer.getChangeDebug("DebugUnderfloor2", "Underfloor2", wpUnderfloor2.Debug());
		}
		if(wpModules.useModuleUnderfloor3) {
			returns += wpWebServer.getChangeDebug("DebugUnderfloor3", "Underfloor3", wpUnderfloor3.Debug());
		}
		if(wpModules.useModuleUnderfloor4) {
			returns += wpWebServer.getChangeDebug("DebugUnderfloor4", "Underfloor4", wpUnderfloor4.Debug());
		}
		return returns += "</ul>";
	}
//###################################################################################
	if(var == "SendRest") {
		returns = "<ul class='wpContainer'><li><span class='bold'>SendRest:</span></li><li><hr /></li>" +
			wpWebServer.getChangeRest("sendRestWiFi", "WiFi", wpWiFi.sendRest) +
			"<li><hr /></li>";
		if(wpModules.useModuleDHT11 || wpModules.useModuleDHT22) {
			returns += wpWebServer.getChangeRest("sendRestDHT", "DHT", wpDHT.SendRest());
		}
		if(wpModules.useModuleLDR) {
			returns += wpWebServer.getChangeRest("sendRestLDR", "LDR", wpLDR.SendRest());
		}
		if(wpModules.useModuleLight) {
			returns += wpWebServer.getChangeRest("sendRestLight", "Light", wpLight.SendRest());
		}
		if(wpModules.useModuleBM) {
			returns += wpWebServer.getChangeRest("sendRestBM", "BM", wpBM.SendRest());
		}
		if(wpModules.useModuleWindow) {
			returns += wpWebServer.getChangeRest("sendRestWindow", "Window", wpWindow.SendRest());
		}
		if(wpModules.useModuleAnalogOut) {
			returns += wpWebServer.getChangeRest("sendRestAnalogOut", "AnalogOut", wpAnalogOut.SendRest());
		}
		if(wpModules.useModuleAnalogOut2) {
			returns += wpWebServer.getChangeRest("sendRestAnalogOut2", "AnalogOut2", wpAnalogOut2.SendRest());
		}
		if(wpModules.useModuleNeoPixel) {
			returns += wpWebServer.getChangeRest("sendRestNeoPixel", "NeoPixel", wpNeoPixel.SendRest());
		}
		if(wpModules.useModuleRelais || wpModules.useModuleRelaisShield) {
			returns += wpWebServer.getChangeRest("sendRestRelais", "Relais", wpRelais.SendRest());
		}
		if(wpModules.useModuleRpm) {
			returns += wpWebServer.getChangeRest("sendRestRpm", "Rpm", wpRpm.SendRest());
		}
		if(wpModules.useModuleRain) {
			returns += wpWebServer.getChangeRest("sendRestRain", "Rain", wpRain.SendRest());
		}
		if(wpModules.useModuleMoisture) {
			returns += wpWebServer.getChangeRest("sendRestMoisture", "Moisture", wpMoisture.SendRest());
		}
		if(wpModules.useModuleDistance) {
			returns += wpWebServer.getChangeRest("sendRestDistance", "Distance", wpDistance.SendRest());
		}
		if(wpModules.useModuleImpulseCounter) {
			returns += wpWebServer.getChangeRest("sendRestImpulseCounter", "ImpulseCounter", wpImpulseCounter.SendRest());
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
	if(var == "debugOnMessage") {
		String returns = "";
		if(wpWebServer.Debug) {
			returns = "console.log('[d.cmd = undefined]:'); console.log(d);";
		}
		return returns;
	}
	return String();
}

