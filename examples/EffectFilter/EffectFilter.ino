#include <AxeFxControl.h>
#include <AxeEffect.h>

AxeSystem Axe;

void setup() {

	Serial.begin(9600);
	Axe.begin(Serial1);

	Axe.registerEffectsReceivedCallback(onEffectsReceived);
	Axe.registerEffectFilterCallback(onEffectFilter);

	Axe.enableRefresh(3000);
	Axe.fetchEffects(true);

	Axe.requestPresetDetails();
	
}

void loop() {
	Axe.update();
}

bool onEffectFilter(const PresetNumber number, AxeEffect effect) {

	//only show drives, reverbs, and delays
	return effect.isDrive() || effect.isDelay() || effect.isReverb();

}

void onEffectsReceived(PresetNumber number, AxePreset preset) {

	const size_t tagSz = 10;
	char tag[tagSz];
	const size_t sz = 25; 
	char buf[sz]; 

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
