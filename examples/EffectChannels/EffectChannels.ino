#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {

	Serial.begin(9600);
	while (!Serial);

	Axe.begin(Serial1);

	Axe.registerPresetChangeCallback(onPresetChange);
	Axe.requestPresetDetails();

}

void loop() {
	Axe.update();
}

void onPresetChange(AxePreset preset) {
	
	if (preset.getEffectCount() < 1) {
		Serial.println("Please move to a preset with some effect blocks.");
	} else {

    AxeEffect effect = preset.getEffectAt(0);

		Serial.print("Watch the channel for effect ");
		effect.printEffectTag(Serial);
		Serial.println(" changing...");

		while (true) {
			
			effect.incrementChannel();

			Serial.println(effect.getChannelChar());
			delay(1000);
			
		}

	}

}
