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
//# Revision     : $Rev:: 269                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleWindow.h 269 2025-07-01 19:25:14Z                  $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleWindow_h
#define moduleWindow_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>

class moduleWindow : public IModuleBase {
	public:
		moduleWindow(uint8_t n);
		moduleBase* mb;
		uint8_t Pin;

		// section for define
		bool bm;
		String mqttTopicBM;

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
	private:
		uint8_t no;
		bool bmLast;
		unsigned long publishBMLast;
		void publishValue();
		void calc();

		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 269 $";

		uint8_t bitDebug;
		byte bitsDebug;
		uint16_t addrDebug;

};
extern moduleWindow wpWindow1;
extern moduleWindow wpWindow2;
extern moduleWindow wpWindow3;

#endif
