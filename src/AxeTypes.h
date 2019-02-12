#pragma once

#include <Arduino.h>

typedef byte Channel;
typedef unsigned EffectId;
typedef int16_t PresetNumber;
typedef int8_t SceneNumber;
typedef byte EffectIndex;
typedef byte Tempo;
typedef unsigned long millis_t;
typedef byte LooperStatus;

struct EffectLabel {
	const char* name;
	const char* tag;
};

struct Version {
	byte major, minor;
};

enum LooperButton {
	LooperRecord,
	LooperPlay,
	LooperUndo,
	LooperOnce,
	LooperReverse,
	LooperHalfSpeed
};

class AxeEffect;
class AxePreset;
class AxeLooper;
class AxeSystem;

template<typename T, size_t N>
size_t arraySize(T(&)[N]) {
    return N;
}