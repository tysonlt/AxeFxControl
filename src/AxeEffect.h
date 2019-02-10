#pragma once

#include <Arduino.h>

#include "AxeEffectEnum.h"

typedef byte Channel;
typedef unsigned EffectId;

class AxeEffect {

	public:

		void setEffectId(EffectId effectId) { _effectId = effectId; }
		EffectId getEffectId() { return _effectId; }

		void copyEffectName(char *buffer, size_t max);
		void copyEffectTag(char *buffer, size_t max);

		void setChannel(Channel channel) { _channel = channel; }
		Channel getChannel() { return _channel; }

		char getChannelChar();

		void setChannelCount(byte count) { _numChannels = count; }
		byte getChannelCount() { return _numChannels; }
		
		bool isBypassed() { return _bypassed; }
		void setBypassed(bool bypassed) { _bypassed = bypassed; }
		bool isSwitchable();

		bool isDrive();
		bool isDelay();
		bool isReverb();

	private:

		void copyEffectNameAndTag(EffectId effectId, char *name, byte szName, char *tag, byte szTag);

		EffectId _effectId;
		Channel _channel;
		byte _numChannels;
		bool _bypassed;

};