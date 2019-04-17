#include <AxeFxControl.h>

AxeSystem Axe;

unsigned long time;

void setup() {
	
	Serial.begin(9600);
	
	Axe.registerPresetChangeCallback(onPresetChange);
	Axe.registerSceneNameCallback(onSceneName);
	Axe.fetchAllScenes(true);
	Axe.begin(Serial1);
	// Axe.requestPresetDetails();	

}

void loop() {
	Axe.update();
}

void onPresetChange(AxePreset preset) {

	Serial.println("\n\n=====================");
	Serial.print("Preset: ");
	Serial.println(preset.getPresetNumber());
	preset.printPresetName(Serial, true);

	Serial.print("\nScene: ");
	Serial.println(preset.getSceneNumber());
	preset.printSceneName(Serial, true);
	Serial.println("=====================\n\n");

}

void onSceneName(const SceneNumber number, const char* name, const byte length) {
	Serial.print("Extra Scene: ");
	Serial.print(number);
	Serial.print(": ");
	Serial.println(name);
}