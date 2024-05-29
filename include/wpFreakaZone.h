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
#ifndef wpFreakaZone_h
#define wpFreakaZone_h
#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <ArduinoOTA.h>
#include <ESPAsyncUDP.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#ifdef wpHT // for DHT11 || DHT22
	#include <DHT.h>
#endif

#define NTP_SERVER "172.17.1.1"
#define TZ "CET-1CEST,M3.5.0,M10.5.0/3"

class wpFreakaZone {
	public:
		const char* ssid = "Mikrowellen";
		const char* password = "Welc0me@wp-Scheid.com";

		const char* mqttServer = "mqtt.freakazone.com";
		const uint16_t mqttServerPort = 1883;
		const char* restServer = "light.freakazone.com";
		const uint16_t restServerPort = 255;
		const uint16_t findListenPort = 51346;
		const uint16_t publishQoS = 4 * 60 * 10;

		uint16_t MajorVersion;
		uint16_t MinorVersion;
		uint16_t Build;
		String MainVersion;

		String OnSince;
		String OnDuration;
		String WiFiSince;
		String MqttSince;

		const String strDEBUG  = "[  DEBUG  ]";
		const String strINFO   = "[- INFO  -]";
		const String strWARN   = "[* WARN  *]";
		const String strERRROR = "[! ERROR !]";

		const enum wpModules {
			wpDHT11,
			wpDHT22,
			wpLDR,
			wpLight,
			wpBM,
			wpRelais,
			wpRelaisShield,
			wpRain,
			wpMoisture,
			wpDistance
		};
		/// @brief  EEPROM byte 0
		const enum settingsBIT0 {
			bitDHT11,
			bitDHT22,
			bitLDR,
			bitLight,
			bitBM,
			bitRelais,
			bitRelaisShield,
			bitRain
		};
		/// @brief  EEPROM byte 1
		const enum settingsBIT1 {
			bitMoisture,
			bitDistance
		};
		
		/// @brief  EEPROM byte 2
		const enum settingsBIT2 {
			bitDebugEprom,
			bitDebugWiFi,
			bitDebugMqtt,
			bitDebugFinder,
			bitDebugRest
		};
		/// @brief  EEPROM byte 3
		const enum settingsBIT3 {
			bitDebugHT,
			bitDebugLDR,
			bitDebugLight,
			bitDebugBM,
			bitDebugRelais,
			bitDebugRain,
			bitDebugMoisture,
			bitDebugDistance
		};
		/// @brief  EEPROM byte 4
		const enum settingsBIT4 {
			bitUseLdrAvg,
			bitUseLightAvg,
			bitRelaisHand,
			bitRelaisHandValue,
			bitUseRainAvg,
			bitUseMoistureAvg
		};
		/// @brief  EEPROM byte ab 10 (uint8_t)
		const enum settingsByte {
			byteMaxCycleHT = 10,
			byteTemperatureCorrection, // int8_t * 10
			byteHumidityCorrection, // int8_t * 10
			byteMaxCycleLDR,
			byteLDRCorrection, // int8_t
			byteMaxCycleLight,
			bytePumpActive,
			byteMaxCycleRain,
			byteRainCorrection, // int8_t
			byteMaxCycleMoisture,
			byteMoistureMin,
			byteMaxCycleDistance,
			byteDistanceCorrection, // int8_t
			byteHeight
		};
		/// @brief  EEPROM 2byte ab 30 (uint16_t)
		const enum settingsWord {
			byteLightCorrection = 30, // int16_t
			byteThreshold = 32,
			bytePumpPause = 34,
			byteMoistureDry = 36,
			byteMoistureWet = 38,
			byteMaxVolume = 40
		};
		/// @brief  EEPROM 4byte ab 60
		const enum settingsDWord {

		};

		String DeviceName;
		String DeviceDescription;
		bool OfflineTrigger;
		bool UpdateFW;
		bool calcValues;

		const enum Debug {
			DebugEprom,
			DebugWiFi,
			DebugMqtt,
			DebugFinder,
			DebugRest,
			DebugRest,

			DebugHT,
			DebugLDR,
			DebugLight,
			DebugBM,
			DebugRelais,
			DebugRain,
			DebugMoisture,
			DebugDistance
		};
		enum useAvg {
			useAvgLDR,
			useAvgLight,
			useAvgRain,
			useAvgMoisture
		};
		enum maxCycle {
			maxCycleHT,
			maxCycleLDR,
			maxCycleLight,
			maxCycleRain,
			maxCycleMoisture,
			maxCycleDistance
		};
#ifdef wpHT
		float temperatureCorrection;
		float humidityCorrection;
#endif
#ifdef wpLDR
		int16_t ldrCorrection;
#endif
#ifdef wpLight
		int16_t lightCorrection;
#endif
#ifdef wpBM
	#ifdef wpLDR
		uint16_t threshold;
		String lightToTurnOn;
	#endif
#endif
#ifdef wpRelais
		bool relaisHand;
		bool relaisHandValue;
	#ifdef wpMoisture
		bool waterEmpty;
		uint16_t pumpActive;
		uint16_t pumpPause;
	#endif
#endif
#ifdef wpRain
		float rainCorrection;
#endif
#ifdef wpMoisture
		bool useMoistureAvg;
		byte moistureMin;
		// Dry = high e.g. 622
		int16_t moistureDry;
		// Wet = low e.g. 224
		int16_t moistureWet;
#endif
#ifdef wpDistance
		int8_t distanceCorrection;
		uint16_t maxVolume;
		uint8_t height;
#endif

		wpFreakaZone(String);

		void loop();

		AsyncWebServer server = AsyncWebServer(80);
		AsyncWebSocket ws = AsyncWebSocket("/ws");

		String getVersion();
		String getTime();
		String getDateTime();
		String getOnlineTime();
		String getOnlineTime(bool forDebug);
		void setVersion(uint16_t v);
		void blink();
		void setupWiFi();
		void scanWiFi();
		bool setupOta();
		void setupFinder();
		String readStringFromEEPROM(int addrOffset, String defaultString);
		int writeStringToEEPROM(int addrOffset, String &strToWrite);
		bool sendRest(String name, String value);
		bool sendRawRest(String target);
		void setupWebServer();

		static String JsonKeyValue(String name, String value);
		static String JsonKeyString(String name, String value);

		void DebugWS(String typ, String func, String msg);
		void DebugWS(String typ, String func, String msg, bool newline);
		void SendWS(String msg);

		void printStart();
		void printRestored();

	private:
		String SVNh = "$Rev: 117 $";
		String Revh;
		String Rev;
		int Buildh;
		int Build;
		String funcToString(String msg);

		String doWebServerDebugChange = "";
		void setWebServerDebugChange(String DebugPlugIn);
		void doTheWebServerDebugChange();

		String printEncryptionType(int thisType);
};
extern wpFreakaZone wpFZ;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
	<title>FreakaZone %DeviceName% Webserial</title>
	<style>
		body { background-color:#606060; color:#ececfb; }
		* { margin:0; padding:0; }
		#FreakaZoneWebSerial { margin:20px 50px; font-family:Consolas, Verdana, Arial, sans-serif; font-size:15px; }
		h1 { font-size:25px; }
		h2 { font-size:20px; }
		.z { color:#a91919; text-shadow:2px 2px #9f9f9f; font-weight:bold; font-size:30px; }
		div.ul { display:flex; }
		ul { margin-left:20px; list-style:none; }
		hr { height:1px; border:none; margin:5px 5%; background-color:#CCC; }
		#WebSerialBox, .ul ul { margin:10px; padding:15px; border:1px solid #ccc; border-radius:10px; box-shadow:3px 3px 5px #222 inset; }
		.ul ul { box-shadow: 3px 3px 5px #222; }
		.ul input { margin-right:5px; }
		.wpButton { display:inline-block; margin:2px 5px; padding:7px; width:150px; color:#ececfb; white-space:nowrap; cursor:pointer;
			line-height:16px; font-size:13px; font-weight:bold; text-align:center;
			border:1px solid #888; border-radius:4px;
			background-color:#555; background-image:linear-gradient(#606060 .25em, #222 1.75em); box-shadow:0px 0px 2px 0px #888; }
		.wpButton:hover { border-color:#AAA; }
		.wpButton:active { border-color:#555; box-shadow:inset 2px 2px 5px 0px #222; color:#888; }
		.bold { font-weight:bold; }
	</style>
</head>
<body>
	<div id="FreakaZoneWebSerial">
		<h1>Freaka<span class="z">Z</span>one %DeviceName% Web<span class="z">S</span>erial:</h1>
		<h2>%DeviceDescription%</h2>
		<div class="ul">
			%Debug%
			%CompiledWith%
			<ul>
				<li><span id="ForceMqttUpdate" class="wpButton" onclick="cmdHandle(event)">ForceMqttUpdate</span></li>
				<li><span id="UpdateFW" class="wpButton" onclick="cmdHandle(event)">UpdateFW</span></li>
				<li><span id="RestartDevice" class="wpButton" onclick="cmdHandle(event)">RestartDevice</span></li>
				<li><span id="ScanWiFi" class="wpButton" onclick="cmdHandle(event)">ScanWiFi</span></li>
				<li><span id="Blink" class="wpButton" onclick="cmdHandle(event)">Blink</span></li>
			</ul>
		</div>
		<pre id="WebSerialBox"></pre>
	</div>
	<script>
var gateway = 'ws://%IPADRESS%/ws';
var websocket;
var WebSerialBox;
var xmlHttp;
window.addEventListener('load', onLoad);
function onLoad(event) {
	initWebSocket();
	xmlHttp = new XMLHttpRequest();
}
function initWebSocket() {
	console.log('Trying to open a WebSocket connection...');
	websocket = new WebSocket(gateway);
	WebSerialBox = document.getElementById('WebSerialBox');
	websocket.onopen = onOpen;
	websocket.onclose = onClose;
	websocket.onmessage = onMessage;
}
function onOpen(event) {
	console.log('Connection opened');
}
function onClose(event) {
	console.log('Connection closed');
	setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
	const d = JSON.parse(event.data);
	if(typeof d.cmd != undefined && d.cmd == 'setDebug') {
		document.getElementById(d.msg.id).checked = d.msg.value;;
	}
	if(!d.newline) {
		if(WebSerialBox.hasChildNodes()) {
			WebSerialBox.removeChild(WebSerialBox.children[0]);
		}
	}
	WebSerialBox.innerHTML = '<p>' + d.msg + '</p>' + WebSerialBox.innerHTML;
}
function changeHandle(e) {
	xmlHttp.open( "GET", "/setDebug?Debug=" + e.target.id, false);
	xmlHttp.send( null );
}
function cmdHandle(e) {
	xmlHttp.open("GET", "/setCmd?cmd=" + e.target.id, false);
	xmlHttp.send(null);
}
	</script>
</body>
</html>
)rawliteral";
const uint8_t favicon[] PROGMEM = { // http://tomeko.net/online_tools/file_to_hex.php
	0x1F, 0x8B, 0x08, 0x08, 0xDC, 0x41, 0x23, 0x66, 0x04, 0x00, 0x46, 0x72, 0x65, 0x61, 0x6B, 0x61, 
	0x5A, 0x6F, 0x6E, 0x65, 0x5F, 0x6D, 0x69, 0x6E, 0x2E, 0x69, 0x63, 0x6F, 0x00, 0xA5, 0x93, 0xBF, 
	0x4B, 0x42, 0x51, 0x14, 0xC7, 0x8F, 0x19, 0x44, 0x54, 0x24, 0x81, 0xE9, 0xF3, 0x57, 0x96, 0x08, 
	0xBA, 0x88, 0xF0, 0xC0, 0x4D, 0x04, 0x17, 0x17, 0x1D, 0xB2, 0x45, 0x04, 0xD1, 0x67, 0xFA, 0xD4, 
	0x6C, 0x69, 0xAA, 0xC1, 0x41, 0x82, 0x28, 0x22, 0x08, 0x2A, 0x0A, 0x8A, 0x88, 0x1A, 0x4C, 0xDF, 
	0xD0, 0xD8, 0x16, 0xB4, 0xB7, 0xF4, 0x07, 0x34, 0x38, 0xF4, 0x07, 0x34, 0x94, 0x35, 0x44, 0xB7, 
	0x73, 0x9F, 0x5E, 0xBD, 0xBC, 0x8A, 0x1E, 0xF8, 0xE4, 0xCB, 0xF5, 0x1D, 0xEF, 0xE7, 0xFB, 0xEE, 
	0xF9, 0x7A, 0x1E, 0x80, 0x01, 0x3F, 0x26, 0x13, 0x5D, 0xDD, 0x50, 0x1E, 0x05, 0x98, 0x05, 0x00, 
	0x1F, 0x0A, 0x4B, 0x58, 0xE9, 0xD6, 0xFF, 0xBC, 0x48, 0x57, 0x99, 0x4C, 0x66, 0xA2, 0x58, 0x2C, 
	0xEE, 0xE8, 0x55, 0xA1, 0x50, 0x10, 0x05, 0xA1, 0xA5, 0xB2, 0x0E, 0x87, 0x32, 0xEE, 0xF3, 0x9D, 
	0x2F, 0x25, 0x93, 0x1B, 0x44, 0x8F, 0xB2, 0xD9, 0x55, 0x82, 0x1E, 0x39, 0xC6, 0x5B, 0xAD, 0xD7, 
	0x6E, 0xFC, 0x4E, 0xF4, 0x2A, 0x1E, 0xAF, 0x0D, 0xC5, 0xFB, 0xFD, 0x67, 0xD5, 0x7C, 0x3E, 0x3F, 
	0xC5, 0x78, 0xB3, 0x59, 0x99, 0x14, 0x84, 0x66, 0x9B, 0xDF, 0x23, 0x8A, 0x07, 0x24, 0x16, 0xDB, 
	0xFC, 0x12, 0xC5, 0xC3, 0x53, 0x41, 0x50, 0x64, 0x5E, 0x16, 0x8B, 0x32, 0x4F, 0xD9, 0x1E, 0xAF, 
	0xCA, 0x6E, 0x57, 0x82, 0x78, 0xFF, 0xC6, 0x78, 0x97, 0xAB, 0x41, 0xD2, 0xE9, 0x35, 0x22, 0xCB, 
	0x72, 0x87, 0x66, 0x45, 0xF7, 0x30, 0x59, 0x2C, 0x57, 0x60, 0xB3, 0xF5, 0x79, 0x26, 0xAC, 0x29, 
	0x69, 0xFE, 0x0C, 0x5E, 0xEF, 0x05, 0x91, 0xA4, 0x15, 0xEA, 0xD1, 0x96, 0x24, 0xC9, 0xDC, 0xDB, 
	0xC7, 0x7A, 0x66, 0xBC, 0xC6, 0xA3, 0xB5, 0xC7, 0x7B, 0x04, 0x83, 0x47, 0x98, 0x95, 0x4C, 0x4A, 
	0xA5, 0xD2, 0x5D, 0xBD, 0x5E, 0x1F, 0x45, 0x2F, 0x60, 0x4A, 0x26, 0xD7, 0xB5, 0x3C, 0x5E, 0x8A, 
	0x11, 0x7B, 0xBC, 0xE5, 0x3D, 0x22, 0x91, 0x6D, 0x7A, 0x06, 0xAA, 0x7D, 0x95, 0x1D, 0xE8, 0xB7, 
	0xE7, 0xD3, 0x79, 0x98, 0xC1, 0xF5, 0x69, 0xE0, 0xD1, 0xA4, 0xFF, 0x19, 0xF2, 0xEA, 0x39, 0x96, 
	0xC3, 0xE1, 0x5D, 0x60, 0xFA, 0xC9, 0x37, 0xB1, 0x2F, 0xEA, 0xD1, 0x08, 0xE0, 0xFA, 0xCA, 0x3C, 
	0x9C, 0xCE, 0x06, 0x49, 0xA5, 0xD4, 0x3C, 0xDF, 0xCB, 0xE5, 0x72, 0x88, 0xCB, 0xBF, 0x2F, 0xF4, 
	0x96, 0xF9, 0x19, 0x8D, 0x46, 0xB7, 0x6E, 0xE8, 0xB3, 0x99, 0x87, 0xC7, 0x73, 0x49, 0x72, 0xB9, 
	0x2A, 0xF5, 0x78, 0xA6, 0xAF, 0x05, 0xD6, 0xB8, 0x3C, 0x95, 0x50, 0x22, 0x51, 0x7B, 0xD4, 0xCE, 
	0x6A, 0x20, 0x70, 0xC2, 0xCF, 0x0E, 0xDE, 0x1F, 0xAB, 0x7D, 0x60, 0x96, 0x23, 0x3C, 0x8F, 0x99, 
	0x3D, 0x08, 0x3A, 0x66, 0x0F, 0xFB, 0x1A, 0x8A, 0xC7, 0x5C, 0x3B, 0x95, 0x4A, 0x65, 0x81, 0xB2, 
	0x1A, 0x7E, 0x91, 0xCE, 0xE6, 0x7F, 0x42, 0x3E, 0xCF, 0x67, 0x76, 0x6F, 0x00, 0xA8, 0xF3, 0x32, 
	0x62, 0x2D, 0x02, 0xD0, 0x9E, 0x03, 0x78, 0x99, 0xEE, 0xEA, 0x63, 0x0C, 0xE0, 0xD3, 0x88, 0x35, 
	0x03, 0xFE, 0xC6, 0x44, 0xF7, 0x61, 0xFD, 0x1B, 0x0F, 0x2D, 0x1D, 0xC3, 0x7E, 0x04, 0x00, 0x00
};
String processor(const String& var);
#endif
