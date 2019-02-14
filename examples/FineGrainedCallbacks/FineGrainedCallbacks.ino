#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {

	Serial.begin(9600);
	Axe.begin(Serial1);

	Axe.registerPresetNameCallback(onPresetName);
	Axe.registerSceneNameCallback(onSceneName);
	Axe.registerEffectsReceivedCallback(onEffectsReceived);

	Axe.enableRefresh(3000);
	Axe.requestPresetDetails();
	
}

void loop() {
	Axe.update();
}

void onPresetName(PresetNumber number, const char *name, const byte length) {
	Serial.print("PRESET NAME: ");
	Serial.println(name);
}

void onSceneName(SceneNumber number, const char *name, const byte length) {
	Serial.print("SCENE NAME:  ");
	Serial.println(name);
}

//NOTE: only rely on the effects, the reset of the preset is not guaranteed to be complete.
void onEffectsReceived(PresetNumber number, AxePreset preset) {

	const size_t sz = 25; 
	char buf[sz]; 

	snprintf(buf, sz, "Effects[%d]: ", preset.getEffectCount());
	Serial.println(buf);

	for (EffectIndex index = 0; index < preset.getEffectCount(); index++) {
    AxeEffect effect = preset.getEffectAt(index);
		effect.printEffectTag(Serial);
		char engaged = effect.isBypassed() ? ' ' : 'X';
		snprintf(buf, sz, "(ch:%c) [%c]", effect.getChannelChar(), engaged);
		Serial.println(buf);
	}
}