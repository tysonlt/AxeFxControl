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

void AxeEffect::copyEffectName(char *buffer, byte max) { 	
	char tag[0];
	copyEffectNameAndTag(buffer, max, tag, 0);
}

void AxeEffect::copyEffectTag(char *buffer, byte max) { 	
	char name[0];
	copyEffectNameAndTag(name, 0, buffer, max);
}

// Thanks to https://forum.fractalaudio.com/members/alberta.7980/ for the hash wizardry

byte AxeEffect::offset() {
	// Needed because ID_MIDIBLOCK messes up the nice 
	// blocks-of-four property in the IDs
	return _effectId > ID_MIDIBLOCK ? 3 : 0;
}

byte AxeEffect::hash() {
	byte hash = (_effectId - FIRST_EFFECT + offset()) / 4;
	unsigned last = arraySize(__EFFECT_NAME_TABLE) - 1;
	// Return 'unknown' for hash out of bounds
	if (hash < 0 || hash >= last) {
		hash = last;
	}
	return hash;
}

byte AxeEffect::blockNum() {
	unsigned last = arraySize(__EFFECT_NAME_TABLE) - 1;
	if (_effectId == ID_MIDIBLOCK || hash() == last) {
		return 0;
	}
	return ((_effectId - FIRST_EFFECT + offset()) % 4) + 1;
}

char AxeEffect::blockNumChar() {
	byte num = blockNum();
	return num > 0 ? num + ASCII_ZERO : '\0';
}

void AxeEffect::copyEffectNameAndTag(char *name, uint8_t szName, char *tag, uint8_t szTag) {    
	char c = blockNumChar();
	EffectLabel label;
	memcpy_P(&label, &__EFFECT_NAME_TABLE[hash()], sizeof label);
	snprintf(name, szName, "%s%c", label.name, c);
	snprintf(tag, szTag, "%s%c", label.tag, c);
}

void AxeEffect::printEffectName(Print& printer, bool newline) {
	char c = blockNumChar();
	EffectLabel label;
	memcpy_P(&label, &__EFFECT_NAME_TABLE[hash()], sizeof label);
	printer.print(label.name);
	if (c) printer.print(c);
	if (newline) printer.println();
}

void AxeEffect::printEffectTag(Print& printer, bool newline) {
	char c = blockNumChar();
	EffectLabel label;
	memcpy_P(&label, &__EFFECT_NAME_TABLE[hash()], sizeof label);
	printer.print(label.tag);
	if (c) printer.print(c);
	if (newline) printer.println();
}

char AxeEffect::getChannelChar() {
	return 'A' + _channel;
}

bool AxeEffect::isDrive() { return (_effectId >= ID_FUZZ1 && _effectId <= ID_FUZZ4); }
bool AxeEffect::isDelay() { return (_effectId >= ID_DELAY1 && _effectId <= ID_DELAY4); }
bool AxeEffect::isReverb() { return (_effectId >= ID_REVERB1 && _effectId <= ID_REVERB4); }