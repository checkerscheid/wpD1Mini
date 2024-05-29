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
#include <wpUpdate.h>

wpUpdate::wpUpdate() {

}
void wpUpdate::check() {

}
void wpUpdate::start() {
	WiFiClient client;
	ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

	// Add optional callback notifiers
	ESPhttpUpdate.onStart(started);
	ESPhttpUpdate.onEnd(finished);
	ESPhttpUpdate.onProgress(progress);
	ESPhttpUpdate.onError(error);

	t_httpUpdate_return ret = ESPhttpUpdate.update(client, server);
	// Or:
	// t_httpUpdate_return ret = ESPhttpUpdate.update(client, "server", 80, "file.bin");

	switch (ret) {
		case HTTP_UPDATE_FAILED: Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str()); break;

		case HTTP_UPDATE_NO_UPDATES: Serial.println("HTTP_UPDATE_NO_UPDATES"); break;

		case HTTP_UPDATE_OK: Serial.println("HTTP_UPDATE_OK"); break;
	}
}
void wpUpdate::started() {
	Serial.println("CALLBACK:  HTTP update process started");
}
void wpUpdate::finished() {
	Serial.println("CALLBACK:  HTTP update process finished");
}
void wpUpdate::progress(int cur, int total) {
	Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}
void wpUpdate::error(int err) {
	Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}
