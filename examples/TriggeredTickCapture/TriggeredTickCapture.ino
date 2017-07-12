/*
	TriggeredTickCapture
	Using interrupt-able pin for Tick time capture using output pin to generate ticks
	Sampling starts when out pin goes high, and sample is caught on falling edge
	thus serial output results should be close in measurement,
	and represent half of the real spent time (see threshold)

	This example code is in the public domain.

	created Dec 23 2016
	latest mod Jan 7 2016
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

void tickGenerator()
{
	// Note that this tick generator stretches following the amount of code runned by cpu cycle & interrupts
	// use a real freq generator of any kind for accurate tick(s) if you want to test library results accuracy
	static const uint16_t ovf = ((uint16_t) -1L) / 1;	// overflow at 100% of full cnt scale
	static const uint16_t thr = ovf / 2;				// threshold at 50% of overflow cnt scale
	static uint16_t cnt = 0;
	
	digitalWrite(tickOUTPin, (cnt >= thr) ? HIGH : LOW);
	if (++cnt > ovf)	{ cnt = 0; }
	
	if (cnt == thr)
	{
		Serial.print("Start capture at:");
		Serial.print(micros());
		Serial.print(" us");
		CaptureTimer::startTickCapture();
	}
}


void setup()
{
	Serial.begin(115200);

	// initialize the capture timer pin & period
	CaptureTimer::initCapTime(ctINPin);

	pinMode(tickOUTPin, OUTPUT);
}


void loop()
{
	uint32_t Time;

	tickGenerator();
	
	if (CaptureTimer::getTickCapture(&Time) == true)
	{	// Only print on serial if new data is available
		Serial.print("\tTick after: ");
		Serial.print(Time);
		Serial.println(" us since triggered");
	}
}
