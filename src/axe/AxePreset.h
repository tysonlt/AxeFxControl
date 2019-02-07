#pragma once

#include <Arduino.h>
#include "AxeEffect.h"
#include "AxeLooper.h"

typedef byte EffectIndex;

class AxePreset {

	public:

		void setPresetNumber(int number) {  _preset = number; }
		void setSceneNumber(int number) {  _scene = number; }
		int getPresetNumber() { return _preset; }
		int getSceneNumber() { return _scene; }

		void setPresetName(char *buffer);
		void setSceneName(char *buffer);
		void copyPresetName(char *buffer, size_t max);
		void copySceneName(char *buffer, size_t max);

		AxeLooper& getLooper() { return _looper; }

		void setEffects(const EffectId effectIdList[], const bool bypassedList[], const unsigned count);
		void copyEffectName(EffectIndex index, char *buffer, size_t max);
		void copyEffectTag(EffectIndex index, char *buffer, size_t max);
		bool isEffectSwitchable(EffectIndex index);
		bool isEffectBypassed(EffectIndex index);
		unsigned getEffectCount() { return _effectCount; }
		EffectId getEffectId(EffectIndex index) { return _effectIdList[index] ;}

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

		int _scene = -1;
		int _preset = -1;
		char _presetName[MAX_PRESET_NAME+1] = {' '};
		char _sceneName[MAX_SCENE_NAME+1] = {' '};
		int _effectCount = -1;	
		EffectId _effectIdList[MAX_EFFECTS];
		bool _bypassedList[MAX_EFFECTS];
		AxeLooper _looper;

};

