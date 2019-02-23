#include <AxeFxControl.h>

AxeSystem Axe;

void setup() {

	Serial.begin(9600);

	Axe.begin(Serial1);
	Axe.registerLooperStatusCallback(onLooperChange);
	Axe.enableRefresh(1000);

}

void loop() {
	Axe.update();
}

void onLooperChange(AxeLooper looper) {

	const size_t sz = 100;
	char buf[sz];

	Serial.print("Looper status: ");
	snprintf(buf, sz, 
		"record[%d] play[%d] overdub[%d] once[%d] reverse[%d] halfspeed[%d]",
		looper.isRecord(),
		looper.isPlay(),
		looper.isOverdub(),
		looper.isOnce(),
		looper.isReverse(),
		looper.isHalfSpeed()
	);
	Serial.println(buf);

}
