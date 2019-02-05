#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {

	//note that this won't work on a Nano, it uses Serial for MIDI
	Serial.begin(9600);
	// while (!Serial);

	Axe.init();
	
	//register a callback so we know when preset details are available
	Axe.registerPresetChangeCallback(onPresetChange);

	//ask for details of the current preset
	// Axe.requestPresetDetails();

}


void loop() {
	Axe.update();
}

void onPresetChange(AxePreset *preset) {

	const size_t sz = 33;
	char buf[sz];

	//AxeSystem has notified that all requested information has arrived!
	Serial.print("Preset number: ");
	Serial.println(preset->getPresetNumber());

	Serial.print("Preset name: |");
	preset->copyPresetName(buf, sz);
	Serial.print(buf);
	Serial.println("|");

	Serial.print("Scene number: ");
	Serial.println(preset->getSceneNumber());

	Serial.print("Scene name: |");
	preset->copySceneName(buf, sz);
	Serial.print(buf);
	Serial.println("|");

	Serial.println("Effects:");
	const size_t tagSz = 10;
	char tag[tagSz];
	for (byte effect = 0; effect < preset->getEffectCount(); effect++) {
		preset->copyEffectTag(effect, tag, tagSz);
		char engaged = preset->isEffectBypassed(effect) ? ' ' : 'X';
		snprintf(buf, sz, "%s [%c]", tag, engaged);
		Serial.println(buf);
	}

}
