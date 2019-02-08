#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {
	Serial.begin(9600);
}

void loop() {
	
	Serial.println("record");
	Axe.getLooper().record();
  delay(1000);
	Serial.println("record");
	Axe.getLooper().record();
  delay(1000);

	Serial.println("play");
	Axe.getLooper().play();
  delay(1000);
	Serial.println("play");
	Axe.getLooper().play();
  delay(1000);

	Serial.println("undo");
	Axe.getLooper().undo();
  delay(1000);

	Serial.println("once");
	Axe.getLooper().once();
  delay(1000);

	Serial.println("reverse");
	Axe.getLooper().reverse();
  delay(1000);
	Serial.println("reverse");
	Axe.getLooper().reverse();
  delay(1000);
	
	Serial.println("halfSpeed");
	Axe.getLooper().halfSpeed();
  delay(1000);
	Serial.println("halfSpeed");
	Axe.getLooper().halfSpeed();
  delay(1000);
}

