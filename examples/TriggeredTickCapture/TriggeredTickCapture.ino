/*
	TriggeredTickCapture
	Using interrupt-able pin for Tick time capture using output pin to generate ticks
	Sampling starts when out pin goes high, and sample is caught on falling edge
	thus serial output results should be close in measurement,
	and represent half of the real spent time (see threshold)

	This example code is in the public domain.

	created 23 December 2016
	by SMFSW
*/

#include <CaptureTimer.h>

#define tickOUTPin		4		// Pin used as tick generator (shall be connected to ctINPin)
// Literal Pin / Board
// 4 / most arduino boards (or D4)
// D2 / WeMos D1 R2
// #4 / Feather HUZZAH
#define ctINPin			2		// Pin used for Input Capture (ticks count)
// Literal Pin / Board
// 2 / most arduino boards (or D2)
// D4 / WeMos D1 R2
// #2 / Feather HUZZAH


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);

	// initialize the capture timer pin & period
	CaptureTimer::initCapTime(ctINPin);

    pinMode(tickOUTPin, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
	static uint16_t cnt = 0;
	static const uint16_t threshold = pow(256, sizeof(cnt)) / 2;    // threshold at 50% of full cnt scale
	uint32_t Time;
	boolean val, trig;

	val = (cnt++ >= threshold) ? HIGH : LOW;
	digitalWrite(tickOUTPin, val);
	if (cnt == threshold)	{ CaptureTimer::startTickCapture(); }

	if (CaptureTimer::getTickCapture(&Time) == true)
	{	// Only print on serial if new data is available
		Serial.print("Tick after: ");
		Serial.print(Time);
		Serial.println(" us since triggered");
	}
}
