#include <AxeFxControl.h>
#include <AxeEffect.h>

AxeSystem Axe;

void setup() {

	Serial.begin(9600);
	Axe.begin(Serial1);

	Axe.registerEffectsReceivedCallback(onEffectsReceived);
	Axe.registerEffectFilterCallback(onEffectFilter);

	Axe.enableRefresh(3000);
	Axe.requestPresetDetails();
	
}

void loop() {
	Axe.update();
}

bool onEffectFilter(const PresetNumber number, const EffectId effectId, const bool effectPypassed) {

	//only show drives, reverbs, and delays
	return AxeEffect::isDrive(effectId) || AxeEffect::isDelay(effectId) || AxeEffect::isReverb(effectId);

}

void onEffectsReceived(PresetNumber number, AxePreset preset) {

	const size_t tagSz = 10;
	char tag[tagSz];
	const size_t sz = 25; 
	char buf[sz]; 

	snprintf(buf, sz, "Effects[%d]: ", preset.getEffectCount());
	Serial.println(buf);

	for (EffectIndex effect = 0; effect < preset.getEffectCount(); effect++) {
		preset.copyEffectTag(effect, tag, tagSz);
		char engaged = preset.isEffectBypassed(effect) ? ' ' : 'X';
		snprintf(buf, sz, "%s [%c]", tag, engaged);
		Serial.println(buf);
	}

}