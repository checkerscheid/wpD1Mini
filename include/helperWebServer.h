//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 29.05.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 118                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: main.h 118 2024-05-29 01:29:33Z                          $ #
//#                                                                                 #
//###################################################################################
#ifndef helperWebServer_h
#define helperWebServer_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <helperWiFi.h>
#include <helperMqtt.h>
class helperWebServer {
	public:
		
		static const int8_t WebServerCommanddoNothing = -1;
		static const int8_t WebServerCommandblink = 1;
		static const int8_t WebServerCommandpublishSettings = 2;
		static const int8_t WebServerCommandupdateFW = 3;
		static const int8_t WebServerCommandrestartESP = 4;
		static const int8_t WebServerCommandscanWiFi = 5;
		static int8_t doWebServerCommand;
		static int8_t doWebServerBlink;

		helperWebServer();
		void loop();
		uint16_t getVersion();

		void setupWebServer();
		static void setWebServerCommand(int8_t command);
		static void setWebServerBlink();
		void doTheWebServerCommand();
		void doTheWebserverBlink();
	private:
		String SVNh = "$Rev: 118 $";
};
extern helperWebServer wpWebServer;
#endif