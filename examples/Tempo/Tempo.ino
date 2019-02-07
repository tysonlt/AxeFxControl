#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {
  Serial.begin(9600);
	Axe.registerSystemChangeCallback(onSystemChange);
}

void loop() {
  for (Tempo tempo = AxeSystem::TEMPO_MIN; tempo <= AxeSystem::TEMPO_MAX; tempo++) {
    Axe.setTempo(tempo);
    delay(1000);
  }
	Axe.update();
}

void onSystemChange() {
    Serial.print("TEMPO is now: ");
		Serial.println(Axe.getTempo());
}