#include "interface/AxeSystem.h"

void AxeSystem::onPresetChange(const PresetNumber number) {
	_incomingPreset.reset();
	_incomingPreset.setPresetNumber(number);
	requestPresetName(number);
	callPresetChangingCallback(number);
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
	debugSysex(sysex, length, "<- AxeSystem::onSystemExclusive(): ");
	#endif

	switch (sysex[5]) {

		case SYSEX_TAP_TEMPO_PULSE: {
			callTapTempoCallback();
			break;
		}

		case SYSEX_REQUEST_PRESET_INFO: {
			_lastRefresh = millis();
			const byte max = AxePreset::MAX_PRESET_NAME + 1;
			const PresetNumber number = midiBytesToInt(sysex[6], sysex[7]);
			if (isRequestedPreset(number)) { //drop older responses
				parseName(sysex, length, 8, buffer, max);
				_incomingPreset.reset(); //in case stale packets arrived from prev request
				_incomingPreset.setPresetNumber(number);
				_incomingPreset.setPresetName(buffer); 
				_incomingPreset.copyPresetName(buffer, max); //copy back out in case preset changed it
				callPresetNameCallback(number, (const char*) buffer, max);
				requestSceneName();
				requestEffectDetails();
				checkIncomingPreset();
			} else {
				#ifdef AXE_DEBUG
				DEBUGGER.print("Dropping stale preset name packet ");
				DEBUGGER.print(number);
				DEBUGGER.print(", waiting for ");
				DEBUGGER.println(_incomingPreset.getPresetNumber());
				#endif
			} 
			break;
		}

		case SYSEX_REQUEST_SCENE_INFO: {
			//TODO during fast changes, can we guarantee this is for current preset?
			if (!_incomingPreset.isComplete()) { //TODO is this necessary given guard in preset name case?
				const SceneNumber number = sysex[6] + 1;
				const byte max = AxePreset::MAX_SCENE_NAME + 1;
				parseName(sysex, length, 7, buffer, max);
				_incomingPreset.setSceneNumber(number);
				_incomingPreset.setSceneName(buffer); 
				_incomingPreset.copySceneName(buffer, max); //copy back out in case preset changed it
				callSceneNameCallback(number, (const char*) buffer, max);
				checkIncomingPreset();
			} else {
				#ifdef AXE_DEBUG
				DEBUGGER.println("Ignoring SYSEX_REQUEST_SCENE_INFO, preset already complete");
				#endif
			} 
			break;
		}

		case SYSEX_REQUEST_SCENE_NUMBER: {
			//TODO during fast changes, can we guarantee this is for current preset?
			if (!_incomingPreset.isComplete()) { //TODO is this necessary given guard in preset name case?
				_incomingPreset.setSceneNumber(sysex[6] + 1);
				checkIncomingPreset();
			} else {
				#ifdef AXE_DEBUG
				DEBUGGER.println("Ignoring SYSEX_REQUEST_SCENE_NUMBER, preset already complete");
				#endif
			} 
			break;
		}

		case SYSEX_EFFECT_DUMP: {
			//TODO during fast changes, can we guarantee this is for current preset?
			if (!_incomingPreset.isComplete()) { //TODO is this necessary given guard in preset name case?
				processEffectDump(sysex, length);
				callEffectsReceivedCallback(&_incomingPreset);
				checkIncomingPreset();
			} else {
				#ifdef AXE_DEBUG
				DEBUGGER.println("Ignoring SYSEX_EFFECT_DUMP, preset already complete");
				#endif
			} 
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

bool AxeSystem::isRequestedPreset(const PresetNumber number) {
	return _incomingPreset.getPresetNumber() == -1 || _incomingPreset.getPresetNumber() == number;
}

void AxeSystem::checkIncomingPreset() {
  if (_incomingPreset.isComplete() && !_preset.equals(_incomingPreset)) {
		_preset = _incomingPreset;
		callPresetChangeCallback(&_preset);
  }
}

// TODO: need to prioritise which effects are shown in order
// TODO: sometimes first message is wrong, corrected on refresh (ie change tempo wheel and fx list goes whacky)
// If assuming naked amps pack it gets a lot easier :)
void AxeSystem::processEffectDump(const byte *sysex, const byte length) {

	unsigned count = 0;
	unsigned numEffects = (length -5 -1) / 3; //groups of three, minus header and checksum
	AxeEffect effects[numEffects];

  for (byte i = 6; i < length - 3 && count < AxePreset::MAX_EFFECTS; i += 3) {

    EffectId effectId = sysex[i];
    byte msb = sysex[i + 1], status = sysex[i + 2];
    bool bypassed = !!(status & 1); 
		Channel channel = (status >> 1) & 0x03;
  	byte numChannels = (status >> 4) & 0x07;
    if (msb) effectId |= 128;

		AxeEffect effect;
		effect.setAxeSystem(this);
		effect.setEffectId(effectId);
		effect.setBypassed(bypassed);
		effect.setChannel(channel);
		effect.setChannelCount(numChannels);

		//NOTE assumes preset number has already been received
		if (callEffectFilterCallback(_incomingPreset.getPresetNumber(), effect)) {
			effects[count++] = effect;
		}

	}

	_incomingPreset.setEffects(effects, count);
			
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