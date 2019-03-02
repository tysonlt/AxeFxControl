#include <AxeFxControl.h>
#include "AxeFxEffectList.h"


#define PL Serial.println
#define P  Serial.print

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 
  
const byte sz = 50;
char input[sz];
byte index = 0;
bool readFinished = false;

AxeSystem Axe;

void setup() {
  
	Serial.begin(9600);
  while (!Serial);
  delay(1500);

	Axe.begin(Serial1);
	Axe.registerSysexPluginCallback(onSysex);

  PL("");
  PL("Type hex AxeFX3 command and data. Header and checksum will be added.");
  PL("Type '08' and this will be sent: F0 00 01 74 10 08 1D F7");
  PL("");
	
  PL("");
  PL("MIDI Ready.");
  PL("");
  
}

void loop() {

  Axe.update();
    
  readInput();
  if (readFinished) {
    sendSysEx(input);
    readFinished = false;    
    memset(input, 0, sz);
  }
    	
}

void readInput() {

  while (Serial.available() > 0 && readFinished == false) {
    char c = Serial.read();
    if (c != '\n') {
      input[index++] = c;
    } else {
      readFinished = true;
      index = 0;
    }
  }

}


bool onSysex(const byte *sysex, const byte length) {
  char buf[15];
  snprintf(buf, sizeof(buf), "RECV[%02d]: ", length);
  P(buf);
  dumpHex(sysex, length, 0, length);
  processSysex(sysex, length);
	return true; //stop further processing
}

void processSysex(const byte *sysex, const byte length) {

  if (sysex[5] == 0x13) {
    PL("");
    PL("EFFECT DUMP");
    for (byte i=6; i<length-3; i+=3) {      
      printEffect(sysex[i], sysex[i+1], sysex[i+2]);
    }
    
  } else if (sysex[5] == 0x0E) {
    Serial.print("Scene: >");
    for (byte i=6; i<length-2; i++) {      
      Serial.print((char) sysex[i]);
    }
    Serial.println("<");
  }
  
}

void printEffect(const byte effectId, const byte data1, byte const data2) {

  unsigned intEffectId = effectId;
  char buf[100], name[50], tag[10];

  //Because midi data bytes always have the MSB set to 0, data one contains the MSB.
  //If this set, we need to set the MSB of the effect number to get higher than 127.
  if (data1) {
    intEffectId = effectId | 128;
  }
  
  bool bypassed = (data2 & 0x01);
  char channel = 'A' + ((data2 >> 1) & 0x03);
  byte numChannels = (data2 >> 4) & 0x07;
  if (assignNameAndTag(intEffectId, name, 50, tag, 10)) {
    snprintf(buf, 100, 
      "[%c][%s][0x%02X 0x%02X 0x%02X][%c%c%c%c%c%c%c%c][%03d][ch=%c/%d] %s", 
      bypassed ? ' ' : 'X', tag, effectId, data1, data2, BYTE_TO_BINARY(data2), intEffectId, channel, numChannels, name);
    Serial.println(buf);
  }
  
  
}

void sendSysEx(char input[]) {

    byte sysex[50];
    
    char *p = input;
    char *str;

    byte length = 0;

    //header
    sysex[length++] = 0xF0;
    sysex[length++] = 0x00;
    sysex[length++] = 0x01;
    sysex[length++] = 0x74;
    sysex[length++] = 0x10;
    
    while ((str = strtok_r(p, " ", &p)) != NULL) {
      sysex[length++] = (byte) strtoul(str, NULL, 16);
    }

    //add checksum
    sysex[length++] = 0x00;
    addChecksum(length, sysex, true);

    sysex[length++] = 0xF7;

    char buf[15];
    PL("");
    snprintf(buf, sizeof(buf), "SEND[%02d]: ", length);
    P(buf);
    dumpHex(sysex, length, 0, length);

    Axe.sendSysEx(sysex, length);
  
}

void addChecksum(const byte length, byte *sysex, bool headersAdded) {

  byte sum = headersAdded ? 0 : 0xF0;
  for (int i=0; i<length-1; ++i) {
    sum ^= sysex[i];
  }
  sysex[length-1] = (sum & 0x7F);
  
}

void dumpHex(const byte *arr, const int len, int from, int to) {
  char buf[6];
  if (to == 0) to = len;
  for (byte i=from; i<to; i++) {
    snprintf(buf, sizeof(buf), "0x%02X ", arr[i]);
    P(buf);
  }
  PL();
}