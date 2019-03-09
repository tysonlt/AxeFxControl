#include <AxeFxControl.h>

AxeSystem Axe;

unsigned long time;

void setup() {
	Serial.begin(9600);
	Axe.registerPresetChangingCallback(onPresetChanging);
	Axe.registerPresetNameCallback(onPresetName);
	Axe.begin(Serial1);
	time = millis();
}

void loop() {

	static PresetNumber preset = 0;
	if (preset >= AxeSystem::MAX_PRESETS) {
		preset = 0;
	}

	if (millis() - time > 1000) {	
		Serial.print("Changing to ");
		Serial.println(preset);
		Axe.sendPresetChange(preset);
		preset += 11;
		time = millis();
	}

	Axe.update();

}

void onPresetChanging(PresetNumber preset) {
	Serial.print(preset);
	Serial.print(": ");
}

void onPresetName(PresetNumber number, const char *name, const byte length) {
	Serial.println(name);
}