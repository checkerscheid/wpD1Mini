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
//# Revision     : $Rev:: 123                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: helperRest.h 123 2024-06-02 04:37:07Z                    $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleOled096_h
#define moduleOled096_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <U8g2lib.h>
#include <Wire.h>
const char COPYRIGHT_SYMBOL[] = { 0xa9, '\0' };
class moduleOled096 {
	public:
		bool Debug = false;
		// settings
		// commands
		String mqttTopicError;
		String mqttTopicDebug;
		
		bool error = false;

		moduleOled096();
		void init();
		void cycle();
		uint16_t getVersion();
		void changeDebug();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
	private:
		String SVNh = "$Rev: 123 $";
		U8G2_SSD1306_128X64_NONAME_F_HW_I2C* u8g2;
		bool errorLast = false;
		uint16_t publishCountError = 0;
		bool DebugLast = false;
		uint16_t publishCountDebug = 0;
		uint16_t displayCounter;
		uint16_t displayChecker;

		uint8_t X;
		uint8_t Y;
		String mqttTopicX;
		String mqttTopicY;
		uint8_t FX;
		uint8_t FY;
		String mqttTopicFX;
		String mqttTopicFY;

		uint8_t TX;
		uint8_t TY;
		String mqttTopicTX;
		String mqttTopicTY;
		uint8_t HX;
		uint8_t HY;
		String mqttTopicHX;
		String mqttTopicHY;

		uint8_t TTX;
		uint8_t TTY;
		String mqttTopicTTX;
		String mqttTopicTTY;
		uint8_t THX;
		uint8_t THY;
		String mqttTopicTHX;
		String mqttTopicTHY;

		void u8g2_temp_hum();
		void u8g2_FreakaZone();
		void u8g2_prepare();
		void u8g2_box_frame();
		void u8g2_r_frame_box();
		void u8g2_disc_circle();
		void u8g2_string_orientation();
		void u8g2_line();
		void u8g2_triangle();
		void u8g2_unicode();
};
extern moduleOled096 wpOled096;
#endif