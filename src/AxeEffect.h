#pragma once

#include <Arduino.h>
#include "AxeEffectEnum.h"

typedef byte Channel;
typedef unsigned EffectId;

class AxeSystem;
class AxeEffect {

	friend class AxeSystem;

	public:
		
		EffectId getEffectId() { return _effectId; }

		void copyEffectName(char *buffer, size_t max);
		void copyEffectTag(char *buffer, size_t max);

		byte getChannelCount() { return _numChannels; }
		Channel getChannel() { return _channel; }
		char getChannelChar();
		
		bool isBypassed() { return _bypassed; }
		bool isSwitchable();

		bool isDrive();
		bool isDelay();
		bool isReverb();

		void bypass();
		void enable();
		void switchChannel(Channel channel);

	protected:

		void setAxeSystem(AxeSystem *axe) { _axe = axe; }
		void setBypassed(bool bypassed) { _bypassed = bypassed; }
		void setChannelCount(byte count) { _numChannels = count; }
		void setChannel(Channel channel) { _channel = channel; }
		void setEffectId(EffectId effectId) { _effectId = effectId; }
		void copyEffectNameAndTag(EffectId effectId, char *name, byte szName, char *tag, byte szTag);

	private:

		EffectId _effectId;
		Channel _channel;
		byte _numChannels;
		bool _bypassed;

		AxeSystem *_axe = nullptr;

};