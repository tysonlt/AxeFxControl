#pragma once

#include <Arduino.h>

typedef unsigned EffectId;

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
		void copyEffectName(byte index, char *buffer, size_t max);
		void copyEffectTag(byte index, char *buffer, size_t max);
		bool isEffectSwitchable(byte index);
		bool isEffectBypassed(byte index);

		void reset();	
		bool isComplete();

	public:
	
		const static byte MAX_PRESET_NAME 	= 32;
		const static byte MAX_SCENE_NAME 		= 32;
		const static byte MAX_EFFECTS 			= 15;
		const static byte MAX_EFFECT_NAME 	= 20;
		const static byte MAX_EFFECT_TAG 		= 5;

	private:

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

enum EFFECT_ID_ {
	ID_CONTROL = 2,
	ID_TUNER = 35,
	ID_IRCAPTURE,
	ID_INPUT1,
	ID_INPUT2,
	ID_INPUT3,
	ID_INPUT4,
	ID_INPUT5, // USB Input
	ID_OUTPUT1,
	ID_OUTPUT2,
	ID_OUTPUT3,
	ID_OUTPUT4,
	ID_COMP1,
	ID_COMP2,
	ID_COMP3,
	ID_COMP4,
	ID_GRAPHEQ1,
	ID_GRAPHEQ2,
	ID_GRAPHEQ3,
	ID_GRAPHEQ4,
	ID_PARAEQ1,
	ID_PARAEQ2,
	ID_PARAEQ3,
	ID_PARAEQ4,
	ID_DISTORT1,
	ID_DISTORT2,
	ID_DISTORT3,
	ID_DISTORT4,
	ID_CAB1,
	ID_CAB2,
	ID_CAB3,
	ID_CAB4,
	ID_REVERB1,
	ID_REVERB2,
	ID_REVERB3,
	ID_REVERB4,
	ID_DELAY1,
	ID_DELAY2,
	ID_DELAY3,
	ID_DELAY4,
	ID_MULTITAP1,
	ID_MULTITAP2,
	ID_MULTITAP3,
	ID_MULTITAP4,
	ID_CHORUS1,
	ID_CHORUS2,
	ID_CHORUS3,
	ID_CHORUS4,
	ID_FLANGER1,
	ID_FLANGER2,
	ID_FLANGER3,
	ID_FLANGER4,
	ID_ROTARY1,
	ID_ROTARY2,
	ID_ROTARY3,
	ID_ROTARY4,
	ID_PHASER1,
	ID_PHASER2,
	ID_PHASER3,
	ID_PHASER4,
	ID_WAH1,
	ID_WAH2,
	ID_WAH3,
	ID_WAH4,
	ID_FORMANT1,
	ID_FORMANT2,
	ID_FORMANT3,
	ID_FORMANT4,
	ID_VOLUME1,
	ID_VOLUME2,
	ID_VOLUME3,
	ID_VOLUME4,
	ID_TREMOLO1,
	ID_TREMOLO2,
	ID_TREMOLO3,
	ID_TREMOLO4,
	ID_PITCH1,
	ID_PITCH2,
	ID_PITCH3,
	ID_PITCH4,
	ID_FILTER1,
	ID_FILTER2,
	ID_FILTER3,
	ID_FILTER4,
	ID_FUZZ1,
	ID_FUZZ2,
	ID_FUZZ3,
	ID_FUZZ4,
	ID_ENHANCER1,
	ID_ENHANCER2,
	ID_ENHANCER3,
	ID_ENHANCER4,
	ID_MIXER1,
	ID_MIXER2,
	ID_MIXER3,
	ID_MIXER4,
	ID_SYNTH1,
	ID_SYNTH2,
	ID_SYNTH3,
	ID_SYNTH4,
	ID_VOCODER1,
	ID_VOCODER2,
	ID_VOCODER3,
	ID_VOCODER4,
	ID_MEGATAP1,
	ID_MEGATAP2,
	ID_MEGATAP3,
	ID_MEGATAP4,
	ID_CROSSOVER1,
	ID_CROSSOVER2,
	ID_CROSSOVER3,
	ID_CROSSOVER4,
	ID_GATE1,
	ID_GATE2,
	ID_GATE3,
	ID_GATE4,
	ID_RINGMOD1,
	ID_RINGMOD2,
	ID_RINGMOD3,
	ID_RINGMOD4,
	ID_MULTICOMP1,
	ID_MULTICOMP2,
	ID_MULTICOMP3,
	ID_MULTICOMP4,
	ID_TENTAP1,
	ID_TENTAP2,
	ID_TENTAP3,
	ID_TENTAP4,
	ID_RESONATOR1,
	ID_RESONATOR2,
	ID_RESONATOR3,
	ID_RESONATOR4,
	ID_LOOPER1,
	ID_LOOPER2,
	ID_LOOPER3,
	ID_LOOPER4,
	ID_TONEMATCH1,
	ID_TONEMATCH2,
	ID_TONEMATCH3,
	ID_TONEMATCH4,
	ID_RTA1,
	ID_RTA2,
	ID_RTA3,
	ID_RTA4,
	ID_PLEX1,
	ID_PLEX2,
	ID_PLEX3,
	ID_PLEX4,
	ID_FBSEND1,
	ID_FBSEND2,
	ID_FBSEND3,
	ID_FBSEND4,
	ID_FBRETURN1,
	ID_FBRETURN2,
	ID_FBRETURN3,
	ID_FBRETURN4,
	ID_MIDIBLOCK,
	ID_MULTIPLEXER1,
	ID_MULTIPLEXER2,
	ID_MULTIPLEXER3,
	ID_MULTIPLEXER4,
	ID_IRPLAYER1,
	ID_IRPLAYER2,
	ID_IRPLAYER3,
	ID_IRPLAYER4,
	ID_FOOTCONTROLLER,
	ID_PRESET_FC,
};