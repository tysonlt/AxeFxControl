#pragma once

#include <Arduino.h>
#include "AxeFxControl.h"
#include "private/AxeTypes.h"
#include "AxeLooper.h"
#include "AxeEffect.h"
#include "AxePreset.h"

//
// This is the main class you will interact with. There is no need to worry about 
// including any other classes.
// 
// Create an instance of this in your main sketch, above the setup function.
// Then just use it anywhere in your code. Check the examples for how to set it up.
//
// The examples also function as unit tests, and as such they (should) cover every 
// method in this class. If you need help, just go to the examples to see how to use 
// the library.
// 
class AxeSystem {

	public:

		AxeSystem() {
			_looper.setAxeSystem(this);
		}

		// You must call begin with a hardware serial such as Serial1. Optionally set the 
		// MIDI channel, defaults to OMNI.
		// You cannot call this more than once. (Well you can, but it will be ignored.)
		void begin(HardwareSerial& serial, byte midiChannel = DEFAULT_MIDI_CHANNEL);

		// Optionally set the MIDI channel. You can safely change this at any time.
		void setMidiChannel(byte channel) { _midiChannel = channel; }		
		byte getMidiChannel() { return _midiChannel; }
		
		// Update preset details every millis. Don't refresh if another preset request 
		// was received within throttle interval.
		void enableRefresh(const millis_t millis = 1000, const millis_t throttle = 100);

		// Remember to call this from loop(). Avoid using delay() anywhere in your code 
		// for best results.
		void update();

		// Manually refresh the current preset. This is called automatically 
		// if enableRefresh() was called. 
		// Pass true to refresh immediately, regardless of when the last sysex 
		// message was receieved.   
		void refresh(bool ignoreThrottle = false);

		// Whether to ask for the list of effects for each preset. Defaults to false.
		void fetchEffects(bool fetch) { _fetchEffects = fetch; }

		// These are the commands you can send to the Axe.
		//
		// To get the results back, you should register a callback.
		// Call registerPresetChangeCallback() to be notified of preset changes.
		//
		// Alternatively, you can call getPreset() any time you want, but it is
		// not guaranteed to be current. You should register a callback instead.
		void requestFirmwareVersion();		
		void requestTempo();
		void requestLooperStatus();
		void pressLooperButton(const LooperButton);
		void setTempo(const Tempo tempo);
		void sendTap();
		void toggleTuner();
		void enableTuner();
		void disableTuner();

		void enableEffect(const EffectId);
		void disableEffect(const EffectId);
		void toggleEffect(const EffectId); 
		bool isEffectEnabled(const EffectId); 
		void setEffectChannel(const EffectId, const Channel);
		void sendEffectChannelIncrement(const EffectId);
		void sendEffectChannelDecrement(const EffectId);
		
		void requestPresetDetails() { requestPresetName(); }
		void requestPresetName(const PresetNumber number = -1);
		void requestSceneName(const SceneNumber number = -1);
		void requestSceneNumber();
		void requestEffectDetails();

		// You can send your own command bytes to the Axe!
		// Currently we have implemented the entire 3rd part MIDI spec,
		// but if you want to send other stuff, go for your life!
		void sendCommand(const byte command);
		void sendCommand(const byte command, const byte *data, const byte paramCount);

		// Change the preset and scene.
		void sendPresetIncrement();
		void sendPresetDecrement();
		void sendSceneIncrement(bool triggerSceneNameRefresh = true);
		void sendSceneDecrement(bool triggerSceneNameRefresh = true);
		void sendPresetChange(const PresetNumber preset);
		void sendSceneChange(const SceneNumber scene);

		// For you hackers out there, you can send standard MIDI CC/PC/SysEx
		// on any channel. Unlike sendPresetChange(), these will not trigger
		// any state changes or state updates, so you can safely call away.
		void sendControlChange(byte controller, byte value, byte channel);
		void sendProgramChange(byte value, byte channel);
		void sendSysEx(const byte *sysex, const byte length);

		// Well is it, or isn't it? As far as we know! This is pretty accurate
		// even if you enable/disable the tuner from the front panel.
		bool isTunerEngaged() { return _tunerEngaged; }

		// Get the current tempo as a BPM value.
		Tempo getTempo() { return _tempo; }

		// Return a reference to the current preset.
		AxePreset& getCurrentPreset() { return _preset; }

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
		// your screen or something, or set to 0 to disable altogether.
		void setStartupDelay(const millis_t ms) { _startupDelay = ms; }

		// This is the best way to be notified of state changes. If you just
		// want to know when a preset changes, call registerPresetChangeCallback(),
		// and then update your screen/lcd whenever this is called.
		typedef void (*PresetChangeCallback)(AxePreset);
		void registerPresetChangeCallback(PresetChangeCallback);

		// Called on our first message from the Axe, and if the sysex timeout has passed.
		typedef void (*ConnectionStatusCallback)(const bool connected);
		void registerConnectionStatusCallback(ConnectionStatusCallback);

		// Called when we detect a preset is about to change. 
		typedef void (*PresetChangingCallback)(const PresetNumber);
		void registerPresetChangingCallback(PresetChangingCallback);

		// These three give you fine-grained notification when different parts of the preset
		// are received. This is useful if you only want the name and don't care about the effects.
		typedef void (*PresetNameCallback)(const PresetNumber, const char* name, const byte length);
		void registerPresetNameCallback(PresetNameCallback);

		// Called when the scene name is received
		typedef void (*SceneNameCallback)(const SceneNumber, const char* name, const byte length);
		void registerSceneNameCallback(SceneNameCallback);

		// Called when the effects list is receieved (after being filtered)
		typedef void (*EffectsReceivedCallback)(const PresetNumber, AxePreset);
		void registerEffectsReceivedCallback(EffectsReceivedCallback); 
		
		// Called when a global parameter changes... well, when the tempo changes. 
		// That's all we know about. Will also be called when we read the firmware 
		// version at startup. Query the global Axe object to get new data.
		typedef void (*SystemChangeCallback)();
		void registerSystemChangeCallback(SystemChangeCallback);

		// The Axe sends real-time pulses in time with the tempo. 
		// Flash a led! I woudln't update a TFT or LCD, it's too slow.
		// This requires Setup -> MIDI -> Realtime Sysex to be enabled on the Axe.
		typedef void (*TapTempoCallback)();
		void registerTapTempoCallback(TapTempoCallback);
		
		// Tuner data comes flooding in when the tuner is engaged.
		// String goes from 1 to 6, and finetune from 0-127, where 63 is in-tune.
		// This requires Setup -> MIDI -> Realtime Sysex to be enabled on the Axe.
		typedef void (*TunerDataCallback)(const char *note, const byte string, const byte fineTune);
		void registerTunerDataCallback(TunerDataCallback);

		// Tuner on/off notifications. Due to some heuristic magic, it's pretty accurate.
		typedef void (*TunerStatusCallback)(const bool engaged);
		void registerTunerStatusCallback(TunerStatusCallback);

		// Something changed on the looper. 
		typedef void (*LooperStatusCallback)(AxeLooper);
		void registerLooperStatusCallback(LooperStatusCallback);

		// Hackers delight! This lets you complement or override the standard sysex
		// and effect filtering code. For the sysex plugin, you get to respond to any
		// incoming sysex before standard processing. Returning false will cancel
		// standard processing, returning true will continue on as normal.
		typedef bool (*SysexPluginCallback)(const byte *sysex, const byte length);
		void registerSysexPluginCallback(SysexPluginCallback); 

		// By default, we filter some effects out of the effect list that we deem 
		// unlikely to be useful if you want to display current effects. If you 
		// don't like this, register this callback and you will be able to control
		// which effects are filtered out. Return true to allow an effect, false
		// to filter it out of the effect list.
		typedef bool (*EffectFilterCallback)(const PresetNumber, const AxeEffect);
		void registerEffectFilterCallback(EffectFilterCallback); 

		// These are values supported by the AxeFX 3, and can't be changed.
		const static byte BANK_SIZE 					= 128;
		const static byte MAX_BANKS 					= 4;
		const static byte MAX_SCENES 					= 8;
		const static byte TEMPO_MIN 					= 24;
		const static byte TEMPO_MAX 					= 250;
		const static byte MIDI_CHANNEL_OMNI				= 0;
		const static byte DEFAULT_MIDI_CHANNEL			= MIDI_CHANNEL_OMNI;
		constexpr static PresetNumber MAX_PRESETS 		= (MAX_BANKS * BANK_SIZE) - 1;

		// OK, there ends the tour! Continue on to AxePreset.h, 
		// AxeEffect.h, and AxeLooper.h for more information.

	private: 
		#include "private/AxeSystem_Private.h"

};