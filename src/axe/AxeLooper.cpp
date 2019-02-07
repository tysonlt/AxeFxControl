#include "AxeLooper.h"
#include "AxeSystem.h"

void AxeLooper::record() 		{ addStatus(LOOPER_RECORD); }
void AxeLooper::play() 			{ addStatus(LOOPER_PLAY); }
void AxeLooper::overdub() 	{ addStatus(LOOPER_OVERDUB); }
void AxeLooper::once() 			{ addStatus(LOOPER_ONCE); }
void AxeLooper::reverse() 	{ addStatus(LOOPER_REVERSE); }
void AxeLooper::halfSpeed() { addStatus(LOOPER_HALF_SPEED); }

void AxeLooper::addStatus(LooperStatus status) {
	_status |= status;
	_axe->setLooperStatus(_status);
	_axe->requestLooperStatus();
}