#include "interface/AxeSystem.h"
#include "interface/AxeEffect.h"

void AxeSystem::requestFirmwareVersion() {
	sendCommand(SYSEX_REQUEST_FIRMWARE);
}

void AxeSystem::requestPresetName(const PresetNumber number) {
	byte data[2] = {SYSEX_QUERY_BYTE, SYSEX_QUERY_BYTE};
  if (number != -1) {
    intToMidiBytes(number, &data[0], &data[1]);
  }
	sendCommand(SYSEX_REQUEST_PRESET_INFO, data, 2);
}

void AxeSystem::requestSceneName(const SceneNumber number) {
	byte data[1] = {SYSEX_QUERY_BYTE};
	if (number != -1) {
		data[0] = (byte) (number - 1);
	}
	sendCommand(SYSEX_REQUEST_SCENE_INFO, data, 1);
}

void AxeSystem::requestAllSceneNames() {
	for (SceneNumber scene = 1; scene <= 8; scene++) {
		requestSceneName(scene);
	}
}

void AxeSystem::requestSceneNumber() {
	byte data[1] = {SYSEX_QUERY_BYTE};
	sendCommand(SYSEX_REQUEST_SCENE_NUMBER, data, 1);
}

void AxeSystem::requestEffectDetails() {
	sendCommand(SYSEX_EFFECT_DUMP);
}

//Assumes you are sending 1-based scenes.
void AxeSystem::sendSceneChange(const SceneNumber number) {
	byte data[1] = { (byte) (number - 1) }; 
	sendCommand(SYSEX_REQUEST_SCENE_NUMBER, data, 1);
}

void AxeSystem::requestLooperStatus() {
	byte data[1] = {SYSEX_QUERY_BYTE};
	sendCommand(SYSEX_REQUEST_LOOPER_STATUS, data, 1);
}

void AxeSystem::pressLooperButton(const LooperButton button) {
	byte data[1] = {button};
	sendCommand(SYSEX_REQUEST_LOOPER_STATUS, data, 1);
}

void AxeSystem::requestTempo() {
	byte data[2] = {SYSEX_QUERY_BYTE, SYSEX_QUERY_BYTE};
	sendCommand(SYSEX_REQUEST_TEMPO, data, 2);
}

void AxeSystem::setTempo(const Tempo tempo) {
	byte data[2];
	intToMidiBytes(tempo, &data[0], &data[1]);
	sendCommand(SYSEX_REQUEST_TEMPO, data, 2);
	_tempo = tempo;
	callSystemChangeCallback();
}

void AxeSystem::sendTap() {
	sendCommand(SYSEX_TAP_TEMPO_PULSE);
}

void AxeSystem::toggleTuner() {
	_tunerEngaged ? disableTuner() : enableTuner();
}

void AxeSystem::enableTuner() {
	byte data[1] = {SYSEX_TUNER_ON};
	_tunerEngaged = true;
	_lastTunerResponse = millis(); 
	sendCommand(SYSEX_TUNER, data, 1);
	callTunerStatusCallback(_tunerEngaged);
}

void AxeSystem::disableTuner() {
	_tunerEngaged = false;
	byte data[1] = {SYSEX_TUNER_OFF};
	sendCommand(SYSEX_TUNER, data, 1);
	callTunerStatusCallback(_tunerEngaged);
}

void AxeSystem::setEffectChannel(const EffectId effectId, const Channel channel) {
	byte data[3];
	intToMidiBytes(effectId, &data[0], &data[1]);
	data[2] = channel;
	sendCommand(SYSEX_REQUEST_EFFECT_CHANNEL, data, 3);
}

void AxeSystem::enableEffect(const EffectId effectId) {
  byte data[3];
	intToMidiBytes(effectId, &data[0], &data[1]);
	data[2] = SYSEX_EFFECT_ENABLE;
	sendCommand(SYSEX_REQUEST_EFFECT_BYPASS, data, 3);
}

void AxeSystem::disableEffect(const EffectId effectId) {
  byte data[3];
	intToMidiBytes(effectId, &data[0], &data[1]);
	data[2] = SYSEX_EFFECT_BYPASS;
	sendCommand(SYSEX_REQUEST_EFFECT_BYPASS, data, 3);
}

void AxeSystem::toggleEffect(const EffectId effectId) {
	if (AxeEffect *effect = findEffect(effectId)) {
		effect->toggle();
	}
}

bool AxeSystem::isEffectEnabled(const EffectId effectId) {
	if (AxeEffect *effect = findEffect(effectId)) {
		return !effect->isBypassed();
	}
	return false;
}

void AxeSystem::sendEffectChannelIncrement(const EffectId effectId) {
	if (AxeEffect *effect = findEffect(effectId)) {
		effect->incrementChannel();
	}
}

void AxeSystem::sendEffectChannelDecrement(const EffectId effectId) {
	if (AxeEffect *effect = findEffect(effectId)) {
		effect->decrementChannel();
	}
}

void AxeSystem::sendPresetIncrement() {
  int number = _preset.getPresetNumber();
  if (isValidPresetNumber(number)) {
    if (++number > MAX_PRESETS) {
      number = 0;
    }
    sendPresetChange(number);
  }
}

void AxeSystem::sendPresetDecrement() {
  int number = _preset.getPresetNumber();
  if (isValidPresetNumber(number)) {
    if (number == 0) {
      number = MAX_PRESETS;
    } else {
      number--;
    }
    sendPresetChange(number);
  }
}

void AxeSystem::sendSceneIncrement(bool triggerSceneNameRefresh) {
  byte number = _preset.getSceneNumber();
  if (isValidSceneNumber(number)) {
    if (++number > MAX_SCENES) {
      number = 1;
    }
    sendSceneChange(number);
		if (triggerSceneNameRefresh) {
			requestSceneName();
		}
  }
}

void AxeSystem::sendSceneDecrement(bool triggerSceneNameRefresh) {
  byte number = _preset.getSceneNumber();
  if (isValidSceneNumber(number)) {
    if (number == 1) {
      number = MAX_SCENES;
    } else {
      number--;
    }
    sendSceneChange(number);
		if (triggerSceneNameRefresh) {
			requestSceneName();
		}
  }
}
