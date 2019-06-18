#pragma once

#include "private/AxeTypes.h"
#include <Arduino.h>

class AxeSystem;

// Ask the AxeSystem for this object. Use it to control and read the looper.
class AxeLooper {

  public:
    // These are your status methods. I don't think they require much explanation.
    bool isRecord() { return _status & LOOPER_RECORD; }
    bool isPlay() { return _status & LOOPER_PLAY; }
    bool isOverdub() { return _status & LOOPER_OVERDUB; }
    bool isOnce() { return _status & LOOPER_ONCE; }
    bool isReverse() { return _status & LOOPER_REVERSE; }
    bool isHalfSpeed() { return _status & LOOPER_HALF_SPEED; }

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

  private:
#include "private/AxeLooper_Private.h"
};