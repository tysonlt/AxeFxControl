#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {
	Serial.begin(9600);
	Axe.registerPresetChangingCallback(onPresetChanging);
	Axe.begin(Serial1, 4);
	//Go to Axe, Setup->MIDI/Remote->Midi Channel=4 to see channel filter in action
	//Otherwise, just remove the channel param above and it will default to OMIN
}

void loop() {

	static PresetNumber preset = 0;
	if (preset >= AxeSystem::MAX_PRESETS) {
		preset = 0;
	}
	
	Axe.sendPresetChange(preset);
	delay(350);
	
	preset += 11;

}

void onPresetChanging(PresetNumber preset) {
	Serial.println(preset);
}