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
//# File-ID      : $Id:: moduleNeoPixel.h 269 2025-07-01 19:25:14Z                $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleNeoPixel_h
#define moduleNeoPixel_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <Adafruit_NeoPixel.h>

class moduleNeoPixel : public IModuleBase {
	public:
		moduleNeoPixel();
		moduleBase* mb;
		Adafruit_NeoPixel* strip;
		uint8_t Pin;

		static const uint8_t ModeStatic = 0;
		static const uint8_t ModeColorWipe = 1;
		static const uint8_t ModeTheaterChase = 2;
		static const uint8_t ModeRainbow = 3;
		static const uint8_t ModeWheelRainbow = 4;
		static const uint8_t ModeTheaterChaseRainbow = 5;
		static const uint8_t ModeRunner = 6;
		static const uint8_t ModeDisco = 7;
		static const uint8_t ModeRainbowTv = 8;

		static const uint8_t ModeBlender = 90; //CW, WW
		static const uint8_t ModeOffRunner = 97;
		static const uint8_t ModeComplex = 99;

		bool demoMode;
		bool useBorder;

		// values
		String mqttTopicMaxPercent;
		String mqttTopicValueR;
		String mqttTopicValueG;
		String mqttTopicValueB;
		String mqttTopicWW;
		String mqttTopicCW;
		//String mqttTopicBrightness;
		//String mqttTopicStatus;
		String mqttTopicDemoMode;
		String mqttTopicModeName;
		String mqttTopicEffectSpeed;
		String mqttTopicSleep;
		// settings
		String mqttTopicPixelCount;
		String mqttTopicUseBorder;
		// commands
		String mqttTopicSetR;
		String mqttTopicSetG;
		String mqttTopicSetB;
		String mqttTopicSetWW;
		String mqttTopicSetCW;
		//String mqttTopicSetBrightness;
		String mqttTopicSetDemoMode;
		String mqttTopicSetMode;
		String mqttTopicSetEffectSpeed;
		String mqttTopicSetSleep;
		String mqttTopicSetPixelCount;
		String mqttTopicSetUseBorder;
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
		void InitValueR(uint8_t r);
		uint8_t GetValueR();
		String SetValueR(uint8_t r);
		void InitValueG(uint8_t g);
		uint8_t GetValueG();
		String SetValueG(uint8_t g);
		void InitValueB(uint8_t b);
		uint8_t GetValueB();
		String SetValueB(uint8_t b);
		//void InitBrightness(uint8_t bn);
		//uint8_t GetBrightness();
		//void SetBrightness(uint8_t bn);
		String SetEffectSpeed(uint8_t es);
		String ComplexEffect(uint pixel, byte r, byte g, byte b);
		String ComplexEffect(uint pixel, uint32_t color);
		String GetModeName(uint actualMode);
		String SetMode(uint8_t newMode);
		String SetSleep(uint seconds);
		String SetOn();
		String SetOff();
		String SetWW(uint ww);
		String SetCW(uint cw);
		String SetOffRunner(uint8_t steps);
		void InitPixelCount(uint16_t pc);
		void InitRGB(bool rgb);
		bool GetRGB();
		void SetRGB(bool rgb);
		uint16_t GetPixelCount();
		void SetPixelCount(uint16_t pc);
		//String getStripStatus();
		void setBorder(uint32_t c);
		String ChangeUseWW();
		bool GetUseWW() { return useWW; }
		void SetUseWW(bool ww) { useWW = ww; }
		String ChangeUseCW();
		bool GetUseCW() { return useCW; }
		void SetUseCW(bool cw) { useCW = cw; }
		unsigned long lastBorderSend;
	private:
		uint8_t PinWW;
		uint8_t PinCW;
		bool useWW = false;
		bool useCW = false;
		uint8_t AnalogOutWW = 0;
		uint8_t AnalogOutWWLast = 0;
		unsigned long publishAnalogOutWWLast;
		uint8_t AnalogOutCW = 0;
		uint8_t AnalogOutCWLast = 0;
		unsigned long publishAnalogOutCWLast;
		uint16_t pixelCount = 50;
		uint16_t pixelStartForTv = 25;
		uint maxPercent;
		uint maxPercentLast;
		unsigned long publishMaxPercentLast;
		uint8_t valueR = 255;
		uint8_t valueRLast;
		uint8_t valueG = 75;
		uint8_t valueGLast;
		uint8_t valueB = 0;
		uint8_t valueBLast;
		//uint8_t brightness = 0;
		//uint8_t brightnessLast;
		//String status;
		//String statusLast;
		//unsigned long publishStatusLast;
		bool useBorderLast;
		bool staticIsSet;
		bool isRGB = false;
		unsigned long publishValueLast;
		uint modeCurrent;
		uint modeCurrentLast;
		unsigned long publishModeLast;
		uint8_t effectSpeed; // 1 x = 25 ms
		uint8_t effectSpeedLast;
		unsigned long publishEffectSpeedLast;
		uint sleep;
		uint sleepLast;
		unsigned long publishSleepLast;
		unsigned long sleepAt;
		unsigned long publishUseBorderLast;

		unsigned long pixelPrevious;	// Previous Pixel Millis
		unsigned long patternPrevious;	// Previous Pattern Millis
		uint patternInterval;			// Pattern Interval (ms)

		uint pixelInterval;				// Pixel Interval (ms)
		int pixelQueue;					// Pattern Pixel Queue
		int pixelCycle;					// Pattern Pixel Cycle
		uint8_t steps;

		void publishValue();
		void calc();
		
		uint8_t targetR;
		uint8_t targetG;
		uint8_t targetB;
		//uint8_t targetBr;
		uint8_t targetWW;
		uint8_t targetCW;
		void calcDuration();
		void BlenderEffect();
		bool BlenderWWEffect();
		bool BlenderCWEffect();
		bool BlenderREffect();
		bool BlenderGEffect();
		bool BlenderBEffect();
		//bool BlenderBrightnessEffect();

		void ColorWipeEffect(uint wait);
		void TheaterChaseEffect(uint wait);
		void RainbowEffect(uint wait);
		void RainbowWheelEffect(uint wait);
		void RainbowTvEffect(uint wait);
		void TheaterChaseRainbowEffect(uint wait);
		void RunnerEffect(uint wait);
		void DiscoEffect(uint wait);
		void OffRunnerEffect(uint wait);
		void StaticEffect();
		uint32_t Wheel(byte WheelPos);
		uint8_t GetMaxPercent();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 269 $";
};
extern moduleNeoPixel wpNeoPixel;

#endif