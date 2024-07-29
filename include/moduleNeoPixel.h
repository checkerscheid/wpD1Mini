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
//# Revision     : $Rev:: 183                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleNeoPixel.h 183 2024-07-29 03:32:26Z                $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleNeoPixel_h
#define moduleNeoPixel_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <Adafruit_NeoPixel.h>

class moduleNeoPixel {
	public:
		moduleNeoPixel();
		moduleBase* mb;
		Adafruit_NeoPixel* strip;
		uint8 Pin;

		static const uint8 ModeStatic = 0;
		static const uint8 ModeColorWipeRed = 1;
		static const uint8 ModeColorWipeGreen = 2;
		static const uint8 ModeColorWipeBlue = 3;
		static const uint8 ModeTheaterChaseWhite = 4;
		static const uint8 ModeTheaterChaseRed = 5;
		static const uint8 ModeTheaterChaseGreen = 6;
		static const uint8 ModeTheaterChaseBlue = 7;
		static const uint8 ModeRainbow = 8;
		static const uint8 ModeTheaterChaseRainbow = 9;
		static const uint8 ModeRunnerRed = 10;
		static const uint8 ModeRunnerGreen = 11;
		static const uint8 ModeRunnerBlue = 12;
		static const uint8 ModeRandom = 13;
		static const uint8 ModeComplex = 14;
		// Pia
		static const uint8 ModeColorWipePurple = 20;
		static const uint8 ModeRunnerPurple = 21;

		bool demoMode;

		// values
		String mqttTopicModeName;
		// settings
		// commands
		String mqttTopicValueR;
		String mqttTopicValueG;
		String mqttTopicValueB;
		String mqttTopicBrightness;
		String mqttTopicSetMode;
		String mqttTopicDemoMode;

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
		void changeSendRest();
		void changeDebug();
		// getter / setter
		bool SendRest();
		bool SendRest(bool sendRest);
		bool Debug();
		bool Debug(bool debug);
		void SimpleEffect(byte r, byte g, byte b);
		void SimpleEffect(byte r, byte g, byte b, byte br);
		void PiaEffect();
		void ComplexEffect(uint pixel, byte r, byte g, byte b);
		void ComplexEffect(uint pixel, uint32_t color);
		void setValueR(uint8 r);
		uint8 getValueR();
		void setValueG(uint8 g);
		uint8 getValueG();
		void setValueB(uint8 b);
		uint8 getValueB();
		void setBrightness(uint8 bn);
		uint8 getBrightness();
		String GetModeName(uint actualMode);
		void SetMode(uint8 newMode);
		String getStripStatus();
	private:
		uint pixelCount;
		uint8 valueR = 255;
		uint8 valueRLast;
		uint8 valueG = 75;
		uint8 valueGLast;
		uint8 valueB = 0;
		uint8 valueBLast;
		uint8 brightness = 0;
		uint8 brightnessLast;
		unsigned long publishValueLast;
		uint modeCurrent;
		uint modeCurrentLast;
		unsigned long publishModeLast;

		uint32_t piasFavColor;
		uint8_t piasFavColorR;
		uint8_t piasFavColorG;
		uint8_t piasFavColorB;

		unsigned long pixelPrevious;	// Previous Pixel Millis
		unsigned long patternPrevious;	// Previous Pattern Millis
		uint patternInterval;			// Pattern Interval (ms)

		uint pixelInterval;				// Pixel Interval (ms)
		int pixelQueue;					// Pattern Pixel Queue
		int pixelCycle;					// Pattern Pixel Cycle
		uint16_t pixelNumber;			// Total Number of Pixels

		void publishValue();
		void calc();
		
		void ColorWipeEffect(uint32_t color, int wait);
		void TheaterChaseEffect(uint32_t color, int wait);
		void RainbowEffect(uint8_t wait);
		void TheaterChaseRainbowEffect(uint8_t wait);
		void RunnerEffect(uint32_t color, int wait);
		void RandomEffect(int wait);
		uint32_t Wheel(byte WheelPos);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 183 $";
};
extern moduleNeoPixel wpNeoPixel;

#endif