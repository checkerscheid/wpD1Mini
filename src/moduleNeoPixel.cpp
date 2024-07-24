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
//# Revision     : $Rev:: 176                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleNeoPixel.cpp 176 2024-07-24 16:02:43Z              $ #
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
	wpFZ.loopTime = 10;
}
void moduleNeoPixel::init() {
	neoPixelPin = D5;
	pixelCount = 50;
	// Declare our NeoPixel strip object:
	strip = new Adafruit_NeoPixel(pixelCount, neoPixelPin, NEO_RGB + NEO_KHZ800);
	if(wpModules.useModuleAnalogOut && wpModules.useModuleAnalogOut2) {
		// RGB LED has CW + WW
		// must have output limitations
		// @todo make logik, that CW + WW <= 254
		wpAnalogOut.hardwareoutMax = 127;
		wpAnalogOut2.hardwareoutMax = 127;
	}

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
	pixelNumber = pixelCount;  // Total Number of Pixels

	demoMode = false;

	// values
	mqttTopicModeName = wpFZ.DeviceName + "/" + ModuleName + "/ModeName";
	// settings
	// commands
	mqttTopicValueR = wpFZ.DeviceName + "/ValueR";
	mqttTopicValueG = wpFZ.DeviceName + "/ValueG";
	mqttTopicValueB = wpFZ.DeviceName + "/ValueB";
	mqttTopicBrightness = wpFZ.DeviceName + "/Brightness";
	mqttTopicSetMode = wpFZ.DeviceName + "/settings/" + ModuleName + "/SetMode";
	mqttTopicDemoMode = wpFZ.DeviceName + "/settings/" + ModuleName + "/DemoMode";

	publishCountValue = 0;

	// section to copy
	mb->initRest(wpEEPROM.addrBitsSendRestModules1, wpEEPROM.bitsSendRestModules1, wpEEPROM.bitSendRestNeoPixel);
	mb->initDebug(wpEEPROM.addrBitsDebugModules1, wpEEPROM.bitsDebugModules1, wpEEPROM.bitDebugNeoPixel);
	mb->initMaxCycle(wpEEPROM.byteMaxCycleNeoPixel);

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
}

void moduleNeoPixel::publishSettings() {
	publishSettings(false);
}
void moduleNeoPixel::publishSettings(bool force) {
	wpMqtt.mqttClient.publish(mqttTopicSetMode.c_str(), String(modeCurrent).c_str());
	wpMqtt.mqttClient.publish(mqttTopicDemoMode.c_str(), String(demoMode).c_str());
	mb->publishSettings(force);
}

void moduleNeoPixel::publishValues() {
	publishValues(false);
}
void moduleNeoPixel::publishValues(bool force) {
	if(force) {
		publishCountValue = wpFZ.publishQoS;
	}
	if(valueRLast != valueR || valueGLast != valueG || valueBLast != valueB || brightness != brightnessLast ||
		++publishCountValue > wpFZ.publishQoS) {
		publishValue();
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
	mb->setSubscribes();
}

void moduleNeoPixel::checkSubscribes(char* topic, String msg) {
	if(strcmp(topic, mqttTopicValueR.c_str()) == 0) {
		uint8 readValueR = msg.toInt();
		if(valueR != readValueR) {
			setValueR(readValueR);
			wpFZ.DebugcheckSubscribes(mqttTopicValueR, String(valueR));
		}
	}
	if(strcmp(topic, mqttTopicValueG.c_str()) == 0) {
		uint8 readValueG = msg.toInt();
		if(valueG != readValueG) {
			setValueG(readValueG);
			wpFZ.DebugcheckSubscribes(mqttTopicValueG, String(valueG));
		}
	}
	if(strcmp(topic, mqttTopicValueB.c_str()) == 0) {
		uint8 readValueB = msg.toInt();
		if(valueB != readValueB) {
			setValueB(readValueB);
			wpFZ.DebugcheckSubscribes(mqttTopicValueB, String(valueB));
		}
	}
	if(strcmp(topic, mqttTopicBrightness.c_str()) == 0) {
		uint8 readBrightness = msg.toInt();
		if(brightness != readBrightness) {
			setBrightness(readBrightness);
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
	mb->checkSubscribes(topic, msg);
}
void moduleNeoPixel::setValueR(uint8 r) {
	valueR = r;
	EEPROM.write(wpEEPROM.byteNeoPixelValueR, valueR);
	EEPROM.commit();
}
uint8 moduleNeoPixel::getValueR() { return valueR; }
void moduleNeoPixel::setValueG(uint8 g) {
	valueG = g;
	EEPROM.write(wpEEPROM.byteNeoPixelValueG, valueG);
	EEPROM.commit();
}
uint8 moduleNeoPixel::getValueG() { return valueG; }
void moduleNeoPixel::setValueB(uint8 b) {
	valueB = b;
	EEPROM.write(wpEEPROM.byteNeoPixelValueB, valueB);
	EEPROM.commit();
}
uint8 moduleNeoPixel::getValueB() { return valueB; }
void moduleNeoPixel::setBrightness(uint8 br) {
	brightness = br;
	EEPROM.write(wpEEPROM.byteNeoPixelBrightness, brightness);
	EEPROM.commit();
}
uint8 moduleNeoPixel::getBrightness() { return brightness; }

String moduleNeoPixel::getStrip() {
	String returns = "{";
	for(uint i = 0; i < pixelCount; i++) {
		uint32_t c = strip->getPixelColor(i);
		uint8_t r = c >> 16;
		uint8_t g = c >> 8;
		uint8_t b = c;
		returns += "\"p" + String(i) + "\"={"
			"\"r\"=\"" + String(r) + "\","
			"\"g\"=\"" + String(g) + "\","
			"\"b\"=\"" + String(b) + "\""
			"},";
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
	wpMqtt.mqttClient.publish(mqttTopicValueR.c_str(), String(valueR).c_str());
	wpMqtt.mqttClient.publish(mqttTopicValueG.c_str(), String(valueG).c_str());
	wpMqtt.mqttClient.publish(mqttTopicValueB.c_str(), String(valueB).c_str());
	wpMqtt.mqttClient.publish(mqttTopicBrightness.c_str(), String(brightness).c_str());
	if(mb->sendRest) {
		wpRest.error = wpRest.error | !wpRest.sendRestRGB(valueR, valueG, valueB, brightness);
		wpRest.trySend = true;
	}
	valueRLast = valueR;
	valueGLast = valueG;
	valueBLast = valueB;
	brightnessLast = brightness;
	if(wpMqtt.Debug) {
		printPublishValueDebug("NeoPixel Value R", String(valueR), String(publishCountValue));
		printPublishValueDebug("NeoPixel Value G", String(valueG), String(publishCountValue));
		printPublishValueDebug("NeoPixel Value B", String(valueB), String(publishCountValue));
		printPublishValueDebug("NeoPixel Brightness", String(brightness), String(publishCountValue));
	}
	mb->cycleCounter = 0;
	publishCountValue = 0;
}

void moduleNeoPixel::calc() {
	unsigned long currentMillis = millis();                     //  Update current time
	if(demoMode) {
		if((currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired time
			patternPrevious = currentMillis;
			if(++modeCurrent > 9)
				modeCurrent = 1;
		}
	}
	if(currentMillis - pixelPrevious >= pixelInterval) {        //  Check for expired time
		pixelPrevious = currentMillis;                            //  Run current frame
		switch (modeCurrent) {
			case ModeColorWipeRed:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "colorWipe Red");
				ColorWipeEffect(strip->Color(255, 0, 0), 50); // Red
				break;
			case ModeColorWipeGreen:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "colorWipe Green");
				ColorWipeEffect(strip->Color(0, 255, 0), 50); // Green
				break;
			case ModeColorWipeBlue:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "colorWipe Blue");
				ColorWipeEffect(strip->Color(0, 0, 255), 50); // Blue
				break;
			case ModeTheaterChaseWhite:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "theaterChase White");
				TheaterChaseEffect(strip->Color(127, 127, 127), 50); // White
				break;
			case ModeTheaterChaseRed:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "theaterChase Red");
				TheaterChaseEffect(strip->Color(127, 0, 0), 50); // Red
				break;
			case ModeTheaterChaseGreen:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "theaterChase Green");
				TheaterChaseEffect(strip->Color(0, 127, 0), 50); // Green
				break;
			case ModeTheaterChaseBlue:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "theaterChase Blue");
				TheaterChaseEffect(strip->Color(0, 0, 127), 50); // Blue
				break;
			case ModeRainbow:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "rainbow");
				RainbowEffect(10); // Flowing rainbow cycle along the whole strip
				break;
			case ModeTheaterChaseRainbow:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "theaterChaseRainbow");
				TheaterChaseRainbowEffect(50); // Rainbow-enhanced theaterChase variant
				break;
			case ModeRunnerRed:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "Runner Red");
				RunnerEffect(strip->Color(127, 0, 0), 50); // Runner Red
				break;
			case ModeRunnerGreen:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "Runner Green");
				RunnerEffect(strip->Color(0, 127, 0), 50); // Runner Green
				break;
			case ModeRunnerBlue:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "Runner Blue");
				RunnerEffect(strip->Color(0, 0, 127), 50); // Runner Blue
				break;
			case ModeRandom:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "Random");
				RandomEffect(500); // Random
				break;
			case ModeComplex:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "Complex");
				break;
			default:
				if(modeCurrentLast != modeCurrent)
					wpMqtt.mqttClient.publish(mqttTopicModeName.c_str(), "Mode Static");
				SimpleEffect(valueR, valueG, valueB, brightness);
				break;
		}
		modeCurrentLast = modeCurrent;
	}
}
void moduleNeoPixel::printPublishValueDebug(String name, String value, String publishCount) {
	String logmessage = "MQTT Send '" + name + "': " + value + " (" + publishCount + " / " + wpFZ.publishQoS + ")";
	wpFZ.DebugWS(wpFZ.strDEBUG, "publishInfo", logmessage);
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
	if(current_pixel >= pixelNumber) {            //  Loop the pattern from the first LED
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

	for(int c = current_pixel; c < pixelNumber; c += 3) {
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
	for(uint16_t i=0; i < pixelNumber; i++) {
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
	for(int i=0; i < pixelNumber; i+=3) {
		strip->setPixelColor(i + pixelQueue, Wheel((i + pixelCycle) % 255)); //  Update delay time  
	}
	strip->show();
	for(int i=0; i < pixelNumber; i+=3) {
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
	if(current_pixel >= pixelNumber) {            //  Loop the pattern from the first LED
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
	setBrightness(br);
	strip->setBrightness(br);
	SimpleEffect(r, g, b);
}
void moduleNeoPixel::SimpleEffect(byte r, byte g, byte b) {
	setValueR(r);
	setValueG(g);
	setValueB(b);
	demoMode = false;
	modeCurrent = ModeStatic;
	strip->fill(strip->Color(r, g, b));
	strip->show();
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
	String SVN = "$Rev: 176 $";
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
uint8 moduleNeoPixel::MaxCycle(){
	return mb->maxCycle / (1000 / wpFZ.loopTime);
}
uint8 moduleNeoPixel::MaxCycle(uint8 maxCycle){
	mb->maxCycle = maxCycle;
	return 0;
}
