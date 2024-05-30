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
#include <helperUpdate.h>

helperUpdate::helperUpdate() {

}
void helperUpdate::check() {

}
void helperUpdate::start() {
	WiFiClient client;
	ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

	// Add optional callback notifiers
	ESPhttpUpdate.onStart(started);
	ESPhttpUpdate.onEnd(finished);
	ESPhttpUpdate.onProgress(progress);
	ESPhttpUpdate.onError(error);

	t_httpUpdate_return ret = ESPhttpUpdate.update(client, wpFZ.updateServer);
	// Or:
	// t_httpUpdate_return ret = ESPhttpUpdate.update(client, "server", 80, "file.bin");

	switch (ret) {
		case HTTP_UPDATE_FAILED:
			wpFZ.DebugWS(wpFZ.strERRROR, "wpUpdate::start", "HTTP_UPDATE_FAILD Error (" +
				String(ESPhttpUpdate.getLastError()) + "): " +
				ESPhttpUpdate.getLastErrorString());
			break;

		case HTTP_UPDATE_NO_UPDATES:
			wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::start", "HTTP_UPDATE_NO_UPDATES");
			break;

		case HTTP_UPDATE_OK:
			wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::start", "HTTP_UPDATE_OK");
			break;
	}
}
void helperUpdate::started() {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::started", "HTTP update started");
}
void helperUpdate::finished() {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::finished", "HTTP update finished");
}
void helperUpdate::progress(int cur, int total) {
	String logmessage = "HTTP update: " + String(cur) + " of " + String(total) + " bytes";
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::progress", logmessage, false);
}
void helperUpdate::error(int err) {
	wpFZ.DebugWS(wpFZ.strINFO, "wpUpdate::error", "HTTP update fatal error, code: " + String(err));
}
