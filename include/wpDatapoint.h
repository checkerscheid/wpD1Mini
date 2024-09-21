//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 21.09.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 183                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleBM.h 183 2024-07-29 03:32:26Z                      $ #
//#                                                                                 #
//###################################################################################
#ifndef wpDatapoint_h
#define wpDatapoint_h
#include <Arduino.h>

class wpDatapoint {
	public:
		wpDatapoint(const String* name);
	private:
		String _name;
		String _topic;
};

#endif