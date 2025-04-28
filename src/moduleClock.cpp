//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 02.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 258                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleClock.cpp 258 2025-04-28 13:34:51Z                 $ #
//#                                                                                 #
//###################################################################################
#include <moduleClock.h>

moduleClock wpClock;

moduleClock::moduleClock() {
	// section to config and copy
	ModuleName = "Clock";
	mb = new moduleBase(ModuleName);
}
void moduleClock::init() {
	// section for define
	Pin1 = D1;
	Pin2 = D2;
	Pin3 = D5;
	Pin4 = D6;
	// D7 = Neopixel
	Motor = new Stepper(spr, Pin1, Pin2, Pin3, Pin4);
	Motor->setSpeed(rpm);

	// values
	mqttTopicSpr = wpFZ.DeviceName + "/" + ModuleName + "/Spr";
	mqttTopicRpm = wpFZ.DeviceName + "/" + ModuleName + "/Rpm";
	// settings
	mqttTopicSetSpr = wpFZ.DeviceName + "/settings/" + ModuleName + "/Spr";
	mqttTopicSetRpm = wpFZ.DeviceName + "/settings/" + ModuleName + "/Rpm";

	// section to copy
	mb->initDebug(wpEEPROM.addrBitsDebugModules2, wpEEPROM.bitsDebugModules2, wpEEPROM.bitDebugClock);
	mb->initCalcCycle(wpEEPROM.byteCalcCycleClock);

	wpNeoPixel.InitValueR(0);
	wpNeoPixel.InitValueG(0);
	wpNeoPixel.InitValueB(0);
}

//###################################################################################
// public
//###################################################################################
void moduleClock::cycle() {
	if(wpFZ.calcValues && wpFZ.loopStartedAt > mb->calcLast + mb->calcCycle) {
		calc();
		mb->calcLast = wpFZ.loopStartedAt;
	}
	publishValues();
}

void moduleClock::publishSettings() {
	publishSettings(false);
}
void moduleClock::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicSetSpr.c_str(), String(spr).c_str());
	wpMqtt.mqttClient.publish(mqttTopicSetRpm.c_str(), String(rpm).c_str());
	mb->publishSettings(force);
}

void moduleClock::publishValues() {
	publishValues(false);
}
void moduleClock::publishValues(bool force) {
	if(force) {
		publishSprLast = 0;
		publishRpmLast = 0;
	}
	if(sprLast != spr || wpFZ.CheckQoS(publishSprLast)) {
		wpMqtt.mqttClient.publish(mqttTopicSpr.c_str(), String(spr).c_str());
		sprLast = spr;
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("spr", String(spr));
		}
		publishSprLast = wpFZ.loopStartedAt;
	}
	if(rpmLast != rpm || wpFZ.CheckQoS(publishRpmLast)) {
		wpMqtt.mqttClient.publish(mqttTopicRpm.c_str(), String(rpm).c_str());
		rpmLast = rpm;
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug("rpm", String(rpm));
		}
		publishRpmLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleClock::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicSetSpr.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetRpm.c_str());
	mb->setSubscribes();
}

void moduleClock::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicSetSpr.c_str()) == 0) {
		uint16 readSpr = msg.toInt();
		SetSpr(readSpr);
	}
	if(strcmp(topic, mqttTopicSetRpm.c_str()) == 0) {
		uint16 readRpm = msg.toInt();
		SetRpm(readRpm);
	}
	mb->checkSubscribes(topic, msg);
}

//###################################################################################
// private
//###################################################################################
void moduleClock::calc() {
	//uint16 millis = wpFZ.loopStartedAt  % 1000;
	if(!simulateTime) {
		wpFZ.getTime(hour, minute, second);
	}
	if(secondLast != second) {
		secondLast = second;
		if(hour >= 12) hour -= 12;
		double him = (hour * 60.0) + minute; // minute des tages
		double gm = 12.0 * 60.0;
		double pg = 48.0;
		double pixel = him * pg / gm;
		short p = round(pixel);
		
		wpNeoPixel.setClock(60 + p, minute, second);
		if(mb->debug) {
			wpFZ.DebugWS(wpFZ.strDEBUG, "Clock::calc", "set Minute: " + String(minute));
			wpFZ.DebugWS(wpFZ.strDEBUG, "Clock::calc", "set Hour: " + String(hour) +
				", h in m: " + String(him) +
				", set Pixel: " + String(pixel) +
				", round Pixel: " + String(p));
		}
	}
	

	// int read = analogRead(Pin);
	// int minMax, avg, correct;
	// if(!isnan(read)) {
	// 	minMax = read;
	// 	if(minMax > 1023) minMax = 1023;
	// 	if(minMax < 0) minMax = 0;
	// 	avg = minMax;
	// 	if(mb->useAvg) {
	// 		avg = calcAvg(avg);
	// 	}
	// 	correct = avg;
	// 	ldr = (1023 - correct) + correction;
	// 	mb->error = false;
	// 	if(mb->debug) {
	// 		String logmessage = "LDR: " + String(ldr) + " ("
	// 			"Read: " + String(read) + ", "
	// 			"MinMax: " + String(minMax) + ", "
	// 			"Avg: " + String(avg) + ")";;
	// 		wpFZ.DebugWS(wpFZ.strDEBUG, "calcLDR", logmessage);
	// 	}
	// } else {
	// 	mb->error = true;
	// 	String logmessage = "Sensor Failure";
	// 	wpFZ.DebugWS(wpFZ.strERRROR, "calcLDR", logmessage);
	// }
	if(steps > 0) {
		Motor->step(steps);
		steps = 0;
	}
}
uint16 moduleClock::GetSpr() {
	return spr;
}
void moduleClock::SetSpr(uint16 StepsPerRound) {
	if(spr != StepsPerRound) {
		spr = StepsPerRound;
		//EEPROM.put(wpEEPROM.byteClockSpr, spr);
		//EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.DebugcheckSubscribes(mqttTopicSetSpr, String(spr));
	}
}
uint16 moduleClock::GetRpm() {
	return rpm;
}
void moduleClock::SetRpm(uint16 RoundsPerMinute) {
	if(rpm != RoundsPerMinute) {
		rpm = RoundsPerMinute;
		//EEPROM.put(wpEEPROM.byteClockRpm, rpm);
		//EEPROM.commit();
		wpFZ.restartRequired = true;
		wpFZ.DebugcheckSubscribes(mqttTopicSetRpm, String(rpm));
	}
}

void moduleClock::SetSteps(short StepsToRun) {
	steps = StepsToRun;
}

String moduleClock::GetColorH() {
	return F("{") + 
	wpFZ.JsonKeyValue("R", String(ColorHR)) + F(",") +
	wpFZ.JsonKeyValue("G", String(ColorHG)) + F(",") +
	wpFZ.JsonKeyValue("B", String(ColorHB)) + F("}");
}
String moduleClock::GetColorM() {
	return F("{") + 
	wpFZ.JsonKeyValue("R", String(ColorMR)) + F(",") +
	wpFZ.JsonKeyValue("G", String(ColorMG)) + F(",") +
	wpFZ.JsonKeyValue("B", String(ColorMB)) + F("}");
}
String moduleClock::GetColorS() {
	return F("{") + 
	wpFZ.JsonKeyValue("R", String(ColorSR)) + F(",") +
	wpFZ.JsonKeyValue("G", String(ColorSG)) + F(",") +
	wpFZ.JsonKeyValue("B", String(ColorSB)) + F("}");
}
String moduleClock::GetColorQ() {
	return F("{") + 
	wpFZ.JsonKeyValue("R", String(ColorQR)) + F(",") +
	wpFZ.JsonKeyValue("G", String(ColorQG)) + F(",") +
	wpFZ.JsonKeyValue("B", String(ColorQB)) + F("}");
}
String moduleClock::GetColor5() {
	return F("{") + 
	wpFZ.JsonKeyValue("R", String(Color5R)) + F(",") +
	wpFZ.JsonKeyValue("G", String(Color5G)) + F(",") +
	wpFZ.JsonKeyValue("B", String(Color5B)) + F("}");
}

void moduleClock::SetColorH(uint8 r, uint8 g, uint8 b) {
	ColorHR = r;
	wpEEPROM.WriteByteToEEPROM("SetColorHR", wpEEPROM.byteClockColorHR, r, false);
	ColorHG = g;
	wpEEPROM.WriteByteToEEPROM("SetColorHG", wpEEPROM.byteClockColorHG, g, false);
	ColorHB = b;
	wpEEPROM.WriteByteToEEPROM("SetColorHB", wpEEPROM.byteClockColorHB, b);
	wpFZ.DebugWS(wpFZ.strINFO, "SetColorH", "New Color, save to EEPROM: " + GetColorH());
}
void moduleClock::SetColorM(uint8 r, uint8 g, uint8 b) {
	ColorMR = r;
	wpEEPROM.WriteByteToEEPROM("SetColorMR", wpEEPROM.byteClockColorMR, r, false);
	ColorMG = g;
	wpEEPROM.WriteByteToEEPROM("SetColorMG", wpEEPROM.byteClockColorMG, g, false);
	ColorMB = b;
	wpEEPROM.WriteByteToEEPROM("SetColorMB", wpEEPROM.byteClockColorMB, b);
	wpFZ.DebugWS(wpFZ.strINFO, "SetColorM", "New Color, save to EEPROM: " + GetColorM());
}
void moduleClock::SetColorS(uint8 r, uint8 g, uint8 b) {
	ColorSR = r;
	wpEEPROM.WriteByteToEEPROM("SetColorSR", wpEEPROM.byteClockColorSR, r, false);
	ColorSG = g;
	wpEEPROM.WriteByteToEEPROM("SetColorSG", wpEEPROM.byteClockColorSG, g, false);
	ColorSB = b;
	wpEEPROM.WriteByteToEEPROM("SetColorSB", wpEEPROM.byteClockColorSB, b);
	wpFZ.DebugWS(wpFZ.strINFO, "SetColorS", "New Color, save to EEPROM: " + GetColorS());
}
void moduleClock::SetColorQ(uint8 r, uint8 g, uint8 b) {
	ColorQR = r;
	wpEEPROM.WriteByteToEEPROM("SetColorQR", wpEEPROM.byteClockColorQR, r, false);
	ColorQG = g;
	wpEEPROM.WriteByteToEEPROM("SetColorQG", wpEEPROM.byteClockColorQG, g, false);
	ColorQB = b;
	wpEEPROM.WriteByteToEEPROM("SetColorQB", wpEEPROM.byteClockColorQB, b);
	wpFZ.DebugWS(wpFZ.strINFO, "SetColorQ", "New Color, save to EEPROM: " + GetColorQ());
}
void moduleClock::SetColor5(uint8 r, uint8 g, uint8 b) {
	Color5R = r;
	wpEEPROM.WriteByteToEEPROM("SetColor5R", wpEEPROM.byteClockColor5R, r, false);
	Color5G = g;
	wpEEPROM.WriteByteToEEPROM("SetColor5G", wpEEPROM.byteClockColor5G, g, false);
	Color5B = b;
	wpEEPROM.WriteByteToEEPROM("SetColor5B", wpEEPROM.byteClockColor5B, b);
	wpFZ.DebugWS(wpFZ.strINFO, "SetColor5", "New Color, save to EEPROM: " + GetColor5());
}

void moduleClock::SimulateTime() {
	simulateTime = false;
}
void moduleClock::SimulateTime(short h, short m, short s) {
	simulateTime = true;
	hour = h;
	minute = m;
	second = s;
}
//###################################################################################
// section to copy
//###################################################################################
uint16 moduleClock::getVersion() {
	String SVN = "$Rev: 258 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

String moduleClock::GetJsonSettings() {
	String json = F("\"") + ModuleName + F("\":{") +
		wpFZ.JsonKeyString(F("Pin1"), String(wpFZ.Pins[Pin1])) + F(",") +
		wpFZ.JsonKeyString(F("Pin2"), String(wpFZ.Pins[Pin2])) + F(",") +
		wpFZ.JsonKeyString(F("Pin3"), String(wpFZ.Pins[Pin3])) + F(",") +
		wpFZ.JsonKeyString(F("Pin4"), String(wpFZ.Pins[Pin4])) + F(",") +
		wpFZ.JsonKeyValue(F("Hour"), GetColorH()) + F(",") +
		wpFZ.JsonKeyValue(F("Minute"), GetColorM()) + F(",") +
		wpFZ.JsonKeyValue(F("Second"), GetColorS()) + F(",") +
		wpFZ.JsonKeyValue(F("Quarter"), GetColorQ()) + F(",") +
		wpFZ.JsonKeyValue(F("Five"), GetColor5()) +
		F("}");
	return json;
}

void moduleClock::changeDebug() {
	mb->changeDebug();
}
bool moduleClock::Debug() {
	return mb->debug;
}
bool moduleClock::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
uint32 moduleClock::CalcCycle(){
	return mb->calcCycle;
}
uint32 moduleClock::CalcCycle(uint32 calcCycle){
	mb->calcCycle = calcCycle;
	return 0;
}
