#include <AxeFxControl.h>

AxeSystem Axe;
bool presetRequested = false;

void setup() {

	//note that this won't work on a Nano, it uses Serial for MIDI
	Serial.begin(9600);
	while (!Serial);

	//init AxeSystem
	Axe.init(Serial1);

	//register a callback so we know when preset details are available
	Axe.registerPresetChangeCallback(onPresetChange);

	//request current preset
	Axe.requestPresetDetails();

	//ask for refresh call
	Axe.enableRefresh(5000);

}

void loop() {
	Axe.update();
}

void onPresetChange(AxePreset preset) {

	const size_t sz = 33; 
	char buf[sz]; 

	preset.copyPresetName(buf, sz);
	Serial.println(buf);
	Serial.println("Effects:");
	const size_t tagSz = 10;
	char tag[tagSz];

	for (EffectIndex effect = 0; effect < preset.getEffectCount(); effect++) {
		preset.copyEffectTag(effect, tag, tagSz);
		bool bypassed = preset.isEffectBypassed(effect);
		snprintf(buf, sz, "%s [%c]", tag, bypassed ? ' ' : 'X');
		Serial.println(buf);

		EffectId effectId = preset.getEffectId(effect);
		bypassed ? Axe.enableEffect(effectId) : Axe.disableEffect(effectId);
	}

	Serial.println();

}
