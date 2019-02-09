#include <AxeFxControl.h>

AxeSystem Axe;
Tempo tempo;
unsigned long timer;

void setup() {
  
	Serial.begin(9600);
	
	timer = millis();
	tempo = AxeSystem::TEMPO_MIN;

	Axe.registerSystemChangeCallback(onSystemChange);
	Axe.registerTapTempoCallback(onTapTempo);
	Axe.begin(Serial1);

}

void loop() {

	unsigned long now = millis();
	if (now - timer > 1000) {
		timer = now;
		tempo++;
		if (tempo > AxeSystem::TEMPO_MAX) {
			tempo = AxeSystem::TEMPO_MIN;
		}
		Axe.setTempo(tempo);

	}
  
	Axe.update();
}

void onSystemChange() {
    Serial.print("TEMPO is now: ");
		Serial.println(Axe.getTempo());
}

//this will only work if realtime sysex is enabled
void onTapTempo() {
	Serial.println("TAP!");
}