#include "AxePreset.h"

void AxePreset::setPresetName(char *buffer) {
	snprintf(_presetName, MAX_PRESET_NAME+1, buffer);
}

void AxePreset::setSceneName(char *buffer) {
	snprintf(_sceneName, MAX_SCENE_NAME+1, buffer);
}

void AxePreset::setEffects(const EffectId effectIdList[], const bool bypassedList[], const unsigned count) {
	for (unsigned i=0; i < count && i < MAX_EFFECTS; i++) {
		if (AxeEffect::isSwitchable(effectIdList[i])) {
			_effectIdList[_effectCount] = effectIdList[i];
			_bypassedList[_effectCount] = bypassedList[i];
			_effectCount++;
		}
	}
}

void AxePreset::copyPresetName(char *buffer, size_t max) {
	snprintf(buffer, max, _presetName);
}

void AxePreset::copySceneName(char *buffer, size_t max) {
	snprintf(buffer, max, _sceneName);
}

void AxePreset::copyEffectName(EffectIndex index, char *buffer, size_t max) { 	
	char tag[0];
	EffectId effectId = _effectIdList[index];
	AxeEffect::copyEffectNameAndTag(effectId, buffer, max, tag, 0);
}

void AxePreset::copyEffectTag(EffectIndex index, char *buffer, size_t max) { 	
	char name[0];
	EffectId effectId = _effectIdList[index];
	AxeEffect::copyEffectNameAndTag(effectId, name, 0, buffer, max);
}

bool AxePreset::isEffectSwitchable(EffectIndex index) {
	EffectId effectId = _effectIdList[index];
	return AxeEffect::isSwitchable(effectId);
}

bool AxePreset::isEffectBypassed(EffectIndex index) {
	return _bypassedList[index];
}

void AxePreset::reset() {
	_scene = -1;
	_preset = -1;
	_effectCount = -1;	
	memset(_presetName, 0, MAX_PRESET_NAME);
	memset(_sceneName, 0, MAX_SCENE_NAME);
}

bool AxePreset::isComplete() {
	return (_preset != -1 && _scene != -1 && _effectCount != -1); //status & STATUS_ALL_RECEIVED;
}

