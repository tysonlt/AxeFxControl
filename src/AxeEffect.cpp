#include "AxeEffect.h"
#include "AxeSystem.h"

void AxeEffect::bypass() {
	_axe->disableEffect(_effectId);
}
	
void AxeEffect::enable() {
	_axe->enableEffect(_effectId);
}

void AxeEffect::switchChannel(Channel channel) {
	setChannel(channel);
	_axe->setEffectChannel(_effectId, channel);
}

void AxeEffect::copyEffectName(char *buffer, size_t max) { 	
	char tag[0];
	copyEffectNameAndTag(_effectId, buffer, max, tag, 0);
}

void AxeEffect::copyEffectTag(char *buffer, size_t max) { 	
	char name[0];
	copyEffectNameAndTag(_effectId, name, 0, buffer, max);
}

bool AxeEffect::isDrive() { return (_effectId >= ID_FUZZ1 && _effectId <= ID_FUZZ4); }
bool AxeEffect::isDelay() { return (_effectId >= ID_DELAY1 && _effectId <= ID_DELAY4); }
bool AxeEffect::isReverb() { return (_effectId >= ID_REVERB1 && _effectId <= ID_REVERB4); }

char AxeEffect::getChannelChar() {
	return 'A' + _channel;
}

// Thanks to https://forum.fractalaudio.com/members/alberta.7980/ for the hash wizardry

byte AxeEffect::offset(EffectId effectId) {
	// Needed because ID_MIDIBLOCK messes up the nice 
	// blocks-of-four property in the IDS
	return effectId > ID_MIDIBLOCK ? 3 : 0;
}

byte AxeEffect::hashFor(EffectId effectId) {
	return (effectId - FIRST_EFFECT + offset(effectId)) / 4;
}

byte AxeEffect::effectIndex(EffectId effectId) {
	return ((effectId - FIRST_EFFECT + offset(effectId)) % 4) + 1;
}

void AxeEffect::copyEffectNameAndTag(EffectId effectId, char *name, uint8_t szName, char *tag, uint8_t szTag) {    

	byte hash = hashFor(effectId);
	int size = arraySize(__EFFECT_NAME_TABLE) - 1;

	//use 'unknown' for out-of-bounds ids
	if (hash < 0 || hash >= size) {
		hash = size;
	}

	//grab label from progmem
	EffectLabel label;
	memcpy_P(&label, &__EFFECT_NAME_TABLE[hash], sizeof label);

	if (effectId == ID_MIDIBLOCK || hash == size) {
		//MIDI block and 'unknown' have no number to append
		snprintf(name, szName, "%s", label.name);
		snprintf(tag, szTag, "%s", label.tag);
	} else {
		byte num = effectIndex(effectId);
		snprintf(name, szName, "%s%d", label.name, num);
		snprintf(tag, szTag, "%s%d", label.tag, num);
	}

}
