#pragma once

#include <Arduino.h>

class AxeMidi {

	public:

		void init(HardwareSerial& serial);
		bool read();
		void sendPresetChange(unsigned preset);

		void sendCommand(const byte command);
		void sendCommand(const byte command, const byte *data, const byte paramCount);

	private:

		enum MidiType {
			ControlChange = 0xB0,
			ProgramChange = 0xC0,
			SystemExclusive = 0xF0
		};

		const static byte MIDI_CC 			= 0xB0;
		const static byte MIDI_PC 			= 0xC0;
		const static byte MIDI_BANK_CC 	= 0x00;
		const static byte SYSEX_END			= 0xF7;
		const static byte MAX_SYSEX 		= 128;
		const static unsigned MIDI_BAUD	= 31250;

		byte _bank;
		byte _sysexBuffer[MAX_SYSEX];
		byte _sysexCount;
		bool _readingSysex = false;
		HardwareSerial *_serial = nullptr;

		void send(byte value);
		void onPresetChange(unsigned preset);
		void onSysEx();
};