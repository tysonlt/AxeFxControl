#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {

	//note that this won't work on a Nano, it uses Serial for MIDI
	Serial.begin(9600);
	while (!Serial);

	//Axe.begin(Serial1, AxeSystem::MIDI_CHANNEL_OMNI); //same as...
	Axe.begin(Serial1);

	//register a callback so we know when preset details are available
	Axe.registerPresetChangeCallback(onPresetChange);

	//request current preset
	Axe.requestPresetDetails();

}

void loop() {
	Axe.update();
}

void onPresetChange(AxePreset preset) {

	const size_t sz = 150; 
	char buf[sz]; 

	//NOTE! Preset/scene names with % in them will not print to debug properly

	//AxeSystem has notified that all requested information has arrived!
	Serial.println();
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
