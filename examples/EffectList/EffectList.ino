#include <AxeFxControl.h>

AxeSystem Axe;
bool presetRequested = false;

void setup() {

	Serial.begin(9600);
	while (!Serial);

	//init AxeSystem
	Axe.begin(Serial1);

	//register a callback so we know when preset details are available
	Axe.registerPresetChangeCallback(onPresetChange);

	//ask for refresh call
	Axe.enableRefresh(5000);

	//ask for effects to be fetched with preset
	Axe.fetchEffects(true);
	
	//request current preset
	Axe.requestPresetDetails();

}

void loop() {
	Axe.update();
}

void onPresetChange(AxePreset preset) {

	const size_t sz = 33; 
	char buf[sz]; 

	preset.copyPresetName(buf, sz);
	Serial.println(buf);

	const size_t tagSz = 10;
	char tag[tagSz];

	snprintf(buf, sz, "Effects[%d]: ", preset.getEffectCount());
	Serial.println(buf);

	for (EffectIndex index = 0; index < preset.getEffectCount(); index++) {
    AxeEffect effect = preset.getEffectAt(index);
		effect.copyEffectTag(tag, tagSz);
		char engaged = effect.isBypassed() ? ' ' : 'X';
		snprintf(buf, sz, "%s(ch:%c) [%c]", tag, effect.getChannelChar(), engaged);
		Serial.println(buf);
	}

}
