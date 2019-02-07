#include <AxeFxControl.h>

AxeSystem Axe;
bool presetRequested = false;

void setup() {

	//note that this won't work on a Nano, it uses Serial for MIDI
	Serial.begin(9600);
	while (!Serial);

	//register a callback so we know when preset details are available
	Axe.registerPresetChangeCallback(onPresetChange);

	//request current preset
	Axe.requestPresetDetails();

}

void loop() {

	Axe.update();

}

void onPresetChange(AxePreset preset) {

	const size_t sz = 33; 
	char buf[sz]; 

	//NOTE! Preset/scene names with % in them will not print to debug properly

	//AxeSystem has notified that all requested information has arrived!
	Serial.print("Preset number: ");
	Serial.println(preset.getPresetNumber());

	Serial.print("Preset name: ");
	preset.copyPresetName(buf, sz);
	Serial.println(buf);

	Serial.print("Scene number: ");
	Serial.println(preset.getSceneNumber());

	Serial.print("Scene name: ");
	preset.copySceneName(buf, sz);
	Serial.println(buf);

	Serial.println("Effects:");
	const size_t tagSz = 10;
	char tag[tagSz];
	for (EffectIndex effect = 0; effect < preset.getEffectCount(); effect++) {
		preset.copyEffectTag(effect, tag, tagSz);
		char engaged = preset.isEffectBypassed(effect) ? ' ' : 'X';
		snprintf(buf, sz, "%s [%c]", tag, engaged);
		Serial.println(buf);
	}

}
