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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperWebServer.h 269 2025-07-01 19:25:14Z               $ #
//#                                                                                 #
//###################################################################################
#ifndef helperWebServer_h
#define helperWebServer_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <ESPAsyncWebServer.h>
class helperWebServer {
	public:
		bool Debug = false;
		// commands
		String mqttTopicDebug;
		const uint8_t cmdDoNothing = 0;

		const uint8_t cmdBlink = 1;
		const uint8_t cmdForceMqttUpdate = 2;
		const uint8_t cmdForceRenewValue = 3;
		const uint8_t cmdUpdateFW = 4;
		const uint8_t cmdUpdateCheck = 5;
		const uint8_t cmdUpdateHTTP = 6;
		const uint8_t cmdRestartESP = 7;
		const uint8_t cmdScanWiFi = 8;
		const uint8_t cmdCheckDns = 9;
		const uint8_t cmdSetName = 10;
		const uint8_t cmdMaxWorking = 11;
		uint8_t doCommand;

		const uint8_t cmdDebugEEPROM = 1;
		const uint8_t cmdDebugFinder = 2;
		const uint8_t cmdDebugModules = 3;
		const uint8_t cmdDebugMqtt = 4;
		const uint8_t cmdDebugOnlineToggler = 5;
		const uint8_t cmdDebugUpdate = 7;
		const uint8_t cmdDebugWebServer = 8;
		const uint8_t cmdDebugWiFi = 9;

		const uint8_t cmdDebugDHT = 20;
		const uint8_t cmdDebugLDR = 21;
		const uint8_t cmdDebugLight = 22;
		const uint8_t cmdDebugBM = 23;
		const uint8_t cmdDebugWindow = 24;
		const uint8_t cmdDebugRelais = 25;
		const uint8_t cmdDebugRain = 26;
		const uint8_t cmdDebugMoisture = 27;
		const uint8_t cmdDebugDistance = 28;
		const uint8_t cmdDebugCwWw = 29;
		const uint8_t cmdDebugAnalogOut = 30;
		const uint8_t cmdDebugAnalogOut2 = 31;
		const uint8_t cmdDebugNeoPixel = 32;
		const uint8_t cmdDebugRpm = 33;
		const uint8_t cmdDebugImpulseCounter = 34;
		const uint8_t cmdDebugUnderfloor1 = 35;
		const uint8_t cmdDebugUnderfloor2 = 36;
		const uint8_t cmdDebugUnderfloor3 = 37;
		const uint8_t cmdDebugUnderfloor4 = 38;
		const uint8_t cmdDebugWindow2 = 39;
		const uint8_t cmdDebugWindow3 = 40;
		const uint8_t cmdDebugWeight = 41;
		const uint8_t cmdDebugRFID = 42;
		const uint8_t cmdDebugClock = 43;
		const uint8_t cmdDebugDS18B20 = 44;
		uint8_t doDebugChange;

		const uint8_t cmdModuleDHT11 = 1;
		const uint8_t cmdModuleDHT22 = 2;
		const uint8_t cmdModuleLDR = 3;
		const uint8_t cmdModuleLight = 4;
		const uint8_t cmdModuleBM = 5;
		const uint8_t cmdModuleWindow = 6;
		const uint8_t cmdModuleRelais = 7;
		const uint8_t cmdModuleRelaisShield = 8;
		const uint8_t cmdModuleRain = 9;
		const uint8_t cmdModuleMoisture = 10;
		const uint8_t cmdModuleDistance = 11;
		const uint8_t cmdModuleCwWw = 12;
		const uint8_t cmdModuleAnalogOut = 13;
		const uint8_t cmdModuleAnalogOut2 = 14;
		const uint8_t cmdModuleNeoPixel = 15;
		const uint8_t cmdModuleRpm = 16;
		const uint8_t cmdmoduleImpulseCounter = 17;
		const uint8_t cmdmoduleUnderfloor1 = 18;
		const uint8_t cmdmoduleUnderfloor2 = 19;
		const uint8_t cmdmoduleUnderfloor3 = 20;
		const uint8_t cmdmoduleUnderfloor4 = 21;
		const uint8_t cmdModuleWindow2 = 22;
		const uint8_t cmdModuleWindow3 = 23;
		const uint8_t cmdModuleWeight = 24;
		const uint8_t cmdModuleRFID = 25;
		const uint8_t cmdModuleClock = 26;
		const uint8_t cmdmoduleDS18B20 = 27;
		uint8_t doModuleChange;

		uint8_t doBlink;

		helperWebServer();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();

		AsyncWebServer webServer = AsyncWebServer(80);
		AsyncWebSocket webSocket = AsyncWebSocket("/ws");

		void setupWebServer();
		void setCommand(uint8_t command);
		void setModuleChange(uint8_t modul);
		void setDebugChange(uint8_t debug);
		void setBlink();
		void doTheCommand();
		void doTheModuleChange();
		void doTheDebugChange();
		void doTheBlink();
		
		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
		String getchangeModule(String id, String name, bool state);
		String getChangeDebug(String id, String name, bool state);
		String getChangeCmd(String id, String name, bool state);
	private:
		String SVNh = "$Rev: 269 $";
		bool DebugLast = false;
		unsigned long publishDebugLast = 0;
		String newName;
		String newDescription;
};
extern helperWebServer wpWebServer;

String processor(const String& var);
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
	<title>%DeviceName% Webserial</title>
	<meta charset="utf-8" />
	<meta name="author" content="Christian Scheid" />
	<meta name="company" content="FreakaZone" />
	<style>
		body { background-color:#606060; color:#ececfb; }
		* { margin:0; padding:0; }
		h1 { font-size:25px; }
		h2 { font-size:20px; }
		ul { margin-left:20px; list-style:none; }
		hr { height:1px; border:none; margin:5px 5%; background-color:#CCC; }
		.z { color:#a91919; text-shadow:2px 2px #9f9f9f; font-weight:bold; font-size:30px; }
		.ulContainer { display:flex; }
		.wpContainer { margin:10px; padding:15px; border:1px solid #ccc; border-radius:10px; box-shadow:3px 3px 5px #222 inset;
			background-color:#444; }
		.setChange { border: 1px solid transparent; padding: 3px 5px; border-radius: 5px; cursor: pointer; }
		.setChange:hover { border-color:#ccc; }
		.setChange span { white-space:nowrap; }
		#FreakaZoneWebSerial { margin:20px 50px; font-family:Verdana, Arial, sans-serif; font-size:12px; }
		#WebSerialBox * { font-family:Consolas, Verdana, Arial, sans-serif; font-size:12px; }
		#restartRequired, #newVersion, .ulContainer ul { box-shadow: 3px 3px 5px #222; }
		#restartRequired { text-align:center; border-color:#a91919; color:#df0d0d; text-shadow:0 0 3px #1e1414; }
		#newVersion { text-align:center; border-color:#cebd2f; color:#cebd2f; text-shadow:0 0 3px #12130c; }
		#progressBg { border:1px solid #555; border-radius: 5px; box-shadow: 2px 2px 3px #333 inset; }
		#progress { background-color:#6060dd; height:25px; width:25%; text-align:left;
			box-shadow: -1px -3px 5px #222 inset, 1px 1px 5px #ccc inset; border-radius: 5px; }
		#progressVal { line-height:25px; padding-left: 10px; white-space:nowrap; }
		.ulContainer input { margin-right:5px; }
		.wpHidden { display:none !important; }
		.wpInput { padding:5px; text-align:center; }
		.wpButton { display:inline-block; margin:2px 5px; padding:7px; width:150px; color:#ececfb; white-space:nowrap; cursor:pointer;
			line-height:16px; font-weight:bold; text-align:center;
			border:1px solid #888; border-radius:4px;
			background-color:#555; background-image:linear-gradient(#606060 .25em, #222 1.75em); box-shadow:0px 0px 2px 0px #888; }
		.wpButton:hover { border-color:#AAA; }
		.wpButton:active { border-color:#555; box-shadow:inset 2px 2px 5px 0px #222; color:#888; }
		.bold { font-weight:bold; }
		.color-debug { color:#aaa; text-shadow:0 0 3px #313131; }
		.color-info { color:#ececfb; text-shadow:0 0 3px #000; }
		.color-ok { color:#38C133; text-shadow:0 0 3px #313131; }
		.color-warn { color:#cebd2f; text-shadow:0 0 3px #12130c; }
		.color-error { color:#df0d0d; text-shadow:0 0 3px #1e1414; }
		.setChange span::before { content:'✘ '; }
		.setChange span.color-ok::before { content:'✔ '; }
	</style>
</head>
<body>
	<div id="FreakaZoneWebSerial">
		<h1>Freaka<span class="z">Z</span>one %DeviceName% <span class="setChange" onclick="changeName()">&#9998;</span> Web<span class="z">S</span>erial:</h1>
		<h2>%DeviceDescription% (%Version%)</h2>
		<div id="restartRequired" class="wpContainer wpHidden"></div>
		<div id="newVersion" class="wpContainer%newVersion%">%innernewVersion%</div>
		<div id="progressContainer" class="wpHidden">
			<div class="wpContainer">
				<div id="progressBg">
					<div id="progress">
						<span id="progressVal">0</span>
					</div>
				</div>
			</diV>
		</div>
		<div class="wpContainer wpHidden" id="changeNameContainer">
			<span>New Name:</span>
			<input type="text" id="SetNewDevicename" value="%DeviceName%" /><br />
			<span>New Description:</span>
			<input type="text" id="SetNewDeviceDescription" value="%DeviceDescription%" /><br />
			<span id="SetDevicename" class="wpButton" onclick="cmdNewDevicename()">save</span>
		</div>
		<div class="ulContainer">
			%Debug%
			%CompiledWith%
			<ul class="wpContainer">
				<li><span class='bold'>Device:</span></li><li><hr /></li>
				<li><span id="RestartDevice" class="wpButton" onclick="cmdHandle(event)">RestartDevice</span></li>
				<li><span id="ForceMqttUpdate" class="wpButton" onclick="cmdHandle(event)">ForceMqttUpdate</span></li>
				<li><span id="ForceRenewValue" class="wpButton" onclick="cmdHandle(event)">ForceRenewValue</span></li>
				<li><a class="wpButton" href="/status" target="_blank">Status</a></li>
				<li><span class='bold'>Updates:</span></li><li><hr /></li>
				<!--li><span id="UpdateFW" class="wpButton" onclick="cmdHandle(event)">set Update Mode</span></li-->
				<li><span id="UpdateCheck" class="wpButton" onclick="cmdHandle(event)">Check Update</span></li>
				<li><span id="UpdateHTTP" class="wpButton" onclick="cmdHandle(event)">Update</span></li>
				<li><span class='bold'>Stuff:</span></li><li><hr /></li>
				<li><span id="ScanWiFi" class="wpButton" onclick="cmdHandle(event)">Scan WiFi</span></li>
				<li><span id="CheckDns" class="wpButton" onclick="cmdHandle(event)">Check DNS</span></li>
				<li><span id="Blink" class="wpButton" onclick="cmdHandle(event)">Blink</span></li>
			</ul>
			<ul id="LiPump" class="wpContainer wpHidden">
				<li><li><span class='bold'>Pump:</span></li><hr /></li>
				<li>
					Cycle: <span id="pumpCycleActive" class="color-info"></span><br />
					Pump: <span id="pumpStarted" class="color-info"></span><br />
					Pause: <span id="pumpInPause" class="color-info"></span>
					<p>in Pause:</p>
					<p id="remainPumpInPause" class="wpContainer wpInput"></p>
				</li>
			</ul>
		</div>
		<pre id="WebSerialBox" class="wpContainer"></pre>
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
	WebSerialBox = document.getElementById('WebSerialBox');
	setInterval(checkConnection, 5000);
}
function initWebSocket() {
	console.log('Trying to open a WebSocket connection...');
	websocket = new WebSocket(gateway);
	websocket.onopen = onOpen;
	websocket.onclose = onClose;
	websocket.onmessage = onMessage;
	document.getElementById('restartRequired').classList.add('wpHidden');
	//document.getElementById('newVersion').classList.add('wpHidden');
	document.getElementById('LiPump').classList.add('wpHidden');
	document.getElementById('progressContainer').classList.add('wpHidden');
}
function checkConnection() {
	websocket.send('PING');
	if(websocket.readyState != WebSocket.OPEN) {
		console.log('Connection Lost');
		initWebSocket();
	}
}
function onOpen(event) {
	console.log('Connection opened');
	pingpongmsg = 1;
}
function onClose(event) {
	console.log('Connection closed, waitForReconnect');
	pingpong = null;
}
function onMessage(event) {
	%debugWebServer%
	const d = tryParseJSONObject(event.data);
	if(typeof d.cmd != 'undefined') {
		if(d.cmd == 'setDebug') {
			console.log(d);
			changeBoolValue(d.msg.id, d.msg.value);
		} else if(d.cmd == 'setModule') {
			console.log(d);
			changeBoolValue(d.msg.id, d.msg.value);
		} else if(d.cmd == 'restartRequired') {
			console.log(d);
			let restartRequired = document.getElementById('restartRequired');
			restartRequired.classList.remove('wpHidden');
			restartRequired.innerHTML = '!!! Restart Required !!!';
		} else if(d.cmd == 'newVersion') {
			let newVersion = document.getElementById('newVersion');
			if(d.msg.newVersion) {
				console.log(d);
				newVersion.classList.remove('wpHidden');
				newVersion.innerHTML = '--- Update Available ---<br />installed: ' + d.msg.installedVersion + '<br />update: ' + d.msg.serverVersion;
			} else {
				newVersion.classList.add('wpHidden');
			}
		} else if(d.cmd == 'remainPumpInPause') {
			console.log(d);
			let LiPump = document.getElementById('LiPump');
			LiPump.classList.remove('wpHidden');
			let remainPumpInPause = document.getElementById('remainPumpInPause');
			remainPumpInPause.innerHTML = d.msg;
		} else if(d.cmd == 'pumpStatus') {
			console.log(d);
			let LiPump = document.getElementById('LiPump');
			LiPump.classList.remove('wpHidden');
			changePumpState('pumpCycleActive', d.msg.pumpCycleActive != 0);
			changePumpState('pumpStarted', d.msg.pumpStarted != 0);
			changePumpState('pumpInPause', d.msg.pumpInPause != 0);
		} else if(d.cmd == 'pumpCycleFinished') {
			console.log(d);
			let LiPump = document.getElementById('LiPump');
			LiPump.classList.add('wpHidden');
		} else if(d.cmd == 'updateProgress') {
			console.log(d);
			document.getElementById('progressContainer').classList.remove('wpHidden');
			let progress = document.getElementById('progress');
			let progressVal = document.getElementById('progressVal');
			progress.style.width = d.percent.replace(/\s/g, '');
			progressVal.innerHTML = d.percent;
		} else {
			console.log('unknown command:');
			console.log(d);
		}
	}
	if(typeof d.msgheader != 'undefined') {
		console.log(d);
		let spanHeader = document.createElement('span');
		spanHeader.classList.add(d.cssClass);
		spanHeader.innerHTML = d.msgheader;
		let spanBody = document.createElement('span');
		spanBody.innerHTML = d.msgbody;
		let p = document.createElement('p');
		p.appendChild(spanHeader);
		p.appendChild(spanBody);
		WebSerialBox.prepend(p);
	}
}
function tryParseJSONObject(jsonString) {
    try {
        var o = JSON.parse(jsonString);
        if (o && typeof o === "object") {
            return o;
        }
    }
    catch (e) { }
    return false;
}
function changePumpState(elem, state) {
	let elemHTML = document.getElementById(elem);
	if(state) {
		elemHTML.classList.add('color-ok');
		elemHTML.innerHTML = 'On';
	} else {
		elemHTML.classList.remove('color-ok');
		elemHTML.innerHTML = 'Off';
	}
}
function changeBoolValue(elem, state) {
	let elemHTML = document.getElementById(elem);
	if(state) {
		elemHTML.classList.add('color-ok');
	} else {
		elemHTML.classList.remove('color-ok');
	}
}
function changeModule(e) {
	xmlHttp.open("GET", "/setModule?Module=" + e.target.id, false);
	xmlHttp.send(null);
}
function changeDebug(e) {
	xmlHttp.open("GET", "/setDebug?Debug=" + e.target.id, false);
	xmlHttp.send(null);
}
function cmdHandle(e) {
	xmlHttp.open("GET", "/setCmd?cmd=" + e.target.id, false);
	xmlHttp.send(null);
}
function changeName() {
	document.getElementById('changeNameContainer').classList.remove('wpHidden');
}
function cmdNewDevicename() {
	let newName = document.getElementById('SetNewDevicename').value;
	if(newName != '') newName = '&newName=' + newName;
	let newDescription = document.getElementById('SetNewDeviceDescription').value;
	if(newDescription != '') newDescription = '&newDescription=' + newDescription;
	xmlHttp.open("GET", "/setCmd?cmd=SetDeviceName" + newName + newDescription, false);
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
#endif