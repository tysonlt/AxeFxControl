#pragma once

#include <Arduino.h>

typedef byte LooperStatus;

enum LooperButton {
	LooperRecord,
	LooperPlay,
	LooperUndo,
	LooperOnce,
	LooperReverse,
	LooperHalfSpeed
};

class AxeSystem;
class AxeLooper {

	public:

		void setStatus(LooperStatus status) { _status = status; }
		LooperStatus getStatus() { return _status; }

		bool isRecord() 		{ return _status & LOOPER_RECORD; }
		bool isPlay() 			{ return _status & LOOPER_PLAY; }
		bool isOverdub() 		{ return _status & LOOPER_OVERDUB; }
		bool isOnce() 			{ return _status & LOOPER_ONCE; }
		bool isReverse() 		{ return _status & LOOPER_REVERSE; }
		bool isHalfSpeed() 	{ return _status & LOOPER_HALF_SPEED; }

		//TODO: toggle
		void record();
		void play();
		void undo();
		void once();
		void reverse(); 
		void halfSpeed();

		void setAxeSystem(AxeSystem *axe) { _axe = axe; }

	private:

		const static byte LOOPER_RECORD			= 1 << 0;
		const static byte LOOPER_PLAY				= 1 << 1;
		const static byte LOOPER_OVERDUB		= 1 << 2;
		const static byte LOOPER_ONCE				= 1 << 3;
		const static byte LOOPER_REVERSE		= 1 << 4;
		const static byte LOOPER_HALF_SPEED	= 1 << 5;

		void pressButton(LooperButton);

		LooperStatus _status;
		AxeSystem *_axe = nullptr;

};