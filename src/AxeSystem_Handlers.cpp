#include "AxeSystem.h"

void AxeSystem::onPresetChange(const PresetNumber number) {

	// ignore stale responses due to very fast preset changes, 
	// but allow refreshes to current preset
	if (true/*!_presetChanging || number == _preset.getPresetNumber()*/) {
		_incomingPreset.reset();
		requestPresetName(number);
		if (!_presetChanging) {
			setChanging();
			callPresetChangingCallback(number);
		}
	}
		
}

void AxeSystem::onSystemExclusive(const byte *sysex, const byte length) {

	if (callSysexPluginCallback(sysex, length)) {
		return;
	}

	const size_t bufferSize = 40;
	char buffer[bufferSize];

	checkTimers();
	_lastSysexResponse = millis();

	#ifdef AXE_DEBUG_SYSEX
	char b[100];
	snprintf(b, 100, "AxeSystem::onSystemExclusive(0x%02X): ", sysex[5]);
	DEBUGGER.print(b);
	for (byte i=1; i<length-1; i++) {
		snprintf(b, 6, "0x%02X ", sysex[i]);
		DEBUGGER.print(b);
	}
	DEBUGGER.println();
	#endif

	switch (sysex[5]) {

		case SYSEX_TAP_TEMPO_PULSE: {
			callTapTempoCallback();
			break;
		}

		case SYSEX_REQUEST_FIRMWARE: {
			_firmwareVersion.major = sysex[6];
			_firmwareVersion.minor = sysex[7];
			_usbVersion.major = sysex[9];
			_usbVersion.minor = sysex[10];
			callSystemChangeCallback();
			break;
		}
					
		case SYSEX_REQUEST_TEMPO: {
			byte newTempo = (byte) midiBytesToInt(sysex[6], sysex[7]);
			if (newTempo != _tempo) {
				_tempo = newTempo;
				callSystemChangeCallback();
			}
			break;
		}

		case SYSEX_REQUEST_PRESET_INFO: {
			_lastRefresh = millis();
			int number = midiBytesToInt(sysex[6], sysex[7]);
			if (true/*!_presetChanging || number == _preset.getPresetNumber()*/) {
				parseName(sysex, length, 8, buffer, AxePreset::MAX_PRESET_NAME);
				_incomingPreset.setPresetName(buffer); 
				_incomingPreset.setPresetNumber(number);
				requestSceneName();
				requestEffectDetails();
				checkIncomingPreset();
			}
			break;
		}

		case SYSEX_REQUEST_SCENE_INFO: {
			parseName(sysex, length, 7, buffer, AxePreset::MAX_SCENE_NAME);
			_incomingPreset.setSceneName(buffer); 
			_incomingPreset.setSceneNumber(sysex[6] + 1);
			checkIncomingPreset();
			break;
		}

		case SYSEX_REQUEST_SCENE_NUMBER: {
			_incomingPreset.setSceneNumber(sysex[6] + 1);
			checkIncomingPreset();
			break;
		}

		case SYSEX_EFFECT_DUMP: {
			processEffectDump(sysex, length);
			checkIncomingPreset();
			break;
		}

		case SYSEX_TUNER: {
			_lastTunerResponse = millis();
			if (!_tunerEngaged && ++_tunerIncomingCount > _tunerTriggerThreshold) { 
				_tunerEngaged = true;
				_tunerIncomingCount = 0;
				callTunerStatusCallback(_tunerEngaged);
			}

			if (_tunerEngaged) {
				byte note = sysex[6];
				byte string = sysex[7] + 1;
				byte fineTune = sysex[8];
				callTunerDataCallback(_notes[note], string, fineTune);
			}

			break;
		}

		case SYSEX_REQUEST_LOOPER_STATUS: {
			LooperStatus newStatus = sysex[6];
			if (_looper.getStatus() != newStatus) {
				_looper.setStatus(newStatus);
				callLooperStatusCallback(&_looper);
			}
			break;
		}

		case SYSEX_REQUEST_EFFECT_BYPASS: {
			/*
			EffectId effectId = midiBytesToInt(sysex[6], sysex[7]);
			bool bypassed = sysex[8];
			*/
			//adding so it's not unhandled during debug...
			//not much to do because Axe doesn't send status updates for this
			break;
		}

		default: {
			#ifdef AXE_DEBUG_SYSEX
			DEBUGGER.println(F("******** AxeSystem::onSystemExclusive(): UNHANDLED SYSEX: "));
			char d[100];
			for (byte i=1; i<length-1; i++) {
				snprintf(d, 6, "0x%02X ", sysex[i]);
				DEBUGGER.print(d);
			}
			DEBUGGER.println();
			DEBUGGER.println(F(" ********"));
			#endif
		}

	}; //end case
		
}

// TODO: need to prioritise which effects are shown in order
// If assuming naked amps pack it gets a lot easier :)
void AxeSystem::processEffectDump(const byte *sysex, const byte length) {

	unsigned count = 0;
	unsigned numEffects = (length -5 -1) / 3; //groups of three, minus header and checksum
	EffectId effectIds[numEffects];
	bool effectBypassed[numEffects];

  for (byte i = 6; i < length - 3 && count < AxePreset::MAX_EFFECTS; i += 3) {

    EffectId effectId = sysex[i];
    byte msb = sysex[i + 1], status = sysex[i + 2];
    bool bypassed = !!(status & 1); 
    if (msb) effectId |= 128;

		effectIds[count] = effectId;
		effectBypassed[count] = bypassed;
		count++;

	}

	_incomingPreset.setEffects(effectIds, effectBypassed, count);
			
}

void AxeSystem::parseName(const byte *sysex, const byte length, const byte offset, char *buffer, const byte size) {
	memset(buffer, ' ', size);
	byte count = 0;
  for (byte i = offset; i < length - 2 && count < size; i++) {
    if (sysex[i] == 0) break;
		buffer[count++] = sysex[i];
  }
  buffer[size] = '\0';
}

bool AxeSystem::isValidPresetNumber(const PresetNumber preset) {
  return preset >= 0 && preset <= MAX_PRESETS;
}

bool AxeSystem::isValidSceneNumber(const SceneNumber scene) { 
	return scene > 0 && scene <= MAX_SCENES; 
}