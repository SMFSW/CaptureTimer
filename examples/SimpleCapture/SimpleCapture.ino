/*
	SimpleCapture
	Using interruptible pin for frequency sampling (raw)

	This example code is in the public domain.

	created 22 November 2016
	latest mod 24 November 2016
	by SMSFSW
*/

#include <CaptureTimer.h>

#define ctINPin			2		// Pin used for Input Capture (ticks count)

#define samplingPer		50


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	
	// initialize the capture timer pin & period
	CaptureTimer::init(samplingPer, ctINPin);
}

// the loop function runs over and over again forever
void loop() {
	uint16_t ticks;

	if (CaptureTimer::getTicks(&ticks) == true)
	{	// Only print on serial if new datas are available
		Serial.print("Ticks: ");
		Serial.print(ticks);
		Serial.print("\tFreq: ");
		(void) CaptureTimer::getFreq(&ticks);
		Serial.print(ticks);
		Serial.println("Hz");
	}
	
	delay(5);	// here for nuthin'
}
