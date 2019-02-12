#pragma once

#include <Arduino.h>
#include "AxeTypes.h"
#include "AxeEffect.h"

// This class represents the current preset. You can get it by calling getPreset() on AxeSystem,
// or better yet, register a preset change callback.
class AxePreset {

	public:

		// The current preset and scene numbers.
		PresetNumber getPresetNumber() { return _preset; }
		SceneNumber  getSceneNumber() { return _scene; }
		
		// Call these with a buffer of size 'max' to be 
		// given the preset and scene names.
		void copyPresetName(char *buffer, size_t max);
		void copySceneName(char *buffer, size_t max);

		// Use these methods to get a list of effects and their state.
		// First call getEffectCount() to set up your loop, and then
		// call getEffectAt() to retrieve an individual effect.
		// See AxeEffect.h for what you can do with the effect object.
		unsigned getEffectCount() { return _effectCount; }
		AxeEffect getEffectAt(const EffectIndex index) { return _effects[index]; }

		// Defaults to 50. If you are running short on ram, lower this to 
		// truncate the number of effects that will be read from the Axe.
		// Don't increase it without changing MAX_EFFECTS as well.
		byte getMaxEffects() { return _maxEffects; }
		void setMaxEffects(byte max) { _maxEffects = max; }

	protected:
		
		friend class AxeSystem;
		AxePreset() {}

		// These methods are used by AxeSystem to build the preset.
		// You don't need to call them, and you can't.
		void setPresetNumber(int number) {  _preset = number; }
		void setSceneNumber(int number) {  _scene = number; }
		void setPresetName(char *buffer);
		void setSceneName(char *buffer);
		void reset();	
		bool isComplete();

	public:
	
		// These maxiumums are set by the Axe.
		const static byte MAX_PRESET_NAME	= 32;
		const static byte MAX_SCENE_NAME	= 32;

		//Our internal limit
		const static byte MAX_EFFECTS			= 50;

	private:

		void setEffects(const AxeEffect effects[], const unsigned count);

		int _scene = -1;
		int _preset = -1;
		int _effectCount = -1;	
		byte _maxEffects = MAX_EFFECTS;
		bool _presetNameReceived = false, _sceneNameReceived = false;
		char _presetName[MAX_PRESET_NAME + 1];
		char _sceneName[MAX_SCENE_NAME + 1];
		AxeEffect _effects[MAX_EFFECTS];
		
};

