#include <AxeFxControl.h>

#define NUM_SCENES 8

//Struct to hold information about each scene
struct SceneInfo {
	SceneNumber number = -1;
	const char *name;
};

//A list of all of the scenes for this preset
SceneInfo scenes[NUM_SCENES];

AxeSystem Axe;

void setup() {

	Serial.begin(9600);

	Axe.begin(Serial1);
	Axe.registerPresetChangingCallback(onPresetChanging);
	Axe.registerPresetChangeCallback(onPresetChange);
	Axe.registerSceneNameCallback(onSceneName);
	Axe.requestPresetDetails(); 

}

void loop() {
	Axe.update();
}

void onPresetChanging(const PresetNumber number) {
	//Reset the scene list for the new preset
	for (byte i=0; i<NUM_SCENES; i++) {
		scenes[i].number = -1;
	}
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

	if (number == 1) {
		Serial.println("\n++++++++++++++++++++++++\n\n");
	}

	Serial.print("onSceneName(): ");
	Serial.print(number);
	Serial.print(": ");
	Serial.println(name);

	//Record current scene in the list
	scenes[number-1].number = number;
	scenes[number-1].name = name;

	//Request the first scene that we don't have yet.
	//Only request one at a time to avoid filling up RX buffer
	for (byte i=0; i<NUM_SCENES; i++) {
		if (scenes[i].number == -1) {
			Axe.requestSceneName(i+1);
			break;
		}
	}

}