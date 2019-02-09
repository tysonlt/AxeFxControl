#pragma once

#include <Arduino.h>
#include "AxePreset.h"
#include "AxeEffect.h"
#include "AxeLooper.h"
#include "AxeFxControl.h"

typedef byte Tempo;
typedef int16_t PresetNumber;
typedef int8_t SceneNumber;
typedef unsigned long millis_t;

typedef void (*ConnectionStatusCallback)(bool);
typedef void (*TapTempoCallback)();
typedef void (*PresetChangingCallback)(PresetNumber);
typedef void (*PresetChangeCallback)(AxePreset);
typedef void (*SystemChangeCallback)();
typedef void (*TunerStatusCallback)(bool);
typedef void (*TunerDataCallback)(const char *, const byte, const byte);
typedef void (*LooperStatusCallback)(AxeLooper);
typedef bool (*SysexPluginCallback)(const byte*, const byte);

struct Version {
	byte major, minor;
};

class AxeSystem {

	public:

		AxeSystem() {
			_looper.setAxeSystem(this);
		}

		void begin(HardwareSerial& serial, byte midiChannel = DEFAULT_MIDI_CHANNEL);
		void setMidiChannel(byte channel) { _midiChannel = channel; }		
		void update();

		// Update preset details every millis. Don't refresh if another preset request was received within throttle interval.
		void enableRefresh(const millis_t millis = 3000, const millis_t throttle = 500);
		void refresh(bool ignoreThrottle = false);

		void requestPresetDetails() { requestPresetName(); }
		void requestFirmwareVersion();		
		void requestTempo();
		void setTempo(const Tempo tempo);
		void sendTap();
		void toggleTuner();
		void enableTuner();
		void disableTuner();
		void enableEffect(const EffectId);
		void disableEffect(const EffectId);
		void requestLooperStatus();
		void pressLooperButton(const LooperButton);

		//for generic commands
		void sendCommand(const byte command);
		void sendCommand(const byte command, const byte *data, const byte paramCount);

		void sendPresetIncrement();
		void sendPresetDecrement();
		void sendSceneIncrement();
		void sendSceneDecrement();
		void sendPresetChange(const PresetNumber preset);
		void sendSceneChange(const SceneNumber scene);

		//for you hackers out there...
		void sendControlChange(byte controller, byte value, byte channel);
		void sendProgramChange(byte value, byte channel);
		void sendSysEx(const byte *sysex, const byte length);

		bool isPresetChanging() { return _presetChanging; }
		bool isTunerEngaged() { return _tunerEngaged; }
		Tempo getTempo() { return _tempo; }
		AxePreset& getCurrentPreset() { return _preset; }
		AxeLooper& getLooper() { return _looper; }
		Version getFirmwareVersion() { return _firmwareVersion; }
		Version getUsbVersion() { return _usbVersion; }

		void setSysexTimout(const millis_t ms) { _sysexTimout = ms; }
		void setTunerTimeout(const millis_t ms) { _tunerTimout = ms; }
		void setStartupDelay(const millis_t ms) { _startupDelay = ms; }

		void registerConnectionStatusCallback(ConnectionStatusCallback);
		void registerPresetChangingCallback(PresetChangingCallback);
		void registerPresetChangeCallback(PresetChangeCallback);
		void registerSystemChangeCallback(SystemChangeCallback);
		void registerTapTempoCallback(TapTempoCallback);
		void registerTunerDataCallback(TunerDataCallback);
		void registerTunerStatusCallback(TunerStatusCallback);
		void registerLooperStatusCallback(LooperStatusCallback);
		void registerSysexPluginCallback(SysexPluginCallback); //return true to halt further processing

	public:

		const static byte BANK_SIZE 												= 128;
		const static byte MAX_BANKS 												= 4;
		const static byte MAX_SCENES 												= 8;
		const static byte TEMPO_MIN 												= 24;
		const static byte TEMPO_MAX 												= 250;
		const static byte MIDI_CHANNEL_OMNI									= 0;
		const static byte DEFAULT_MIDI_CHANNEL							= MIDI_CHANNEL_OMNI;
		constexpr static PresetNumber MAX_PRESETS 					= (MAX_BANKS * BANK_SIZE) - 1;

	private:

		const static byte SYSEX_END													= 0xF7;
		const static byte MAX_SYSEX 												= 128;
		const static unsigned MIDI_BAUD											= 31250;

		const static byte BANK_CHANGE_CC 										= 0x00;
		const static byte SYSEX_TUNER_ON 										= 0x01;
		const static byte SYSEX_TUNER_OFF 									= 0x00;
		const static byte SYSEX_EFFECT_BYPASS 							= 0x01;
		const static byte SYSEX_EFFECT_ENABLE 							= 0x00;

		const static byte SYSEX_MANUFACTURER_BYTE1 					= 0x00;
		const static byte SYSEX_MANUFACTURER_BYTE2 					= 0x01;
		const static byte SYSEX_MANUFACTURER_BYTE3 					= 0x74;
		const static byte SYSEX_AXE_VERSION 								= 0x10;
		const static byte SYSEX_QUERY_BYTE 									= 0x7F;
		const static byte SYSEX_CHECKSUM_PLACEHOLDER 				= 0x00;

		const static byte SYSEX_REQUEST_FIRMWARE 						= 0x08;
		const static byte SYSEX_REQUEST_EFFECT_BYPASS 			= 0x0A;
		const static byte SYSEX_REQUEST_EFFECT_CHANNEL 			= 0x0B;
		const static byte SYSEX_REQUEST_SCENE_NUMBER 				= 0x0C;
		const static byte SYSEX_REQUEST_PRESET_INFO 				= 0x0D;
		const static byte SYSEX_REQUEST_SCENE_INFO 					= 0x0E;
		const static byte SYSEX_REQUEST_LOOPER_STATUS 			= 0x0F;
		const static byte SYSEX_TAP_TEMPO_PULSE 						= 0x10;
		const static byte SYSEX_TUNER 											= 0x11;
		const static byte SYSEX_EFFECT_DUMP 								= 0x13;
		const static byte SYSEX_REQUEST_TEMPO 							= 0x14;

		enum MidiType {
			ControlChange = 0xB0,
			ProgramChange = 0xC0,
			SystemExclusive = 0xF0
		};
		
		void readMidi();
		void checkFirmware();
		void checkTimers();
		void checkIncomingPreset();
		void setChanging();
		void sendSysEx(const byte length, byte *sysex);
		void setSystemConnected(const bool connected);
		bool isAxeSysEx(const byte *sysex, const byte length);
		void parseName(const byte *sysex, const byte length, const byte offset, char *buffer, const byte size);
		void processEffectDump(const byte *sysex, const byte length);
		void onSystemExclusive(const byte *sysex, const byte length);
		void onPresetChange(const PresetNumber number);

		void requestPresetName(const PresetNumber number = -1);
		void requestSceneName(const SceneNumber number = -1);
		void requestSceneNumber();
		void requestEffectDetails();

		void callConnectionStatusCallback(bool connected);	
		void callTapTempoCallback();
		void callPresetChangingCallback(PresetNumber number);
		void callPresetChangeCallback(AxePreset*);
		void callSystemChangeCallback();
		void callTunerDataCallback(const char *note, const byte string, const byte fineTune);
		void callTunerStatusCallback(bool enabled);
		void callLooperStatusCallback(AxeLooper*);
		bool callSysexPluginCallback(const byte*, const byte); 

		bool isValidPresetNumber(const PresetNumber preset);
		bool isValidSceneNumber(const SceneNumber scene); 
		void intToMidiBytes(const int, byte*, byte*);
		int midiBytesToInt(const byte, const byte);
		bool filterChannel(byte message);

		AxePreset _preset, _incomingPreset;
		AxeLooper _looper;
		Version _firmwareVersion;
		Version _usbVersion;
		HardwareSerial *_serial = nullptr;
		byte _tempo;
		byte _bank;
		byte _midiChannel = 1; //TODO: ask Axe
		bool _firmwareRequested = false;
		bool _tunerEngaged = false;
		bool _systemConnected = false;
		bool _presetChanging = false;
		bool _midiReady = false;
		byte _sysexBuffer[MAX_SYSEX];
		byte _sysexCount;
		bool _readingSysex = false;
		byte _tunerIncomingCount = 0, _tunerTriggerThreshold = 5;
		millis_t _startupDelay = 1000;
		millis_t _refreshRate = 0, _refreshThrottle = 500;
		millis_t _sysexTimout = 2000, _tunerTimout = 250;
		millis_t _lastSysexResponse = 0, _lastTunerResponse = 0, _lastRefresh = 0;

		ConnectionStatusCallback 	_connectionStatusCallback;
		TapTempoCallback 					_tapTempoCallback;
		PresetChangingCallback 		_presetChangingCallback;
		PresetChangeCallback 			_presetChangeCallback;
		SystemChangeCallback 			_systemChangeCallback;
		TunerStatusCallback 			_tunerStatusCallback;
		TunerDataCallback 				_tunerDataCallback;
		LooperStatusCallback 			_looperStatusCallback;
		SysexPluginCallback 			_sysexPluginCallback;
	
		const char *_notes[12] = {"A ", "Bb", "B ", "C ", "C#", "D ", "Eb", "E ", "F ", "F#", "G ", "Ab"};

};