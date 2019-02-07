#include "AxeSystem.h"

void AxeSystem::onControlChange(byte channel, byte number, byte value) {
	if (number == BANK_CHANGE_CC) {
		_bank = value;
	}
}

void AxeSystem::onProgramChange(byte channel, byte value) {

	int number = (_bank * BANK_SIZE) + value;

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

void AxeSystem::onSystemExclusive(const byte *sysex, unsigned length) {

	size_t bufferSize = 40;
	char buffer[bufferSize];

	checkTimers();

	if (isAxeSysEx(sysex, length)) {

		_lastSysexResponse = millis();

		#ifdef AXE_DEBUG_SYSEX
		char b[100];
		snprintf(b, 100, "AxeSystem::onSystemExclusive(0x%02X): ", sysex[5]);
		DEBUGGER.print(b);
		for (unsigned i=1; i<length-1; i++) {
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
				byte newTempo = sysex[6] + (sysex[7] * BANK_SIZE);
				if (newTempo != _tempo) {
					_tempo = newTempo;
					callSystemChangeCallback();
				}
				break;
			}
	
			case SYSEX_REQUEST_PRESET_INFO: {
				_lastRefresh = millis();
				int number = sysex[6] + (sysex[7] * BANK_SIZE);
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
				_incomingPreset.setSceneNumber(sysex[6]);
				checkIncomingPreset();
				break;
			}
	
			case SYSEX_REQUEST_SCENE_NUMBER: {
				_incomingPreset.setSceneNumber(sysex[6]);
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
					byte string = sysex[7];
					byte fineTune = sysex[8];
					callTunerDataCallback(_notes[note], string, fineTune);
				}

				break;
			}

			default: {
				#ifdef AXE_DEBUG_SYSEX
				DEBUGGER.println(F("******** AxeSystem::onSystemExclusive(): UNHANDLED SYSEX: "));
				char d[100];
				for (unsigned i=1; i<length-1; i++) {
					snprintf(d, 6, "0x%02X ", sysex[i]);
					DEBUGGER.print(d);
				}
				DEBUGGER.println();
				DEBUGGER.println(F(" ********"));
				#endif
			}
	
		}; //end case

	} //end isAxeSysEx
		
}

// TODO: need to prioritise which effects are shown in order
// If assuming naked amps pack it gets a lot easier :)
void AxeSystem::processEffectDump(const byte *sysex, unsigned length) {

	unsigned count = 0;
	unsigned numEffects = (length -5 -1) / 3; //groups of three, minus header and checksum
	EffectId effectIds[numEffects];
	bool effectBypassed[numEffects];

  for (unsigned i = 6; i < length - 3 && count < AxePreset::MAX_EFFECTS; i += 3) {

    EffectId effectId = sysex[i];
    byte msb = sysex[i + 1], status = sysex[i + 2];
    bool bypassed = !!(status & 1); 
    if (msb) effectId |= 128;

    if (_preset.isEffectSwitchable(effectId)) {
      effectIds[count] = effectId;
      effectBypassed[count] = bypassed;
      count++;
    }

	}

	_incomingPreset.setEffects(effectIds, effectBypassed, count);
			
}
