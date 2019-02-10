#include <AxeSystem.h>

#define SER_AVLB _serial->available()
#define SER_READ _serial->read()
#define SER_SEND _serial->write

void AxeSystem::begin(HardwareSerial& serial, byte midiChannel) {	
	if (_startupDelay > 0) { 
		delay(_startupDelay);
	}
	setMidiChannel(midiChannel);
	_serial = &serial;
	_serial->begin(MIDI_BAUD);
	_midiReady = true;
}

void AxeSystem::readMidi() {

	if (SER_AVLB) {
		
		byte data = SER_READ;
		
		if (_readingSysex) {

			_sysexBuffer[_sysexCount++] = data;
			if (data == SYSEX_END) {
				_readingSysex = false;
				if (isAxeSysEx(_sysexBuffer, _sysexCount)) {
					onSystemExclusive(_sysexBuffer, _sysexCount);
				} else {
					_sysexCount = 0;
				}
			}

		} else {

			//mask channel nibble
			switch (data & 0xF0) {

				case ControlChange: {
					if (filterChannel(data)) {
						while (SER_AVLB < 2); //assume rest of message is in buffer or coming
						if (BANK_CHANGE_CC == SER_READ) {
							_bank = SER_READ;
						}
					}
					break;
				}

				case ProgramChange: {
					if (filterChannel(data)) {
						while (SER_AVLB < 1); //assume rest of message is in buffer or coming
						byte patch = SER_READ;
						onPresetChange(_bank * 128 + patch);
					}
					break;
				}

				case SystemExclusive: {
					_readingSysex = true;
					_sysexCount = 0;
					_sysexBuffer[_sysexCount++] = data;
					break;
				}

			} //end switch

		} //end if reading sysex

	} //end if ser avail

}

bool AxeSystem::filterChannel(byte data) {
	return _midiChannel == MIDI_CHANNEL_OMNI || _midiChannel == ((data & 0x0F) + 1);
}

void AxeSystem::sendPresetChange(const PresetNumber number) {
	sendControlChange(BANK_CHANGE_CC, number / 128, _midiChannel);
	sendProgramChange(number % 128, _midiChannel);
	onPresetChange(number);
}

//One-based channel
void AxeSystem::sendControlChange(byte controller, byte value, byte channel) {
	SER_SEND(ControlChange | ((channel - 1) & 0x0F));
	SER_SEND(controller);
	SER_SEND(value);
}

//One-based channel
void AxeSystem::sendProgramChange(byte value, byte channel) {
	SER_SEND(ProgramChange | ((channel - 1) & 0x0F));
	SER_SEND(value);
}

void AxeSystem::sendSysEx(const byte *sysex, const byte length) {
	for (byte i=0; i<length; i++) {
		SER_SEND(sysex[i]);
	}
}

void AxeSystem::sendCommand(const byte command) {
	sendCommand(command, nullptr, 0);
}

void AxeSystem::sendCommand(const byte command, const byte *data, const byte paramCount) {
	
	byte length = 0;
	byte sysex[paramCount + 8];
	
	//header
	sysex[length++] = SystemExclusive;
	sysex[length++] = SYSEX_MANUFACTURER_BYTE1;
	sysex[length++] = SYSEX_MANUFACTURER_BYTE2;
	sysex[length++] = SYSEX_MANUFACTURER_BYTE3;
	sysex[length++] = SYSEX_AXE_VERSION;
	sysex[length++] = command;

	//optional data
	for (byte i=0; i<paramCount; i++) {
		sysex[length++] = data[i];
	}

	//checksum
	byte sum = 0xF0;
	for (int i=1; i<length; i++) {
		sum ^= sysex[i];
	}

	//footer
	sysex[length++] = (sum & 0x7F);
	sysex[length++] = SYSEX_END;

	//punch it!
	sendSysEx(sysex, length);

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

bool AxeSystem::isAxeSysEx(const byte *sysex, const byte length) {
	return 
		length > 4 && 
		sysex[1] == SYSEX_MANUFACTURER_BYTE1 && 
		sysex[2] == SYSEX_MANUFACTURER_BYTE2 && 
		sysex[3] == SYSEX_MANUFACTURER_BYTE3;
}