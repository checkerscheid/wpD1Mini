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

	mqttTopicX = wpFZ.DeviceName + "/X";
	mqttTopicY = wpFZ.DeviceName + "/Y";
	X = 72;
	Y = 19;
	mqttTopicFX = wpFZ.DeviceName + "/FX";
	mqttTopicFY = wpFZ.DeviceName + "/FY";
	FX = 11;
	FY = 20;

	mqttTopicTX = wpFZ.DeviceName + "/TX";
	mqttTopicTY = wpFZ.DeviceName + "/TY";
	TX = 52;
	TY = 13;
	mqttTopicTTX = wpFZ.DeviceName + "/TTX";
	mqttTopicTTY = wpFZ.DeviceName + "/TTY";
	TTX = 0;
	TTY = 0;

	mqttTopicHX = wpFZ.DeviceName + "/HX";
	mqttTopicHY = wpFZ.DeviceName + "/HY";
	HX = 37;
	HY = 44;
	mqttTopicTHX = wpFZ.DeviceName + "/THX";
	mqttTopicTHY = wpFZ.DeviceName + "/THY";
	THX = 0;
	THY = 31;
	
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
		u8g2->clearBuffer();
		u8g2_prepare();
		u8g2_FreakaZone();
		u8g2->sendBuffer();
	}
	if(displayCounter > 0 && displayCounter % 5 == 0) {
		u8g2->clearBuffer();
		u8g2_prepare();
		u8g2_temp_hum();
		u8g2->sendBuffer();
	}
	if(++displayCounter >= 60) {
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
	wpMqtt.mqttClient.subscribe(mqttTopicX.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicY.c_str());
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
	if(strcmp(topic, mqttTopicX.c_str()) == 0) X = msg.toInt();
	if(strcmp(topic, mqttTopicY.c_str()) == 0) Y = msg.toInt();
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
}

//###################################################################################
// private
//###################################################################################

void moduleOled096::u8g2_prepare() {
	u8g2->setFont(u8g2_font_6x10_tf); 
	u8g2->setFontRefHeightExtendedText(); 
	u8g2->setDrawColor(1);
	u8g2->setFontPosTop();
	u8g2->setFontDirection(0);
}
void moduleOled096::u8g2_temp_hum() {
	u8g2->setFont(u8g2_font_6x10_tf);
	u8g2->drawStr(TTX, TTY, "Temperature:");
	u8g2->setFont(u8g2_font_helvB14_tf);
	u8g2->drawStr(TX, TY, (String(wpDHT.temperature / 100.0) + " \xb0" + "C").c_str());
	u8g2->setFont(u8g2_font_6x10_tf);
	u8g2->drawStr(THX, THY, "Humidity:");
	u8g2->setFont(u8g2_font_helvB14_tf);
	u8g2->drawStr(HX, HY, (String(wpDHT.humidity / 100.0) + " %rF").c_str());
}
void moduleOled096::u8g2_FreakaZone() {
	u8g2->setFont(u8g2_font_helvB14_tf);
	u8g2->drawStr(X, Y, "Z");
	u8g2->drawStr(FX, FY, "FreakaZone");
}
void moduleOled096::u8g2_box_frame() {
	u8g2->drawStr(0, 0, "drawBox");
	u8g2->drawBox(5, 10, 20, 10);
	u8g2->drawStr(60, 0, "drawFrame");
	u8g2->drawFrame(65, 10, 20, 10);
}
void moduleOled096::u8g2_r_frame_box() {
	u8g2->drawStr(0, 0, "drawRFrame");
	u8g2->drawRFrame(5, 10, 40, 15, 3);
	u8g2->drawStr(70, 0, "drawRBox");
	u8g2->drawRBox(70, 10, 25, 15, 3);
}
void moduleOled096::u8g2_disc_circle() {
	u8g2->drawStr(0, 0, "drawDisc");
	u8g2->drawDisc(10, 18, 9);
	u8g2->drawDisc(30, 16, 7);
	u8g2->drawStr(60, 0, "drawCircle");
	u8g2->drawCircle(70, 18, 9);
	u8g2->drawCircle(90, 16, 7);
}
void moduleOled096::u8g2_string_orientation() {
	u8g2->setFontDirection(0);
	u8g2->drawStr(5, 15, "0");
	u8g2->setFontDirection(3);
	u8g2->drawStr(40, 25, "90");
	u8g2->setFontDirection(2);
	u8g2->drawStr(75, 15, "180");
	u8g2->setFontDirection(1);
	u8g2->drawStr(100, 10, "270");
}
void moduleOled096::u8g2_line() {
	u8g2->drawStr( 0, 0, "drawLine");
	u8g2->drawLine(7, 10, 40, 32);
	u8g2->drawLine(14, 10, 60, 32);
	u8g2->drawLine(28, 10, 80, 32);
	u8g2->drawLine(35, 10, 100, 32);
}
void moduleOled096::u8g2_triangle() {
	u8g2->drawStr( 0, 0, "drawTriangle");
	u8g2->drawTriangle(14, 7, 45, 30, 10, 32);
}
void moduleOled096::u8g2_unicode() {
	u8g2->drawStr(0, 0, "Unicode");
	u8g2->setFont(u8g2_font_unifont_t_symbols);
	u8g2->setFontPosTop();
	u8g2->setFontDirection(0);
	u8g2->drawUTF8(10, 15, "☀");
	u8g2->drawUTF8(30, 15, "☁");
	u8g2->drawUTF8(50, 15, "☂");
	u8g2->drawUTF8(70, 15, "☔");
	u8g2->drawUTF8(95, 15, COPYRIGHT_SYMBOL);  //COPYRIGHT SIMBOL
	u8g2->drawUTF8(115, 15, "\xb0");  // DEGREE SYMBOL
}
