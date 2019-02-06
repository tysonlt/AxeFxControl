#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {
  Serial.begin(9600);
  Axe.init();
}

void loop() {
  for (byte tempo = AxeSystem::TEMPO_MIN; tempo <= AxeSystem::TEMPO_MAX; tempo++) {
    Axe.setTempo(tempo);
    Serial.println(tempo);
    delay(1000);
  }
}