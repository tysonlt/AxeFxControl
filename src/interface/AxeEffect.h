#pragma once

#include <Arduino.h>
#include "private/AxeTypes.h"
#include "private/AxeEffectEnum.h"
#include "private/AxeEffectLabels.h"

// You can retrieve a list of effects from an AxePreset object.
class AxeEffect {

	public:
		
		// Like in AxePreset, pass a buffer of at least 'max'
		// size to retrieve the full name of the current effect.
		// These names are not official, they are just the enum
		// entry with 'ID_' stripped off. Maybe a contributer
		// can enter the full effect names? :)
		void copyEffectName(char *buffer, byte max);

		// Get the 'tag', or short 4-character label shown in the grid.
		// Pass a buffer of 5 to fit the null byte at the end.
		void copyEffectTag(char *buffer, byte max);

		// Get both
		void copyEffectNameAndTag(char *name, byte szName, char *tag, byte szTag);

		// Print directly to a Print instance
		void printEffectName(Print&, bool newline = false);
		void printEffectTag(Print&, bool newline = false);

		// Most effects support 4 channels.
		byte getChannelCount() { return _numChannels; }

		// Get the current channel, either as a 0-based byte,
		// or as an A-B-C-D char. I like the char option myself.
		Channel getChannel() { return _channel; }
		char getChannelChar();
		
		// Is this effect currently bypassed?
		bool isBypassed() { return _bypassed; }

		// What kind of effect is this? Needs to be updated by
		// a charming contributer to include the full list.
		bool isDrive();
		bool isDelay();
		bool isReverb();

		// Control the state of this effect. Bypass or enable
		// it, or change the channel using a 0-based byte.
		// Eg. 0='A', 3='D'.
		void bypass();
		void enable();
		void toggle();
		void switchChannel(Channel channel);
		void incrementChannel();
		void decrementChannel();

		// This returns the internal EFFECT_ID_ as defined in
		// the enum published by Fractal Audio Systems. You can
		// pass these to some AxeSystem methods, but generally
		// you don't need to worry about this unless you are
		// hacking your own sysex that wasn't published in the spec.
		EffectId getEffectId() { return _effectId; }

		const static byte MAX_EFFECT_NAME	= 20;
		const static byte MAX_EFFECT_TAG	= 5;

	private:
		#include "private/AxeEffect_Private.h"

};