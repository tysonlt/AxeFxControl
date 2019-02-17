#pragma once

#include <Arduino.h>
#include "private/AxeTypes.h"
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

		// Print directly to a stream
		void printPresetName(Print&, bool newline = false);
		void printSceneName(Print&, bool newline = false);

		// Use these methods to get a list of effects and their state.
		// First call getEffectCount() to set up your loop, and then
		// call getEffectAt() to retrieve an individual effect.
		// See AxeEffect.h for what you can do with the effect object.
		unsigned getEffectCount() { return _effectCount; }
		AxeEffect getEffectAt(const EffectIndex index) { return _effects[index]; }

		// Use to determine if the effect list has changed compared to
		// another preset. Useful if deciding whether to reprint etc.
		bool effectsChanged(AxePreset& preset);

		// Determine whether these presets have the same data
		bool equals(AxePreset& compare);

		// Defaults to 50. If you are running short on ram, lower this to 
		// truncate the number of effects that will be read from the Axe.
		// Don't increase it without changing MAX_EFFECTS as well.
		byte getMaxEffects() { return _maxEffects; }
		void setMaxEffects(byte max) { _maxEffects = max; }

		//Our internal limit
		const static byte MAX_EFFECTS			= 50;

		// These maxiumums are set by the Axe.
		const static byte MAX_PRESET_NAME	= 32;
		const static byte MAX_SCENE_NAME	= 32;

	private:
		#include "private/AxePreset_Private.h"		
		
};

