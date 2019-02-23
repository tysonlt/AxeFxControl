#pragma once

friend class AxeSystem;

// These methods are used by AxeSystem to build the preset.
// You don't need to call them, and you can't.
void setPresetNumber(int number) {  _preset = number; }
void setSceneNumber(int number) {  _scene = number; }
void setPresetName(char *buffer);
void setSceneName(char *buffer);
void reset();	
bool isComplete();
void setEffects(const AxeEffect effects[], const unsigned count);

int _scene = -1;
int _preset = -1;
int _effectCount = -1;	
byte _maxEffects = MAX_EFFECTS;
bool _presetNameReceived = false, _sceneNameReceived = false;
char _presetName[MAX_PRESET_NAME + 1];
char _sceneName[MAX_SCENE_NAME + 1];
AxeEffect _effects[MAX_EFFECTS];
