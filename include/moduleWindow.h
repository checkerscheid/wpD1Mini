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
//# Revision     : $Rev:: 214                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleWindow.h 214 2024-10-17 10:17:02Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleWindow_h
#define moduleWindow_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleWindow {
	public:
		moduleWindow(uint8 n);
		moduleBase* mb;
		uint8 Pin;

		// section for define
		bool bm;
		String mqttTopicBM;

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
		// getter / setter
		bool Debug();
		bool Debug(bool debug);
	private:
		uint8 no;
		bool bmLast;
		unsigned long publishBMLast;
		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 214 $";

		uint8 bitDebug;
		byte bitsDebug;
		uint16 addrDebug;

};
extern moduleWindow wpWindow1;
extern moduleWindow wpWindow2;
extern moduleWindow wpWindow3;

#endif
