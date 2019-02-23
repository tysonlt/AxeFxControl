#pragma once

const static EFFECT_ID_ FIRST_EFFECT = ID_COMP1;
const static char ASCII_ZERO = 0x30;

friend class AxePreset;
friend class AxeSystem;

void setAxeSystem(AxeSystem *axe) { _axe = axe; }
void setBypassed(bool bypassed) { _bypassed = bypassed; }
void setChannelCount(byte count) { _numChannels = count; }
void setChannel(Channel channel) { _channel = channel; }
void setEffectId(EffectId effectId) { _effectId = effectId; }

EffectId _effectId;
Channel _channel;
byte _numChannels;
bool _bypassed;

byte offset();
byte hash();
byte blockNum();
char blockNumChar();

AxeSystem *_axe = nullptr;