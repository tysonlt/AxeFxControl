#include "interface/AxePreset.h"

void AxePreset::printPresetName(Print& printer, bool newline) {
	printer.print(_presetName);
	if (newline) {
		printer.println();
	}
}

void AxePreset::printSceneName(Print& printer, bool newline) {
	printer.print(_sceneName);
	if (newline) {
		printer.println();
	}
}

void AxePreset::setPresetName(char *buffer) {
	snprintf(_presetName, MAX_PRESET_NAME+1, buffer);
	_presetNameReceived = true;
}

void AxePreset::setSceneName(char *buffer) {
	snprintf(_sceneName, MAX_SCENE_NAME+1, buffer);
	_sceneNameReceived = true;
}

void AxePreset::setEffects(const AxeEffect effects[], const unsigned count) {
	_effectCount = 0;
	for (unsigned i=0; i < count && i < _maxEffects; i++) {
		_effects[_effectCount++] = effects[i];
	}
}

void AxePreset::copyPresetName(char *buffer, size_t max) {
	snprintf(buffer, max, _presetName);
}

void AxePreset::copySceneName(char *buffer, size_t max) {
	snprintf(buffer, max, _sceneName);
}

void AxePreset::reset() {
	_scene = -1;
	_preset = -1; 
	_effectCount = -1;	
	_presetNameReceived = false;
	_sceneNameReceived = false;
	memset(_presetName, 0, MAX_PRESET_NAME);
	memset(_sceneName, 0, MAX_SCENE_NAME);
}

bool AxePreset::isComplete() {
	return 
		_preset != -1 && 
		_scene != -1 && 
		_effectCount != -1 && 
		_presetNameReceived &&
		_sceneNameReceived; 
}

