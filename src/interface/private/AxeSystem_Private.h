#pragma once

friend class AxeEffect;

const static byte SYSEX_END = 0xF7;
const static byte MAX_SYSEX = 128;
const static unsigned MIDI_BAUD = 31250;

const static byte BANK_CHANGE_CC = 0x00;
const static byte SYSEX_TUNER_ON = 0x01;
const static byte SYSEX_TUNER_OFF = 0x00;
const static byte SYSEX_EFFECT_BYPASS = 0x01;
const static byte SYSEX_EFFECT_ENABLE = 0x00;

const static byte SYSEX_MANUFACTURER_BYTE1 = 0x00;
const static byte SYSEX_MANUFACTURER_BYTE2 = 0x01;
const static byte SYSEX_MANUFACTURER_BYTE3 = 0x74;
const static byte SYSEX_AXE_VERSION = 0x10;
const static byte SYSEX_QUERY_BYTE = 0x7F;
const static byte SYSEX_CHECKSUM_PLACEHOLDER = 0x00;

const static byte SYSEX_REQUEST_FIRMWARE = 0x08;
const static byte SYSEX_REQUEST_EFFECT_BYPASS = 0x0A;
const static byte SYSEX_REQUEST_EFFECT_CHANNEL = 0x0B;
const static byte SYSEX_REQUEST_SCENE_NUMBER = 0x0C;
const static byte SYSEX_REQUEST_PRESET_INFO = 0x0D;
const static byte SYSEX_REQUEST_SCENE_INFO = 0x0E;
const static byte SYSEX_REQUEST_LOOPER_STATUS = 0x0F;
const static byte SYSEX_TAP_TEMPO_PULSE = 0x10;
const static byte SYSEX_TUNER = 0x11;
const static byte SYSEX_EFFECT_DUMP = 0x13;
const static byte SYSEX_REQUEST_TEMPO = 0x14;

const static byte MAX_TUNER_NOTE = 11;
const static byte MAX_TUNER_STRING = 6;
const static byte MAX_TUNER_FINETUNE = 127;

enum MidiType {
    ControlChange = 0xB0,
    ProgramChange = 0xC0,
    SystemExclusive = 0xF0
};

void readMidi();
void checkFirmware();
void checkTimers();
void checkIncomingPreset();
void sendSysEx(const byte length, byte *sysex);
void setSystemConnected(const bool connected);
bool isAxeSysEx(const byte *sysex, const byte length);
bool validateSysEx(const byte *sysex, const byte length);
byte calculateChecksum(const byte *, const byte);
bool isRequestedPreset(const PresetNumber);
void parseName(const byte *sysex, const byte length, const byte offset,
               char *buffer, const byte size);
void processEffectDump(const byte *sysex, const byte length);
void onSystemExclusive(const byte *sysex, const byte length);
void onPresetChange(const PresetNumber number, bool receivedFromAxe = true);

AxeEffect *findEffect(const EffectId);

void callConnectionStatusCallback(const bool connected);
void callTapTempoCallback();
void callPresetChangingCallback(const PresetNumber);
void callPresetNameCallback(const PresetNumber, const char *, const byte);
void callSceneNameCallback(const SceneNumber, const char *, const byte);
void callEffectsReceivedCallback(AxePreset *);
void callPresetChangeCallback(AxePreset *);
void callSystemChangeCallback();
void callTunerDataCallback(const char *note, const byte string,
                           const byte fineTune);
void callTunerStatusCallback(bool enabled);
void callLooperStatusCallback(AxeLooper *);
bool callSysexPluginCallback(const byte *, const byte);
bool callEffectFilterCallback(const PresetNumber, AxeEffect);

bool isValidPresetNumber(const PresetNumber preset);
bool isValidSceneNumber(const SceneNumber scene);
void intToMidiBytes(const int, byte *, byte *);
int midiBytesToInt(const byte, const byte);
bool filterMidiChannel(byte message);
byte applyMidiChannel(byte midiByte, byte channel);

#ifdef AXE_DEBUG_SYSEX
void debugSysex(const byte *sysex, const byte length, const char *message);
#endif

UpdateMode _updateMode = UpdateMode::AUTO;
AxePreset _preset, _incomingPreset;
AxeLooper _looper;
Version _firmwareVersion;
Version _usbVersion;
HardwareSerial *_serial = nullptr;
byte _tempo;
byte _bank;
byte _midiChannel;
bool _firmwareRequested = false;
bool _tunerEngaged = false;
bool _systemConnected = false;
bool _midiReady = false;
bool _readingSysex = false;
bool _fetchEffects = false;
byte _sysexBuffer[MAX_SYSEX];
byte _sysexCount;
byte _tunerIncomingCount = 0, _tunerTriggerThreshold = 15;
millis_t _startupDelay = 1000;
millis_t _refreshRate = 0, _refreshThrottle = 500;
millis_t _sysexTimout = 2000, _tunerTimout = 800;
millis_t _lastSysexResponse = 0, _lastTunerResponse = 0, _lastRefresh = 0;

ConnectionStatusCallback _connectionStatusCallback;
TapTempoCallback _tapTempoCallback;
PresetChangingCallback _presetChangingCallback;
PresetNameCallback _presetNameCallback;
SceneNameCallback _sceneNameCallback;
EffectsReceivedCallback _effectsReceivedCallback;
PresetChangeCallback _presetChangeCallback;
SystemChangeCallback _systemChangeCallback;
TunerStatusCallback _tunerStatusCallback;
TunerDataCallback _tunerDataCallback;
LooperStatusCallback _looperStatusCallback;
SysexPluginCallback _sysexPluginCallback;
EffectFilterCallback _effectFilterCallback;

const char *_notes[12] = {"A ", "Bb", "B ", "C ", "C#", "D ",
                          "Eb", "E ", "F ", "F#", "G ", "Ab"};
