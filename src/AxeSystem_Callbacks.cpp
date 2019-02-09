#include "AxeSystem.h"

void AxeSystem::registerConnectionStatusCallback( void (*func)(bool) ) {
	_connectionStatusCallback = func;
}

void AxeSystem::registerPresetChangingCallback( void (*func)(PresetNumber) ) {
	_presetChangingCallback = func;
}

void AxeSystem::registerPresetChangeCallback( void (*func)(AxePreset) ) {
	_presetChangeCallback = func;
}

void AxeSystem::registerSystemChangeCallback( void (*func)() ) {
	_systemChangeCallback = func;
}

void AxeSystem::registerTapTempoCallback( void (*func)() ) {
	_tapTempoCallback = func;
}

void AxeSystem::registerTunerDataCallback( void (*func)(const char*, const byte, const byte) ) {
	_tunerDataCallback = func;
}

void AxeSystem::registerTunerStatusCallback( void (*func)(bool) ) {
	_tunerStatusCallback = func;
}

void AxeSystem::registerLooperStatusCallback( void (*func)(AxeLooper) ) {
	_looperStatusCallback = func;
}

void AxeSystem::callConnectionStatusCallback(bool connected)  {
	if (NULL != _connectionStatusCallback) {
		(_connectionStatusCallback)(connected);
	}
}

void AxeSystem::callTapTempoCallback() {
	if (NULL != _tapTempoCallback) {
		(_tapTempoCallback)();
	}
}

void AxeSystem::callPresetChangingCallback(PresetNumber presetNumber) {
	if (NULL != _presetChangingCallback) {
		(_presetChangingCallback)(presetNumber);
	}
}

void AxeSystem::callPresetChangeCallback(AxePreset *preset) {
	if (NULL != _presetChangeCallback) {
		(_presetChangeCallback)(*preset);
	}
}

void AxeSystem::callSystemChangeCallback() {
	if (NULL != _systemChangeCallback) {
		(_systemChangeCallback)();
	}
}

void AxeSystem::callTunerDataCallback(const char *note, const byte string, const byte fineTune) {
	if (NULL != _tunerDataCallback) {
		(_tunerDataCallback)(note, string, fineTune);
	}
} 

void AxeSystem::callLooperStatusCallback(AxeLooper *looper) {
	if (NULL != _looperStatusCallback) {
		(_looperStatusCallback)(*looper);
	}
}
    
void AxeSystem::callTunerStatusCallback(bool enabled) {
	if (NULL != _tunerStatusCallback) {
		(_tunerStatusCallback)(enabled);
	}
}
