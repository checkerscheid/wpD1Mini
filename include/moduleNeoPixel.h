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
//# Revision     : $Rev:: 187                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleNeoPixel.h 187 2024-08-07 11:05:05Z                $ #
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
		static const uint8 ModeColorWipe = 1;
		static const uint8 ModeTheaterChase = 2;
		static const uint8 ModeRainbow = 3;
		static const uint8 ModeTheaterChaseRainbow = 4;
		static const uint8 ModeRunner = 5;
		static const uint8 ModeRandom = 6;
		static const uint8 ModeRainbowTv = 7;

		static const uint8 ModeComplex = 99;
		uint32_t piasFavColor;
		uint8_t piasFavColorR;
		uint8_t piasFavColorG;
		uint8_t piasFavColorB;

		bool demoMode;
		bool useShelly;

		// values
		String mqttTopicValueR;
		String mqttTopicValueG;
		String mqttTopicValueB;
		String mqttTopicBrightness;
		String mqttTopicDemoMode;
		String mqttTopicModeName;
		String mqttTopicSleep;
		// settings
		String mqttTopicPixelCount;
		// commands
		String mqttTopicSetR;
		String mqttTopicSetG;
		String mqttTopicSetB;
		String mqttTopicSetBrightness;
		String mqttTopicSetDemoMode;
		String mqttTopicSetMode;
		String mqttTopicSetSleep;
		String mqttTopicSetPixelCount;

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
		void InitValueR(uint8 r);
		uint8 GetValueR();
		void SetValueR(uint8 r);
		void InitValueG(uint8 g);
		uint8 GetValueG();
		void SetValueG(uint8 g);
		void InitValueB(uint8 b);
		uint8 GetValueB();
		void SetValueB(uint8 b);
		void InitBrightness(uint8 bn);
		uint8 GetBrightness();
		void SetBrightness(uint8 bn);
		void ComplexEffect(uint pixel, byte r, byte g, byte b);
		void ComplexEffect(uint pixel, uint32_t color);
		String GetModeName(uint actualMode);
		void SetMode(uint8 newMode);
		void SetSleep(uint seconds);
		void SetOff();
		void SetOn();
		void InitPixelCount(uint16 pc);
		uint16 GetPixelCount();
		void SetPixelCount(uint16 pc);
		String getStripStatus();
		void setShelly(uint32_t c);
		unsigned long lastShellySend;
	private:
		uint16 pixelCount = 50;
		uint16 pixelStartForTv = 25;
		uint8 valueR = 255;
		uint8 valueRLast;
		uint8 valueG = 75;
		uint8 valueGLast;
		uint8 valueB = 0;
		uint8 valueBLast;
		uint8 brightness = 0;
		uint8 brightnessLast;
		bool staticIsSet;
		unsigned long publishValueLast;
		uint modeCurrent;
		uint modeCurrentLast;
		unsigned long publishModeLast;
		uint sleep;
		uint sleepLast;
		unsigned long publishSleepLast;
		unsigned long sleepAt;

		unsigned long pixelPrevious;	// Previous Pixel Millis
		unsigned long patternPrevious;	// Previous Pattern Millis
		uint patternInterval;			// Pattern Interval (ms)

		uint pixelInterval;				// Pixel Interval (ms)
		int pixelQueue;					// Pattern Pixel Queue
		int pixelCycle;					// Pattern Pixel Cycle

		void publishValue();
		void calc();
		
		void ColorWipeEffect(int wait);
		void TheaterChaseEffect(int wait);
		void RainbowEffect(uint8_t wait);
		void RainbowTvEffect(uint8_t wait);
		void TheaterChaseRainbowEffect(uint8_t wait);
		void RunnerEffect(int wait);
		void RandomEffect(int wait);
		void StaticEffect();
		uint32_t Wheel(byte WheelPos);

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 187 $";
};
extern moduleNeoPixel wpNeoPixel;

#endif