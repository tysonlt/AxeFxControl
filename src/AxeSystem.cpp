#include "AxeSystem.h"

void AxeSystem::init(HardwareSerial& serial) {	
	if (_startupDelay > 0) { 
		delay(_startupDelay);
	}
	_serial = &serial;
	_serial->begin(MIDI_BAUD);
	_midiReady = true;
}

void AxeSystem::update() {
	
	if (!_presetChanging && _refreshRate > 0) {
		unsigned long now = millis();
		if (now - _lastRefresh > _refreshRate) {
			refresh();
		}
	}

	readMidi();

}

void AxeSystem::enableRefresh(unsigned long millis, unsigned long throttle) {
	_refreshRate = millis;
	_refreshThrottle = throttle;
}

void AxeSystem::refresh(bool ignoreThrottle) {
	unsigned long now = millis();
  if (ignoreThrottle || (!_tunerEngaged && (now - _lastRefresh) > _refreshThrottle)) {
		_lastRefresh = now;
		setChanging();
		requestPresetName();
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

void AxeSystem::setSystemConnected(bool connected) {
	bool oldStatus = _systemConnected;
	_systemConnected = connected;
  if (oldStatus != _systemConnected) {
    callConnectionStatusCallback(_systemConnected);
  }  
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

void AxeSystem::checkIncomingPreset() {
  if (_incomingPreset.isComplete()) {
    _presetChanging = false;
    _preset = _incomingPreset;
    callPresetChangeCallback(&_preset);
  }
}

void AxeSystem::setChanging() {
	_incomingPreset.reset();
	_presetChanging = true;
}

void AxeSystem::intToMidiBytes(int number, byte *byte1, byte *byte2) {
	*byte1 = number % BANK_SIZE;
	*byte2 = number / BANK_SIZE;
}