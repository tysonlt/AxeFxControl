#include "AxeSystem.h"

//TODO: would like more control over this...
MIDI_CREATE_DEFAULT_INSTANCE();

/////// BEGIN MIDI CALLS ///////

void AxeSystem::init() {	
	MIDI.begin(MIDI_CHANNEL_OMNI);
	MIDI.turnThruOff();
	_midiReady = true;
}

void AxeSystem::update() {
	
	checkMidi();

	if (!_presetChanging && _refreshRate > 0) {
		unsigned long now = millis();
		if (now - _lastRefresh > _refreshRate) {
			refresh();
		}
	}

	if (MIDI.read()) {
		if (MIDI.getType() == midi::ControlChange) {
			onControlChange(MIDI.getChannel(), MIDI.getData1(), MIDI.getData2());
		} else if (MIDI.getType() == midi::ProgramChange) {
			onProgramChange(MIDI.getChannel(), MIDI.getData1());
		} else if (MIDI.getType() == midi::SystemExclusive) {
			onSystemExclusive(MIDI.getSysExArray(), MIDI.getSysExArrayLength());
		}
	} 

}

void AxeSystem::sendPresetChange(const unsigned number) {

	checkMidi();

  byte bank = number / BANK_SIZE;
  byte patch = number - (BANK_SIZE * bank);

  MIDI.sendControlChange(BANK_CHANGE_CC, bank, _midiChannel);
  MIDI.sendProgramChange(patch, _midiChannel);

	setChanging();
	callPresetChangingCallback(number);

}

void AxeSystem::sendSysEx(const byte length, byte *sysex) {

	checkMidi();

	byte sum = 0xF0;
	for (int i=0; i<length-1; ++i) {
		sum ^= sysex[i];
	}
	sysex[length-1] = (sum & 0x7F);

	#ifdef AXE_DEBUG_SYSEX
	char buf[100];
	snprintf(buf, 100, "AxeSystem::sendSysEx(0x%02X):         ", sysex[4]);
	DEBUGGER.print(buf);
	for (unsigned i=0; i<length; i++) {
		snprintf(buf, 6, "0x%02X ", sysex[i]);
		DEBUGGER.print(buf);
	}
	DEBUGGER.println();
	#endif

	MIDI.sendSysEx(length, sysex);

}

void AxeSystem::checkMidi() {
	if (!_midiReady) {
		init();
	}
}

//////// END MIDI CALLS ///////

void AxeSystem::enableRefresh(unsigned long millis, unsigned long throttle) {
	_refreshRate = millis;
	_refreshThrottle = throttle;

	Serial.println(_refreshRate);
}


void AxeSystem::refresh(bool ignoreThrottle) {
	unsigned long now = millis();
  if (ignoreThrottle || (!_tunerEngaged && (now - _lastRefresh) > _refreshThrottle)) {
		_lastRefresh = now;
		setChanging();
		requestPresetName();
		requestTempo();
  } else {
		#ifdef AXE_DEBUG
    DEBUGGER.println(F("refresh(): too soon or tuner engaged, ignoring refresh request"));
		#endif
  }
}

void AxeSystem::checkFirmware() {
  if (!_firmwareRequested) {
    requestFirmwareVersion();
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

  if (_systemConnected && (now - _lastSysexResponse) > TIME_LIMIT_SYSEX) {
    _systemConnected = false;
    callConnectionStatusCallback(_systemConnected);
  }

  if (_tunerEngaged && (now - _lastTunerResponse) > TIME_LIMIT_TUNER) {
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

bool AxeSystem::isAxeSysEx(const byte *sysex, const unsigned length) {
	return 
		length > 4 && 
		sysex[1] == SYSEX_MANUFACTURER_BYTE1 && 
		sysex[2] == SYSEX_MANUFACTURER_BYTE2 && 
		sysex[3] == SYSEX_MANUFACTURER_BYTE3;
}

void AxeSystem::parseName(const byte *sysex, byte length, byte offset, char *buffer, byte size) {
	memset(buffer, ' ', size);
	byte count = 0;
  for (byte i = offset; i < length - 2 && count < size; i++) {
    if (sysex[i] == 0) break;
		buffer[count++] = sysex[i];
  }
  buffer[size] = '\0';
}

void AxeSystem::setChanging() {
	_incomingPreset.reset();
	_presetChanging = true;
}

bool AxeSystem::isValidPresetNumber(int preset) {
  return preset >= 0 && preset <= MAX_PRESETS;
}

bool AxeSystem::isValidSceneNumber(int scene) { 
	return scene >= 0 && scene <= MAX_SCENES; 
}

void AxeSystem::byteToMidiBytes(byte number, byte *byte1, byte *byte2) {
	*byte1 = number % BANK_SIZE;
	*byte2 = number / BANK_SIZE;
}