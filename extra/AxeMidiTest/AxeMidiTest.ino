#include <AxeMidi.h>

AxeMidi m;

unsigned long time;

void setup() {
  delay(2000);
  Serial.begin(9600);
  m.init(Serial1);
  time = millis();
}

void loop() {

  static byte scene = 0;

  if (millis() - time > 3000) {
    byte data[1] = {scene++};
//    m.sendCommand(0x0C, data);
    m.sendCommand(0x08);
    time = millis();
    if (scene > 7) scene = 0;
  }
  
  m.read();

}

void testPresetChange() {
    for (int i=300; i<310; i++) {
    Serial.println(i);
    m.sendPresetChange(i);
    delay(1000);
  }
}
