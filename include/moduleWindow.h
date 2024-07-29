//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 18.06.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 183                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleWindow.h 183 2024-07-29 03:32:26Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleWindow_h
#define moduleWindow_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleWindow {
	public:
		moduleWindow();
		moduleBase* mb;
		uint8 Pin;

		// section for define
		bool bm;
		uint16 threshold = 500;
		String lightToTurnOn = "_";
		String mqttTopicBM;
		String mqttTopicThreshold;
		String mqttTopicLightToTurnOn;

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
	private:
		bool bmLast;
		unsigned long publishBMLast;
		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 183 $";

};
extern moduleWindow wpWindow;

#endif
