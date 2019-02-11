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

struct Version {
	byte major, minor;
};

//
// This is the main class you will interact with. There is no need to worry about any other classes.
// Create an instance of this in your main sketch, above the setup function, like this: AxeSystem Axe;
// Then just use it anywhere in your code. Check the examples for how to set it up.
//
// The examples also function as unit tests, and as such they (should) cover every method in this class.
// If you need help, just go to the examples to see how to use the library.
// 
class AxeSystem {

	public:

		AxeSystem() {
			_looper.setAxeSystem(this);
		}

		// You must call begin with a hardware serial such as Serial1. Optionally set the MIDI channel, defaults to OMNI
		// You cannot call this more than once. (Well you can, but it will be ignored.)
		void begin(HardwareSerial& serial, byte midiChannel = DEFAULT_MIDI_CHANNEL);

		// Optionally set the MIDI channel, defaults to OMNI. You can safely change this at any time.
		void setMidiChannel(byte channel) { _midiChannel = channel; }		

		// Update preset details every millis. Don't refresh if another preset request was received within throttle interval.
		void enableRefresh(const millis_t millis = 3000, const millis_t throttle = 500);

		// Remember to call this from loop(). Avoid using delay() anywhere in your code for best results.
		void update();

		// Manually refresh the current preset. This is called automatically if enableRefresh() was called. 
		// Pass true to refresh immediately, regardless of when the last sysex message was recei ved.   
		void refresh(bool ignoreThrottle = false);

		//
		// These are the commands you can send to the Axe.
		// Notice that you cannot directly query the scene and effects. 
		// These must be fetched as part of a preset details request.
		// This helps maintain a proper state in case of fast preset changes.
		//
		// To get the information back, you should register a callback.
		// Call registerPresetChangeCallback() to be notified of preset details.
		//
		// Alternatively, just poll isPresetChanging() after sending a command and
		// then call get getPreset(). This method will return NULL until the preset 
		// is ready if you forget to check.
		//
		void requestPresetDetails() { requestPresetName(); }
		void requestFirmwareVersion();		
		void requestTempo();
		void requestLooperStatus();
		void pressLooperButton(const LooperButton);
		void setTempo(const Tempo tempo);
		void sendTap();
		void toggleTuner();
		void enableTuner();
		void disableTuner();

		// The preset also lets you access AxeEffect objects, 
		// which are a more convenient way to call these methods.
		void enableEffect(const EffectId);
		void disableEffect(const EffectId);
		void setEffectChannel(const EffectId, const Channel);

		// You can send your own command bytes to the Axe!
		// Currently we have implemented the entire 3rd part MIDI spec,
		// but if you want to send other stuff, go for your life!
		void sendCommand(const byte command);
		void sendCommand(const byte command, const byte *data, const byte paramCount);

		// Change the preset and scene.
		void sendPresetIncrement();
		void sendPresetDecrement();
		void sendSceneIncrement();
		void sendSceneDecrement();
		void sendPresetChange(const PresetNumber preset);
		void sendSceneChange(const SceneNumber scene);

		// For you hackers out there, you can send standard MIDI CC/PC/SysEx
		// on any channel. Unlike sendPresetChange(), these will not trigger
		// any state changes or state updates, so you can safely call away.
		void sendControlChange(byte controller, byte value, byte channel);
		void sendProgramChange(byte value, byte channel);
		void sendSysEx(const byte *sysex, const byte length);

		// These two methods tell you whether the preset is in the middle of
		// changing. During this time, sysex data is being read from the Axe
		// so you can't retrieve the current preset. 
		bool isPresetReady() { return !_presetChanging; }
		bool isPresetChanging() { return _presetChanging; }

		// Well is it, or isn't it? As far as we know! This is pretty accurate
		// even if you enable/disable the tuner from the front panel.
		bool isTunerEngaged() { return _tunerEngaged; }

		// Get the current tempo as a BPM value.
		Tempo getTempo() { return _tempo; }

		// Return a COPY of the current preset. This is not a pointer, so it
		// won't magically update after a refresh. Either call getPreset() again,
		// or better yet, register a preset change callback to get fresh state.
		AxePreset getCurrentPreset() { return _preset; }

		// Get the looper object to query and play with. 
		AxeLooper& getLooper() { return _looper; }

		// Get the firmware version. Not that useful but nice to display at boot perhaps.
		Version getFirmwareVersion() { return _firmwareVersion; }
		Version getUsbVersion() { return _usbVersion; }

		// Change at your peril. Determines how much radio silence will result 
		// in a disconnect callback being fired. (Which does nothing, just lets you know.)
		void setSysexTimout(const millis_t ms) { _sysexTimout = ms; }

		// Change at your peril. If we think the tuner is engaged, but this time passes
		// without a tuner message, we will assume the tuner was turned off at the front panel.
		void setTunerTimeout(const millis_t ms) { _tunerTimout = ms; }

		// OK, you probably can mess with this one. Some hardware requires a delay after 
		// calling begin() on the Serial object before it is actually ready. Perhaps this
		// is only a problem with cheap clones, but the default 1000ms pause solves a lot
		// of stability problems, so it is safe to leave it in. Show a pretty pattern on 
		// your screen or something.
		void setStartupDelay(const millis_t ms) { _startupDelay = ms; }

		// These are the various state change callbacks you can register.
		// This is the easiest way to be notified of what is going with the Axe.
		// Just call the releveant registerXXXCallback() method from setup(), and
		// As long as you call Axe.update() from loop(), you will be notified of 
		// relevant state changes. PresetChangeCallback is probably the one you want.
		typedef void (*ConnectionStatusCallback)(const bool);
		typedef void (*TapTempoCallback)();
		typedef void (*PresetChangingCallback)(const PresetNumber);
		typedef void (*PresetNameCallback)(const PresetNumber, const char*, const byte);
		typedef void (*SceneNameCallback)(const SceneNumber, const char*, const byte);
		typedef void (*EffectsReceivedCallback)(const PresetNumber, AxePreset);
		typedef void (*PresetChangeCallback)(AxePreset);
		typedef void (*SystemChangeCallback)();
		typedef void (*TunerStatusCallback)(const bool);
		typedef void (*TunerDataCallback)(const char *, const byte, const byte);
		typedef void (*LooperStatusCallback)(AxeLooper);

		// Hackers delight! These let you complement or override the standard sysex
		// and effect filtering code. For the sysex plugin, you get to respond to any
		// incoming sysex before standard processing. Returning false will cancel
		// standard processing, returning true will continue on as normal.
		typedef bool (*SysexPluginCallback)(const byte*, const byte);

		// By default, we filter some effects out of the effect list that we deem 
		// unlikely to be useful if you want to display current effects. If you 
		// don't like this, register this callback and you will be able to control
		// which effects are filtered out. Return true to allow an effect, false
		// to filter it out of the effect list.
		typedef bool (*EffectFilterCallback)(const PresetNumber, const AxeEffect);

		// These are the best way to be notified of state changes. If you just
		// want to know when a preset changes, call registerPresetChangeCallback(),
		// and then update your screen/lcd whenever this is called.

		// Called on our first message from the Axe, and if the sysex timeout has passed.
		void registerConnectionStatusCallback(ConnectionStatusCallback);

		// Called when we detect a preset is about to change (ie you called it, or front panel change.)
		void registerPresetChangingCallback(PresetChangingCallback);

		// This is probably the one you want. Lets you know when the new preset is ready to query.
		void registerPresetChangeCallback(PresetChangeCallback);

		// These three give you fine-grained notification when different parts of the preset
		// are received. This is useful if you only want the name and don't care about the effects.
		void registerPresetNameCallback(PresetNameCallback);
		void registerSceneNameCallback(SceneNameCallback);
		void registerEffectsReceivedCallback(EffectsReceivedCallback); 
		
		// Called when a global parameter changes... well, when the tempo changes. 
		// That's all we know about!
		void registerSystemChangeCallback(SystemChangeCallback);

		// The Axe sends real-time pulses in time with the tempo. 
		// Flash a led! I woudln't update a TFT or LCD, it's too slow.
		// This requires Setup -> MIDI -> Realtime Sysex to be enabled on the Axe.
		void registerTapTempoCallback(TapTempoCallback);
		
		// Tuner data comes flooding in when the tuner is engaged.
		// String goes from 1 to 6, and finetune from 0-127, where 63 is in tune.
		// This requires Setup -> MIDI -> Realtime Sysex to be enabled on the Axe.
		void registerTunerDataCallback(TunerDataCallback);

		// Tuner on/off notifications. Due to some heuristic magic, it's pretty accurate.
		void registerTunerStatusCallback(TunerStatusCallback);

		// Something changed on the looper.
		void registerLooperStatusCallback(LooperStatusCallback);

		// These let you override the way messages are processed. 
		// See the typedef documentation above to learn how they work.
		void registerSysexPluginCallback(SysexPluginCallback); //return true to halt further processing
		void registerEffectFilterCallback(EffectFilterCallback); //return false to remove from the effect list

	public:

		//These are values supported by the AxeFX 3, and can't be changed.
		const static byte BANK_SIZE 												= 128;
		const static byte MAX_BANKS 												= 4;
		const static byte MAX_SCENES 												= 8;
		const static byte TEMPO_MIN 												= 24;
		const static byte TEMPO_MAX 												= 250;
		const static byte MIDI_CHANNEL_OMNI									= 0;
		const static byte DEFAULT_MIDI_CHANNEL							= MIDI_CHANNEL_OMNI;
		constexpr static PresetNumber MAX_PRESETS 					= (MAX_BANKS * BANK_SIZE) - 1;

	private:

		// OK, there ends the tour! Continue on to AxePreset.h, 
		// AxeEffect.h, and AxeLooper.h for more information.

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
		void sendSysEx(const byte length, byte *sysex);
		void setSystemConnected(const bool connected);
		bool isAxeSysEx(const byte *sysex, const byte length);
		bool isRequestedPreset(const PresetNumber);
		void parseName(const byte *sysex, const byte length, const byte offset, char *buffer, const byte size);
		void processEffectDump(const byte *sysex, const byte length);
		void onSystemExclusive(const byte *sysex, const byte length);
		void onPresetChange(const PresetNumber number);

		void requestPresetName(const PresetNumber number = -1);
		void requestSceneName(const SceneNumber number = -1);
		void requestSceneNumber();
		void requestEffectDetails();
		void callConnectionStatusCallback(const bool connected);	
		void callTapTempoCallback();
		void callPresetChangingCallback(const PresetNumber);
		void callPresetNameCallback(const PresetNumber, const char*, const byte);
		void callSceneNameCallback(const SceneNumber, const char*, const byte);
		void callEffectsReceivedCallback(AxePreset*);
		void callPresetChangeCallback(AxePreset*);
		void callSystemChangeCallback();
		void callTunerDataCallback(const char *note, const byte string, const byte fineTune);
		void callTunerStatusCallback(bool enabled);
		void callLooperStatusCallback(AxeLooper*);
		bool callSysexPluginCallback(const byte*, const byte); 
		bool callEffectFilterCallback(const PresetNumber, AxeEffect); 

		bool isValidPresetNumber(const PresetNumber preset);
		bool isValidSceneNumber(const SceneNumber scene); 
		void intToMidiBytes(const int, byte*, byte*);
		int midiBytesToInt(const byte, const byte);
		bool filterChannel(byte message);

		#ifdef AXE_DEBUG_SYSEX
		void debugSysex(const byte *sysex, const byte length, const char *message);
		#endif

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
		PresetNameCallback				_presetNameCallback;
		SceneNameCallback					_sceneNameCallback;
		EffectsReceivedCallback		_effectsReceivedCallback;
		PresetChangeCallback 			_presetChangeCallback;
		SystemChangeCallback 			_systemChangeCallback;
		TunerStatusCallback 			_tunerStatusCallback;
		TunerDataCallback 				_tunerDataCallback;
		LooperStatusCallback 			_looperStatusCallback;
		SysexPluginCallback 			_sysexPluginCallback;
		EffectFilterCallback			_effectFilterCallback;

		const char *_notes[12] = {"A ", "Bb", "B ", "C ", "C#", "D ", "Eb", "E ", "F ", "F#", "G ", "Ab"};

};