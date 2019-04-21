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

	Serial.println("\nonPresetChange()\n=====================");
	Serial.print("Preset: ");
	Serial.print(preset.getPresetNumber());
	Serial.print(" - ");
	preset.printPresetName(Serial, true);

	Serial.print("Scene: ");
	Serial.print(preset.getSceneNumber());
	Serial.print(" - ");
	preset.printSceneName(Serial, true);
	Serial.println("=====================\n\n");

}

void onSceneName(const SceneNumber number, const char* name, const byte length) {
	Serial.print("onSceneName(): ");
	Serial.print(number);
	Serial.print(": ");
	Serial.println(name);
}