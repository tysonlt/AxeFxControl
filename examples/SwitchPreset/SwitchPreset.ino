#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {
	Serial.begin(9600);
	Axe.registerPresetChangingCallback(onPresetChanging);
}

void loop() {

	static unsigned preset = 0;
	if (preset >= AxeSystem::MAX_PRESETS) {
		preset = 0;
	}
	
	Axe.sendPresetChange(preset);
	delay(350);
	
	preset += 11;

}

void onPresetChanging(int preset) {
	Serial.println(preset);
}