//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 05.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 131                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperRest.cpp 131 2024-06-05 03:01:06Z                  $ #
//#                                                                                 #
//###################################################################################
#include <moduleOled096.h>

moduleOled096 wpOled096;

// uses PIN D1 (SCL) & D2 (SDA) for I2C Bus
moduleOled096::moduleOled096() {}
void moduleOled096::init() {
	// settings
	// commands
	mqttTopicError = wpFZ.DeviceName + "/ERROR/Oled096";
	mqttTopicDebug = wpFZ.DeviceName + "/settings/Debug/Oled096";
	// FreakaZone - Z
	mqttTopicZX = wpFZ.DeviceName + "/ZX";
	mqttTopicZY = wpFZ.DeviceName + "/ZY";
	ZX = 72;
	ZY = 19;
	// FreakaZone
	mqttTopicFX = wpFZ.DeviceName + "/FX";
	mqttTopicFY = wpFZ.DeviceName + "/FY";
	FX = 11;
	FY = 20;

	// Temperatur
	mqttTopicTX = wpFZ.DeviceName + "/TX";
	mqttTopicTY = wpFZ.DeviceName + "/TY";
	TX = 52;
	TY = 13;
	// Temperatur Text
	mqttTopicTTX = wpFZ.DeviceName + "/TTX";
	mqttTopicTTY = wpFZ.DeviceName + "/TTY";
	TTX = 0;
	TTY = 0;

	// Humidity
	mqttTopicHX = wpFZ.DeviceName + "/HX";
	mqttTopicHY = wpFZ.DeviceName + "/HY";
	HX = 38;
	HY = 45;
	// Humidity Text
	mqttTopicTHX = wpFZ.DeviceName + "/THX";
	mqttTopicTHY = wpFZ.DeviceName + "/THY";
	THX = 0;
	THY = 31;

	// Moisture
	mqttTopicMX = wpFZ.DeviceName + "/MX";
	mqttTopicMY = wpFZ.DeviceName + "/MY";
	MX = 70;
	MY = 25;
	// Moisture Text
	mqttTopicTMX = wpFZ.DeviceName + "/TMX";
	mqttTopicTMY = wpFZ.DeviceName + "/TMY";
	TMX = 0;
	TMY = 11;

	// Pia
	mqttTopicPX = wpFZ.DeviceName + "/PX";
	mqttTopicPY = wpFZ.DeviceName + "/PY";
	PX = 50;
	PY = 25;
	// Pia Text
	mqttTopicTPX = wpFZ.DeviceName + "/TPX";
	mqttTopicTPY = wpFZ.DeviceName + "/TPY";
	TPX = 0;
	TPY = 11;
	
	u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
	u8g2->begin();
	u8g2_prepare();
	displayCounter = 0;
	displayChecker = 0;
}

//###################################################################################
// public
//###################################################################################
void moduleOled096::cycle() {
	if(displayCounter == 0) {
		FreakaZone();
	}
	if(displayCounter == 10) {
		temp_hum();
	}
	if(displayCounter == 60) {
		moisture();
	}
	if(displayCounter == 110) {
		pia();
	}
	if(++displayCounter > 160) {
		displayCounter = 0;
	}
	publishValues();
}

uint16_t moduleOled096::getVersion() {
	String SVN = "$Rev: 131 $";
	uint16_t v = wpFZ.getBuild(SVN);
	uint16_t vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleOled096::changeDebug() {
	Debug = !Debug;
	bitWrite(wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugOled096, Debug);
	EEPROM.write(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1);
	EEPROM.commit();
	wpFZ.SendWSDebug("DebugOled096", Debug);
	wpFZ.blink();
}

void moduleOled096::publishSettings() {
	publishSettings(false);
}
void moduleOled096::publishSettings(bool force) {
	if(force) {
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
	}
}

void moduleOled096::publishValues() {
	publishValues(false);
}
void moduleOled096::publishValues(bool force) {
	if(force) {
		publishCountError = wpFZ.publishQoS;
		publishCountDebug = wpFZ.publishQoS;
	}
	if(errorLast != error || ++publishCountError > wpFZ.publishQoS) {
		errorLast = error;
		wpMqtt.mqttClient.publish(mqttTopicError.c_str(), String(error).c_str());
		publishCountError = 0;
	}
	if(DebugLast != Debug || ++publishCountDebug > wpFZ.publishQoS) {
		DebugLast = Debug;
		wpMqtt.mqttClient.publish(mqttTopicDebug.c_str(), String(Debug).c_str());
		publishCountDebug = 0;
	}
}

void moduleOled096::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicDebug.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicZX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicZY.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicFX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicFY.c_str());

	wpMqtt.mqttClient.subscribe(mqttTopicTX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTY.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTTX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTTY.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicHX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicHY.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTHX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTHY.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicMX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicMY.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTMX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTMY.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicPX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicPY.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTPX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicTPY.c_str());
}
void moduleOled096::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicDebug.c_str()) == 0) {
		bool readDebug = msg.toInt();
		if(Debug != readDebug) {
			Debug = readDebug;
			bitWrite(wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugOled096, Debug);
			EEPROM.write(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1);
			EEPROM.commit();
			wpFZ.SendWSDebug("DebugOled096", Debug);
			wpFZ.DebugcheckSubscribes(mqttTopicDebug, String(Debug));
		}
	}
	if(strcmp(topic, mqttTopicZX.c_str()) == 0) ZX = msg.toInt();
	if(strcmp(topic, mqttTopicZY.c_str()) == 0) ZY = msg.toInt();
	if(strcmp(topic, mqttTopicFX.c_str()) == 0) FX = msg.toInt();
	if(strcmp(topic, mqttTopicFY.c_str()) == 0) FY = msg.toInt();
	
	if(strcmp(topic, mqttTopicTX.c_str()) == 0) TX = msg.toInt();
	if(strcmp(topic, mqttTopicTY.c_str()) == 0) TY = msg.toInt();
	if(strcmp(topic, mqttTopicTTX.c_str()) == 0) TTX = msg.toInt();
	if(strcmp(topic, mqttTopicTTY.c_str()) == 0) TTY = msg.toInt();

	if(strcmp(topic, mqttTopicHX.c_str()) == 0) HX = msg.toInt();
	if(strcmp(topic, mqttTopicHY.c_str()) == 0) HY = msg.toInt();
	if(strcmp(topic, mqttTopicTHX.c_str()) == 0) THX = msg.toInt();
	if(strcmp(topic, mqttTopicTHY.c_str()) == 0) THY = msg.toInt();

	if(strcmp(topic, mqttTopicMX.c_str()) == 0) MX = msg.toInt();
	if(strcmp(topic, mqttTopicMY.c_str()) == 0) MY = msg.toInt();
	if(strcmp(topic, mqttTopicTMX.c_str()) == 0) TMX = msg.toInt();
	if(strcmp(topic, mqttTopicTMY.c_str()) == 0) TMY = msg.toInt();

	if(strcmp(topic, mqttTopicPX.c_str()) == 0) PX = msg.toInt();
	if(strcmp(topic, mqttTopicPY.c_str()) == 0) PY = msg.toInt();
	if(strcmp(topic, mqttTopicTPX.c_str()) == 0) TPX = msg.toInt();
	if(strcmp(topic, mqttTopicTPY.c_str()) == 0) TPY = msg.toInt();
}

//###################################################################################
// private
//###################################################################################

void moduleOled096::u8g2_prepare() {
	u8g2->setFontRefHeightExtendedText(); 
	u8g2->setDrawColor(1);
	u8g2->setFontPosTop();
	u8g2->setFontDirection(0);
}
void moduleOled096::FreakaZone() {
	u8g2->clearBuffer();
	u8g2->setFont(u8g2_font_helvB14_tf);
	u8g2->drawStr(ZX, ZY, "Z");
	u8g2->drawStr(FX, FY, "FreakaZone");
	u8g2->sendBuffer();
}
void moduleOled096::temp_hum() {
	u8g2->clearBuffer();
	u8g2->setFont(u8g2_font_6x10_tf);
	u8g2->drawStr(TTX, TTY, "Temperature:");
	u8g2->setFont(u8g2_font_helvB14_tf);
	u8g2->drawStr(TX, TY, (String(wpDHT.temperature / 100.0) + " \xb0" + "C").c_str());
	u8g2->setFont(u8g2_font_6x10_tf);
	u8g2->drawStr(THX, THY, "Humidity:");
	u8g2->setFont(u8g2_font_helvB14_tf);
	u8g2->drawStr(HX, HY, (String(wpDHT.humidity / 100.0) + " %rF").c_str());
	u8g2->sendBuffer();
}
void moduleOled096::moisture() {
	u8g2->clearBuffer();
	u8g2->setFont(u8g2_font_6x10_tf);
	u8g2->drawStr(TMX, TMY, "Moisture:");
	u8g2->setFont(u8g2_font_helvB14_tf);
	u8g2->drawStr(MX, MY, (String(wpMoisture.moisture) + " %").c_str());
	u8g2->sendBuffer();
}
void moduleOled096::pia() {
	u8g2->clearBuffer();
	u8g2->setFont(u8g2_font_6x10_tf);
	u8g2->drawStr(TPX, TPY, "Hallo");
	u8g2->setFont(u8g2_font_helvB14_tf);
	u8g2->drawStr(PX, PY, "Pia");
	u8g2->sendBuffer();
}
