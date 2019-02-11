#pragma once

#include <Arduino.h>
#include "AxeTypes.h"

class AxeSystem;

// Ask the AxeSystem for this to control and read the looper.
class AxeLooper {

	public:

		// These are your status methods. I don't think they require much explanation.
		bool isRecord() 		{ return _status & LOOPER_RECORD; }
		bool isPlay() 			{ return _status & LOOPER_PLAY; }
		bool isOverdub() 		{ return _status & LOOPER_OVERDUB; }
		bool isOnce() 			{ return _status & LOOPER_ONCE; }
		bool isReverse() 		{ return _status & LOOPER_REVERSE; }
		bool isHalfSpeed() 	{ return _status & LOOPER_HALF_SPEED; }

		// These are the available virtual 'buttons' you can press.
		// AxeFX 3 doesn't let you directly 'set' the state, you can
		// only simulate pressing the button. Perhaps you could map
		// a CC to directly set state if you needed that.
		void record();
		void play();
		void undo();
		void once();
		void reverse(); 
		void halfSpeed();

	protected:

		friend class AxeSystem;

		AxeLooper() {}

		const static byte LOOPER_RECORD			= 1 << 0;
		const static byte LOOPER_PLAY				= 1 << 1;
		const static byte LOOPER_OVERDUB		= 1 << 2;
		const static byte LOOPER_ONCE				= 1 << 3;
		const static byte LOOPER_REVERSE		= 1 << 4;
		const static byte LOOPER_HALF_SPEED	= 1 << 5;

		LooperStatus getStatus() { return _status; }
		void pressButton(LooperButton);
		void setStatus(LooperStatus status) { _status = status; }
		void setAxeSystem(AxeSystem *axe) { _axe = axe; }

	private:

		LooperStatus _status;
		AxeSystem *_axe = nullptr;

};