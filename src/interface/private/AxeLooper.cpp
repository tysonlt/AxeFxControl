#include "interface/AxeLooper.h"
#include "interface/AxeSystem.h"

void AxeLooper::record() 		{ pressButton(LooperRecord); }
void AxeLooper::play() 			{ pressButton(LooperPlay); }
void AxeLooper::undo() 			{ pressButton(LooperUndo); }
void AxeLooper::once() 			{ pressButton(LooperOnce); }
void AxeLooper::reverse() 	{ pressButton(LooperReverse); } 
void AxeLooper::halfSpeed() { pressButton(LooperHalfSpeed); }

void AxeLooper::pressButton(LooperButton button) {
	_axe->pressLooperButton(button);
	_axe->requestLooperStatus();
}