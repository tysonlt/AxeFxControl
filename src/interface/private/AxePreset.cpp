#include "interface/AxePreset.h"

void AxePreset::printPresetName(Print &printer, bool newline) {
  printer.print(_presetName);
  if (newline) {
    printer.println();
  }
}

void AxePreset::printSceneName(Print &printer, bool newline) {
  printer.print(_sceneName);
  if (newline) {
    printer.println();
  }
}

char *AxePreset::getPresetName() {
  return _presetName;
}

char *AxePreset::getSceneName() {
  return _sceneName;
}

void AxePreset::setPresetName(char *buffer) {
  snprintf(_presetName, MAX_PRESET_NAME + 1, buffer);
  _presetNameReceived = true;
}

void AxePreset::setSceneName(char *buffer) {
  snprintf(_sceneName, MAX_SCENE_NAME + 1, buffer);
  _sceneNameReceived = true;
}

void AxePreset::setEffects(const AxeEffect effects[], const unsigned count) {
  _effectCount = 0;
  for (unsigned i = 0; i < count && i < _maxEffects; i++) {
    _effects[_effectCount++] = effects[i];
  }
}

bool AxePreset::hasEffect(EffectId effectId) {
  if (effectId >= AxeEffect::MIN_EFFECT_ID && effectId <= AxeEffect::MAX_EFFECT_ID) {
    for (unsigned i = 0; i < _maxEffects; i++) {
      if (_effects[i]._effectId == effectId) {
        return true;
      }
    }
  }
  return false;
}

AxeEffect *AxePreset::getEffectById(const EffectId effectId) {
  if (effectId >= AxeEffect::MIN_EFFECT_ID && effectId <= AxeEffect::MAX_EFFECT_ID) {
    for (unsigned i = 0; i < _maxEffects; i++) {
      if (_effects[i]._effectId == effectId) {
        return &_effects[i];
      }
    }
  }
  return nullptr;
}

bool AxePreset::equals(AxePreset &p) {
  return _preset == p._preset &&
         _scene == p._scene &&
         0 == strcmp(_presetName, p._presetName) &&
         0 == strcmp(_sceneName, p._sceneName) &&
         !effectsChanged(p);
}

bool AxePreset::effectsChanged(AxePreset &p) {

  if (_preset != p._preset ||
      _scene != p._scene ||
      _effectCount != p._effectCount) {
    return true;
  }

  for (byte i = 0; i < _effectCount; i++) {
    if (_effects[i]._effectId != p._effects[i]._effectId ||
        _effects[i]._bypassed != p._effects[i]._bypassed) {
      return true;
    }
  }

  return false;
}

void AxePreset::copyPresetName(char *buffer, size_t max) {
  snprintf(buffer, max, _presetName);
}

void AxePreset::copySceneName(char *buffer, size_t max) {
  snprintf(buffer, max, _sceneName);
}

void AxePreset::reset() {
  _scene = -1;
  _preset = -1;
  _effectCount = -1;
  _presetNameReceived = false;
  _sceneNameReceived = false;
  memset(_presetName, 0, MAX_PRESET_NAME);
  memset(_sceneName, 0, MAX_SCENE_NAME);
}

bool AxePreset::isComplete() {
  return _preset != -1 &&
         _scene != -1 &&
         _effectCount != -1 &&
         _presetNameReceived &&
         _sceneNameReceived;
}
