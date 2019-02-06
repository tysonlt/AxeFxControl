#pragma once

#include <Arduino.h>
#include "AxeEffect.h"

typedef unsigned EffectId;
typedef byte EffectIndex;

class AxePreset{

	public:

		int getPresetNumber() { return _preset; }
		int getSceneNumber() { return _scene; }

		void setPresetNumber(int number); 
		void setSceneNumber(int number); 

		void copyPresetName(char *buffer, size_t max);
		void copySceneName(char *buffer, size_t max);

		void setPresetName(char *buffer);
		void setSceneName(char *buffer);

		unsigned getEffectCount() { return _effectCount; }
		void setEffects(const EffectId effectIdList[], const bool bypassedList[], const unsigned count);
		void copyEffectName(EffectIndex index, char *buffer, size_t max);
		void copyEffectTag(EffectIndex index, char *buffer, size_t max);
		bool isEffectSwitchable(EffectIndex index);
		bool isEffectSwitchable(EffectId effectId);
		bool isEffectBypassed(EffectIndex index);

		void reset();	
		bool isComplete();

	public:
	
		const static byte MAX_PRESET_NAME 	= 32;
		const static byte MAX_SCENE_NAME 		= 32;
		const static byte MAX_EFFECTS 			= 15;
		const static byte MAX_EFFECT_NAME 	= 20;
		const static byte MAX_EFFECT_TAG 		= 5;

	// private:

		const static byte EFFECT_NAME_SIZE = 21;
		const static byte EFFECT_TAG_SIZE = 6;

		const static byte STATUS_PRESET_NAME_RECEIVED 	= 1 << 0;
		const static byte STATUS_PRESET_NUMBER_RECEIVED = 1 << 1;
		const static byte STATUS_SCENE_NAME_RECEIVED 		= 1 << 2;
		const static byte STATUS_SCENE_NUMBER_RECEIVED 	= 1 << 3;
		const static byte STATUS_EFFECT_LIST_RECEIVED 	= 1 << 4;
		constexpr static byte STATUS_ALL_RECEIVED = STATUS_PRESET_NAME_RECEIVED | 
																								STATUS_PRESET_NUMBER_RECEIVED |
																								STATUS_SCENE_NAME_RECEIVED |
																								STATUS_SCENE_NUMBER_RECEIVED |
																								STATUS_EFFECT_LIST_RECEIVED;

		void copyEffectNameAndTag(EffectId effectId, char *name, byte szName, char *tag, byte szTag);
		bool isSwitchable(EffectId effectId);

		int _scene = -1;
		int _preset = -1;
		char _presetName[MAX_PRESET_NAME+1] = {' '};
		char _sceneName[MAX_SCENE_NAME+1] = {' '};
		int _effectCount = -1;	
		EffectId _effectIdList[MAX_EFFECTS] = {0};
		bool _bypassedList[MAX_EFFECTS] = {false};
		byte status = 0;

};

