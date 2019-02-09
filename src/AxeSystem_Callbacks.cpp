#include "AxeSystem.h"

void AxeSystem::registerConnectionStatusCallback(ConnectionStatusCallback func) {
	_connectionStatusCallback = func;
}

void AxeSystem::registerPresetChangingCallback(PresetChangingCallback func) {
	_presetChangingCallback = func;
}

void AxeSystem::registerPresetChangeCallback(PresetChangeCallback func) {
	_presetChangeCallback = func;
}

void AxeSystem::registerSystemChangeCallback(SystemChangeCallback func) {
	_systemChangeCallback = func;
}

void AxeSystem::registerTapTempoCallback(TapTempoCallback func) {
	_tapTempoCallback = func;
}

void AxeSystem::registerTunerDataCallback(TunerDataCallback func) {
	_tunerDataCallback = func;
}

void AxeSystem::registerTunerStatusCallback(TunerStatusCallback func) {
	_tunerStatusCallback = func;
}

void AxeSystem::registerLooperStatusCallback(LooperStatusCallback func) {
	_looperStatusCallback = func;
}

void AxeSystem::registerSysexPluginCallback(SysexPluginCallback func) {
	_sysexPluginCallback = func;
}

//////////////////////////////////////////////////////////////////////////////

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

bool AxeSystem::callSysexPluginCallback(const byte *sysex, const byte length) {
	if (NULL != _sysexPluginCallback) {
		return (_sysexPluginCallback)(sysex, length);
	} else {
		return false; 
	}
}