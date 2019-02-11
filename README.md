# AxeFxControl

An Arduino library for interfacing with the AxeFX. 

Currently tested for the AxeFX 3 - I don't have a 2 so I can't test it - contributers welcome!

# Documentation

## Quick Start

To get started quickly, just check out the examples. They contain a complete reference for the API (in fact they double as the unit tests!) Start with PresetDetails.ino, Refresh.ino, and SwitchPreset.ino.

## Classes

### AxeSystem

This is the main class you will interact with. There is no need to worry about 
including any other classes.

Create an instance of this in your main sketch, above the setup function, like this: 
AxeSystem Axe;

Then just use it anywhere in your code. Check the examples for how to set it up.

The examples also function as unit tests, and as such they (should) cover every 
method in this class. If you need help, just go to the examples to see how to use 
the library.


		// Declare an instance of this class above your setup() function.
		// Optionally register a notification callback, optionally turn on auto-refresh, 
		// and then just call 'Axe.update();' in the main loop and you are ready to roll.
		AxeSystem();

		// You must call begin with a hardware serial such as Serial1. Optionally set the 
		// MIDI channel, defaults to OMNI.
		// You cannot call this more than once. (Well you can, but it will be ignored.)
		void begin(HardwareSerial& serial, byte midiChannel = DEFAULT_MIDI_CHANNEL);

		// Optionally set the MIDI channel. You can safely change this at any time.
		void setMidiChannel(byte channel);

		// Update preset details every millis. Don't refresh if another preset request 
		// was received within throttle interval.
		void enableRefresh(const millis_t millis = 3000, const millis_t throttle = 500);

		// Remember to call this from loop(). Avoid using delay() anywhere in your code 
		// for best results.
		void update();

		// Manually refresh the current preset. This is called automatically 
		// if enableRefresh() was called. 
		// Pass true to refresh immediately, regardless of when the last sysex 
		// message was receieved.   
		void refresh(bool ignoreThrottle = false);

		// These are the commands you can send to the Axe.
		// Notice that you cannot directly query the scene and effects. 
		// These must be fetched as part of a preset details request.
		// This helps maintain a proper state in case of fast preset changes.
		//
		// To get the results back, you should register a callback.
		// Call registerPresetChangeCallback() to be notified of preset changes.
		//
		// Alternatively, you can call getPreset() any time you want, but it will
		// not reflect the next preset until isPresetReady() returns true. This
		// is not the best way to go. You should register a callback instead.
		void requestPresetDetails();
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
		// changing. (They are just the inverse of each other.) During this time, 
		// sysex data is being read from the Axe, so the result will still reflect
		// the old preset until isPresetReady() returns true, or isPresetChanging()
		// returns false. You are better of registering a preset change callback!
		bool isPresetReady();
		bool isPresetChanging();

		// Well is it, or isn't it? As far as we know! This is pretty accurate
		// even if you enable/disable the tuner from the front panel.
		bool isTunerEngaged();

		// Get the current tempo as a BPM value.
		Tempo getTempo();

		// Return a COPY of the current preset. This is not a pointer, so it
		// won't magically update after a refresh. Either call getPreset() again,
		// or better yet, register a preset change callback to get fresh state.
		AxePreset getCurrentPreset();

		// Get the looper object to query and play with. 
		AxeLooper& getLooper();

		// Get the firmware version. Not that useful but nice to display at boot perhaps.
		Version getFirmwareVersion();
		Version getUsbVersion();

		// Change at your peril. Determines how much radio silence will result 
		// in a disconnect callback being fired. (Which does nothing, just lets you know.)
		void setSysexTimout(const millis_t ms);

		// Change at your peril. If we think the tuner is engaged, but this time passes
		// without a tuner message, we will assume the tuner was turned off at the front panel.
		void setTunerTimeout(const millis_t ms);

		// OK, you probably can mess with this one. Some hardware requires a delay after 
		// calling begin() on the Serial object before it is actually ready. Perhaps this
		// is only a problem with cheap clones, but the default 1000ms pause solves a lot
		// of stability problems, so it is safe to leave it in. Show a pretty pattern on 
		// your screen or something, or set to 0 to disable altogether.
		void setStartupDelay(const millis_t ms);

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

		// Called when we detect a preset is about to change. 
		void registerPresetChangingCallback(PresetChangingCallback);

		// This is probably the one you want. Lets you know when the new preset is ready to query.
		void registerPresetChangeCallback(PresetChangeCallback);

		// These three give you fine-grained notification when different parts of the preset
		// are received. This is useful if you only want the name and don't care about the effects.
		void registerPresetNameCallback(PresetNameCallback);
		void registerSceneNameCallback(SceneNameCallback);
		void registerEffectsReceivedCallback(EffectsReceivedCallback); 
		
		// Called when a global parameter changes... well, when the tempo changes. 
		// That's all we know about! Will also be called when we read the firmware 
		// version at startup.
		void registerSystemChangeCallback(SystemChangeCallback);

		// The Axe sends real-time pulses in time with the tempo. 
		// Flash a led! I woudln't update a TFT or LCD, it's too slow.
		// This requires Setup -> MIDI -> Realtime Sysex to be enabled on the Axe.
		void registerTapTempoCallback(TapTempoCallback);
		
		// Tuner data comes flooding in when the tuner is engaged.
		// String goes from 1 to 6, and finetune from 0-127, where 63 is in-tune.
		// This requires Setup -> MIDI -> Realtime Sysex to be enabled on the Axe.
		void registerTunerDataCallback(TunerDataCallback);

		// Tuner on/off notifications. Due to some heuristic magic, it's pretty accurate.
		void registerTunerStatusCallback(TunerStatusCallback);

		// Something changed on the looper. 
		void registerLooperStatusCallback(LooperStatusCallback);

		// These let you override the way messages are processed. 
		// See the typedef documentation above to learn how they work.
		void registerSysexPluginCallback(SysexPluginCallback); 
		void registerEffectFilterCallback(EffectFilterCallback); 
