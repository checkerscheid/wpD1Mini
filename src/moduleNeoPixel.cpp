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
//# File-ID      : $Id:: moduleNeoPixel.cpp 183 2024-07-29 03:32:26Z              $ #
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

moduleNeoPixel::moduleNeoPixel() {
	// section to config and copy
	ModuleName = "NeoPixel";
	mb = new moduleBase(ModuleName);

	piasFavColorR = 137;
	piasFavColorG = 0;
	piasFavColorB = 183;
	//wpFZ.loopTime = 100;
}
void moduleNeoPixel::init() {
	Pin = D7;
	// Declare our NeoPixel strip object:
	strip = new Adafruit_NeoPixel(pixelCount, Pin, NEO_RGB + NEO_KHZ800);
	piasFavColor = strip->Color(piasFavColorR, piasFavColorG, piasFavColorB);

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

	demoMode = false;

	// values
	mqttTopicModeName = wpFZ.DeviceName + "/" + ModuleName + "/ModeName";
	// settings
	mqttTopicPixelCount = wpFZ.DeviceName + "/settings/" + ModuleName + "/PixelCount";
	// commands
	mqttTopicValueR = wpFZ.DeviceName + "/" + ModuleName + "/R";
	mqttTopicValueG = wpFZ.DeviceName + "/" + ModuleName + "/G";
	mqttTopicValueB = wpFZ.DeviceName + "/" + ModuleName + "/B";
	mqttTopicBrightness = wpFZ.DeviceName + "/" + ModuleName + "/Brightness";
	mqttTopicSetMode = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetMode";
	mqttTopicDemoMode = wpFZ.DeviceName + "/settings/" + ModuleName + "/DemoMode";

	publishValueLast = 0;
	publishModeLast = 0;

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules1, wpEEPROM.bitsSendRestModules1, wpEEPROM.bitSendRestNeoPixel);
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
	strip->setBrightness(brightness); // Set BRIGHTNESS to about 1/5 (max = 255)
}

// loop() function -- runs repeatedly as long as board is on ---------------
void moduleNeoPixel::cycle() {
	if(wpFZ.calcValues) {
		calc();
	}
	publishValues();

	if(wpModules.useModuleAnalogOut && wpModules.useModuleAnalogOut2) {
		// RGB LED has CW + WW
		// use AnalogOut for WW or White
		// use AnalogOut2 for CW with WW
		// must have output limitations
		// @todo make logik, that CW + WW <= 254
		//wpAnalogOut.hardwareoutMax = 50;
		//wpAnalogOut2.hardwareoutMax = 50;
	}
}

void moduleNeoPixel::publishSettings() {
	publishSettings(false);
}
void moduleNeoPixel::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicSetMode.c_str(), String(modeCurrent).c_str());
	wpMqtt.mqttClient.publish(mqttTopicDemoMode.c_str(), String(demoMode).c_str());
	wpMqtt.mqttClient.publish(mqttTopicPixelCount.c_str(), String(pixelCount).c_str());
	mb->publishSettings(force);
}

void moduleNeoPixel::publishValues() {
	publishValues(false);
}
void moduleNeoPixel::publishValues(bool force) {
	if(force) {
		publishValueLast = 0;
		publishModeLast = 0;
	}
	if(valueRLast != valueR || valueGLast != valueG || valueBLast != valueB || brightnessLast != brightness ||
		wpFZ.CheckQoS(publishValueLast)) {
		publishValue();
	}
	if(modeCurrentLast != modeCurrent || wpFZ.CheckQoS(publishModeLast)) {
		modeCurrentLast = modeCurrent;
		wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), GetModeName(modeCurrent).c_str());
		if(mb->sendRest) {
			wpRest.error = wpRest.error | !wpRest.sendRest("modeCurrent", GetModeName(modeCurrent));
			wpRest.trySend = true;
		}
		if(wpMqtt.Debug) {
			mb->printPublishValueDebug(mqttTopicModeName, GetModeName(modeCurrent));
		}
		publishModeLast = wpFZ.loopStartedAt;
	}
	mb->publishValues(force);
}

void moduleNeoPixel::setSubscribes() {
	wpMqtt.mqttClient.subscribe(mqttTopicValueR.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicValueG.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicValueB.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicBrightness.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicSetMode.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicDemoMode.c_str());
	wpMqtt.mqttClient.subscribe(mqttTopicPixelCount.c_str());
	mb->setSubscribes();
}

void moduleNeoPixel::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicValueR.c_str()) == 0) {
		uint8 readValueR = msg.toInt();
		if(valueR != readValueR) {
			SetValueR(readValueR);
			wpFZ.DebugcheckSubscribes(mqttTopicValueR, String(valueR));
		}
	}
	if(strcmp(topic, mqttTopicValueG.c_str()) == 0) {
		uint8 readValueG = msg.toInt();
		if(valueG != readValueG) {
			SetValueG(readValueG);
			wpFZ.DebugcheckSubscribes(mqttTopicValueG, String(valueG));
		}
	}
	if(strcmp(topic, mqttTopicValueB.c_str()) == 0) {
		uint8 readValueB = msg.toInt();
		if(valueB != readValueB) {
			SetValueB(readValueB);
			wpFZ.DebugcheckSubscribes(mqttTopicValueB, String(valueB));
		}
	}
	if(strcmp(topic, mqttTopicBrightness.c_str()) == 0) {
		uint8 readBrightness = msg.toInt();
		if(brightness != readBrightness) {
			SetBrightness(readBrightness);
			wpFZ.DebugcheckSubscribes(mqttTopicBrightness, String(brightness));
		}
	}
	if(strcmp(topic, mqttTopicSetMode.c_str()) == 0) {
		uint readSetMode = msg.toInt();
		if(modeCurrent != readSetMode) {
			modeCurrent = readSetMode;
			wpFZ.DebugcheckSubscribes(mqttTopicSetMode, String(modeCurrent));
		}
	}
	if(strcmp(topic, mqttTopicDemoMode.c_str()) == 0) {
		bool readDemoMode = msg.toInt();
		if(demoMode != readDemoMode) {
			demoMode = readDemoMode;
			wpFZ.DebugcheckSubscribes(mqttTopicDemoMode, String(demoMode));
		}
	}
	if(strcmp(topic, mqttTopicPixelCount.c_str()) == 0) {
		uint16 readPixelCount = msg.toInt();
		if(pixelCount != readPixelCount) {
			SetPixelCount(readPixelCount);
			wpFZ.DebugcheckSubscribes(mqttTopicPixelCount, String(readPixelCount));
		}
	}
	mb->checkSubscribes(topic, msg);
}
void moduleNeoPixel::InitValueR(uint8 r) {
	valueR = r;
}
uint8 moduleNeoPixel::GetValueR() { return valueR; }
void moduleNeoPixel::SetValueR(uint8 r) {
	valueR = r;
	EEPROM.write(wpEEPROM.byteNeoPixelValueR, valueR);
	EEPROM.commit();
}
void moduleNeoPixel::InitValueG(uint8 g) {
	valueG = g;
}
uint8 moduleNeoPixel::GetValueG() { return valueG; }
void moduleNeoPixel::SetValueG(uint8 g) {
	valueG = g;
	EEPROM.write(wpEEPROM.byteNeoPixelValueG, valueG);
	EEPROM.commit();
}
void moduleNeoPixel::InitValueB(uint8 b) {
	valueB = b;
}
uint8 moduleNeoPixel::GetValueB() { return valueB; }
void moduleNeoPixel::SetValueB(uint8 b) {
	valueB = b;
	EEPROM.write(wpEEPROM.byteNeoPixelValueB, valueB);
	EEPROM.commit();
}
void moduleNeoPixel::InitBrightness(uint8 br) {
	brightness = br;
}
uint8 moduleNeoPixel::GetBrightness() { return brightness; }
void moduleNeoPixel::SetBrightness(uint8 br) {
	brightness = br;
	EEPROM.write(wpEEPROM.byteNeoPixelBrightness, brightness);
	EEPROM.commit();
}

void moduleNeoPixel::InitPixelCount(uint16 pc) {
	pixelCount = pc;
}
uint16 moduleNeoPixel::GetPixelCount() { return pixelCount; }
void moduleNeoPixel::SetPixelCount(uint16 pc) {
	EEPROM.put(wpEEPROM.byteNeoPixelPixelCount, pc);
	EEPROM.commit();
	wpFZ.restartRequired = true;
}

String moduleNeoPixel::GetModeName(uint actualMode) {
	String returns;
	switch(actualMode) {
		case ModeStatic:
			returns = "ModeStatic";
			break;
		case ModeColorWipeRed:
			returns = "ModeColorWipeRed";
			break;
		case ModeColorWipeGreen:
			returns = "ModeColorWipeGreen";
			break;
		case ModeColorWipeBlue:
			returns = "ModeColorWipeBlue";
			break;
		case ModeTheaterChaseWhite:
			returns = "ModeTheaterChaseWhite";
			break;
		case ModeTheaterChaseRed:
			returns = "ModeTheaterChaseRed";
			break;
		case ModeTheaterChaseGreen:
			returns = "ModeTheaterChaseGreen";
			break;
		case ModeTheaterChaseBlue:
			returns = "ModeTheaterChaseBlue";
			break;
		case ModeRainbow:
			returns = "ModeRainbow";
			break;
		case ModeTheaterChaseRainbow:
			returns = "ModeTheaterChaseRainbow";
			break;
		case ModeRunnerRed:
			returns = "ModeRunnerRed";
			break;
		case ModeRunnerGreen:
			returns = "ModeRunnerGreen";
			break;
		case ModeRunnerBlue:
			returns = "ModeRunnerBlue";
			break;
		case ModeRandom:
			returns = "ModeRandom";
			break;
		case ModeComplex:
			returns = "ModeComplex";
			break;
		case ModeColorWipePurple:
			returns = "ModeColorWipePurple";
			break;
		case ModeRunnerPurple:
			returns = "ModeRunnerPurple";
			break;
		default:
			returns = String(actualMode);
			break;
	}
	return returns;
}
void moduleNeoPixel::SetMode(uint8 newMode) {
	modeCurrent = newMode;
}
String moduleNeoPixel::getStripStatus() {
	String returns = "{";
	for(uint i = 0; i < pixelCount; i++) {
		uint32_t c = strip->getPixelColor(i);
		uint8_t r = c >> 16;
		uint8_t g = c >> 8;
		uint8_t b = c;
		returns += "\"p" + String(i) + "\":{"
			"\"r\":" + String(r) + ","
			"\"g\":" + String(g) + ","
			"\"b\":" + String(b) + "},";
	}
	returns += "\"ww\":" + String(wpAnalogOut.output) + ","
		"\"cw\":" + String(wpAnalogOut2.output) + ","
		"\"b\":" + String(strip->getBrightness()) + "}";
	return returns;
}
//###################################################################################
// private
//###################################################################################
void moduleNeoPixel::publishValue() {
	valueRLast = valueR;
	valueGLast = valueG;
	valueBLast = valueB;
	brightnessLast = brightness;
	wpMqtt.mqttClient.publish(mqttTopicValueR.c_str(), String(valueR).c_str());
	wpMqtt.mqttClient.publish(mqttTopicValueG.c_str(), String(valueG).c_str());
	wpMqtt.mqttClient.publish(mqttTopicValueB.c_str(), String(valueB).c_str());
	wpMqtt.mqttClient.publish(mqttTopicBrightness.c_str(), String(brightness).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRestRGB(valueR, valueG, valueB, brightness);
		wpRest.trySend = true;
	}
	if(wpMqtt.Debug) {
		mb->printPublishValueDebug(mqttTopicValueR, String(valueR));
		mb->printPublishValueDebug(mqttTopicValueG, String(valueG));
		mb->printPublishValueDebug(mqttTopicValueB, String(valueB));
		mb->printPublishValueDebug(mqttTopicBrightness, String(brightness));
	}
	publishValueLast = wpFZ.loopStartedAt;
}

void moduleNeoPixel::calc() {
	unsigned long currentMillis = millis();                     //  Update current time
	if(demoMode) {
		if((currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired time
			patternPrevious = currentMillis;
			if(++modeCurrent > 14)
				modeCurrent = 1;
		}
	}
	if(currentMillis - pixelPrevious >= pixelInterval) {        //  Check for expired time
		pixelPrevious = currentMillis;                            //  Run current frame
		switch (modeCurrent) {
			case ModeColorWipeRed:
				ColorWipeEffect(strip->Color(255, 0, 0), 50); // Red
				break;
			case ModeColorWipeGreen:
				ColorWipeEffect(strip->Color(0, 255, 0), 50); // Green
				break;
			case ModeColorWipeBlue:
				ColorWipeEffect(strip->Color(0, 0, 255), 50); // Blue
				break;
			case ModeColorWipePurple:
				ColorWipeEffect(piasFavColor, 50); // Purple
				break;
			case ModeTheaterChaseWhite:
				TheaterChaseEffect(strip->Color(127, 127, 127), 50); // White
				break;
			case ModeTheaterChaseRed:
				TheaterChaseEffect(strip->Color(127, 0, 0), 50); // Red
				break;
			case ModeTheaterChaseGreen:
				TheaterChaseEffect(strip->Color(0, 127, 0), 50); // Green
				break;
			case ModeTheaterChaseBlue:
				TheaterChaseEffect(strip->Color(0, 0, 127), 50); // Blue
				break;
			case ModeRainbow:
				RainbowEffect(10); // Flowing rainbow cycle along the whole strip
				break;
			case ModeTheaterChaseRainbow:
				TheaterChaseRainbowEffect(50); // Rainbow-enhanced theaterChase variant
				break;
			case ModeRunnerRed:
				RunnerEffect(strip->Color(127, 0, 0), 50); // Runner Red
				break;
			case ModeRunnerGreen:
				RunnerEffect(strip->Color(0, 127, 0), 50); // Runner Green
				break;
			case ModeRunnerBlue:
				RunnerEffect(strip->Color(0, 0, 127), 50); // Runner Blue
				break;
			case ModeRunnerPurple:
				RunnerEffect(piasFavColor, 50); // Runner Purple
				break;
			case ModeRandom:
				RandomEffect(500); // Random
				break;
			case ModeComplex:
				// nothing todo, but save LED state
				break;
			default:
				SimpleEffect(valueR, valueG, valueB, brightness);
				break;
		}
	}
}

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void moduleNeoPixel::ColorWipeEffect(uint32_t color, int wait) {
	static uint16_t current_pixel = 0;
	static bool ison = false;
	pixelInterval = wait;                         //  Update delay time
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
void moduleNeoPixel::TheaterChaseEffect(uint32_t color, int wait) {
	static uint32_t loop_count = 0;
	static uint16_t current_pixel = 0;

	pixelInterval = wait;                   //  Update delay time

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

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void moduleNeoPixel::RainbowEffect(uint8_t wait) {
	if(pixelInterval != wait)
		pixelInterval = wait;
	for(uint16_t i=0; i < pixelCount; i++) {
		strip->setPixelColor(i, Wheel((i + pixelCycle) & 255)); //  Update delay time  
	}
	strip->show();                          //  Update strip to match
	pixelCycle++;                           //  Advance current cycle
	if(pixelCycle >= 256)
		pixelCycle = 0;                     //  Loop the cycle back to the begining
}

//Theatre-style crawling lights with rainbow effect
void moduleNeoPixel::TheaterChaseRainbowEffect(uint8_t wait) {
	if(pixelInterval != wait)
		pixelInterval = wait;               //  Update delay time  
	for(int i=0; i < pixelCount; i+=3) {
		strip->setPixelColor(i + pixelQueue, Wheel((i + pixelCycle) % 255)); //  Update delay time  
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

void moduleNeoPixel::RunnerEffect(uint32_t color, int wait) {
	static uint16_t current_pixel = 0;
	pixelInterval = wait;                         //  Update delay time
	strip->clear();
	strip->setPixelColor(current_pixel++, color); //  Set pixel's color (in RAM)
	strip->show();                                //  Update strip to match
	if(current_pixel >= pixelCount) {            //  Loop the pattern from the first LED
		current_pixel = 0;
	}
}
void moduleNeoPixel::RandomEffect(int wait) {
	int pixel;
	byte r, g, b;
	pixelInterval = wait;                         //  Update delay time
	strip->clear();
	r = random(0, 255);
	g = random(0, 255);
	b = random(0, 255);
	strip->fill(strip->Color(r, g, b));
	for(uint p = 0; p < 20; p++) {
		pixel = random(0, pixelCount);
		r = random(0, 255);
		g = random(0, 255);
		b = random(0, 255);
		strip->setPixelColor(pixel, strip->Color(r, g, b));
	}
	strip->show();
}

void moduleNeoPixel::SimpleEffect(byte r, byte g, byte b, byte br) {
	SetBrightness(br);
	strip->setBrightness(br);
	SimpleEffect(r, g, b);
}
void moduleNeoPixel::SimpleEffect(byte r, byte g, byte b) {
	SetValueR(r);
	SetValueG(g);
	SetValueB(b);
	uint32_t color = strip->Color(r, g, b);
	demoMode = false;
	modeCurrent = ModeStatic;
	strip->fill(color);
	strip->show();
}
void moduleNeoPixel::PiaEffect() {
	SetValueR(piasFavColorR);
	SetValueG(piasFavColorG);
	SetValueB(piasFavColorB);
	SimpleEffect(piasFavColorR, piasFavColorG, piasFavColorB);
}
void moduleNeoPixel::ComplexEffect(uint pixel, byte r, byte g, byte b) {
	if(pixel > pixelCount) pixel = pixelCount;
	ComplexEffect(pixel, strip->Color(r, g, b));
}
void moduleNeoPixel::ComplexEffect(uint pixel, uint32_t color) {
	demoMode = false;
	modeCurrent = ModeComplex;
	strip->setPixelColor(pixel, color);
	strip->show();
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

//###################################################################################
// section to copy
//###################################################################################
uint16 moduleNeoPixel::getVersion() {
	String SVN = "$Rev: 183 $";
	uint16 v = wpFZ.getBuild(SVN);
	uint16 vh = wpFZ.getBuild(SVNh);
	return v > vh ? v : vh;
}

void moduleNeoPixel::changeSendRest() {
	mb->changeSendRest();
}
void moduleNeoPixel::changeDebug() {
	mb->changeDebug();
}
bool moduleNeoPixel::SendRest() {
	return mb->sendRest;
}
bool moduleNeoPixel::SendRest(bool sendRest) {
	mb->sendRest = sendRest;
	return true;
}
bool moduleNeoPixel::Debug() {
	return mb->debug;
}
bool moduleNeoPixel::Debug(bool debug) {
	mb->debug = debug;
	return true;
}
