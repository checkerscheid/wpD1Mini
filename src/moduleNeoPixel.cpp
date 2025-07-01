//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 22.07.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleNeoPixel.cpp 269 2025-07-01 19:25:14Z              $ #
//#                                                                                 #
//###################################################################################
#include <moduleNeoPixel.h>
// A non-blocking everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

// #ifdef __AVR__
//  #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
// #endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
// #ifdef ESP32
// // Cannot use 6 as output for ESP. Pins 6-11 are connected to SPI flash. Use 16 instead.
// #define LED_PIN    16
// #else
// #define LED_PIN    6
// #endif

// How many NeoPixels are attached to the Arduino?
// #define LED_COUNT 60

moduleNeoPixel wpNeoPixel;
// AnalogOut = W || WW / D6
// AnalogOut2 = CW / D5
moduleNeoPixel::moduleNeoPixel() {
	// section to config and copy
	ModuleName = "NeoPixel";
	mb = new moduleBase(ModuleName);
	PinWW = D6;
	pinMode(PinWW, OUTPUT);
	analogWrite(PinWW, LOW);
	
	PinCW = D5;
	pinMode(PinCW, OUTPUT);
	analogWrite(PinCW, LOW);

	//wpFZ.loopTime = 100;
	steps = 5;
}
void moduleNeoPixel::init() {
	Pin = D7;
	if(pixelCount > 10000) {
		SetPixelCount(0);
	}
	// Declare our NeoPixel strip object:
	if(isRGB) {
		strip = new Adafruit_NeoPixel(pixelCount, Pin, NEO_RGB + NEO_KHZ800);
	} else {
		strip = new Adafruit_NeoPixel(pixelCount, Pin, NEO_GRB + NEO_KHZ800);
	}

	// Argument 1 = Number of pixels in NeoPixel strip
	// Argument 2 = Arduino pin number (most are valid)
	// Argument 3 = Pixel type flags, add together as needed:
	//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
	//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
	//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
	//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
	//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

	pixelPrevious = 0;        // Previous Pixel Millis
	patternPrevious = 0;      // Previous Pattern Millis
	modeCurrent = 0;       // Current Pattern Number
	patternInterval = 5000;   // Pattern Interval (ms)

	pixelInterval = 50;       // Pixel Interval (ms)
	pixelQueue = 0;           // Pattern Pixel Queue
	pixelCycle = 0;           // Pattern Pixel Cycle
	sleep = 0;

	demoMode = false;
	useBorder = false;
	staticIsSet = false;
	//status = "S_OK";

	// values
	mqttTopicMaxPercent = wpFZ.DeviceName + "/" + ModuleName + "/MaxPercent";
	mqttTopicValueR = wpFZ.DeviceName + "/" + ModuleName + "/R";
	mqttTopicValueG = wpFZ.DeviceName + "/" + ModuleName + "/G";
	mqttTopicValueB = wpFZ.DeviceName + "/" + ModuleName + "/B";
	mqttTopicWW = wpFZ.DeviceName + "/" + ModuleName + "/WW";
	mqttTopicCW = wpFZ.DeviceName + "/" + ModuleName + "/CW";
	//mqttTopicStatus = wpFZ.DeviceName + "/" + ModuleName + "/Status";
	mqttTopicDemoMode = wpFZ.DeviceName + "/" + ModuleName + "/DemoMode";
	mqttTopicModeName = wpFZ.DeviceName + "/" + ModuleName + "/ModeName";
	mqttTopicEffectSpeed = wpFZ.DeviceName + "/" + ModuleName + "/EffectSpeed";
	mqttTopicSleep = wpFZ.DeviceName + "/" + ModuleName + "/Sleep";
	// settings
	mqttTopicPixelCount = wpFZ.DeviceName + "/" + ModuleName + "/PixelCount";
	mqttTopicUseBorder = wpFZ.DeviceName + "/" + ModuleName + "/Border";
	// commands
	mqttTopicSetR = wpFZ.DeviceName + "/settings/" + ModuleName + "/R";
	mqttTopicSetG = wpFZ.DeviceName + "/settings/" + ModuleName + "/G";
	mqttTopicSetB = wpFZ.DeviceName + "/settings/" + ModuleName + "/B";
	mqttTopicSetWW = wpFZ.DeviceName + "/settings/" + ModuleName + "/WW";
	mqttTopicSetCW = wpFZ.DeviceName + "/settings/" + ModuleName + "/CW";
	mqttTopicSetDemoMode = wpFZ.DeviceName + "/settings/" + ModuleName + "/DemoMode";
	mqttTopicSetMode = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetMode";
	mqttTopicSetEffectSpeed = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetEffectSpeed";
	mqttTopicSetSleep = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetSleep";
	mqttTopicSetPixelCount = wpFZ.DeviceName + "/settings/" + ModuleName + "/PixelCount";
	mqttTopicSetUseBorder = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetBorder";
	mqttTopicSetIsRGB = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetRGB";

	maxPercent = 0;
	publishValueLast = 0;
	publishModeLast = 0;
	lastBorderSend = 0;
	effectSpeed = 10;

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugNeoPixel);

	// setup() function -- runs once at startup --------------------------------

	// These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
	// Any other board, you can remove this part (but no harm leaving it):
	// #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
	//   clock_prescale_set(clock_div_1);
	// #endif
	// END of Trinket-specific code.

	strip->begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
	strip->show();            // Turn OFF all pixels ASAP
}

// loop() function -- runs repeatedly as long as board is on ---------------
void moduleNeoPixel::cycle() {
	if(wpFZ.calcValues) {
		calc();
	}
	publishValues();

	//if(wpModules.useModuleAnalogOut && wpModules.useModuleAnalogOut2) {
		// RGB LED has CW + WW
		// use AnalogOut for WW or White
		// use AnalogOut2 for CW with WW
		// must have output limitations
		// @todo make logik, that CW + WW <= 254
		//wpAnalogOut.hardwareoutMax = 50;
		//wpAnalogOut2.hardwareoutMax = 50;
	//}
	//ESP.wdtFeed();
}

void moduleNeoPixel::publishSettings() {
	publishSettings(false);
}
void moduleNeoPixel::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicDemoMode.c_str(), String(demoMode).c_str());
	wpMqtt.mqttClient.publish(mqttTopicPixelCount.c_str(), String(pixelCount).c_str());
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicSetR.c_str(), String(valueR).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetG.c_str(), String(valueG).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetB.c_str(), String(valueB).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetDemoMode.c_str(), String(demoMode).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetMode.c_str(), String(modeCurrent).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetEffectSpeed.c_str(), String(effectSpeed).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetPixelCount.c_str(), String(pixelCount).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetSleep.c_str(), String(sleep).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetUseBorder.c_str(), String(useBorder).c_str());
		wpMqtt.mqttClient.publish(mqttTopicSetIsRGB.c_str(), String(isRGB).c_str());
	}
	mb->publishSettings(force);
}

void moduleNeoPixel::publishValues() {
	publishValues(false);
}
void moduleNeoPixel::publishValues(bool force) {
	if(force) {
		publishValueLast = 0;
		publishAnalogOutWWLast = 0;
		publishAnalogOutCWLast = 0;
		//publishStatusLast = 0;
		publishMaxPercentLast = 0;
		publishModeLast = 0;
		publishEffectSpeedLast = 0;
		publishUseBorderLast = 0;
	}
	if(valueRLast != valueR || valueGLast != valueG || valueBLast != valueB ||
		wpFZ.CheckQoS(publishValueLast)) {
		publishValue();
	}
	if(useWW && (AnalogOutWWLast != AnalogOutWW || wpFZ.CheckQoS(publishAnalogOutWWLast))) {
		AnalogOutWWLast = AnalogOutWW;
		int32_t percent = (uint8) AnalogOutWW / 2.55;
		wpMqtt.mqttClient.publish(mqttTopicWW.c_str(), String(percent).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicWW, String(percent));
		}
		publishAnalogOutWWLast = wpFZ.loopStartedAt;
	}
	if(useCW && (AnalogOutCWLast != AnalogOutCW || wpFZ.CheckQoS(publishAnalogOutCWLast))) {
		AnalogOutCWLast = AnalogOutCW;
		int32_t percent = (uint8) AnalogOutCW / 2.55;
		wpMqtt.mqttClient.publish(mqttTopicCW.c_str(), String(percent).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicCW, String(percent));
		}
		publishAnalogOutCWLast = wpFZ.loopStartedAt;
	}
	//if(statusLast != getStripStatus() || wpFZ.CheckQoS(publishStatusLast)) {
	// if(publishStatusLast + 10000 > wpFZ.loopStartedAt) {
	// 	//statusLast = getStripStatus();
	// 	wpMqtt.mqttClient.publish(mqttTopicStatus.c_str(), status.c_str());
	// 	publishStatusLast = wpFZ.loopStartedAt;
	// }
	if(maxPercentLast != maxPercent || wpFZ.CheckQoS(publishMaxPercentLast)) {
		maxPercentLast = maxPercent;
		wpMqtt.mqttClient.publish(mqttTopicMaxPercent.c_str(), String(maxPercent).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicMaxPercent, String(maxPercent));
		}
		publishMaxPercentLast = wpFZ.loopStartedAt;
	}
	if(modeCurrentLast != modeCurrent || wpFZ.CheckQoS(publishModeLast)) {
		modeCurrentLast = modeCurrent;
		wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), GetModeName(modeCurrent).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicModeName, GetModeName(modeCurrent));
		}
		publishModeLast = wpFZ.loopStartedAt;
	}
	if(effectSpeedLast != effectSpeed || wpFZ.CheckQoS(publishEffectSpeedLast)) {
		effectSpeedLast = effectSpeed;
		wpMqtt.mqttClient.publish(mqttTopicEffectSpeed.c_str(), String(effectSpeed).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicEffectSpeed, String(effectSpeed));
		}
		publishEffectSpeedLast = wpFZ.loopStartedAt;
	}
	if(wpFZ.loopStartedAt > publishSleepLast + (2 * 1000)) {
		if(sleepLast != sleep || wpFZ.CheckQoS(publishSleepLast)) {
			sleepLast = sleep;
			wpMqtt.mqttClient.publish(mqttTopicSleep.c_str(), String(sleep).c_str());
			if(wpMqtt.Debug) {
				mb->printPublishValueDebug(mqttTopicSleep, String(sleep));
			}
			publishSleepLast = wpFZ.loopStartedAt;
		}
	}
	if(useBorderLast != useBorder || wpFZ.CheckQoS(publishUseBorderLast)) {
		useBorderLast = useBorder;
		wpMqtt.mqttClient.publish(mqttTopicUseBorder.c_str(), String(useBorder).c_str());
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicUseBorder, String(useBorder));
		}
		publishUseBorderLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleNeoPixel::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetR.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetG.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetB.c_str());
	if(useWW) {
		wpMqtt.mqttClient.subscribe(mqttTopicSetWW.c_str());
	}
	if(useCW) {
		wpMqtt.mqttClient.subscribe(mqttTopicSetCW.c_str());
	}
	wpMqtt.mqttClient.subscribe(mqttTopicSetDemoMode.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetMode.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetEffectSpeed.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetPixelCount.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetSleep.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetUseBorder.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetIsRGB.c_str());
	mb->setSubscribes();
}

void moduleNeoPixel::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetR.c_str()) == 0) {
		uint8_t readValueR = msg.toInt();
		if(valueR != readValueR) {
			SetValueR(readValueR);
			wpFZ.DebugcheckSubscribes(mqttTopicSetR, String(valueR));
		}
	}
	if(strcmp(topic, mqttTopicSetG.c_str()) == 0) {
		uint8_t readValueG = msg.toInt();
		if(valueG != readValueG) {
			SetValueG(readValueG);
			wpFZ.DebugcheckSubscribes(mqttTopicSetG, String(valueG));
		}
	}
	if(strcmp(topic, mqttTopicSetB.c_str()) == 0) {
		uint8_t readValueB = msg.toInt();
		if(valueB != readValueB) {
			SetValueB(readValueB);
			wpFZ.DebugcheckSubscribes(mqttTopicSetB, String(valueB));
		}
	}
	if(useWW && strcmp(topic, mqttTopicSetWW.c_str()) == 0) {
		uint8_t readWW = msg.toInt();
		if(targetWW != readWW) {
			SetWW(readWW);
			wpFZ.DebugcheckSubscribes(mqttTopicSetWW, String(targetWW));
		}
	}
	if(useCW && strcmp(topic, mqttTopicSetCW.c_str()) == 0) {
		uint8_t readCW = msg.toInt();
		if(targetCW != readCW) {
			SetCW(readCW);
			wpFZ.DebugcheckSubscribes(mqttTopicSetCW, String(targetCW));
		}
	}
	if(strcmp(topic, mqttTopicSetDemoMode.c_str()) == 0) {
		bool readDemoMode = msg.toInt();
		if(demoMode != readDemoMode) {
			demoMode = readDemoMode;
			wpFZ.DebugcheckSubscribes(mqttTopicSetDemoMode, String(demoMode));
		}
	}
	if(strcmp(topic, mqttTopicSetMode.c_str()) == 0) {
		uint readSetMode = msg.toInt();
		if(modeCurrent != readSetMode) {
			modeCurrent = readSetMode;
			wpFZ.DebugcheckSubscribes(mqttTopicSetMode, String(modeCurrent));
		}
	}
	if(strcmp(topic, mqttTopicSetEffectSpeed.c_str()) == 0) {
		uint8_t readSetEffectSpeed = msg.toInt();
		if(effectSpeed != readSetEffectSpeed) {
			SetEffectSpeed(readSetEffectSpeed);
			wpFZ.DebugcheckSubscribes(mqttTopicSetEffectSpeed, String(effectSpeed));
		}
	}
	if(strcmp(topic, mqttTopicSetPixelCount.c_str()) == 0) {
		uint16_t readPixelCount = msg.toInt();
		if(pixelCount != readPixelCount) {
			SetPixelCount(readPixelCount);
			wpFZ.DebugcheckSubscribes(mqttTopicSetPixelCount, String(readPixelCount));
		}
	}
	if(strcmp(topic, mqttTopicSetSleep.c_str()) == 0) {
		uint readSleep = msg.toInt();
		SetSleep(readSleep);
		wpFZ.DebugcheckSubscribes(mqttTopicSleep, String(readSleep));
	}
	if(strcmp(topic, mqttTopicSetUseBorder.c_str()) == 0) {
		bool readUseBorder = msg.toInt();
		if(useBorder != readUseBorder) {
			useBorder = readUseBorder;
			wpFZ.DebugcheckSubscribes(mqttTopicSetUseBorder, String(useBorder));
		}
	}
	if(strcmp(topic, mqttTopicSetIsRGB.c_str()) == 0) {
		bool readIsRGB = msg.toInt();
		if(isRGB != readIsRGB) {
			SetRGB(readIsRGB);
			wpFZ.DebugcheckSubscribes(mqttTopicSetIsRGB, String(isRGB));
		}
	}
	mb->checkSubscribes(topic, msg);
}
void moduleNeoPixel::InitValueR(uint8_t r) {
	valueR = r;
	staticIsSet = false;
}
uint8_t moduleNeoPixel::GetValueR() { return valueR; }
String moduleNeoPixel::SetValueR(uint8_t r) {
	targetR = r;
	wpEEPROM.WriteByteToEEPROM("NeoPixelR", wpEEPROM.byteNeoPixelValueR, targetR);
	staticIsSet = false;
	modeCurrent = ModeBlender;
	return wpFZ.JsonKeyValue("R", String(targetR));
}
void moduleNeoPixel::InitValueG(uint8_t g) {
	valueG = g;
	staticIsSet = false;
}
uint8_t moduleNeoPixel::GetValueG() { return valueG; }
String moduleNeoPixel::SetValueG(uint8_t g) {
	targetG = g;
	wpEEPROM.WriteByteToEEPROM("NeoPixelG", wpEEPROM.byteNeoPixelValueG, targetG);
	staticIsSet = false;
	modeCurrent = ModeBlender;
	return wpFZ.JsonKeyValue("G", String(targetG));
}
void moduleNeoPixel::InitValueB(uint8_t b) {
	valueB = b;
	staticIsSet = false;
}
uint8_t moduleNeoPixel::GetValueB() { return valueB; }
String moduleNeoPixel::SetValueB(uint8_t b) {
	targetB = b;
	wpEEPROM.WriteByteToEEPROM("NeoPixelB", wpEEPROM.byteNeoPixelValueB, targetB);
	staticIsSet = false;
	modeCurrent = ModeBlender;
	return wpFZ.JsonKeyValue("B", String(targetB));
}
String moduleNeoPixel::SetEffectSpeed(uint8_t es) {
	if(es > 20) es = 20;
	if(es < 1) es = 1;
	effectSpeed = es;
	return wpFZ.JsonKeyValue("EffectSpeed", String(effectSpeed));
}
String moduleNeoPixel::SetSleep(uint seconds) {
	if(seconds == 0) {
		sleep = 0;
		sleepAt = 0;
	} else {
		sleep = seconds;
		sleepAt = wpFZ.loopStartedAt + (seconds * 1000);
	}
	wpFZ.DebugWS(wpFZ.strDEBUG, "NeoPixel::SetSleep", "Off in " + String(sleep) + " sec");
	return wpFZ.JsonKeyValue("Sleep", String(sleep));
}
void moduleNeoPixel::InitRGB(bool rgb) {
	isRGB = rgb;
}
bool moduleNeoPixel::GetRGB() {
	return isRGB;
}
void moduleNeoPixel::SetRGB(bool rgb) {
	isRGB = rgb;
	wpEEPROM.WriteBoolToEEPROM("NeoPixelRGB", wpEEPROM.addrBitsSettingsModules1, wpEEPROM.bitsSettingsModules1, wpEEPROM.bitNeoPixelRGB, isRGB);
	if(isRGB) {
		strip->updateType(NEO_RGB + NEO_KHZ800);
	} else {
		strip->updateType(NEO_GRB + NEO_KHZ800);
	}
	strip->clear();
}
String moduleNeoPixel::SetOn() {
	targetWW = EEPROM.read(wpEEPROM.byteAnalogOutHandValue);
	targetCW = EEPROM.read(wpEEPROM.byteAnalogOut2HandValue);
	targetR = EEPROM.read(wpEEPROM.byteNeoPixelValueR);
	targetG = EEPROM.read(wpEEPROM.byteNeoPixelValueG);
	targetB = EEPROM.read(wpEEPROM.byteNeoPixelValueB);
	demoMode = false;
	modeCurrent = ModeBlender;
	staticIsSet = false;
	String returns = "{" +
		wpFZ.JsonKeyValue("R", String(targetR)) + "," +
		wpFZ.JsonKeyValue("G", String(targetG)) + "," +
		wpFZ.JsonKeyValue("B", String(targetB));
	if(wpModules.useModuleAnalogOut) {
		returns += "," + wpFZ.JsonKeyValue("WW", String(AnalogOutWW));
	}
	if(wpModules.useModuleAnalogOut2) {
		returns += "," + wpFZ.JsonKeyValue("CW", String(AnalogOutCW));
	}
	return returns += "}";
}
String moduleNeoPixel::SetOff() {
	targetWW = 0;
	targetCW = 0;
	targetR = 0;
	targetG = 0;
	targetB = 0;
	EEPROM.write(wpEEPROM.byteAnalogOutHandValue, targetWW);
	EEPROM.write(wpEEPROM.byteAnalogOut2HandValue, targetCW);
	EEPROM.write(wpEEPROM.byteNeoPixelValueR, targetR);
	EEPROM.write(wpEEPROM.byteNeoPixelValueG, targetG);
	EEPROM.write(wpEEPROM.byteNeoPixelValueB, targetB);
	EEPROM.commit();
	//targetBr = 0;
	demoMode = false;
	modeCurrent = ModeBlender;
	staticIsSet = false;
	String returns = "{" +
		wpFZ.JsonKeyValue("R", String(targetR)) + "," +
		wpFZ.JsonKeyValue("G", String(targetG)) + "," +
		wpFZ.JsonKeyValue("B", String(targetB));
	if(wpModules.useModuleAnalogOut) {
		returns += "," + wpFZ.JsonKeyValue("WW", String(AnalogOutWW));
	}
	if(wpModules.useModuleAnalogOut2) {
		returns += "," + wpFZ.JsonKeyValue("CW", String(AnalogOutCW));
	}
	return returns += "}";
}
String moduleNeoPixel::SetWW(uint ww) {
	//targetCW = wpAnalogOut2.handValue;
	if(ww + targetCW > 255) ww = 255 - targetCW;
	targetWW = ww;
	wpEEPROM.WriteByteToEEPROM("NeoPixelWW", wpEEPROM.byteAnalogOutHandValue, targetWW);
	calcDuration();
	demoMode = false;
	modeCurrent = ModeBlender;
	staticIsSet = false;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
String moduleNeoPixel::SetCW(uint cw) {
	//targetWW = wpAnalogOut.handValue;
	if(cw + targetWW > 255) cw = 255 - targetWW;
	targetCW = cw;
	wpEEPROM.WriteByteToEEPROM("NeoPixelCW", wpEEPROM.byteAnalogOut2HandValue, targetCW);
	calcDuration();
	demoMode = false;
	modeCurrent = ModeBlender;
	staticIsSet = false;
	return "{"
		+ wpFZ.JsonKeyValue("WW", String(targetWW)) + ","
		+ wpFZ.JsonKeyValue("CW", String(targetCW)) + "}";
}
void moduleNeoPixel::calcDuration() {
	uint8_t distWW = abs(AnalogOutWW - targetWW);
	uint8_t distCW = abs(AnalogOutCW - targetCW);
	uint dist = distWW >= distCW ? distWW : distCW;
	uint s = (int)(dist / 80.0);
	steps = s == 0 ? 1 : s;
}
String moduleNeoPixel::SetOffRunner(uint8_t setSteps) {
	steps = setSteps;
	demoMode = false;
	modeCurrent = ModeOffRunner;
	staticIsSet = true;
	wpFZ.DebugWS(wpFZ.strINFO, "NeoPixel::SetOff", "Color, '0'");
	return wpFZ.JsonKeyValue("Mode", GetModeName(modeCurrent));
}
void moduleNeoPixel::InitPixelCount(uint16_t pc) {
	pixelCount = pc;
}
uint16_t moduleNeoPixel::GetPixelCount() { return pixelCount; }
void moduleNeoPixel::SetPixelCount(uint16_t pc) {
	wpEEPROM.WriteWordToEEPROM("NeoPixelPC", wpEEPROM.byteNeoPixelPixelCount, pc);
	wpFZ.restartRequired = true;
}

String moduleNeoPixel::GetModeName(uint actualMode) {
	String returns;
	switch(actualMode) {
		case ModeStatic:
			returns = "ModeStatic";
			break;
		case ModeColorWipe:
			returns = "ModeColorWipe";
			break;
		case ModeTheaterChase:
			returns = "ModeTheaterChase";
			break;
		case ModeRainbow:
			returns = "ModeRainbow";
			break;
		case ModeWheelRainbow:
			returns = "ModeWheelRainbow";
			break;
		case ModeRainbowTv:
			returns = "ModeRainbowTv";
			break;
		case ModeTheaterChaseRainbow:
			returns = "ModeTheaterChaseRainbow";
			break;
		case ModeRunner:
			returns = "ModeRunner";
			break;
		case ModeDisco:
			returns = "ModeDisco";
			break;
		case ModeComplex:
			returns = "ModeComplex";
			break;
		case ModeOffRunner:
			returns = "ModeOffRunner";
			break;
		case ModeBlender:
			returns = "ModeBlender";
			break;
		default:
			returns = String(actualMode);
			break;
	}
	return returns;
}
String moduleNeoPixel::SetMode(uint8_t newMode) {
	modeCurrent = newMode;
	targetCW = 0;
	targetWW = 0;
	staticIsSet = false;
	return wpFZ.JsonKeyString("Mode", GetModeName(modeCurrent));
}
String moduleNeoPixel::ChangeUseWW() {
	useWW = !useWW;
	wpEEPROM.WriteBoolToEEPROM("NeoPixelWW", wpEEPROM.addrBitsSettingsModules1, wpEEPROM.bitsSettingsModules1, wpEEPROM.bitNeoPixelUseWW, useWW);
	return wpFZ.JsonKeyValue("UseWW", useWW ? "true" : "false");
}
String moduleNeoPixel::ChangeUseCW() {
	useCW = !useCW;
	wpEEPROM.WriteBoolToEEPROM("NeoPixelCW", wpEEPROM.addrBitsSettingsModules1, wpEEPROM.bitsSettingsModules1, wpEEPROM.bitNeoPixelUseCW, useCW);
	return wpFZ.JsonKeyValue("UseCW", useCW ? "true" : "false");
}
// String moduleNeoPixel::getStripStatus() {
// 	status = "{";
// 	for(uint i = 0; i < pixelCount; i++) {
// 		status.concat("\"p");
// 		status.concat(i);
// 		uint32_t c = strip->getPixelColor(i);
// 		uint8_t r = c >> 16;
// 		status.concat("\":{\"r\":");
// 		status.concat(r);
// 		uint8_t g = c >> 8;
// 		status.concat(",\"g\":");
// 		status.concat(g);
// 		uint8_t b = c;
// 		status.concat(",\"b\":");
// 		status.concat(b);
// 		status.concat("},");
// 	}
// 	status.concat("\"ww\":");
// 	status.concat(AnalogOutWW);
// 	status.concat(",\"cw\":");
// 	status.concat(AnalogOutCW);
// 	status.concat(",\"b\":");
// 	status.concat(strip->getBrightness());
// 	status.concat("}");
// 	return status;
// }
//###################################################################################
// private
//###################################################################################
void moduleNeoPixel::publishValue() {
	valueRLast = valueR;
	valueGLast = valueG;
	valueBLast = valueB;
	wpMqtt.mqttClient.publish(mqttTopicValueR.c_str(), String(valueR).c_str());
	wpMqtt.mqttClient.publish(mqttTopicValueG.c_str(), String(valueG).c_str());
	wpMqtt.mqttClient.publish(mqttTopicValueB.c_str(), String(valueB).c_str());
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug(mqttTopicValueR, String(valueR));
		mb->printPublishValueDebug(mqttTopicValueG, String(valueG));
		mb->printPublishValueDebug(mqttTopicValueB, String(valueB));
	}
	publishValueLast = wpFZ.loopStartedAt;
}

void moduleNeoPixel::calc() {	
	if(sleepAt > 0) {
		if(wpFZ.loopStartedAt > sleepAt) {
			SetOff();
			sleep = 0;
			sleepAt = 0;
		} else {
			sleep = (sleepAt - wpFZ.loopStartedAt) / 1000;
		}
	}
	if(demoMode) {
		if((wpFZ.loopStartedAt - patternPrevious) >= patternInterval) {  //  Check for expired time
			patternPrevious = wpFZ.loopStartedAt;
			if(++modeCurrent > 7)
				modeCurrent = 1;
		}
	}
	if(wpFZ.loopStartedAt - pixelPrevious >= pixelInterval) {        //  Check for expired time
		pixelPrevious = wpFZ.loopStartedAt;                            //  Run current frame
		switch (modeCurrent) {
			case ModeColorWipe:
				BlenderWWEffect();
				BlenderCWEffect();
				ColorWipeEffect(effectSpeed * 25); // Red
				break;
			case ModeTheaterChase:
				BlenderWWEffect();
				BlenderCWEffect();
				TheaterChaseEffect(effectSpeed * 25); // White
				break;
			case ModeRainbow:
				BlenderWWEffect();
				BlenderCWEffect();
				RainbowEffect(effectSpeed * 25); // Flowing rainbow cycle along the whole strip
				break;
			case ModeWheelRainbow:
				BlenderWWEffect();
				BlenderCWEffect();
				RainbowWheelEffect(effectSpeed * 25);
				break;
			case ModeRainbowTv:
				BlenderWWEffect();
				BlenderCWEffect();
				RainbowTvEffect(effectSpeed * 25);
				break;
			case ModeTheaterChaseRainbow:
				BlenderWWEffect();
				BlenderCWEffect();
				TheaterChaseRainbowEffect(effectSpeed * 25); // Rainbow-enhanced theaterChase variant
				break;
			case ModeRunner:
				BlenderWWEffect();
				BlenderCWEffect();
				RunnerEffect(effectSpeed * 25); // Runner Red
				break;
			case ModeDisco:
				BlenderWWEffect();
				BlenderCWEffect();
				DiscoEffect(effectSpeed * 25); // Disco
				break;
			case ModeOffRunner:
				BlenderWWEffect();
				BlenderCWEffect();
				OffRunnerEffect(25); // ModeOffRunner
				break;
			case ModeComplex:
				BlenderWWEffect();
				BlenderCWEffect();
				// nothing todo, but save LED state
				break;
			case ModeBlender:
				BlenderEffect(); // ModeOn
				break;
			default:
				if(!staticIsSet) {
					StaticEffect();
					staticIsSet = true;
					wpFZ.DebugWS(wpFZ.strDEBUG, "NeoPixel::calc", "Static is set");
				}
				break;
		}
	}
	analogWrite(PinWW, AnalogOutWW);
	analogWrite(PinCW, AnalogOutCW);
	maxPercent = GetMaxPercent();
}
void moduleNeoPixel::BlenderEffect() {
	pixelInterval = 25;                   //  Update delay time
	bool bre = BlenderREffect();
	bool bge = BlenderGEffect();
	bool bbe = BlenderBEffect();
	bool bwwe = BlenderWWEffect();
	bool bcwe = BlenderCWEffect();
	strip->fill(strip->Color(valueR, valueG, valueB));
	strip->show();
	if(bre && bge && bbe && bwwe && bcwe) {
		modeCurrent = ModeStatic;
	}
}
bool moduleNeoPixel::BlenderWWEffect() {
	if(AnalogOutWW != targetWW) {
		if(AnalogOutWW <= targetWW) {
			if(AnalogOutWW + steps <= targetWW) {
				AnalogOutWW = AnalogOutWW + steps;
			} else {
				AnalogOutWW = targetWW;
			}
			if(AnalogOutWW >= targetWW) {
				AnalogOutWW = targetWW;
			}
		} else {
			if(AnalogOutWW - steps >= targetWW) {
				AnalogOutWW = AnalogOutWW - steps;
			} else {
				AnalogOutWW = targetWW;
			}
			if(AnalogOutWW <= targetWW) {
				AnalogOutWW = targetWW;
			}
		}
	}
	return AnalogOutWW == targetWW;
}
bool moduleNeoPixel::BlenderCWEffect() {
	if(AnalogOutCW != targetCW) {
		if(AnalogOutCW <= targetCW) {
			if(AnalogOutCW + steps <= targetCW) {
				AnalogOutCW = AnalogOutCW + steps;
			} else {
				AnalogOutCW = targetCW;
			}
			if(AnalogOutCW >= targetCW) {
				AnalogOutCW = targetCW;
			}
		} else {
			if(AnalogOutCW - steps >= targetCW) {
				AnalogOutCW = AnalogOutCW - steps;
			} else {
				AnalogOutCW = targetCW;
			}
			if(AnalogOutCW <= targetCW) {
				AnalogOutCW = targetCW;
			}
		}
	}
	return AnalogOutCW == targetCW;
}
bool moduleNeoPixel::BlenderREffect() {
	if(valueR != targetR) {
		if(valueR <= targetR) {
			if(valueR + steps <= targetR) {
				valueR += steps;
			} else {
				valueR = targetR;
			}
			if(valueR >= targetR) {
				valueR = targetR;
			}
		} else {
			if(valueR - steps >= targetR) {
				valueR -= steps;
			} else {
				valueR = targetR;
			}
			if(valueR <= targetR) {
				valueR = targetR;
			}
		}
	}
	return valueR == targetR;
}
bool moduleNeoPixel::BlenderGEffect() {
	if(valueG != targetG) {
		if(valueG <= targetG) {
			if(valueG + steps <= targetG) {
				valueG += steps;
			} else {
				valueG = targetG;
			}
			if(valueG >= targetG) {
				valueG = targetG;
			}
		} else {
			if(valueG - steps >= targetG) {
				valueG -= steps;
			} else {
				valueG = targetG;
			}
			if(valueG <= targetG) {
				valueG = targetG;
			}
		}
	}
	return valueG == targetG;
}
bool moduleNeoPixel::BlenderBEffect() {
	if(valueB != targetB) {
		if(valueB <= targetB) {
			if(valueB + steps <= targetB) {
				valueB += steps;
			} else {
				valueB = targetB;
			}
			if(valueB >= targetB) {
				valueB = targetB;
			}
		} else {
			if(valueB - steps >= targetB) {
				valueB -= steps;
			} else {
				valueB = targetB;
			}
			if(valueB <= targetB) {
				valueB = targetB;
			}
		}
	}
	return valueB == targetB;
}
// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void moduleNeoPixel::ColorWipeEffect(uint wait) {
	pixelInterval = wait;
	uint32_t color = strip->Color(valueR, valueG, valueB);
	static uint16_t current_pixel = 0;
	static bool ison = false;
	if(ison) {
		strip->setPixelColor(current_pixel++, strip->Color(0, 0, 0));
	} else {
		strip->setPixelColor(current_pixel++, color); //  Set pixel's color (in RAM)
	}
	strip->show();                                //  Update strip to match
	if(current_pixel >= pixelCount) {            //  Loop the pattern from the first LED
		current_pixel = 0;
		ison = !ison;
	}
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void moduleNeoPixel::TheaterChaseEffect(uint wait) {
	pixelInterval = wait;
	uint32_t color = strip->Color(valueR, valueG, valueB);
	static uint32_t loop_count = 0;
	static uint16_t current_pixel = 0;

	strip->clear();

	for(int c = current_pixel; c < pixelCount; c += 3) {
		strip->setPixelColor(c, color);
	}
	strip->show();

	current_pixel++;
	if (current_pixel >= 3) {
		current_pixel = 0;
		loop_count++;
	}

	if (loop_count >= 10) {
		current_pixel = 0;
		loop_count = 0;
	}
}

void moduleNeoPixel::RainbowEffect(uint wait) {
	pixelInterval = wait;
	uint32_t lastval = Wheel(pixelCycle);
	strip->fill(lastval);
	if(useBorder) {
		if(lastBorderSend == 0 || lastBorderSend + 2000 < wpFZ.loopStartedAt) {
			setBorder(lastval);
		}
	}
	strip->show();                          //  Update strip to match
	pixelCycle++;                           //  Advance current cycle
	if(pixelCycle >= 256)
		pixelCycle = 0;                     //  Loop the cycle back to the begining

}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void moduleNeoPixel::RainbowWheelEffect(uint wait) {
	pixelInterval = wait;
	uint32_t lastval = 0;
	for(uint16_t i = 0; i < pixelCount; i++) {
		lastval = Wheel((i + pixelCycle) % 255);
		strip->setPixelColor(i, lastval); //  Update delay time  
	}
	if(useBorder) {
		if(lastBorderSend == 0 || lastBorderSend + 2000 < wpFZ.loopStartedAt) {
			setBorder(lastval);
		}
	}
	strip->show();                          //  Update strip to match
	pixelCycle++;                           //  Advance current cycle
	if(pixelCycle >= 256)
		pixelCycle = 0;                     //  Loop the cycle back to the begining
}
// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void moduleNeoPixel::RainbowTvEffect(uint wait) {
	pixelInterval = wait;
	for(uint16_t i = 0; i < pixelStartForTv; i++) {
		strip->setPixelColor(i, strip->Color(0, 0, 0)); //  Update delay time  
	}
	for(uint16_t i = pixelStartForTv; i < pixelCount; i++) {
		strip->setPixelColor(i, Wheel((i + pixelCycle) & 255)); //  Update delay time  
	}
	strip->show();                          //  Update strip to match
	pixelCycle++;                           //  Advance current cycle
	if(pixelCycle >= 256)
		pixelCycle = 0;                     //  Loop the cycle back to the begining
}

//Theatre-style crawling lights with rainbow effect
void moduleNeoPixel::TheaterChaseRainbowEffect(uint wait) {
	pixelInterval = wait;
	for(int i=0; i < pixelCount; i+=3) {
		strip->setPixelColor(i + pixelQueue, Wheel((i + pixelCycle) & 255)); //  Update delay time
	}
	strip->show();
	for(int i=0; i < pixelCount; i+=3) {
		strip->setPixelColor(i + pixelQueue, strip->Color(0, 0, 0)); //  Update delay time  
	}
	pixelQueue++;                           //  Advance current queue  
	pixelCycle++;                           //  Advance current cycle
	if(pixelQueue >= 3)
		pixelQueue = 0;                     //  Loop
	if(pixelCycle >= 256)
		pixelCycle = 0;                     //  Loop
}

void moduleNeoPixel::RunnerEffect(uint wait) {
	pixelInterval = wait;
	uint32_t color = strip->Color(valueR, valueG, valueB);
	static uint16_t current_pixel = 0;
	strip->clear();
	strip->setPixelColor(current_pixel++, color); //  Set pixel's color (in RAM)
	strip->show();                                //  Update strip to match
	if(current_pixel >= pixelCount) {            //  Loop the pattern from the first LED
		current_pixel = 0;
	}
}
void moduleNeoPixel::DiscoEffect(uint wait) {
	pixelInterval = wait;
	int pixel;
	byte r, g, b;
	strip->clear();
	r = random(0, 255);
	g = random(0, 255);
	b = random(0, 255);
	strip->fill(strip->Color(r, g, b));
	uint pCount = floor(pixelCount * 100 / 25);
	for(uint p = 0; p < pCount; p++) {
		pixel = random(0, pixelCount);
		r = random(0, 255);
		g = random(0, 255);
		b = random(0, 255);
		strip->setPixelColor(pixel, strip->Color(r, g, b));
	}
	strip->show();
}
void moduleNeoPixel::OffRunnerEffect(uint wait) {
	pixelInterval = wait;
	uint32_t color = strip->Color(0, 0, 0);
	static uint16_t current_pixel = pixelCount;
	strip->setPixelColor(current_pixel--, color); //  Set pixel's color (in RAM)
	strip->show();                                //  Update strip to match
	if(wpModules.useModuleAnalogOut) {
		if(AnalogOutWW >= steps) {
			AnalogOutWW = AnalogOutWW - steps;
		} else {
			AnalogOutWW = 0;
		}
	}
	if(wpModules.useModuleAnalogOut2) {
		if(AnalogOutCW >= steps) {
			AnalogOutCW = AnalogOutCW - steps;
		} else {
			AnalogOutCW = 0;
		}
	}
	if(
		(wpModules.useModuleAnalogOut && AnalogOutWW <= 0) &&
		(wpModules.useModuleAnalogOut2 && AnalogOutCW <= 0)) {            //  Loop the pattern from the first LED
		staticIsSet = true;
		modeCurrent = ModeStatic;
	}
}

void moduleNeoPixel::StaticEffect() {
	uint32_t color = strip->Color(valueR, valueG, valueB);
	demoMode = false;
	modeCurrent = ModeStatic;
	strip->fill(color);
	strip->show();
	if(useBorder) {
		if(lastBorderSend == 0 || lastBorderSend + 5000 < wpFZ.loopStartedAt) {
			setBorder(color);
		}
	}
}
String moduleNeoPixel::ComplexEffect(uint pixel, byte r, byte g, byte b) {
	if(pixel > pixelCount) pixel = pixelCount;
	return ComplexEffect(pixel, strip->Color(r, g, b));
}
String moduleNeoPixel::ComplexEffect(uint pixel, uint32_t color) {
	demoMode = false;
	modeCurrent = ModeComplex;
	strip->setPixelColor(pixel, color);
	strip->show();
	return F("\"effect\":{") + 
		wpFZ.JsonKeyValue("Pixel", String(pixel)) + F(",") +
		wpFZ.JsonKeyValue("R", String((uint8_t)(color >> 16))) + F(",") +
		wpFZ.JsonKeyValue("G", String((uint8_t)(color >>  8))) + F(",") +
		wpFZ.JsonKeyValue("B", String((uint8_t)(color >>  0))) + F("}");
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t moduleNeoPixel::Wheel(byte WheelPos) {
	WheelPos = 255 - WheelPos;
	if(WheelPos < 85) {
		return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if(WheelPos < 170) {
		WheelPos -= 85;
		return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
void moduleNeoPixel::setBorder(uint32_t c) {
	String target = "http://172.17.80.163/color/0?"
		"red=" + String((uint8_t)(c >> 16)) + "&"
		"green=" + String((uint8_t)(c >>  8)) + "&"
		"blue=" + String((uint8_t)(c >>  0));
	wpFZ.sendRawRest(target);
	lastBorderSend = wpFZ.loopStartedAt;
}
uint8_t moduleNeoPixel::GetMaxPercent() {
	uint8_t returns = 0;
	returns = valueR > returns ? valueR : returns;
	returns = valueG > returns ? valueG : returns;
	returns = valueB > returns ? valueB : returns;
	if(wpModules.useModuleAnalogOut)
		returns = AnalogOutWW > returns ? AnalogOutWW : returns;
	if(wpModules.useModuleAnalogOut2)
		returns = AnalogOutCW > returns ? AnalogOutCW : returns;
	return returns;
}
//###################################################################################
// section to copy
//###################################################################################
uint16_t moduleNeoPixel::getVersion() {
	String SVN = "$Rev: 269 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleNeoPixel::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin"), String(wpFZ.Pins[Pin])) + F(",") +
		wpFZ.JsonKeyValue(F("ValueR"), String(GetValueR())) + F(",") +
		wpFZ.JsonKeyValue(F("ValueG"), String(GetValueG())) + F(",") +
		wpFZ.JsonKeyValue(F("ValueB"), String(GetValueB())) + F(",") +
		wpFZ.JsonKeyValue(F("useWW"), useWW ? "true" : "false") + F(",") +
		wpFZ.JsonKeyValue(F("useCW"), useCW ? "true" : "false") + F(",");
	if(useWW) {
		json += wpFZ.JsonKeyValue(F("ValueWW"), String(GetValueB())) + F(",");
	}
	if(useCW) {
		json += wpFZ.JsonKeyValue(F("ValueCW"), String(GetValueB())) + F(",");
	}
	json +=
		wpFZ.JsonKeyValue(F("PixelCount"), String(GetPixelCount())) + F(",") +
		wpFZ.JsonKeyValue(F("isRGB"), GetRGB() ? "true" : "false") +
		F("}");
	return json;
}

void moduleNeoPixel::changeDebug() {
	mb->changeDebug();
}
bool moduleNeoPixel::Debug() {
	return mb->debug;
}
bool moduleNeoPixel::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
