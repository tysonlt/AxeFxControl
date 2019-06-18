#pragma once

friend class AxeSystem;

AxeLooper() {}

const static byte LOOPER_RECORD = 1 << 0;
const static byte LOOPER_PLAY = 1 << 1;
const static byte LOOPER_OVERDUB = 1 << 2;
const static byte LOOPER_ONCE = 1 << 3;
const static byte LOOPER_REVERSE = 1 << 4;
const static byte LOOPER_HALF_SPEED = 1 << 5;

LooperStatus getStatus() { return _status; }
void pressButton(LooperButton);
void setStatus(LooperStatus status) { _status = status; }
void setAxeSystem(AxeSystem *axe) { _axe = axe; }

LooperStatus _status;
AxeSystem *_axe = nullptr;