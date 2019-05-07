#include "interface/AxeSystem.h"

void AxeSystem::update() {

  if (_refreshRate > 0) {
    if (millis() - _lastRefresh > _refreshRate) {
      refresh();
    }
  }

  readMidi();
}

void AxeSystem::enableRefresh(const millis_t millis, const millis_t throttle) {
  _refreshRate = millis;
  _refreshThrottle = throttle;
}

void AxeSystem::refresh(bool ignoreThrottle) {
  unsigned long now = millis();
  if (ignoreThrottle || (!_tunerEngaged && (now - _lastRefresh) > _refreshThrottle)) {
    _lastRefresh = now;
    onPresetChange(_preset.getPresetNumber(), false); //trigger a full read
    requestTempo();
    requestLooperStatus();
  } else {
#ifdef AXE_DEBUG
    DEBUGGER.println(F("refresh(): too soon or tuner engaged, ignoring refresh request"));
#endif
  }
}

void AxeSystem::checkFirmware() {
  if (!_firmwareRequested) {
    requestFirmwareVersion();
    requestLooperStatus();
    _firmwareRequested = true;
  }
}

void AxeSystem::setSystemConnected(const bool connected) {
  bool oldStatus = _systemConnected;
  _systemConnected = connected;
  if (oldStatus != _systemConnected) {
    callConnectionStatusCallback(_systemConnected);
  }
}

AxeEffect *AxeSystem::findEffect(const EffectId effectId) {
  for (EffectIndex i = 0; i < _preset._effectCount; i++) {
    if (_preset._effects[i]._effectId == effectId) {
      return &_preset._effects[i];
    }
  }
  return nullptr;
}

void AxeSystem::checkTimers() {

  unsigned long now = millis();

  if (_systemConnected && (now - _lastSysexResponse) > (_sysexTimout + _refreshRate)) {
    _systemConnected = false;
    callConnectionStatusCallback(_systemConnected);
  }

  if (_tunerEngaged && (now - _lastTunerResponse) > _tunerTimout) {
    _tunerEngaged = false;
    callTunerStatusCallback(_tunerEngaged);
  }
}

void AxeSystem::intToMidiBytes(const int number, byte *byte1, byte *byte2) {
  *byte1 = number % BANK_SIZE;
  *byte2 = number / BANK_SIZE;
}

int AxeSystem::midiBytesToInt(const byte lsb, const byte msb) {
  return lsb + (msb * BANK_SIZE);
}

#ifdef AXE_DEBUG_SYSEX
void AxeSystem::debugSysex(const byte *sysex, const byte length, const char *message) {
  char buf[6];
  DEBUGGER.print(message);
  for (byte i = 0; i < length; i++) {
    snprintf(buf, 6, "0x%02X ", sysex[i]);
    DEBUGGER.print(buf);
  }
  DEBUGGER.println();
}
#endif