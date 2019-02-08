#include "AxeMidi.h"

#define SER_AVLB _serial->available()
#define SER_READ _serial->read()

void AxeMidi::init(HardwareSerial& serial) {
	_serial = &serial;
	_serial->begin(MIDI_BAUD);
}

bool AxeMidi::read() {

	if (SER_AVLB) {
		
		byte data = SER_READ;

		if (_readingSysex) {
			_sysexBuffer[_sysexCount++] = data;
			if (data == SYSEX_END) {
				_readingSysex = false;
				onSysEx();
			}

		} else {

			switch (data) {

				case ControlChange:
					while (SER_AVLB < 2);
					if (MIDI_BANK_CC == SER_READ) {
						_bank = SER_READ;
					}
					break;

				case ProgramChange:
					while (SER_AVLB < 1);
					byte patch = SER_READ;
					onPresetChange(_bank * 128 + _patch);
					break;

				case SystemExclusive:
					_readingSysex = true;
					_sysexCount = 0;
					_sysexBuffer[_sysexCount++] = data;
					break;

			}

		}

	}

	return false;
}

void AxeMidi::onPresetChange(unsigned preset) {
	Serial.print("PRESET: ");
	Serial.println(preset);
}

void AxeMidi::onSysEx() {
	Serial.print("GOT SYSEX: ");
	for (byte i=0; i<_sysexCount; i++) {
		char buf[6];
		snprintf(buf, 6, "0x%02X ", _sysexBuffer[i]);
		Serial.print(buf);
	}
	Serial.println();
}

void AxeMidi::sendPresetChange(unsigned preset) {
	send(MIDI_CC);
	send(MIDI_BANK_CC);
	send(preset / 128);
	send(MIDI_PC);
	send(preset % 128);
}

void AxeMidi::sendCommand(const byte command) {
	sendCommand(command, nullptr, 0);
}

void AxeMidi::sendCommand(const byte command, const byte *data, const byte paramCount) {
	
	byte length = 0;
	byte sysex[paramCount + 8];
	
	//header
	sysex[length++] = 0xF0;
	sysex[length++] = 0x00;
	sysex[length++] = 0x01;
	sysex[length++] = 0x74;
	sysex[length++] = 0x10;
	sysex[length++] = command;

	//optional data
	for (byte j=0; j<paramCount; j++) {
		sysex[length++] = data[j];
	}

	//checksum
	byte sum = 0xF0;
	for (int i=1; i<length; ++i) {
		sum ^= sysex[i];
	}

	//footer
	sysex[length++] = (sum & 0x7F);
	sysex[length++] = 0xF7;

	//punch it!
	for (byte i=0; i<length; i++) {
		send(sysex[i]);
	}

	#ifdef AXE_DEBUG_SYSEX
	char buf[6];
	Serial.print("SYSEX OUT: ");
	for (byte i=0; i<length; i++) {
		snprintf(buf, 6, "0x%02X ", sysex[i]);
		Serial.print(buf);
	}
	Serial.println();
	#endif

}

void AxeMidi::send(byte value) {
	_serial->write(value);
}