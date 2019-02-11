#include "AxeSystem.h"

void AxeSystem::registerConnectionStatusCallback(ConnectionStatusCallback func) {
	_connectionStatusCallback = func;
}

void AxeSystem::registerPresetChangingCallback(PresetChangingCallback func) {
	_presetChangingCallback = func;
}

void AxeSystem::registerPresetNameCallback(PresetNameCallback func) {
	_presetNameCallback = func;
}

void AxeSystem::registerSceneNameCallback(SceneNameCallback func) {
	_sceneNameCallback = func;
}

void AxeSystem::registerEffectsReceivedCallback(EffectsReceivedCallback func) {
	_effectsReceivedCallback = func;
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

void AxeSystem::registerEffectFilterCallback(EffectFilterCallback func) {
	_effectFilterCallback = func;
}

//////////////////////////////////////////////////////////////////////////////

void AxeSystem::callConnectionStatusCallback(const bool connected)  {
	if (NULL != _connectionStatusCallback) {
		(_connectionStatusCallback)(connected);
	}
}

void AxeSystem::callTapTempoCallback() {
	if (NULL != _tapTempoCallback) {
		(_tapTempoCallback)();
	}
}

void AxeSystem::callPresetChangingCallback(const PresetNumber number) {
	if (NULL != _presetChangingCallback) {
		(_presetChangingCallback)(number);
	}
}

void AxeSystem::callPresetNameCallback(const PresetNumber number, const char *name, const byte length) {
	if (NULL != _presetNameCallback) {
		(_presetNameCallback)(number, name, length);
	}
}

void AxeSystem::callSceneNameCallback(const SceneNumber number, const char* name, const byte length) {
	if (NULL != _sceneNameCallback) {
		(_sceneNameCallback)(number, name, length);
	}
}

void AxeSystem::callEffectsReceivedCallback(AxePreset *preset) {
	if (NULL != _effectsReceivedCallback) {
		(_effectsReceivedCallback)(preset->getPresetNumber(), *preset);
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
    
void AxeSystem::callTunerStatusCallback(const bool enabled) {
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

bool AxeSystem::callEffectFilterCallback(const PresetNumber number, AxeEffect effect) {
	if (NULL != _effectFilterCallback) {
		return (_effectFilterCallback)(number, effect);
	} else {
		return defaultEffectFilter(number, effect); //use default filter
	}
}

bool AxeSystem::defaultEffectFilter(const PresetNumber number, AxeEffect effect) {
	switch (effect.getEffectId()) {
		case ID_CONTROL:
		case ID_DISTORT1: //these are actually amps, fuz is distortion
		case ID_DISTORT2:
		case ID_DISTORT3:
		case ID_DISTORT4:
		case ID_CAB1:
		case ID_CAB2:
		case ID_CAB3:
		case ID_CAB4:
		case ID_TUNER:
		case ID_INPUT1:
		case ID_INPUT2:
		case ID_INPUT3:
		case ID_INPUT4:
		case ID_INPUT5:
		case ID_OUTPUT1:
		case ID_OUTPUT2:
		case ID_OUTPUT3:
		case ID_OUTPUT4:
		case ID_IRCAPTURE:
		case ID_RTA1:
		case ID_RTA2:
		case ID_RTA3:
		case ID_RTA4:
		case ID_LOOPER1:
		case ID_LOOPER2:
		case ID_LOOPER3:
		case ID_LOOPER4:
		case ID_TONEMATCH1:
		case ID_TONEMATCH2:
		case ID_TONEMATCH3:
		case ID_TONEMATCH4:
		case ID_FBSEND1:
		case ID_FBSEND2:
		case ID_FBSEND3:
		case ID_FBSEND4:
		case ID_FBRETURN1:
		case ID_FBRETURN2:
		case ID_FBRETURN3:
		case ID_FBRETURN4:
		case ID_MULTIPLEXER1:
		case ID_MULTIPLEXER2:
		case ID_MULTIPLEXER3:
		case ID_MULTIPLEXER4:
		case ID_IRPLAYER1:
		case ID_IRPLAYER2:
		case ID_IRPLAYER3:
		case ID_IRPLAYER4:
		case ID_FOOTCONTROLLER:
		case ID_PRESET_FC:
			return false;
		default:
			return true;
	};
}
