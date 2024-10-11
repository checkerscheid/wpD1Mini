//###################################################################################
//#                                                                                 #
//#                (C) FreakaZone GmbH                                              #
//#                =======================                                          #
//#                                                                                 #
//###################################################################################
//#                                                                                 #
//# Author       : Christian Scheid                                                 #
//# Date         : 08.10.2024                                                       #
//#                                                                                 #
//# Revision     : $Rev:: 207                                                     $ #
//# Author       : $Author::                                                      $ #
//# File-ID      : $Id:: moduleRpm.h 207 2024-10-07 12:59:22Z                     $ #
//#                                                                                 #
//###################################################################################
#ifndef moduleSML_h
#define moduleSML_h
#include <Arduino.h>
#include <wpFreakaZone.h>
#include <moduleBase.h>
#include <ehz_bin.h>
#include <sml.h>

#define MAX_STR_MANUF 5

class moduleSML {
	public:
		moduleSML();
		moduleBase* mb;

		// section for define

		// values
		// settings

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

		static void Manufacturer();
		static void PowerT1();
		static void PowerSum();
		static void PowerW();
	private:
		void publishValue();
		void calc();


		// section to config and copy
		String ModuleName;
		String SVNh = "$Rev: 207 $";

		
		static unsigned char manuf[MAX_STR_MANUF];
		static double T1Wh;
		static double SumWh;
		static double Watt;
};
extern moduleSML wpSML;

typedef struct {
	const unsigned char OBIS[6];
	void (*Handler)();
} OBISHandler;

OBISHandler OBISHandlers[] = {
	{{ 0x81, 0x81, 0xc7, 0x82, 0x03, 0xff }, &wpSML.Manufacturer}, /* 129-129:199.130.3*255 */
	{{ 0x01, 0x00, 0x01, 0x08, 0x01, 0xff }, &wpSML.PowerT1},      /*   1-  0:  1.  8.1*255 (T1) */
	{{ 0x01, 0x00, 0x01, 0x08, 0x00, 0xff }, &wpSML.PowerSum},     /*   1-  0:  1.  8.0*255 (T1 + T2) */
	{{ 0x01, 0x00, 0x0F, 0x07, 0x00, 0xff }, &wpSML.PowerW},       /*   1-  0: 15.  7.0*255 (Watt) */
	{{ 0, 0 }}
};

#endif