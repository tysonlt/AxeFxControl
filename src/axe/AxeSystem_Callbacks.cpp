#include "AxeSystem.h"

/**
 * Callback to run when MIDI connection status changes.
 */ 
void AxeSystem::registerConnectionStatusCallback( void (*func)(bool) ) {
	_connectionStatusCallback = func;
}

/**
 * Callback to run when a preset change is received.
 */
void AxeSystem::registerPresetChangingCallback( void (*func)(int) ) {
	_presetChangingCallback = func;
}

/**
 * Callback to run when preset detals are changed.
 */
void AxeSystem::registerPresetChangeCallback( void (*func)(AxePreset *preset) ) {
	_presetChangeCallback = func;
}

/**
 * Callback to run when Axe settings are changed (eg global tempo).
 */
void AxeSystem::registerSystemChangeCallback( void (*func)() ) {
	_systemChangeCallback = func;
}

/**
 * Callback to run when a tap tempo pulse is received.
 */
void AxeSystem::registerTapTempoCallback( void (*func)() ) {
	_tapTempoCallback = func;
}

/**
 * Callback to run when realtime tuner data is received.
 */
void AxeSystem::registerTunerDataCallback( void (*func)(const char*, const byte, const byte) ) {
	_tunerDataCallback = func;
}

/**
 * Callback to run when tuner is turned on/off at the panel
 */
void AxeSystem::registerTunerStatusCallback( void (*func)(bool) ) {
	_tunerStatusCallback = func;
}

/**
 * 
 */
void AxeSystem::callConnectionStatusCallback(bool connected)  {
	if (NULL != _connectionStatusCallback) {
		(_connectionStatusCallback)(connected);
	}
}

/**
 * 
 */
void AxeSystem::callTapTempoCallback() {
	if (NULL != _tapTempoCallback) {
		(_tapTempoCallback)();
	}
}

/**
 * 
 */
void AxeSystem::callPresetChangingCallback(int presetNumber) {
	if (NULL != _presetChangingCallback) {
		(_presetChangingCallback)(presetNumber);
	}
}

/**
 * 
 */
void AxeSystem::callPresetChangeCallback(AxePreset *preset) {
	if (NULL != _presetChangeCallback) {
		(_presetChangeCallback)(preset);
	}
}

/**
 * 
 */
void AxeSystem::callSystemChangeCallback() {
	if (NULL != _systemChangeCallback) {
		(_systemChangeCallback)();
	}
}

/**
 * 
 */
void AxeSystem::callTunerDataCallback(const char *note, const byte string, const byte fineTune) {
	if (NULL != _tunerDataCallback) {
		(_tunerDataCallback)(note, string, fineTune);
	}
}

/**
 * 
 */
void AxeSystem::callTunerStatusCallback(bool enabled) {
	if (NULL != _tunerStatusCallback) {
		(_tunerStatusCallback)(enabled);
	}
}
    
