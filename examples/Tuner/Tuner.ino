#include <AxeFxControl.h>

/**
 * Turn realtime sysex on for this to work.
 */ 

AxeSystem Axe;
unsigned long timer;

void setup() {
	timer = millis();
	Axe.registerTunerDataCallback(onTunerData);
	Axe.registerTunerStatusCallback(onTunerStatus);
}

void loop() {

	Axe.update();

	//don't use delay in the main loop or we will miss updates from axe
	unsigned long now = millis();
	if (now - timer > 3000) {
		Axe.toggleTuner();
		timer = now;
	}

}

void onTunerData(const char *note, const byte string, const byte fineTune) {
	char buf[50];
	snprintf(buf, 50, "TUNING: note=%s string=%d fineTune=%d", note, string, fineTune);
	Serial.println(buf);
}

void onTunerStatus(bool engaged) {
	char buf[20];
	snprintf(buf, 20, "TUNER STATUS: %s", engaged ? "ON" : "OFF");
	Serial.println(buf);
}