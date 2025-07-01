//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 10.11.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 262                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleClock.h 262 2025-04-30 12:00:50Z                   $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleClock_h
#define moduleClock_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <Adafruit_NeoPixel.h>
#include <Stepper.h>

class moduleClock : public IModuleBase {
	public:
		moduleClock();
		moduleBase* mb;
		Adafruit_NeoPixel* strip;
		uint8_t Pin;
		uint8_t Pin1;
		uint8_t Pin2;
		uint8_t Pin3;
		uint8_t Pin4;

		uint8_t ColorHR;
		uint8_t ColorHG;
		uint8_t ColorHB;

		uint8_t ColorMR;
		uint8_t ColorMG;
		uint8_t ColorMB;

		uint8_t ColorSR;
		uint8_t ColorSG;
		uint8_t ColorSB;

		uint8_t ColorQR = 16;
		uint8_t ColorQG = 0;
		uint8_t ColorQB = 8;

		uint8_t Color5R = 8;
		uint8_t Color5G = 0;
		uint8_t Color5B = 8;

		// section for define
		Stepper* Motor;

		// values
		String mqttTopicSpr;
		String mqttTopicRpm;
		// settings
		String mqttTopicPixelCount;
		String mqttTopicSetSpr;
		String mqttTopicSetRpm;
		// commands
		String mqttTopicSetPixelCount;
		String mqttTopicSetIsRGB;

		// section to copy
		void init();
		void cycle();
		uint16_t getVersion();

		void publishSettings();
		void publishSettings(bool force);
		void publishValues();
		void publishValues(bool force);
		void setSubscribes();
		void checkSubscribes(char* topic, String msg);
		void changeDebug();
		String GetJsonSettings();
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
		uint32_t CalcCycle();
		uint32_t CalcCycle(uint32_t calcCycle);
		uint16_t GetSpr();
		void SetSpr(uint16_t StepsPerRound);
		uint16_t GetRpm();
		void SetRpm(uint16_t RoundsPerMinute);
		void SetSteps(short StepsToRun);
		void InitPixelCount(uint16_t pc);
		void InitRGB(bool rgb);
		bool GetRGB();
		void SetRGB(bool rgb);
		uint16_t GetPixelCount();
		void SetPixelCount(uint16_t pc);
		String GetColorH();
		String GetColorM();
		String GetColorS();
		String GetColorQ();
		String GetColor5();
		void SetColorH(uint8_t r, uint8_t g, uint8_t b);
		void SetColorM(uint8_t r, uint8_t g, uint8_t b);
		void SetColorS(uint8_t r, uint8_t g, uint8_t b);
		void SetColorQ(uint8_t r, uint8_t g, uint8_t b);
		void SetColor5(uint8_t r, uint8_t g, uint8_t b);
		void setClock(short ph, short pm, short ps);

		void SimulateTime();
		void SimulateTime(short h, short m, short s);
	private:
		uint8_t hour;
		uint8_t minute;
		uint8_t minuteLast;
		uint8_t second;
		uint8_t secondLast;
		bool simulateTime;
		uint16_t pixelCount = 50;
		bool isRGB = false;
		short steps;
		// Steps per Round
		uint16_t spr = 2048;
		uint16_t sprLast;
		unsigned long publishSprLast;
		// Rounds per Minute
		uint16_t rpm = 5;
		uint16_t rpmLast;
		unsigned long publishRpmLast;
		void calc();

	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 262 $";
};
extern moduleClock wpClock;

#endif