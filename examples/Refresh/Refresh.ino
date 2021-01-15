#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {

	Serial.begin(9600);
	// while (!Serial);

	//Axe.begin(Serial1, AxeSystem::MIDI_CHANNEL_OMNI); //same as...
	Axe.begin(Serial1);

	//register a callback so we know when preset details are available
	Axe.registerPresetChangeCallback(onPresetChange);

	//ask for details to be refreshed every 3 seconds
	Axe.enableRefresh();
	
	//ask for effects to be fetched as well
	Axe.fetchEffects(true);

	//request current preset
	Axe.requestPresetDetails();
	
}

void loop() {
	Axe.update();
}

void onPresetChange(AxePreset preset) {

	const size_t sz = 150;
	char buf[sz];

	//Preset/scene names with % in them will not print to debug properly

	//AxeSystem has notified that all requested information has arrived!
	Serial.print("Preset number: ");
	Serial.println(preset.getPresetNumber());

	Serial.print("Preset name: ");
	preset.printPresetName(Serial, true);

	Serial.print("Scene number: ");
	Serial.println(preset.getSceneNumber());

	Serial.print("Scene name: ");
	preset.printSceneName(Serial, true);

	Serial.print("Looper status: ");
	snprintf(buf, sz, 
		"record[%d] play[%d] overdub[%d] once[%d] reverse[%d] halfspeed[%d]",
		Axe.getLooper().isRecord(),
		Axe.getLooper().isPlay(),
		Axe.getLooper().isOverdub(),
		Axe.getLooper().isOnce(),
		Axe.getLooper().isReverse(),
		Axe.getLooper().isHalfSpeed()
	);
	Serial.println(buf);

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
