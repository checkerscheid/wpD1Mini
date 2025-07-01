//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 19.12.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 233                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: IModuleBase.h 233 2024-12-19 15:28:34Z                   $ #
//#                                                                                 #
//###################################################################################
#ifndef IModuleBase_h
#define IModuleBase_h
#include <Arduino.h>
class IModuleBase {
	public:
		virtual ~IModuleBase() {}
		virtual void init() = 0;
		virtual void cycle() = 0;
		virtual uint16_t getVersion() = 0;
		virtual void publishSettings() = 0;
		virtual void publishSettings(bool force) = 0;
		virtual void publishValues() = 0;
		virtual void publishValues(bool force) = 0;
		virtual void setSubscribes() = 0;
		virtual void checkSubscribes(char* topic, String msg) = 0;
		virtual void changeDebug() = 0;
		virtual String GetJsonSettings() = 0;
		bool Debug();
		bool Debug(bool debug);
	private:
		virtual void calc() = 0;
};

#endif
