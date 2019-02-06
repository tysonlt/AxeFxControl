#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {
	Axe.init();
}

void loop() {
  Axe.toggleTuner();
  delay(1000);
}