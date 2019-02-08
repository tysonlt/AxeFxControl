#include "AxeSystem.h"
#include "AxeEffect.h"

		const byte REQUEST_EFFECT_BYPASSED_COMMAND_9_BYTES[9] = {
				SYSEX_MANUFACTURER_BYTE1,  		SYSEX_MANUFACTURER_BYTE2,
				SYSEX_MANUFACTURER_BYTE3,  		SYSEX_AXE_VERSION,
				SYSEX_REQUEST_EFFECT_BYPASS, 	SYSEX_QUERY_BYTE,
				SYSEX_QUERY_BYTE,          		SYSEX_QUERY_BYTE,
				SYSEX_CHECKSUM_PLACEHOLDER};

		const byte REQUEST_SCENE_NAME_COMMAND_7_BYTES[7] = {
				SYSEX_MANUFACTURER_BYTE1,  SYSEX_MANUFACTURER_BYTE2,
				SYSEX_MANUFACTURER_BYTE3,  SYSEX_AXE_VERSION,
				SYSEX_REQUEST_SCENE_INFO,  SYSEX_QUERY_BYTE,
				SYSEX_CHECKSUM_PLACEHOLDER};

		const byte REQUEST_SCENE_NUMBER_COMMAND_7_BYTES[7] = {
				SYSEX_MANUFACTURER_BYTE1,   SYSEX_MANUFACTURER_BYTE2,
				SYSEX_MANUFACTURER_BYTE3,   SYSEX_AXE_VERSION,
				SYSEX_REQUEST_SCENE_NUMBER, SYSEX_QUERY_BYTE,
				SYSEX_CHECKSUM_PLACEHOLDER};

		const byte REQUEST_TEMPO_COMMAND_8_BYTES[8] = {
				SYSEX_MANUFACTURER_BYTE1, SYSEX_MANUFACTURER_BYTE2,
				SYSEX_MANUFACTURER_BYTE3, SYSEX_AXE_VERSION,
				SYSEX_REQUEST_TEMPO,      SYSEX_QUERY_BYTE,
				SYSEX_QUERY_BYTE,         SYSEX_CHECKSUM_PLACEHOLDER};

		const byte TAP_TEMPO_PULSE_COMMAND_6_BYTES[6] = {
				SYSEX_MANUFACTURER_BYTE1, SYSEX_MANUFACTURER_BYTE2,
				SYSEX_MANUFACTURER_BYTE3, SYSEX_AXE_VERSION,
				SYSEX_TAP_TEMPO_PULSE,    SYSEX_CHECKSUM_PLACEHOLDER};

		const byte ENABLE_TUNER_COMMAND_7_BYTES[7] = {
				SYSEX_MANUFACTURER_BYTE1,	SYSEX_MANUFACTURER_BYTE2,
				SYSEX_MANUFACTURER_BYTE3,	SYSEX_AXE_VERSION,
				SYSEX_TUNER,							SYSEX_TUNER_ON,
				SYSEX_CHECKSUM_PLACEHOLDER};

		const byte DISABLE_TUNER_COMMAND_7_BYTES[7] = {
				SYSEX_MANUFACTURER_BYTE1,	SYSEX_MANUFACTURER_BYTE2,
				SYSEX_MANUFACTURER_BYTE3,	SYSEX_AXE_VERSION,
				SYSEX_TUNER,							SYSEX_TUNER_OFF,
				SYSEX_CHECKSUM_PLACEHOLDER};

		const byte REQUEST_LOOPER_STATUS_COMMAND_7_BYTES[7] = {
				SYSEX_MANUFACTURER_BYTE1,  		SYSEX_MANUFACTURER_BYTE2,
				SYSEX_MANUFACTURER_BYTE3,  		SYSEX_AXE_VERSION,
				SYSEX_REQUEST_LOOPER_STATUS,  SYSEX_QUERY_BYTE,
				SYSEX_CHECKSUM_PLACEHOLDER};

void AxeSystem::requestFirmwareVersion() {
	sendCommand(SYSEX_REQUEST_FIRMWARE);
}

void AxeSystem::requestPresetName(int preset) {

	byte data[2] = {SYSEX_QUERY_BYTE};

  if (preset != -1) {
    intToMidiBytes(preset, &data[0], &data[1]);
  }

	sendCommand(SYSEX_REQUEST_PRESET_INFO, data, 2);
}

void AxeSystem::requestSceneName() {
	sendSysEx(7, (byte*) REQUEST_SCENE_NAME_COMMAND_7_BYTES);
}

void AxeSystem::requestSceneNumber() {
	sendSysEx(7, (byte*) REQUEST_SCENE_NUMBER_COMMAND_7_BYTES);
}


void AxeSystem::requestEffectDetails() {
	sendCommand(SYSEX_EFFECT_DUMP);
}

void AxeSystem::sendSceneChange(byte scene) {
	byte command[7];
	memcpy(command, REQUEST_SCENE_NUMBER_COMMAND_7_BYTES, 7);
	command[5] = scene - 1;
	sendSysEx(7, (byte*) command);
}

void AxeSystem::requestLooperStatus() {
	sendSysEx(7, (byte*) REQUEST_LOOPER_STATUS_COMMAND_7_BYTES);
}

void AxeSystem::pressLooperButton(LooperButton button) {
	byte command[7];
	memcpy(command, REQUEST_LOOPER_STATUS_COMMAND_7_BYTES, 7);
	command[5] = button;
	sendSysEx(7, (byte*) command);
}

void AxeSystem::requestTempo() {
	sendSysEx(8, (byte*) REQUEST_TEMPO_COMMAND_8_BYTES);
}

void AxeSystem::setTempo(Tempo tempo) {
  byte command[8];
	memcpy(command, REQUEST_TEMPO_COMMAND_8_BYTES, 8);
	intToMidiBytes(tempo, &command[5], &command[6]);
	sendSysEx(8, (byte*) command);

	//Axe won't notify us of this, so send a manual system change event
	_tempo = tempo;
	callSystemChangeCallback();
}

void AxeSystem::sendTap() {
	sendSysEx(6, (byte*) TAP_TEMPO_PULSE_COMMAND_6_BYTES);
}

void AxeSystem::toggleTuner() {
	_tunerEngaged ? disableTuner() : enableTuner();
}

void AxeSystem::enableTuner() {
	_tunerEngaged = true;
	_lastTunerResponse = millis(); //reset timer
  sendSysEx(7, (byte*) ENABLE_TUNER_COMMAND_7_BYTES);
	callTunerStatusCallback(_tunerEngaged);
}

void AxeSystem::disableTuner() {
	_tunerEngaged = false;
  sendSysEx(7, (byte*) DISABLE_TUNER_COMMAND_7_BYTES);
	callTunerStatusCallback(_tunerEngaged);
}

void AxeSystem::enableEffect(EffectId effectId) {
  byte command[9];
	memcpy(command, REQUEST_EFFECT_BYPASSED_COMMAND_9_BYTES, 9);
	intToMidiBytes(effectId, &command[5], &command[6]);
	command[7] = SYSEX_EFFECT_ENABLE;
	sendSysEx(9, (byte*) command);
}

void AxeSystem::disableEffect(EffectId effectId) {
	byte command[9];
	memcpy(command, REQUEST_EFFECT_BYPASSED_COMMAND_9_BYTES, 9);
	intToMidiBytes(effectId, &command[5], &command[6]);
	command[7] = SYSEX_EFFECT_BYPASS;
	sendSysEx(9, (byte*) command);
}

void AxeSystem::sendPresetIncrement() {
  int number = _preset.getPresetNumber();
  if (isValidPresetNumber(number)) {
    if (++number > (BANK_SIZE * MAX_BANKS) - 1) {
      number = 0;
    }
    sendPresetChange(number);
  }
}

void AxeSystem::sendPresetDecrement() {
  int number = _preset.getPresetNumber();
  if (isValidPresetNumber(number)) {
    if (number == 0) {
      number = (BANK_SIZE * MAX_BANKS) - 1;
    } else {
      number--;
    }
    sendPresetChange(number);
  }
}

void AxeSystem::sendSceneIncrement() {
  byte number = _preset.getSceneNumber();
  if (isValidSceneNumber(number)) {
    if (++number > MAX_SCENES) {
      number = 0;
    }
    sendSceneChange(number);
  }
}

void AxeSystem::sendSceneDecrement() {
  byte number = _preset.getSceneNumber();
  if (isValidSceneNumber(number)) {
    if (number == 0) {
      number = MAX_SCENES - 1;
    } else {
      number--;
    }
    sendSceneChange(number);
  }
}
