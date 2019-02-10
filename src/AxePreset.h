#pragma once

#include <Arduino.h>
#include "AxeEffect.h"

typedef byte EffectIndex;

class AxePreset {

	friend class AxeSystem;

	public:

		int getPresetNumber() { return _preset; }
		int getSceneNumber() { return _scene; }
		
		void copyPresetName(char *buffer, size_t max);
		void copySceneName(char *buffer, size_t max);

		unsigned getEffectCount() { return _effectCount; }
		AxeEffect getEffectAt(const EffectIndex index) { return _effects[index]; }

	protected:

		void setPresetNumber(int number) {  _preset = number; }
		void setSceneNumber(int number) {  _scene = number; }
		void setPresetName(char *buffer);
		void setSceneName(char *buffer);
		void reset();	
		bool isComplete();

	public:
	
		const static byte MAX_PRESET_NAME 	= 32;
		const static byte MAX_SCENE_NAME 		= 32;
		const static byte MAX_EFFECTS 			= 50;
		const static byte MAX_EFFECT_NAME 	= 20;
		const static byte MAX_EFFECT_TAG 		= 5;

	private:

		const static byte EFFECT_NAME_SIZE 	= 21;
		const static byte EFFECT_TAG_SIZE 	= 6;

		void setEffects(const AxeEffect effects[], const unsigned count);

		int _scene = -1;
		int _preset = -1;
		int _effectCount = -1;	
		bool _presetNameReceived = false, _sceneNameReceived = false;
		char _presetName[MAX_PRESET_NAME + 1];
		char _sceneName[MAX_SCENE_NAME + 1];
		AxeEffect _effects[MAX_EFFECTS];
		
};

