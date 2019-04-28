#include "interface/AxeSystem.h"

#define SER_AVLB _serial->available()
#define SER_READ _serial->read()
#define SER_SEND _serial->write

void AxeSystem::begin(HardwareSerial& serial, byte midiChannel) {	
	if (!_midiReady) {
		if (_startupDelay > 0) { 
			delay(_startupDelay);
		}
		setMidiChannel(midiChannel);
		_serial = &serial;
		_serial->begin(MIDI_BAUD);
		_midiReady = true;
	}
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
					if (filterMidiChannel(data)) {
						while (SER_AVLB < 2); //assume rest of message is in buffer or coming
						if (BANK_CHANGE_CC == SER_READ) {
							_bank = SER_READ;
						}
					}
					break;
				}

				case ProgramChange: {
					if (filterMidiChannel(data)) {
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

bool AxeSystem::filterMidiChannel(byte data) {
	return _midiChannel == MIDI_CHANNEL_OMNI || _midiChannel == ((data & 0x0F) + 1);
}

byte AxeSystem::applyMidiChannel(byte midiByte, byte channel) {
	if (channel == MIDI_CHANNEL_OMNI) {
		return midiByte;
	} else {
		return midiByte | ((channel - 1) & 0x0F);
	}
}

void AxeSystem::sendPresetChange(const PresetNumber number) {
	sendControlChange(BANK_CHANGE_CC, number / 128, _midiChannel);
	sendProgramChange(number % 128, _midiChannel);
	if (_updateMode != PASSIVE_WAIT_FOR_AXE) {
		// Axe PC messages are off (or we don't know yet), so kick off a state update manually.
		// The receivedFromAxe flag tell the library that this was a manual request, so if in 
		// AUTO mode, don't switch to PASSIVE based on this request.
		onPresetChange(number, false); 
	}
}

//One-based channel
void AxeSystem::sendControlChange(byte controller, byte value, byte channel) {
	SER_SEND(applyMidiChannel(ControlChange, channel));
	SER_SEND(controller);
	SER_SEND(value);
}

//One-based channel
void AxeSystem::sendProgramChange(byte value, byte channel) {
	SER_SEND(applyMidiChannel(ProgramChange, channel));
	SER_SEND(value);
}

void AxeSystem::sendSysEx(const byte *sysex, const byte length) {
	for (byte i=0; i<length; i++) {
		SER_SEND(sysex[i]);
	}
	#ifdef AXE_DEBUG_SYSEX     
	debugSysex(sysex, length, "-> AxeSystem::sendSysEx():         ");
	#endif
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

}

bool AxeSystem::isAxeSysEx(const byte *sysex, const byte length) {
	return 
		length > 4 && 
		sysex[1] == SYSEX_MANUFACTURER_BYTE1 && 
		sysex[2] == SYSEX_MANUFACTURER_BYTE2 && 
		sysex[3] == SYSEX_MANUFACTURER_BYTE3;
}