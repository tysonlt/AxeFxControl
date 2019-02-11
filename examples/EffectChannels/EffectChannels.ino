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
	
	const size_t tagSz = 50;
	char tag[tagSz];

	if (preset.getEffectCount() < 1) {
		Serial.println("Please move to a preset with some effect blocks.");
	} else {

    AxeEffect effect = preset.getEffectAt(0);
		effect.copyEffectTag(tag, tagSz);

		Serial.print("Watch the channel for effect ");
		Serial.print(tag);
		Serial.println(" changing...");

		Channel channel = 0;
		while (true) {
			
			effect.switchChannel(channel);
			
			Serial.println(effect.getChannelChar());
			if (++channel >= effect.getChannelCount()) {
				channel = 0;
			}
			delay(1000);
			
		}

	}

}
