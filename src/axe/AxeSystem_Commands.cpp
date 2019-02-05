#include "AxeSystem.h"

/**
 * Ask Axe to send us its firmware.
 */
void AxeSystem::requestFirmwareVersion() {
	sendSysEx(6, (byte*) REQUEST_FIRMWARE_VERSION_COMMAND_6_BYTES);
}

/**
 * Ask for the current preset name and number.
 */
void AxeSystem::requestPresetName(int preset) {
  if (preset == -1) {
    sendSysEx(8, (byte*) REQUEST_PRESET_NAME_COMMAND_8_BYTES);
  } else {
    byte command[8];
    byte bank = preset / BANK_SIZE;
    memcpy(command, REQUEST_PRESET_NAME_COMMAND_8_BYTES, 8);
    command[5] = preset - (bank * BANK_SIZE);
    command[6] = bank;
    sendSysEx(8, (byte*) command);
  }
}

/**
 * Ask for the current scene name and number.
 */
void AxeSystem::requestSceneName() {
	sendSysEx(7, (byte*) REQUEST_SCENE_NAME_COMMAND_7_BYTES);
}

/**
 * Ask for just the scene number.
 */
void AxeSystem::requestSceneNumber() {
	sendSysEx(7, (byte*) REQUEST_SCENE_NUMBER_COMMAND_7_BYTES);
}

/**
 * Ask for list of effects for current preset.
 */ 
void AxeSystem::requestEffectDetails() {
	sendSysEx(6, (byte*) REQUEST_EFFECT_DUMP_6_BYTES);
}


/**
 * Send one-based scene change.
 */ 
void AxeSystem::sendSceneChange(byte scene) {
	byte command[7];
	memcpy(command, REQUEST_SCENE_NUMBER_COMMAND_7_BYTES, 7);
	command[5] = scene - 1;
	sendSysEx(7, (byte*) command);
}

/**
 * Ask for the current tempo.
 */
void AxeSystem::requestTempo() {
	sendSysEx(8, (byte*) REQUEST_TEMPO_COMMAND_8_BYTES);
}

/**
 * Send a tap-tempo pulse.
 */
void AxeSystem::sendTap() {
	sendSysEx(6, (byte*) TAP_TEMPO_PULSE_COMMAND_6_BYTES);
}

/**
 * Switch tuner state.
 */ 
void AxeSystem::toggleTuner() {
	_tunerEngaged ? disableTuner() : enableTuner();
	callTunerStatusCallback(_tunerEngaged);
}

/**
 * Turn the tuner on.
 */
void AxeSystem::enableTuner() {
	sendSysEx(6, (byte*) ENABLE_TUNER_COMMAND_7_BYTES);
	_tunerEngaged = true;
	callTunerStatusCallback(_tunerEngaged);
}

/**
 * As expected, turn the tuner off.
 */
void AxeSystem::disableTuner() {
	sendSysEx(6, (byte*) DISABLE_TUNER_COMMAND_7_BYTES);
	_tunerEngaged = false;
	callTunerStatusCallback(_tunerEngaged);
}

/**
 * Move up one preset.
 */
void AxeSystem::sendPresetIncrement() {
  int number = _preset.getPresetNumber();
  if (isValidPresetNumber(number)) {
    if (++number > (BANK_SIZE * MAX_BANKS) - 1) {
      number = 0;
    }
    sendPresetChange(number);
  }
}

/**
 * Move down one preset.
 */
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

/**
 * Move up one scene.
 */
void AxeSystem::sendSceneIncrement() {
  byte number = _preset.getSceneNumber();
  if (isValidSceneNumber(number)) {
    if (++number > MAX_SCENES) {
      number = 0;
    }
    sendSceneChange(number);
  }
}

/**
 * Move down one scene.
 */
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
