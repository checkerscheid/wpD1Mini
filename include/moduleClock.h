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
		uint8 Pin;
		uint8 Pin1;
		uint8 Pin2;
		uint8 Pin3;
		uint8 Pin4;

		uint8 ColorHR;
		uint8 ColorHG;
		uint8 ColorHB;

		uint8 ColorMR;
		uint8 ColorMG;
		uint8 ColorMB;

		uint8 ColorSR;
		uint8 ColorSG;
		uint8 ColorSB;

		uint8 ColorQR = 16;
		uint8 ColorQG = 0;
		uint8 ColorQB = 8;

		uint8 Color5R = 8;
		uint8 Color5G = 0;
		uint8 Color5B = 8;

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
		uint16 getVersion();

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
		uint32 CalcCycle();
		uint32 CalcCycle(uint32 calcCycle);
		uint16 GetSpr();
		void SetSpr(uint16 StepsPerRound);
		uint16 GetRpm();
		void SetRpm(uint16 RoundsPerMinute);
		void SetSteps(short StepsToRun);
		void InitPixelCount(uint16 pc);
		void InitRGB(bool rgb);
		bool GetRGB();
		void SetRGB(bool rgb);
		uint16 GetPixelCount();
		void SetPixelCount(uint16 pc);
		String GetColorH();
		String GetColorM();
		String GetColorS();
		String GetColorQ();
		String GetColor5();
		void SetColorH(uint8 r, uint8 g, uint8 b);
		void SetColorM(uint8 r, uint8 g, uint8 b);
		void SetColorS(uint8 r, uint8 g, uint8 b);
		void SetColorQ(uint8 r, uint8 g, uint8 b);
		void SetColor5(uint8 r, uint8 g, uint8 b);
		void setClock(short ph, short pm, short ps);

		void SimulateTime();
		void SimulateTime(short h, short m, short s);
	private:
		uint8 hour;
		uint8 minute;
		uint8 minuteLast;
		uint8 second;
		uint8 secondLast;
		bool simulateTime;
		uint16 pixelCount = 50;
		bool isRGB = false;
		short steps;
		// Steps per Round
		uint16 spr = 2048;
		uint16 sprLast;
		unsigned long publishSprLast;
		// Rounds per Minute
		uint16 rpm = 5;
		uint16 rpmLast;
		unsigned long publishRpmLast;
		void calc();

	
		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 262 $";
};
extern moduleClock wpClock;

#endif