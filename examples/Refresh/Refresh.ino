#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {

	//note that this won't work on a Nano, it uses Serial for MIDI
	Serial.begin(9600);
	// while (!Serial);

	//register a callback so we know when preset details are available
	Axe.registerPresetChangeCallback(onPresetChange);

	//ask for details to be refreshed every 3 seconds
	Axe.enableRefresh(3000);

}

void loop() {
	Axe.update();
}

void onPresetChange(AxePreset preset) {

	const size_t sz = 150;
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

	Serial.print("Looper status: ");
	snprintf(buf, sz, 
		"record[%d] play[%d] overdub[%d] once[%d] reverse[%d] halfspeed[%d]",
		preset.getLooper().isRecord(),
		preset.getLooper().isPlay(),
		preset.getLooper().isOverdub(),
		preset.getLooper().isOnce(),
		preset.getLooper().isReverse(),
		preset.getLooper().isHalfSpeed()
	);
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
